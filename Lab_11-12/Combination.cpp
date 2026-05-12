#include <windows.h>
#include <iostream>
#include <vector>
#include <cstdint>

// =========================================================
// SIMPLE ANTI-DEBUGGING
// =========================================================

bool anti_debug()
{
    if (IsDebuggerPresent())
        return true;

    BOOL remoteDebugger = FALSE;

    CheckRemoteDebuggerPresent(GetCurrentProcess(), &remoteDebugger);

    return remoteDebugger;
}

// =========================================================
// SIMPLE ANTI-VM
// =========================================================

bool anti_vm()
{
    if (GetModuleHandleA("VBoxHook.dll"))
        return true;

    if (GetModuleHandleA("vmGuestLib.dll"))
        return true;

    if (GetModuleHandleA("vm3dgl.dll"))
        return true;

    return false;
}

// =========================================================
// CUSTOM VM
// =========================================================

enum OpCode : uint8_t
{
    OP_PUSH = 0x10,
    OP_ADD  = 0x20,
    OP_PRINT = 0x30,
    OP_HALT = 0xFF
};

class TinyVM
{
private:
    std::vector<int> stack;

public:
    void execute(const std::vector<uint8_t>& code)
    {
        size_t ip = 0;

        while (ip < code.size())
        {
            uint8_t opcode = code[ip++];

            switch (opcode)
            {
            case OP_PUSH:
            {
                int value = code[ip++];
                stack.push_back(value);
                break;
            }

            case OP_ADD:
            {
                int b = stack.back();
                stack.pop_back();

                int a = stack.back();
                stack.pop_back();

                stack.push_back(a + b);
                break;
            }

            case OP_PRINT:
            {
                std::cout << "VM Result: " << stack.back() << '
';
                break;
            }

            case OP_HALT:
                return;

            default:
                std::cout << "Invalid opcode
";
                return;
            }
        }
    }
};

// =========================================================
// VERY SIMPLE BYTECODE "ENCRYPTION"
// =========================================================

void xor_crypt(std::vector<uint8_t>& data, uint8_t key)
{
    for (auto& b : data)
    {
        b ^= key;
    }
}

// =========================================================
// MAIN
// =========================================================

int main()
{
    // -----------------------------------------------------
    // Anti-analysis checks
    // -----------------------------------------------------

    if (anti_debug())
    {
        std::cout << "Debugger detected
";
        return 1;
    }

    if (anti_vm())
    {
        std::cout << "Virtual machine detected
";
        return 1;
    }

    // -----------------------------------------------------
    // Encrypted bytecode
    // PUSH 10
    // PUSH 20
    // ADD
    // PRINT
    // HALT
    // -----------------------------------------------------

    std::vector<uint8_t> encryptedCode =
    {
        0x10 ^ 0xAA,
        10   ^ 0xAA,

        0x10 ^ 0xAA,
        20   ^ 0xAA,

        0x20 ^ 0xAA,
        0x30 ^ 0xAA,
        0xFF ^ 0xAA
    };

    // -----------------------------------------------------
    // Decrypt before execution
    // -----------------------------------------------------

    xor_crypt(encryptedCode, 0xAA);

    // -----------------------------------------------------
    // Execute virtualized logic
    // -----------------------------------------------------

    TinyVM vm;
    vm.execute(encryptedCode);

    return 0;
}

