#include <iostream>

#include "engine.hpp"

int main()
{
    try
    {
        Engine engine{};

        engine.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}
