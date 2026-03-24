#include "LicenseKey.h"
#include <iostream>
#include <sstream>
#include <iomanip>

LicenseKey::LicenseKey(const std::string& machine, const std::string& user)
    : machineID(machine), licensee(user), isTemporary(false) {
    issueDate = time(nullptr);
    expiryDate = 0; // 0 նշանակում է մշտական
    generateKey();
}

LicenseKey::LicenseKey(const std::string& machine, const std::string& user, int daysValid)
    : machineID(machine), licensee(user), isTemporary(true) {
    issueDate = time(nullptr);
    expiryDate = issueDate + (daysValid * 24 * 60 * 60);
    generateKey();
}

void LicenseKey::generateKey() {
    const std::string SALT = "X7F!pL9@2026Secret";

    std::stringstream ss;
    std::string base = machineID + "|" + licensee + "|" +
        std::to_string(issueDate) + "|" + SALT;

    unsigned long hash = 2166136261u;

    for (char c : base) {
        hash = (hash * 16777619) ^ c;
    }
    if (isTemporary) {
        hash = (hash * 16777619) ^ expiryDate;
    }
    // additional obfuscation
    hash ^= 0xA5F3C9B1;
    hash = (hash << 13) | (hash >> 19);

    ss << std::hex << std::uppercase;

    for (int i = 0; i < 15; i++) {
        if (i > 0 && i % 5 == 0)
            ss << "-";

        ss << ((hash >> (i * 4)) & 0xF);
    }

    key = ss.str();
}

bool LicenseKey::isValid(const std::string& currentMachineID) {
    // Ստուգել մեքենայի ID-ի համապատասխանությունը
    if (currentMachineID != machineID) {
        return false;
    }

    // Ժամանակավոր լիցենզիայի համար ստուգել ժամկետը
    if (isTemporary) {
        time_t now = time(nullptr);
        if (now > expiryDate) {
            return false; // Լիցենզիայի ժամկետն ավարտվել է
        }
    }

    return true;
}

void LicenseKey::printInfo() {
    std::cout << "======================================" << std::endl;
    std::cout << "License Key: " << key << std::endl;
    std::cout << "Licensee: " << licensee << std::endl;
    std::cout << "Issue Date: " << ctime(&issueDate);

    if (isTemporary) {
        std::cout << "Valid until: " << ctime(&expiryDate);
        std::cout << "Type: TEMPORARY" << std::endl;
    }
    else {
        std::cout << "Type: PERPETUAL" << std::endl;
    }
    std::cout << "======================================" << std::endl;
}
