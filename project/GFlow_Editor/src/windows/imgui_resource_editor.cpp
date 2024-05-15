#include "imgui_resource_editor.hpp"

#include "editor.hpp"
#include "imgui.h"
#include "project.hpp"
#include "resource.hpp"

ImGuiResourceEditorWindow::ImGuiResourceEditorWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

void ImGuiResourceEditorWindow::resourceSelected(const std::string_view resource)
{
    if (m_name.empty())
    {
        m_selectedResource = nullptr;
        return;
    }

    m_selectedResource = &Editor::getProject().getResource(resource.data());
}

void ImGuiResourceEditorWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    if (m_selectedResource)
    {
        drawResource(m_selectedResource->getType(), m_selectedResource, false);
    }
    ImGui::End();
}

void ImGuiResourceEditorWindow::drawFloat(const std::string& name, void* data) const
{
    ImGui::InputFloat(name.c_str(), static_cast<float*>(data), 0.1f, 1.0f);
}

void ImGuiResourceEditorWindow::drawInt(const std::string& name, void* data) const
{
    ImGui::InputInt(name.c_str(), static_cast<int*>(data), 1, 10);
}

void ImGuiResourceEditorWindow::drawString(const std::string& name, void* data) const
{
    std::string* str = static_cast<std::string*>(data);
    char buff[256] = "";
    if (!str->empty()) strcpy_s(buff, str->c_str());
    ImGui::InputText(name.c_str(), buff, 256);
    str->assign(buff);
}

void ImGuiResourceEditorWindow::drawBool(const std::string& name, void* data) const
{
    ImGui::Checkbox(name.c_str(), static_cast<bool*>(data));
}

void ImGuiResourceEditorWindow::drawResource(const std::string& name, void* data, const bool openTree) const
{
    bool isTreeOpen = true;
    if (openTree)
        isTreeOpen = ImGui::TreeNode(name.c_str());

    if (isTreeOpen)
    {
        bool isHeaderOpen = true;
        const gflow::parser::Resource* resource = static_cast<gflow::parser::Resource*>(data);
        for (const gflow::parser::Resource::ExportData& exportElem : resource->getExports())
        {
            if (exportElem.data == nullptr)
                isHeaderOpen = ImGui::CollapsingHeader(exportElem.name.data());

            if (!isHeaderOpen) continue;
            switch (exportElem.type)
            {
            case gflow::parser::DataType::STRING:
                drawString(exportElem.name, exportElem.data);
                break;
            case gflow::parser::DataType::INT:
                drawInt(exportElem.name, exportElem.data);
                break;
            case gflow::parser::DataType::FLOAT:
                drawFloat(exportElem.name, exportElem.data);
                break;
            case gflow::parser::DataType::BOOL:
                drawBool(exportElem.name, exportElem.data);
                break;
            case gflow::parser::DataType::ENUM:
                drawEnum(exportElem.name, exportElem.data, exportElem.enumContext);
                break;
            case gflow::parser::DataType::RESOURCE:
                drawResource(exportElem.name, exportElem.data);
                break;
            }
        }
        if (openTree)
            ImGui::TreePop();
    }
}

void ImGuiResourceEditorWindow::drawEnum(const std::string& name, void* data, const gflow::parser::EnumContext* context) const
{
    uint32_t currentSelection = *static_cast<uint32_t*>(data);
    if (ImGui::BeginCombo(name.c_str(), context->names[currentSelection], 0))
    {
        for (uint32_t n = 0; n < context->names.size(); n++)
        {
            const bool isSelected = (currentSelection == n);
            if (ImGui::Selectable(context->names[n], isSelected))
                currentSelection = n;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    *static_cast<uint32_t*>(data) = currentSelection;
}

template <typename T>
void ImGuiResourceEditorWindow::drawListHeader(const std::string& name, std::vector<T>* data) const
{
    ImGui::Text(name.c_str());
    int size = static_cast<int>(data->size());
    ImGui::InputInt(("Size##List-" + name).c_str(), &size);
    if (size < 0) size = 0;
    if (size != data->size())
        data->resize(size);
    if (size == 0) return;
    ImGui::Separator();
}
