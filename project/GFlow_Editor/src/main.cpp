#include "editor.hpp"

int main(int argc, char *argv[])
{
    Editor::init();
    Editor::run();

    Editor::cleanup();
    return 0;
}
