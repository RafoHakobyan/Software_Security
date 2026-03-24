#include "usb_detector.h"
#include "file_operations.h"
#include "cipher.h"
#include "utils.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <filesystem>
namespace fs = std::filesystem;

void showHelp() {
    std::cout << "\n";
    std::cout << "ProtectUSB - USB Drive Content Protection\n";
    std::cout << "==========================================\n\n";
    std::cout << "Usage:\n";
    std::cout << "  protectusb [command] [arguments]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  scan              - Detect connected USB drives\n";
    std::cout << "  list <path>       - List files in specified path\n";
    std::cout << "  protect <path>    - Protect USB drive (encrypt all files)\n";
    std::cout << "  unprotect <path>  - Restore USB drive (decrypt all files)\n";
    std::cout << "  encrypt <file>    - Encrypt a single file\n";
    std::cout << "  decrypt <file>    - Decrypt a single file\n";
    std::cout << "  info <path>       - Show drive information\n";
    std::cout << "  help              - Show this help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  protectusb scan\n";
    std::cout << "  protectusb list D:\\\n";
    std::cout << "  protectusb protect D:\\\n";
    std::cout << "  protectusb encrypt secret.doc\n";
}

bool protectUSBDrive(const std::string& drivePath, const std::string& key) {
    std::cout << "\nProtecting USB drive: " << drivePath << std::endl;
    std::cout << "--------------------------------\n";

    std::string protectedDir = drivePath + "\\Protected";
    if (!FileOperations::fileExists(protectedDir)) {
        FileOperations::createDirectory(protectedDir);
    }

    std::vector<std::string> files = FileOperations::collectFiles(drivePath);
    std::vector<std::string> filesToProtect;

    for (size_t i = 0; i < files.size(); i++) {
        if (files[i].find(".encrypted") == std::string::npos &&
            files[i].find("config.dat") == std::string::npos &&
            files[i].find("Protected") == std::string::npos) {
            filesToProtect.push_back(files[i]);
        }
    }

    if (filesToProtect.empty()) {
        std::cout << "No files to protect found.\n";
        return true;
    }

    std::cout << "Found " << filesToProtect.size() << " files to encrypt.\n";

    SimpleXORCipher cipher(key);
    int successCount = 0;

    for (size_t i = 0; i < filesToProtect.size(); i++) {
        std::string encryptedFile = filesToProtect[i] + ".encrypted";

        std::cout << "Encrypting: " << filesToProtect[i] << " ... ";

        if (cipher.encryptFile(filesToProtect[i], encryptedFile)) {
            if (FileOperations::deleteFile(filesToProtect[i])) {
                std::cout << "OK\n";
                successCount++;
            }
            else {
                std::cout << "Failed to delete original\n";
            }
        }
        else {
            std::cout << "Encryption failed\n";
        }
    }

    std::cout << "\nProtection completed.\n";
    std::cout << "Encrypted " << successCount << " of " << filesToProtect.size() << " files.\n";

    return true;
}

