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
        bool isHeaderOpen = true;
        for (const gflow::parser::Resource::ExportData& exportElem : m_selectedResource->getExports())
        {
            if (exportElem.data == nullptr)
                isHeaderOpen = ImGui::CollapsingHeader(exportElem.name.data());

            if (!isHeaderOpen) continue;

            switch (exportElem.type)
            {
            case gflow::parser::DataType::FLOAT:
                ImGui::InputFloat(exportElem.name.data(), static_cast<float*>(exportElem.data), 0.1f, 1.0f);
                break;
            case gflow::parser::DataType::INT:
                ImGui::InputInt(exportElem.name.data(), static_cast<int*>(exportElem.data), 1, 10);
                break;
            case gflow::parser::DataType::STRING:
                {
                    std::string* str = static_cast<std::string*>(exportElem.data);
                    char buff[256] = "";
                    if (!str->empty()) strcpy_s(buff, str->c_str());
                    ImGui::InputText(exportElem.name.data(), buff, 256);
                    str->assign(buff);
                }
                break;
            case gflow::parser::DataType::BOOL:
                ImGui::Checkbox(exportElem.name.data(), static_cast<bool*>(exportElem.data));
                break;
            case gflow::parser::DataType::REF:
                if (ImGui::Button(exportElem.name.data()))
                {
                    //TODO: Select resource
                }
                break;
            }
        }
    }
    ImGui::End();
}
