#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/socket.h>

class FirewallRule
{
public:
    std::string blockedIP;
    int blockedPort;

    FirewallRule(const std::string& ip, int port)
        : blockedIP(ip), blockedPort(port)
    {
    }
};

class Firewall
{
private:
    std::vector<FirewallRule> rules;

public:
    void addRule(const FirewallRule& rule)
    {
        rules.push_back(rule);
    }

    bool isBlocked(const std::string& srcIP, int dstPort)
    {
        for (const auto& rule : rules)
        {
            bool ipMatch = (rule.blockedIP == srcIP);
            bool portMatch = (rule.blockedPort == dstPort);

            if (ipMatch || portMatch)
            {
                return true;
            }
        }

        return false;
    }

    void start()
    {
        int rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

        if (rawSocket < 0)
        {
            perror("Socket creation failed");
            return;
        }

        std::cout << "Firewall started..." << std::endl;

        char buffer[65536];

        while (true)
        {
            ssize_t packetSize = recvfrom(
                rawSocket,
                buffer,
                sizeof(buffer),
                0,
                nullptr,
                nullptr);

            if (packetSize < 0)
            {
                perror("recvfrom failed");
                continue;
            }

            struct iphdr* ipHeader = (struct iphdr*)buffer;

            sockaddr_in srcAddr;
            srcAddr.sin_addr.s_addr = ipHeader->saddr;

            sockaddr_in dstAddr;
            dstAddr.sin_addr.s_addr = ipHeader->daddr;

            std::string srcIP = inet_ntoa(srcAddr.sin_addr);
            std::string dstIP = inet_ntoa(dstAddr.sin_addr);

            if (ipHeader->protocol == IPPROTO_TCP)
            {
                struct tcphdr* tcpHeader =
                    (struct tcphdr*)(buffer + ipHeader->ihl * 4);

                int srcPort = ntohs(tcpHeader->source);
                int dstPort = ntohs(tcpHeader->dest);

                bool blocked = isBlocked(srcIP, dstPort);

                std::cout << "[TCP] "
                          << srcIP << ":" << srcPort
                          << " -> "
                          << dstIP << ":" << dstPort;

                if (blocked)
                {
                    std::cout << "  [BLOCKED]" << std::endl;
                }
                else
                {
                    std::cout << "  [ALLOWED]" << std::endl;
                }
            }
            else if (ipHeader->protocol == IPPROTO_UDP)
            {
                struct udphdr* udpHeader =
                    (struct udphdr*)(buffer + ipHeader->ihl * 4);

                int srcPort = ntohs(udpHeader->source);
                int dstPort = ntohs(udpHeader->dest);

                bool blocked = isBlocked(srcIP, dstPort);

                std::cout << "[UDP] "
                          << srcIP << ":" << srcPort
                          << " -> "
                          << dstIP << ":" << dstPort;

                if (blocked)
                {
                    std::cout << "  [BLOCKED]" << std::endl;
                }
                else
                {
                    std::cout << "  [ALLOWED]" << std::endl;
                }
            }
        }

        close(rawSocket);
    }
};

int main()
{
    Firewall firewall;

    firewall.addRule(FirewallRule("192.168.1.100", -1));
    firewall.addRule(FirewallRule("", 80));
    firewall.addRule(FirewallRule("", 23));

    firewall.start();

    return 0;
}
