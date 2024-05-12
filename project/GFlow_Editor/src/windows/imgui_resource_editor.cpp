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

            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 5.0f);
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
            case gflow::parser::DataType::REF:
                drawRef(exportElem.name, exportElem.data);
                break;
            case gflow::parser::DataType::ENUM:
                drawEnum(exportElem.name, exportElem.data, exportElem.enumContext);
                break;
            case gflow::parser::DataType::LIST_STRING:
            {
                std::vector<std::string>* list = static_cast<std::vector<std::string>*>(exportElem.data);
                drawListHeader(exportElem.name, list);
                for (uint32_t i = 0; i < list->size(); ++i)
                    drawString(std::to_string(i) + "##List-" + exportElem.name, &list->at(i));
            }
            break;
            case gflow::parser::DataType::LIST_INT:
            {
                std::vector<int>* list = static_cast<std::vector<int>*>(exportElem.data);
                drawListHeader(exportElem.name, list);
                for (uint32_t i = 0; i < list->size(); ++i)
                    drawInt(std::to_string(i) + "##List-" + exportElem.name, &list->at(i));
            }
            break;
            case gflow::parser::DataType::LIST_FLOAT:
            {
                std::vector<float>* list = static_cast<std::vector<float>*>(exportElem.data);
                drawListHeader(exportElem.name, list);
                for (uint32_t i = 0; i < list->size(); ++i)
                    drawFloat(std::to_string(i) + "##List-" + exportElem.name, &list->at(i));
            }
            break;
            case gflow::parser::DataType::LIST_BOOL:
            {
                std::vector<bool>* list = static_cast<std::vector<bool>*>(exportElem.data);
                drawListHeader(exportElem.name, list);
                for (uint32_t i = 0; i < list->size(); ++i)
                {
                    bool value = list->at(i);
                    drawBool(std::to_string(i) + "##List-" + exportElem.name, &value);
                    list->at(i) = value;
                }
            }
            break;
            case gflow::parser::DataType::LIST_REF:
            {
                std::vector<gflow::parser::Resource::Ref>* list = static_cast<std::vector<gflow::parser::Resource::Ref>*>(exportElem.data);
                drawListHeader(exportElem.name, list);
                for (uint32_t i = 0; i < list->size(); ++i)
                    drawString(std::to_string(i) + "##List-" + exportElem.name, &list->at(i).path);
            }
            break;
            case gflow::parser::DataType::LIST_ENUM:
            {
                std::vector<gflow::parser::EnumExport>* list = static_cast<std::vector<gflow::parser::EnumExport>*>(exportElem.data);
                drawListHeader(exportElem.name, list);
                for (uint32_t i = 0; i < list->size(); ++i)
                    drawEnum(std::to_string(i) + "##List-" + exportElem.name, &list->at(i), exportElem.enumContext);
            }
            }
        }
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

void ImGuiResourceEditorWindow::drawRef(const std::string& name, void* data) const
{
    if (ImGui::Button(name.c_str()))
    {
        //TODO: Select resource
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
