#include <iostream>
#include <string>
#include "LicenseManager.h"

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "    LICENSE SYSTEM - LABORATORY WORK    " << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;

    LicenseManager manager;

    int choice;
    std::string userName;

    do {
        std::cout << "\n--- SELECT ACTION ---" << std::endl;
        std::cout << "1. Create perpetual license" << std::endl;
        std::cout << "2. Create temporary license" << std::endl;
        std::cout << "3. Verify license" << std::endl;
        std::cout << "4. Show all licenses" << std::endl;
        std::cout << "5. Save license to file" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1: {
            std::cout << "Enter licensee name: ";
            std::getline(std::cin, userName);

            LicenseKey license = manager.createPerpetualLicense(userName);
            std::cout << "\n!!!! PERPETUAL LICENSE CREATED:" << std::endl;
            license.printInfo();
            break;
        }

        case 2: {
            std::cout << "Enter licensee name: ";
            std::getline(std::cin, userName);

            std::cout << "Temporary Lincense\n" << "1. 1 day\n" << "2. 14 days\n" << "3. 30 days\n";
            std::cout << "Choose how long you want the lincense (enter the corresponding number): ";
            int days;
            std::string stringDays;
            getline(std::cin, stringDays);
            days = std::stoi(stringDays);
            switch (days) {
            case 1: {
                LicenseKey license = manager.createTemporaryLicense(userName, 1);
                std::cout << "\nv TEMPORARY LICENSE CREATED (1 day):" << std::endl;
                license.printInfo();
                break;
            }
            case 2: {
                LicenseKey license = manager.createTemporaryLicense(userName, 14);
                std::cout << "\nv TEMPORARY LICENSE CREATED (14 days):" << std::endl;
                license.printInfo();
                break;
            }
            case 3: {
                LicenseKey license = manager.createTemporaryLicense(userName, 30);
                std::cout << "\nv TEMPORARY LICENSE CREATED (30 days):" << std::endl;
                license.printInfo();
                break;
            }
            default: {
                std::cout << "Invalid choise!!!";
                break;
            }
            }
        }

        case 3: {
            std::string key;
            std::cout << "Enter license key: ";
            std::getline(std::cin, key);

            if (manager.verifyLicense(key)) {
                std::cout << "v LICENSE IS VALID:" << std::endl;
            }
            else {
                std::cout << "x LICENSE IS INVALID OR NOT FOUND:" << std::endl;
            }
            break;
        }

        case 4: {
            manager.listAllLicenses();
            break;
        }

        case 5: {
            std::string key, filename;
            std::cout << "Enter license key: ";
            std::getline(std::cin, key);
            std::cout << "Enter filename: ";
            std::getline(std::cin, filename);

            LicenseKey temp(manager.getCurrentMachineID(), "Test User");
            if (manager.saveToFile(temp, filename)) {
                std::cout << "v License saved to " << filename << std::endl;
            }
            else {
                std::cout << "x Error saving to file:" << std::endl;
            }
            break;
        }

        case 0: {
            std::cout << "Program finished." << std::endl;
            break;
        }

        default: {
            std::cout << "Invalid choice:" << std::endl;
            break;
        }
        }

    } while (choice != 0);

    return 0;
}


