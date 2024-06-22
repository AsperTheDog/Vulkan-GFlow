#pragma once
#include "ImNodeFlow.h"
#include "resource.hpp"

class GFlowNode : public ImFlow::BaseNode
{
public:
    GFlowNode(const std::string& name);

    virtual gflow::parser::Resource* getLinkedResource() = 0;
    void setInspectionStatus(bool status);
private:

};