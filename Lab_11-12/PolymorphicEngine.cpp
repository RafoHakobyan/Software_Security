#include <iostream>
#include <random>

int add_variant1(int a, int b)
{
    return a + b;
}

int add_variant2(int a, int b)
{
    int result = a;
    result += b;
    return result;
}

int add_variant3(int a, int b)
{
    return b + a;
}

using AddFunc = int(*)(int, int);

AddFunc select_variant()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 2);

    switch (dist(gen))
    {
    case 0:
        return add_variant1;

    case 1:
        return add_variant2;

    default:
        return add_variant3;
    }
}

int main()
{
    AddFunc fn = select_variant();

    std::cout << fn(5, 7) << '\n';
}
