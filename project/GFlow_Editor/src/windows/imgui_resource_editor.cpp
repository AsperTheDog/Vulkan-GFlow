#include "imgui_resource_editor.hpp"

#include "editor.hpp"
#include "imgui.h"
#include "resource.hpp"
#include "resource_manager.hpp"

#define LEFT_ALIGN_ITEM (-13.f)


ImGuiResourceEditorWindow::ImGuiResourceEditorWindow(const std::string_view& name, const bool defaultOpen) : ImGuiEditorWindow(name, defaultOpen)
{
}

void ImGuiResourceEditorWindow::resourceSelected(const std::string& resource)
{
    if (!gflow::parser::ResourceManager::hasResource(resource))
    {
        m_selectedResource = nullptr;
        return;
    }

    m_selectedResource = gflow::parser::ResourceManager::getResource(resource);
    m_nestedResourcesOpened.clear();
}

void ImGuiResourceEditorWindow::resourceSelected(gflow::parser::Resource* resource)
{
    m_selectedResource = resource;
    m_nestedResourcesOpened.clear();
}

void ImGuiResourceEditorWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    if (m_selectedResource)
    {
        drawResource(m_selectedResource->getType(), &m_selectedResource, {});
    }
    ImGui::End();
}

gflow::parser::Resource* ImGuiResourceEditorWindow::getSelectedResource() const
{
    return m_selectedResource;
}

void ImGuiResourceEditorWindow::updateChangedVar(const gflow::parser::Resource* resource, const std::string& name, const bool commit)
{
    for (gflow::parser::ResourceElemPath& elem : m_variablesFlaggedToChange)
    {
        if (elem.pointsToResource(resource, name))
        {
            if (commit) 
                m_variableChangedSignal.emit(elem);
            std::erase(m_variablesFlaggedToChange, elem);
            return;
        }
    }
}

bool ImGuiResourceEditorWindow::drawFloat(const std::string& name, void* data) const
{
    float* value = static_cast<float*>(data);
    float tmp = *value;
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    ImGui::PushItemWidth(LEFT_ALIGN_ITEM);
    ImGui::InputFloat(("##" + name).c_str(), &tmp, 0.1f, 1.0f);
    ImGui::PopItemWidth();
    ImGui::Spacing();
    const bool changed = tmp != *value;
    *value = tmp;
    return changed;
}

bool ImGuiResourceEditorWindow::drawInt(const std::string& name, void* data) const
{
    int* value = static_cast<int*>(data);
    int tmp = *value;
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    ImGui::PushItemWidth(LEFT_ALIGN_ITEM);
    ImGui::InputInt(("##" + name).c_str(), &tmp, 1, 10);
    ImGui::PopItemWidth();
    ImGui::Spacing();
    const bool changed = tmp != *value;
    *value = tmp;
    return changed;
}

bool ImGuiResourceEditorWindow::drawBigInt(const std::string& name, void* data) const
{
    int64_t* value = static_cast<int64_t*>(data);
    int64_t tmp = *value;
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    ImGui::PushItemWidth(LEFT_ALIGN_ITEM);
    constexpr size_t step = 1;
    constexpr size_t step_fast = 10;
    ImGui::InputScalar(("##" + name).c_str(), ImGuiDataType_::ImGuiDataType_U64, &tmp, &step, &step_fast);
    ImGui::PopItemWidth();
    ImGui::Spacing();
    const bool changed = tmp != *value;
    *value = tmp;
    return changed;

}

bool ImGuiResourceEditorWindow::drawString(const std::string& name, void* data, const bool isShort) const
{
    std::string* str = static_cast<std::string*>(data);
    char buff[256] = "";
    if (!str->empty()) strcpy_s(buff, str->c_str());
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    ImGui::PushItemWidth(isShort ? -80.f : LEFT_ALIGN_ITEM);
    ImGui::InputText(("##" + name).c_str(), buff, 256);
    if (!isShort) ImGui::Spacing();
    ImGui::PopItemWidth();
    const bool changed = *str != buff;
    str->assign(buff);
    return changed;
}

