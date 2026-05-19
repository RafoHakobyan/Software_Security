#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include <tcl.h>
#include <tk.h>

std::mutex logMutex;
bool running = true;

std::vector<std::string> blockedIPs;

Tk_Window mainWindow;
Tcl_Interp* interp;

std::ofstream logfile("firewall.log", std::ios::app);

void logMessage(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(logMutex);

    std::cout << msg << std::endl;

    logfile << msg << std::endl;

    std::string cmd =
        ".log insert end {" + msg + "\\n};"
        ".log see end;";

    Tcl_Eval(interp, cmd.c_str());
}

bool isBlocked(const std::string& ip)
{
    for (const auto& blocked : blockedIPs)
    {
        if (blocked == ip)
        {
            return true;
        }
    }

    return false;
}

void addIptablesRule(const std::string& ip)
{
    std::string cmd =
        "iptables -A INPUT -s " + ip + " -j DROP";

    system(cmd.c_str());

    logMessage("[iptables] Blocked IP: " + ip);
}

void removeIptablesRules()
{
    system("iptables -F");

    logMessage("[iptables] Rules cleared");
}

void packetSniffer()
{
    int rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    if (rawSocket < 0)
    {
        logMessage("Socket creation failed");
        return;
    }

    char buffer[65536];

    while (running)
    {
        ssize_t size =
            recvfrom(rawSocket,
                     buffer,
                     sizeof(buffer),
                     0,
                     nullptr,
                     nullptr);

        if (size < 0)
        {
            continue;
        }

        struct iphdr* ip =
            (struct iphdr*)buffer;

        sockaddr_in src;
        src.sin_addr.s_addr = ip->saddr;

        sockaddr_in dst;
        dst.sin_addr.s_addr = ip->daddr;

        std::string srcIP =
            inet_ntoa(src.sin_addr);

        std::string dstIP =
            inet_ntoa(dst.sin_addr);

        if (ip->protocol == IPPROTO_TCP)
        {
            struct tcphdr* tcp =
                (struct tcphdr*)
                (buffer + ip->ihl * 4);

            int srcPort = ntohs(tcp->source);
            int dstPort = ntohs(tcp->dest);

            std::stringstream ss;

            ss << "[TCP] "
               << srcIP
               << ":"
               << srcPort
               << " -> "
               << dstIP
               << ":"
               << dstPort;

            if (isBlocked(srcIP))
            {
                ss << " [BLOCKED]";
            }
            else
            {
                ss << " [ALLOWED]";
            }

            logMessage(ss.str());
        }
    }

    close(rawSocket);
}

int AddRuleCmd(ClientData,
               Tcl_Interp* interp,
               int argc,
               const char* argv[])
{
    if (argc != 2)
    {
        return TCL_ERROR;
    }

    std::string ip = argv[1];

    blockedIPs.push_back(ip);

    addIptablesRule(ip);

    logMessage("[GUI] Added rule: " + ip);

    return TCL_OK;
}

int ClearRulesCmd(ClientData,
                  Tcl_Interp* interp,
                  int argc,
                  const char* argv[])
{
    blockedIPs.clear();

    removeIptablesRules();

    logMessage("[GUI] Cleared rules");

    return TCL_OK;
}

void createGUI()
{
    interp = Tcl_CreateInterp();

    Tk_Init(interp);

    Tcl_CreateCommand(interp,
                      "addRule",
                      AddRuleCmd,
                      nullptr,
                      nullptr);

    Tcl_CreateCommand(interp,
                      "clearRules",
                      ClearRulesCmd,
                      nullptr,
                      nullptr);

    const char* gui = R"(

wm title . "C++ Firewall"

frame .top
pack .top -side top -fill x

entry .ipEntry
pack .ipEntry -in .top -side left

button .addBtn \
-text "Block IP" \
-command {
    addRule [.ipEntry get]
}
