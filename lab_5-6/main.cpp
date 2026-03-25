#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

// XOR Գաղտնագրման ֆունկցիա
void xor_process(const string& input_path, const string& output_path, const string& key) {
    ifstream fin(input_path, ios::binary);
    ofstream fout(output_path, ios::binary);

    if (!fin.is_open() || !fout.is_open()) {
        cerr << "Սխալ: Չհաջողվեց բացել ֆայլը: " << input_path << endl;
        return;
    }

    size_t key_len = key.length();
    size_t key_idx = 0;
    char ch;

    while (fin.get(ch)) {
        fout.put(ch ^ key[key_idx % key_len]);
        key_idx++;
    }
    
    fin.close();
    fout.close();
}

// Ֆայլերի սկանավորում WSL /mnt թղթապանակում
void scan_usb() {
    cout << "Որոնվում են միացված սկավառակները /mnt թղթապանակում..." << endl;
    bool found = false;
    for (const auto& entry : fs::directory_iterator("/mnt")) {
        string drive = entry.path().filename().string();
        // Բացառում ենք 'wsl', 'wslg' և 'c' (հիմնական սկավառակը)
        if (drive != "c" && drive != "wsl" && drive != "wslg") {
            cout << "  - Հայտնաբերվել է կրիչ: /mnt/" << drive << endl;
            found = true;
        }
    }
    if (!found) cout << "Լրացուցիչ USB կրիչներ չեն գտնվել:" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Օգտագործում: ./protectusb [հրաման] [ուղի]" << endl;
        cout << "Հրամաններ: scan, list, protect, unprotect" << endl;
        return 1;
    }

    string command = argv[1];

    if (command == "scan") {
        scan_usb();
    } 
    else if (command == "list" && argc > 2) {
        string path = argv[2];
        if (!fs::exists(path)) { cout << "Ուղին գոյություն չունի:" << endl; return 1; }
        for (const auto& entry : fs::directory_iterator(path)) {
            cout << (entry.is_directory() ? "[DIR]  " : "[FILE] ") << entry.path().filename().string() << endl;
        }
    }
    else if (command == "protect" && argc > 2) {
        string root = argv[2];
        string key;
        cout << "Մուտքագրեք գաղտնագրման բանալին: "; cin >> key;

        for (const auto& entry : fs::recursive_directory_iterator(root)) {
            if (entry.is_regular_file() && entry.path().extension() != ".encrypted") {
                string p = entry.path().string();
                xor_process(p, p + ".encrypted", key);
                fs::remove(p);
                cout << "Պաշտպանված է: " << entry.path().filename() << endl;
            }
        }
    }
    else if (command == "unprotect" && argc > 2) {
        string root = argv[2];
        string key;
        cout << "Մուտքագրեք վերծանման բանալին: "; cin >> key;

        for (const auto& entry : fs::recursive_directory_iterator(root)) {
            if (entry.is_regular_file() && entry.path().extension() == ".encrypted") {
                string p = entry.path().string();
                string out = p.substr(0, p.find(".encrypted"));
                xor_process(p, out, key);
                fs::remove(p);
                cout << "Վերականգնված է: " << fs::path(out).filename() << endl;
            }
        }
    } else {
        cout << "Անհայտ հրաման կամ պակասող փաստարկներ:" << endl;
    }

    return 0;
}