bool ImGuiResourceEditorWindow::drawBool(const std::string& name, void* data) const
{
    bool* value = static_cast<bool*>(data);
    bool tmp = *value;
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    ImGui::PushItemWidth(LEFT_ALIGN_ITEM);
    ImGui::Checkbox(("##" + name).c_str(), &tmp);
    ImGui::PopItemWidth();
    ImGui::Spacing();
    const bool changed = tmp != *value;
    *value = tmp;
    return changed;
}

bool ImGuiResourceEditorWindow::drawVec2(const std::string& name, void* data) const
{
    gflow::parser::Vec2* value = static_cast<gflow::parser::Vec2*>(data);
    gflow::parser::Vec2 tmp = *value;
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    ImGui::PushItemWidth(LEFT_ALIGN_ITEM);
    ImGui::InputFloat2(("##" + name).c_str(), reinterpret_cast<float*>(&tmp));
    ImGui::PopItemWidth();
    ImGui::Spacing();
    const bool changed = tmp != *value;
    *value = tmp;
    return changed;
}

bool ImGuiResourceEditorWindow::drawVec3(const std::string& name, void* data) const
{
    gflow::parser::Vec3* value = static_cast<gflow::parser::Vec3*>(data);
    gflow::parser::Vec3 tmp = *value;
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    ImGui::PushItemWidth(LEFT_ALIGN_ITEM);
    ImGui::InputFloat3(("##" + name).c_str(), reinterpret_cast<float*>(&tmp));
    ImGui::PopItemWidth();
    ImGui::Spacing();
    const bool changed = tmp != *value;
    *value = tmp;
    return changed;
}

bool ImGuiResourceEditorWindow::drawVec4(const std::string& name, void* data) const
{
    gflow::parser::Vec4* value = static_cast<gflow::parser::Vec4*>(data);
    gflow::parser::Vec4 tmp = *value;
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    ImGui::PushItemWidth(LEFT_ALIGN_ITEM);
    ImGui::InputFloat4(("##" + name).c_str(), reinterpret_cast<float*>(&tmp));
    ImGui::PopItemWidth();
    ImGui::Spacing();
    const bool changed = tmp != *value;
    *value = tmp;
    return changed;
}

bool ImGuiResourceEditorWindow::drawFile(const std::string& name, void* data) const
{
    gflow::parser::FilePath* str = static_cast<gflow::parser::FilePath*>(data);
    drawString(name, &str->path, true);
    ImGui::SameLine(0, 10);
    return ImGui::Button("refresh");
}