bool unprotectUSBDrive(const std::string& drivePath, const std::string& key) {
    std::cout << "\nUnprotecting USB drive: " << drivePath << std::endl;
    std::cout << "--------------------------------\n";

    std::vector<std::string> encryptedFiles =
        FileOperations::findFilesByExtension(drivePath, ".encrypted");

    if (encryptedFiles.empty()) {
        std::cout << "No encrypted files found.\n";
        return true;
    }

    std::cout << "Found " << encryptedFiles.size() << " encrypted files.\n";

    SimpleXORCipher cipher(key);
    int successCount = 0;

    for (size_t i = 0; i < encryptedFiles.size(); i++) {
        std::string originalFile = encryptedFiles[i].substr(0,
            encryptedFiles[i].find_last_of(".encrypted"));

        std::cout << "Decrypting: " << encryptedFiles[i] << " ... ";

        if (cipher.decryptFile(encryptedFiles[i], originalFile)) {
            if (FileOperations::deleteFile(encryptedFiles[i])) {
                std::cout << "OK\n";
                successCount++;
            }
            else {
                std::cout << "Failed to delete encrypted file\n";
            }
        }
        else {
            std::cout << "Decryption failed\n";
        }
    }

    std::string protectedDir = drivePath + "\\Protected";
    /*  elder C++17
        if (FileOperations::fileExists(protectedDir)) {
            std::vector<std::string> remaining = FileOperations::collectFiles(protectedDir);
            if (remaining.empty()) {
    #ifdef _WIN32
                //_rmdir(protectedDir.c_str());
                RemoveDirectoryA(protectedDir.c_str());
    #endif
    */
    if (fs::exists(protectedDir) && fs::is_empty(protectedDir)) {
        fs::remove(protectedDir);
    }

    std::cout << "\nUnprotection completed.\n";
    std::cout << "Decrypted " << successCount << " of " << encryptedFiles.size() << " files.\n";

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        showHelp();
        return 1;
    }

    std::string command = argv[1];

    if (command == "help") {
        showHelp();
    }
    else if (command == "scan") {
        std::cout << "Detected USB drives:\n";
        std::cout << "--------------------\n";

        std::vector<std::string> drives = USBDetector::getUSBDrives();

        if (drives.empty()) {
            std::cout << "No USB drives detected.\n";
        }
        else {
            for (size_t i = 0; i < drives.size(); i++) {
                std::cout << i + 1 << ". " << drives[i] << std::endl;
            }
        }
    }
    else if (command == "list") {
        if (argc < 3) {
            std::cerr << "Error: Please specify path\n";
            return 1;
        }
        FileOperations::listFiles(argv[2], false);
    }
    else if (command == "info") {
        if (argc < 3) {
            std::cerr << "Error: Please specify path\n";
            return 1;
        }
        std::string info = USBDetector::getDriveInfo(argv[2]);
        std::cout << info << std::endl;
    }
    else if (command == "protect") {
        if (argc < 3) {
            std::cerr << "Error: Please specify USB drive path\n";
            return 1;
        }

        std::string key = Utils::getPassword("Enter encryption key: ");
        if (key.empty()) {
            std::cerr << "Key cannot be empty.\n";
            return 1;
        }

        protectUSBDrive(argv[2], key);
    }
    else if (command == "unprotect") {
        if (argc < 3) {
            std::cerr << "Error: Please specify USB drive path\n";
            return 1;
        }

        std::string key = Utils::getPassword("Enter decryption key: ");
        if (key.empty()) {
            std::cerr << "Key cannot be empty.\n";
            return 1;
        }

        unprotectUSBDrive(argv[2], key);
    }
    else if (command == "encrypt") {
        if (argc < 3) {
            std::cerr << "Error: Please specify file path\n";
            return 1;
        }

        std::string key = Utils::getPassword("Enter encryption key: ");
        if (key.empty()) {
            std::cerr << "Key cannot be empty.\n";
            return 1;
        }

        std::string outputFile = std::string(argv[2]) + ".encrypted";
        SimpleXORCipher cipher(key);
        cipher.encryptFile(argv[2], outputFile);
    }
    else if (command == "decrypt") {
        if (argc < 3) {
            std::cerr << "Error: Please specify file path\n";
            return 1;
        }

        std::string key = Utils::getPassword("Enter decryption key: ");
        if (key.empty()) {
            std::cerr << "Key cannot be empty.\n";
            return 1;
        }

        std::string inputFile = argv[2];
        std::string outputFile = inputFile;

        size_t pos = inputFile.find(".encrypted");
        if (pos != std::string::npos) {
            outputFile = inputFile.substr(0, pos);
        }
        else {
            outputFile = inputFile + ".decrypted";
        }

        SimpleXORCipher cipher(key);
        cipher.decryptFile(inputFile, outputFile);
    }
    else {
        std::cerr << "Unknown command: " << command << std::endl;
        showHelp();
        return 1;
    }

    return 0;
}
