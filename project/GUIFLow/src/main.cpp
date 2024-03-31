

#include "project.hpp"

int main(int argc, char *argv[])
{
    const gflow::Project proj{};
    proj.serialize("files/test.proj");
    return 0;
}
