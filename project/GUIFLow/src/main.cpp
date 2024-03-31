#include <config_module.hpp>

#include "internal/serialized_list.hpp"

int main(int argc, char *argv[])
{
    gflow::SerializedList<uint32_t> list = gflow::SerializedList<uint32_t>::load("files/list.lst");
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.serialize("files/list.lst");
    return 0;
}
