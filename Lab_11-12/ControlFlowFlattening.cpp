#include <iostream>

void protected_function(int x)
{
    int state = 0;

    while (true)
    {
        switch (state)
        {
        case 0:
            if (x > 5)
                state = 1;
            else
                state = 2;
            break;

        case 1:
            std::cout << "Greater than 5\n";
            state = 3;
            break;

        case 2:
            std::cout << "Less or equal to 5\n";
            state = 3;
            break;

        case 3:
            return;
        }
    }
}

int main()
{
    protected_function(10);
}
