#include <iostream>
#include <vector>

enum OpCode
{
    PUSH,
    ADD,
    SUB,
    PRINT,
    HALT
};

struct Instruction
{
    OpCode op;
    int operand;
};

class VM
{
private:
    std::vector<int> stack;
    std::vector<Instruction> program;
    size_t ip = 0;

public:
    VM(const std::vector<Instruction>& p)
        : program(p)
    {
    }

    void run()
    {
        while (true)
        {
            Instruction ins = program[ip++];

            switch (ins.op)
            {
            case PUSH:
                stack.push_back(ins.operand);
                break;

            case ADD:
            {
                int b = stack.back();
                stack.pop_back();

                int a = stack.back();
                stack.pop_back();

                stack.push_back(a + b);
                break;
            }

            case SUB:
            {
                int b = stack.back();
                stack.pop_back();

                int a = stack.back();
                stack.pop_back();

                stack.push_back(a - b);
                break;
            }

            case PRINT:
                std::cout << "Result: " << stack.back() << '\n';
                break;

            case HALT:
                return;
            }
        }
    }
};

int main()
{
    std::vector<Instruction> bytecode =
    {
        { PUSH, 10 },
        { PUSH, 20 },
        { ADD, 0 },
        { PRINT, 0 },
        { HALT, 0 }
    };

    VM vm(bytecode);
    vm.run();
}
