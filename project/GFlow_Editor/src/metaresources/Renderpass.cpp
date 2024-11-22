#include "renderpass.hpp"

#include "windows/nodes/base_node.hpp"

void SubpassNodeResource::initContext(ExportData* metadata)
{
    if (colorAttachments.isNull())
        initializeExport(colorAttachments.getName());

    if (inputAttachments.isNull())
        initializeExport(inputAttachments.getName());
}

void SubpassNodeResource::clearAttachments()
{
    (*colorAttachments).clear();
    (*inputAttachments).clear();
    *depthAttachment = false;
}

void RenderpassResource::removeNode(GFlowNode* node)
{
    (*nodes).erase(node->getLinkedResource());
    for (int i = 0; i < (*connections).size(); i++)
    {
        if ((*connections)[i]->getLeftUID() == node->getUID() || (*connections)[i]->getRightUID() == node->getUID())
        {
            (*connections).remove(i);
            i--;
        }
    }
}

void RenderpassResource::addConnection(const size_t left_uid, const size_t left_pin, const size_t right_uid, const size_t right_pin)
{
    for (int i = 0; i < (*connections).size(); i++)
    {
        if ((*connections)[i]->getLeftUID() == left_uid && (*connections)[i]->getLeftPin() == left_pin &&
            (*connections)[i]->getRightUID() == right_uid && (*connections)[i]->getRightPin() == right_pin)
        {
            return;
        }
    }
    Connection** connection = (*connections).emplace_back();
    (*connection)->setValues(left_uid, left_pin, right_uid, right_pin);
}

