#include "LicenseManager.h"
#include "HardwareID.h"
#include <iostream>
#include <fstream>
#include <sstream>

LicenseManager::LicenseManager() {
    HardwareID hwid;
    currentMachineID = hwid.getMachineID();

    std::cout << "Current Device ID " << currentMachineID << std::endl;
    std::cout << "CPU ID  " << hwid.getCpuId() << std::endl;
    std::cout << "MAC address " << hwid.getMacAddress() << std::endl;
    std::cout << std::endl;
}

LicenseKey LicenseManager::createPerpetualLicense(const std::string& userName) {
    LicenseKey license(currentMachineID, userName);
    licenses.push_back(license);
    return license;
}

LicenseKey LicenseManager::createTemporaryLicense(const std::string& userName, int days) {
    LicenseKey license(currentMachineID, userName, days);
    licenses.push_back(license);
    return license;
}

bool LicenseManager::verifyLicense(const std::string& licenseKey) {
    // Գտնել լիցենզիան
    for (auto& license : licenses) {
        if (license.getKey() == licenseKey) {
            return license.isValid(currentMachineID);
        }
    }

    // Եթե չի գտնվել, ստուգել ֆայլային պահոցից (Simple way)
    return verifyFromFile(licenseKey);
}

bool LicenseManager::saveToFile(const LicenseKey& license, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "KEY=" << license.getKey() << std::endl;
    file << "MACHINE=" << license.getMachineID() << std::endl;
    file << "TYPE=" << (license.isTemporaryLicense() ? "TEMPORARY" : "PERPETUAL") << std::endl;

    file.close();
    return true;
}

bool LicenseManager::verifyFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    std::string savedKey, savedMachine, savedType;

    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (name == "KEY") savedKey = value;
            else if (name == "MACHINE") savedMachine = value;
            else if (name == "TYPE") savedType = value;
        }
    }

    file.close();

    // Ստուգել մեքենայի ID-ի համապատասխանությունը
    if (savedMachine != currentMachineID) {
        std::cout << "ՍԽԱԼ. Լիցենզիան նախատեսված չէ այս համակարգչի համար:" << std::endl;
        std::cout << "  Պահանջվում է՝ " << savedMachine << std::endl;
        std::cout << "  Ընթացիկ՝ " << currentMachineID << std::endl;
        return false;
    }

    return true;
}

void LicenseManager::listAllLicenses() {
    if (licenses.empty()) {
        std::cout << "Լիցենզիաներ չեն գտնվել:" << std::endl;
        return;
    }

    std::cout << "\nԲՈԼՈՐ ԼԻՑԵՆԶԻԱՆԵՐԸ:" << std::endl;
    for (auto& license : licenses) {
        license.printInfo();
    }
}
