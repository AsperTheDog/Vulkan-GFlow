#include "imgui_resource_editor.hpp"

#include "editor.hpp"
#include "imgui.h"
#include "project.hpp"
#include "resource.hpp"
#include "resource_manager.hpp"

ImGuiResourceEditorWindow::ImGuiResourceEditorWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

void ImGuiResourceEditorWindow::resourceSelected(const std::string& resource)
{
    if (!gflow::parser::ResourceManager::hasResource(resource))
    {
        m_selectedResource = nullptr;
        return;
    }

    m_selectedResource = &gflow::parser::ResourceManager::getResource(resource);
    m_nestedResourcesOpened.clear();
}

void ImGuiResourceEditorWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    if (m_selectedResource)
    {
        drawResource(m_selectedResource->getType(), &m_selectedResource);
    }
    ImGui::End();
}

void ImGuiResourceEditorWindow::drawFloat(const std::string& name, void* data) const
{
    ImGui::Text(name.c_str());
    ImGui::SameLine(150);
    ImGui::InputFloat(("##" + name).c_str(), static_cast<float*>(data), 0.1f, 1.0f);
    ImGui::Spacing();
}

void ImGuiResourceEditorWindow::drawInt(const std::string& name, void* data) const
{
    ImGui::Text(name.c_str());
    ImGui::SameLine(150);
    ImGui::InputInt(("##" + name).c_str(), static_cast<int*>(data), 1, 10);
    ImGui::Spacing();
}

void ImGuiResourceEditorWindow::drawString(const std::string& name, void* data) const
{
    std::string* str = static_cast<std::string*>(data);
    char buff[256] = "";
    if (!str->empty()) strcpy_s(buff, str->c_str());
    ImGui::Text(name.c_str());
    ImGui::SameLine(150);
    ImGui::InputText(("##" + name).c_str(), buff, 256);
    str->assign(buff);
    ImGui::Spacing();
}

void ImGuiResourceEditorWindow::drawBool(const std::string& name, void* data) const
{
    ImGui::Text(name.c_str());
    ImGui::SameLine(150);
    ImGui::Checkbox(("##" + name).c_str(), static_cast<bool*>(data));
    ImGui::Spacing();
}

void ImGuiResourceEditorWindow::drawResource(const std::string& stackedName, void* data) const
{
    bool isHeaderOpen = true;
    gflow::parser::Resource** resource = static_cast<gflow::parser::Resource**>(data);
    if (*resource == nullptr) return;
    for (const gflow::parser::Resource::ExportData& exportElem : (*resource)->getExports())
    {
        if (exportElem.data == nullptr)
        {
	        isHeaderOpen = ImGui::CollapsingHeader(exportElem.name.data());
            ImGui::Spacing();
            continue;
        }

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
            drawSubresource(exportElem.name, stackedName, exportElem);
            break;
        }
    }
    (*resource)->exportsChanged();
}

void ImGuiResourceEditorWindow::drawSubresource(const std::string& name, std::string stackedName, const gflow::parser::Resource::ExportData& data) const
{
    stackedName += "." + name;

    gflow::parser::Resource** resource = static_cast<gflow::parser::Resource**>(data.data);
    std::string subResource = "...";
    if (*resource != nullptr)
    {
        subResource = (*resource)->getType();
    }

    if (!m_nestedResourcesOpened.contains(stackedName)) 
        m_nestedResourcesOpened.emplace(stackedName, false);
    
    bool childBegan = false;
    if (m_nestedResourcesOpened[stackedName])
    {
        ImGui::BeginChild((name + "##Child-" + stackedName).c_str(), ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);
        childBegan = true;
    }

    ImGui::Text(name.c_str());
    ImGui::SameLine(150);
    if (ImGui::Button((subResource + "##" + stackedName).c_str()) && *resource != nullptr)
    {
        m_nestedResourcesOpened[stackedName] = !m_nestedResourcesOpened[stackedName];
        if (childBegan) ImGui::EndChild();
        return;
    }

    if (ImGui::BeginPopupContextItem())
    {
        bool shouldReturn = false;
        if (ImGui::MenuItem("Create embedded"))
        {
            *resource = data.resourceFactory("");
            m_nestedResourcesOpened[stackedName] = true;
            shouldReturn = true;
        }
        if (ImGui::MenuItem("Load"))
        {

        }
        if (ImGui::MenuItem("Clear"))
        {
            *resource = nullptr;
            m_nestedResourcesOpened[stackedName] = false;
            shouldReturn = true;
        }
        ImGui::EndPopup();
        if (shouldReturn)
        {
            if (childBegan) ImGui::EndChild();
            return;
        }
    }

    if (m_nestedResourcesOpened[stackedName])
    {
        ImGui::Separator();
        ImGui::Indent();
        drawResource(stackedName, data.data);
        ImGui::EndChild();
    }
    ImGui::Spacing();
}

void ImGuiResourceEditorWindow::drawEnum(const std::string& name, void* data, const gflow::parser::EnumContext* context) const
{
    ImGui::Text(name.c_str());
    ImGui::SameLine(150);
    uint32_t currentSelection = *static_cast<uint32_t*>(data);
    if (ImGui::BeginCombo(("##" + name).c_str(), context->names[currentSelection], 0))
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
    ImGui::Spacing();
}