void ImGuiResourceEditorWindow::drawResource(const std::string& stackedName, void* data, const std::vector<gflow::parser::Resource*>& parentPath)
{
    bool isHeaderOpen = true;
    gflow::parser::Resource** resource = static_cast<gflow::parser::Resource**>(data);
    if (*resource == nullptr) return;
    for (gflow::parser::Resource::ExportData& exportElem : (*resource)->getExports())
    {
        const gflow::parser::DataUsage usage = (*resource)->isUsed(exportElem.name, parentPath);
        if (usage == gflow::parser::NOT_USED) continue;

        if (exportElem.data == nullptr)
        {
            isHeaderOpen = ImGui::CollapsingHeader(exportElem.name.data());
            ImGui::Spacing();
            continue;
        }

        if (!isHeaderOpen) continue;
        bool changed = false;
        ImGui::BeginDisabled(usage == gflow::parser::READ_ONLY);
        switch (exportElem.type)
        {
        case gflow::parser::DataType::STRING:
            changed = drawString(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::INT:
            changed = drawInt(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::BIGINT:
            changed = drawBigInt(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::FLOAT:
            changed = drawFloat(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::BOOL:
            changed = drawBool(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::VEC2:
            changed = drawVec2(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::VEC3:
            changed = drawVec3(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::VEC4:
            changed = drawVec4(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::FILE:
            changed = drawFile(exportElem.name, exportElem.data);
            break;
        case gflow::parser::DataType::ENUM:
            changed = drawEnum(exportElem.name, exportElem.data, exportElem.enumContext);
            break;
        case gflow::parser::DataType::ENUM_BITMASK:
            changed = drawBitmask(exportElem.name, exportElem.data, exportElem.enumContext);
            break;
        case gflow::parser::DataType::RESOURCE:
            {
                std::vector<gflow::parser::Resource*> resourcePath = parentPath;
                resourcePath.push_back(*resource);
                drawSubresource(exportElem.name, stackedName, exportElem, resourcePath);
                break;
            }
        }
        ImGui::EndDisabled();
        if (changed)
        {
            m_variableChangedSignal.emit({m_selectedResource->getPath(), m_selectedResource, exportElem.name, stackedName});
            (*resource)->exportChanged(exportElem.name);
        }
    }
}

void ImGuiResourceEditorWindow::drawSubresource(const std::string& name, std::string stackedName, gflow::parser::Resource::ExportData& data, const std::vector<gflow::parser::Resource*>& parentPath)
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
    ImGui::SameLine(m_inlinePadding);
    if (ImGui::Button((subResource + "##" + stackedName).c_str()) && *resource != nullptr)
    {
        m_nestedResourcesOpened[stackedName] = !m_nestedResourcesOpened[stackedName];
        if (childBegan) ImGui::EndChild();
        return;
    }

    if (ImGui::BeginPopupContextItem())
    {
        bool shouldReturn = false;
        if (m_allowEmbedded)
        {
            if (ImGui::MenuItem("Create embedded"))
            {
                *resource = gflow::parser::ResourceManager::createResource("", data.resourceFactory, &data);
                m_nestedResourcesOpened[stackedName] = true;
                shouldReturn = true;
                m_variableChangedSignal.emit({m_selectedResource->getPath(), parentPath.back(), name, stackedName});
            }
        }
        
        ImGui::BeginDisabled(gflow::parser::ResourceManager::isTypeSubresource(data.getType()));
        if (ImGui::MenuItem("Load"))
        {
            m_variablesFlaggedToChange.emplace_back(m_selectedResource->getPath(), parentPath.back(), name, stackedName);
            Editor::showResourcePickerModal(this, parentPath.back(), name, data.getType());
        }
        ImGui::EndDisabled();
        if (ImGui::MenuItem("Clear"))
        {
            if (*resource != nullptr && (*resource)->isSubresource())
            {
                gflow::parser::ResourceManager::deleteResource(*resource);
            }
            *resource = nullptr;
            m_variableChangedSignal.emit({ m_selectedResource->getPath(), parentPath.back(), name, stackedName });
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
        drawResource(stackedName, data.data, parentPath);
        ImGui::EndChild();
    }
    ImGui::Spacing();
}

bool ImGuiResourceEditorWindow::drawEnum(const std::string& name, void* data, const gflow::parser::EnumContext* context) const
{
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
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
    ImGui::Spacing();
    const bool changed = *static_cast<uint32_t*>(data) != currentSelection;
    *static_cast<uint32_t*>(data) = currentSelection;
    return changed;
}

bool ImGuiResourceEditorWindow::drawBitmask(const std::string& name, void* data, const gflow::parser::EnumContext* context) const
{
    ImGui::Text(name.c_str());
    ImGui::SameLine(m_inlinePadding);
    uint32_t currentMask = *static_cast<uint32_t*>(data);
    if (ImGui::BeginCombo(("##" + name).c_str(), "Bitmask...", 0))
    {
        for (uint32_t n = 0; n < context->names.size(); n++)
        {
            bool isSelected = currentMask & context->values[n];
            ImGui::MenuItem(context->names[n], "", &isSelected);
            currentMask = isSelected ? currentMask | context->values[n] : currentMask & ~context->values[n];
        }
        ImGui::EndCombo();
    }
    ImGui::Spacing();
    const bool changed = *static_cast<uint32_t*>(data) != currentMask;
    *static_cast<uint32_t*>(data) = currentMask;
    return changed;
}
