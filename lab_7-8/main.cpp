#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <iterator>

namespace fs = std::filesystem;
using namespace std;

// --- 1. RLE Սեղմման ալգորիթմ ---
string rle_compress(const string& data) {
    string compressed = "";
    for (size_t i = 0; i < data.length(); i++) {
        int count = 1;
        while (i < data.length() - 1 && data[i] == data[i + 1] && count < 9) {
            count++;
            i++;
        }
        compressed += data[i];
        compressed += to_string(count); 
    }
    return compressed;
}

// --- 2. RLE Ապասեղմման ալգորիթմ ---
string rle_decompress(const string& data) {
    string decompressed = "";
    for (size_t i = 0; i < data.length(); i += 2) {
        if (i + 1 >= data.length()) break; 
        char ch = data[i];
        int count = data[i + 1] - '0';
        decompressed.append(count, ch);
    }
    return decompressed;
}

// --- 3. XOR Գաղտնագրում ---
string xor_cipher(const string& data, const string& key) {
    string output = data;
    if (key.empty()) return data;
    for (size_t i = 0; i < data.length(); i++) {
        output[i] = data[i] ^ key[i % key.length()];
    }
    return output;
}

// --- 4. Ֆայլի մշակման հիմնական ֆունկցիա ---
void process_file(const string& input_path, const string& key, bool protect) {
    if (protect) {
        ifstream fin(input_path, ios::binary);
        if (!fin) return;
        string content((istreambuf_iterator<char>(fin)), (istreambuf_iterator<char>()));
        fin.close();

        string compressed = rle_compress(content);
        string encrypted = xor_cipher(compressed, key);

        ofstream fout(input_path + ".protected", ios::binary);
        fout << encrypted;
        fout.close();
        
        fs::remove(input_path); 
        cout << "Protected: " << input_path << " -> .protected" << endl;
    } else {
        ifstream fin(input_path, ios::binary);
        if (!fin) return;
        string content((istreambuf_iterator<char>(fin)), (istreambuf_iterator<char>()));
        fin.close();

        string decrypted = xor_cipher(content, key);
        string original = rle_decompress(decrypted);

        string out_path = input_path.substr(0, input_path.find(".protected"));
        ofstream fout(out_path, ios::binary);
        fout << original;
        fout.close();

        fs::remove(input_path);
        cout << "Restored: " << out_path << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: ./protectusb [protect/unprotect] [path]" << endl;
        return 1;
    }

    string cmd = argv[1];
    string root = argv[2];
    string key;

    cout << "Enter key: ";
    cin >> key;

    try {
        if (!fs::exists(root)) {
            cerr << "Error: Path does not exist." << endl;
            return 1;
        }

        if (fs::is_regular_file(root)) {
            if (cmd == "protect" && fs::path(root).extension() != ".protected") {
                process_file(root, key, true);
            } else if (cmd == "unprotect" && fs::path(root).extension() == ".protected") {
                process_file(root, key, false);
            }
        } 
        else if (fs::is_directory(root)) {
            for (const auto& entry : fs::recursive_directory_iterator(root)) {
                if (entry.is_regular_file()) {
                    string path = entry.path().string();
                    if (cmd == "protect" && entry.path().extension() != ".protected") {
                        process_file(path, key, true);
                    } else if (cmd == "unprotect" && entry.path().extension() == ".protected") {
                        process_file(path, key, false);
                    }
                }
            }
        }
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
