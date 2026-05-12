#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <vector>
#include <string>
using namespace std;

//Polymorphic Obfuscation
int add_v1(int a, int b)
{
    return a + b;
}

int add_v2(int a, int b)
{
    while (b != 0)
    {
        int carry = a & b;
        a = a ^ b;
        b = carry << 1;
    }

    return a;
}

int polymorphic_add(int a, int b)
{
    if (rand() % 2)
        return add_v1(a, b);

    return add_v2(a, b);
}

//Metamorphic Obfuscation 
//The original logic of function 
int simCompute(int x)
{
    return (x * 2) + 10;
}
//Another version
int obfCompute(int x)
{
    int y = x << 1;

    y += 5;
    y += 5;

    return y;
}

//Packer 
class SimplePacker
{
private:
    std::vector<unsigned char> packed;
    unsigned char key = 0x5A;

public:
    void pack(const std::string& text)
    {
        packed.clear();

        for (unsigned char c : text)
        {
            packed.push_back(c ^ key);
        }
    }

    std::string unpack()
    {
        std::string result;

        for (unsigned char c : packed)
        {
            result.push_back(c ^ key);
        }

        return result;
    }
};

//Crackme
bool check_password(const string& input)
{
    //This password exists plainly in the binary
    const string secret = "R3v3rs3";
    return input == secret;
}

int main(){
    srand(static_cast<unsigned>(time(nullptr)));
    //The program will randomly choose which version of add to use(the output is the same)
    cout<<"Polymorphic Obfuscation: " << polymorphic_add(5, 7) << endl;
    cout << "Metamorphic Obfuscation: " << endl;
    cout << "Simple Compute: " << simCompute(5) << " | Obfuscation Compute: " << obfCompute(5) << endl;
    //small logic of checking the present of Debugger 
    cout << (IsDebuggerPresent() ? "Debugger detected!\n" : "No debugger detected.\n");
    //Packer
    SimplePacker p;
    p.pack("Hidden payload");
    cout << p.unpack() << '\n';
    //Crackme
    string password;
    cout << "Enter password: ";
    getline(std::cin, password);
    cout << (check_password(password) ? "Access granted!\n" : "Access denied!\n");
    return 0;
}

