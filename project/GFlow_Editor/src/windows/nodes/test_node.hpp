#pragma once
#include "ImNodeFlow.h"


class TestNode final : public ImFlow::BaseNode
{
public:
    explicit TestNode()
    {
        setTitle("I'm custom");
        setStyle(ImFlow::NodeStyle::brown());
        addIN<int>("I'm input", 0, 0, ImFlow::PinStyle::red());
    }
};

