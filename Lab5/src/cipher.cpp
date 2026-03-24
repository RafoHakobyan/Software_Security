#include "cipher.h"
#include <fstream>
#include <iostream>
#include <vector>

SimpleXORCipher::SimpleXORCipher(const std::string& encryptionKey) : key(encryptionKey) {}

std::vector<char> SimpleXORCipher::encrypt(const std::vector<char>& data) {
    std::vector<char> result = data;
    size_t keyLen = key.length();
    if (keyLen == 0) return result;

    for (size_t i = 0; i < result.size(); i++) {
        result[i] ^= key[i % keyLen];
    }
    return result;
}

std::vector<char> SimpleXORCipher::decrypt(const std::vector<char>& data) {
    return encrypt(data);
}

bool SimpleXORCipher::encryptFile(const std::string& inputPath, const std::string& outputPath) {
    try {

        std::ifstream inputFile(inputPath.c_str(), std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error: Cannot open file: " << inputPath << std::endl;
            return false;
        }

        inputFile.seekg(0, std::ios::end);
        size_t fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        std::vector<char> buffer(fileSize);
        inputFile.read(buffer.data(), fileSize);
        inputFile.close();

        std::vector<char> encrypted = encrypt(buffer);

        std::ofstream outputFile(outputPath.c_str(), std::ios::binary);
        if (!outputFile) {
            std::cerr << "Error: Cannot create output file: " << outputPath << std::endl;
            return false;
        }

        outputFile.write(encrypted.data(), encrypted.size());
        outputFile.close();

        std::cout << "File encrypted successfully: " << outputPath << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error during encryption: " << e.what() << std::endl;
        return false;
    }
}

bool SimpleXORCipher::decryptFile(const std::string& inputPath, const std::string& outputPath) {
    try {
        std::ifstream inputFile(inputPath.c_str(), std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error: Cannot open file: " << inputPath << std::endl;
            return false;
        }

        inputFile.seekg(0, std::ios::end);
        size_t fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        std::vector<char> buffer(fileSize);
        inputFile.read(buffer.data(), fileSize);
        inputFile.close();

        std::vector<char> decrypted = decrypt(buffer);

        std::ofstream outputFile(outputPath.c_str(), std::ios::binary);
        if (!outputFile) {
            std::cerr << "Error: Cannot create output file: " << outputPath << std::endl;
            return false;
        }

        outputFile.write(decrypted.data(), decrypted.size());
        outputFile.close();

        std::cout << "File decrypted successfully: " << outputPath << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error during decryption: " << e.what() << std::endl;
        return false;
    }
}