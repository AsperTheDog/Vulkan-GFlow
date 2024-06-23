#include "editor.hpp"


int main(const int argc, char* argv[])
{
    if (argc > 1)
    {
        Editor::init(argv[1]);
    }
    else
    {
        Editor::init();
    }
    Editor::run();
    
    Editor::cleanup();
    return 0;
}
