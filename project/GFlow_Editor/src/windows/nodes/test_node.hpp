#pragma once
#include "ImNodeFlow.h"


class TestNode final : public ImFlow::BaseNode
{
public:
    explicit TestNode()
    {
        std::function<bool(ImFlow::Pin*, ImFlow::Pin*)> filter = [](ImFlow::Pin* a, ImFlow::Pin* b)
        {
            return true;
        };
        setTitle("Test node");
        setStyle(ImFlow::NodeStyle::brown());
        addIN<int>("ENTER", 0, filter, ImFlow::PinStyle::white());
        addOUT<int>("EXIT", ImFlow::PinStyle::white())->behaviour([this](){ return 0; });
        addIN<int>("Pipeline", 0, filter, ImFlow::PinStyle::red());
        addIN<int>("RenderPass", 0, filter, ImFlow::PinStyle::blue());
        addIN<int>("Subpass", 0, filter, ImFlow::PinStyle::green());
    }

    void draw() override
    {
        ImGui::Text("I'm custom node");
        ImGui::Text("I'm custom node");
        ImGui::Text("I'm custom node");
        ImGui::Text("I'm custom node");
        ImGui::Text("I'm custom node");
        ImGui::Text("I'm custom node");
        ImGui::Text("I'm custom node");
        ImGui::Text("I'm custom node");
    }
};

