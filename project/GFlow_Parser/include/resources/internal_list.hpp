#pragma once
#include "list.hpp"

#define EXPORT_INTERNAL_LIST(type, name) gflow::parser::Export<gflow::parser::InternalList<type>*> ##name{#name, this}
#define EXPORT_INTERNAL_ENUM_LIST(name, context) gflow::parser::Export<gflow::parser::InternalList<gflow::parser::EnumExport>*> ##name{#name, this, context}
#define EXPORT_INTERNAL_RESOURCE_LIST(type, name) gflow::parser::Export<gflow::parser::InternalList<type*>*> ##name{#name, this}

namespace gflow::parser
{
    template <typename T>
    class InternalList final :  public List<T>
    {
    public:
        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;
        
        DECLARE_RESOURCE_ANCESTOR(InternalList, List<T>)
    };

    template <typename T>
    DataUsage InternalList<T>::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "size")
            return DataUsage::READ_ONLY;
        return List<T>::isUsed(variable, parentPath);
    }
}


