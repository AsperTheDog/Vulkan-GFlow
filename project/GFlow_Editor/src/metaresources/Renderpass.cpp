#include "Renderpass.hpp"

#include "windows/nodes/base_node.hpp"

void RenderpassResource::removeNode(GFlowNode* node)
{
    (*nodes).erase(node->getLinkedResource());
    for (int i = 0; i < (*connections).size(); i++)
    {
        if ((*connections)[i]->getFirst()->getFirst() == node->getUID() || (*connections)[i]->getSecond()->getFirst() == node->getUID())
        {
            (*connections).remove(i);
            i--;
        }
    }
}

void RenderpassResource::addConnection(const size_t left_uid, const size_t left_pin, const size_t right_uid, const size_t right_pin)
{
    //Check if it exists
    for (int i = 0; i < (*connections).size(); i++)
    {
        if ((*connections)[i]->getFirst()->getFirst() == left_uid && (*connections)[i]->getFirst()->getSecond() == left_pin &&
            (*connections)[i]->getSecond()->getFirst() == right_uid && (*connections)[i]->getSecond()->getSecond() == right_pin)
        {
            return;
        }
    }
    Connection** connection = (*connections).emplace_back(true);
    (*connection)->getFirst()->setValues(left_uid, left_pin);
    (*connection)->getSecond()->setValues(right_uid, static_cast<int>(right_pin));
}

