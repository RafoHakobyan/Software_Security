#include <iostream>
#include <string>
#include <vector>

class SimpleRLE {
public:

    // Encode: store result in array (vector of pairs)
    std::vector<std::pair<char, int>> encode(const std::string& input) {
        std::vector<std::pair<char, int>> encoded;

        if (input.empty()) return encoded;

        size_t i = 0;

        while (i < input.length()) {
            char current = input[i];
            int count = 1;

            // count repetitions
            while (i + count < input.length() && input[i + count] == current) {
                count++;
            }

            encoded.push_back({current, count});

            i += count;
        }

        return encoded;
    }

    // Decode from array
    std::string decode(const std::vector<std::pair<char, int>>& encoded) {
        std::string output;

        for (const auto& element : encoded) {
            char c = element.first;
            int count = element.second;

            for (int i = 0; i < count; i++) {
                output += c;
            }
        }

        return output;
    }
};

int main() {
    SimpleRLE rle;
    std::string input;

    std::cout << "Enter a string to encode: ";
    std::getline(std::cin, input);

    // Encode
    auto encoded = rle.encode(input);

    std::cout << "Encoded array: ";
    for (const auto& e : encoded) {
        std::cout << "[" << e.first << "," << e.second << "] ";
    }
    std::cout << std::endl;

    // Decode
    std::string decoded = rle.decode(encoded);
    std::cout << "Decoded: " << decoded << std::endl;

    if (input == decoded)
        std::cout << "Success: Decoded string matches original!\n";
    else
        std::cout << "Error: Decoded string does not match original!\n";

    return 0;
}
