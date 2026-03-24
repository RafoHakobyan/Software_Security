#pragma once
#ifndef LICENSEMANAGER_H
#define LICENSEMANAGER_H

#include <vector>
#include <string>
#include "LicenseKey.h"

/**
 * LicenseManager դաս - լիցենզիաների ստեղծման և ստուգման կառավարում
 */
class LicenseManager {
private:
    std::vector<LicenseKey> licenses;
    std::string currentMachineID;

    // Ֆայլից լիցենզիայի վերականգնում
    bool verifyFromFile(const std::string& filename);

public:
    LicenseManager();

    // Մշտական լիցենզիայի ստեղծում
    LicenseKey createPerpetualLicense(const std::string& userName);

    // Ժամանակավոր լիցենզիայի ստեղծում
    LicenseKey createTemporaryLicense(const std::string& userName, int days);

    // Լիցենզիայի ստուգում
    bool verifyLicense(const std::string& licenseKey);

    // Լիցենզիայի պահպանում ֆայլում
    bool saveToFile(const LicenseKey& license, const std::string& filename);

    // Ցուցադրել բոլոր լիցենզիաները
    void listAllLicenses();

    // Ընթացիկ մեքենայի ID-ի ստացում
    std::string getCurrentMachineID() { return currentMachineID; }
};

#endif
