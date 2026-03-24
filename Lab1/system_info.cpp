// system_info_windows.cpp
#include <iostream>
#include <cstdlib>

using namespace std;

int main() {
    cout << "CPU Info:\n";
    system("wmic cpu get Name");

    cout << "\nMemory Info:\n";
    system("systeminfo | findstr /C:\"Total Physical Memory\"");

    cout << "\nDisk Info:\n";
    system("wmic diskdrive get Model,Size");

    cout << "\nMAC Addresses:\n";
    system("getmac");

    return 0;
}
