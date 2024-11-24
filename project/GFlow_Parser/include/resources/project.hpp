#pragma once
#include "../resource_manager.hpp"

#include <functional>

#include "list.hpp"
#include "renderpass.hpp"

namespace gflow::parser
{
    class ProjectRenderpassDrawCall final : public Resource
    {
        EXPORT_RESOURCE(Pipeline, pipeline, false, true);

    public:
        DECLARE_PRIVATE_RESOURCE(ProjectRenderpassDrawCall);

        template <typename T>
        friend class List;
    };

    class ProjectRenderpassSubpass final : public Resource
    {
        EXPORT_RESOURCE_LIST(ProjectRenderpassDrawCall, drawcalls);

    public:
        DECLARE_PRIVATE_RESOURCE(ProjectRenderpassSubpass)

        template <typename T>
        friend class List;
    };

    class ProjectRenderpass final : public Resource
    {
    private:
        EXPORT_RESOURCE(RenderPass, renderpass, false, true);
        EXPORT_RESOURCE_LIST(ProjectRenderpassSubpass, subpasses);
        
    public:

        RenderPass* getRenderpass() { return *renderpass; }
        void setRenderpass(RenderPass* renderpass) { this->renderpass.setData(renderpass); }

        ProjectRenderpassSubpass* addSubpass() { return *(*subpasses).emplace_back(); }

        DECLARE_PRIVATE_RESOURCE(ProjectRenderpass)

        template <typename T>
        friend class List;
    };

    class Project final : public Resource
    {
        EXPORT(std::string, name);
        EXPORT_RESOURCE_LIST(ProjectRenderpass, renderpasses);

    public:
        [[nodiscard]] std::string getName() const { return *name; }

        ProjectRenderpass* addRenderpass() { return *(*renderpasses).emplace_back(); }

        void clear() { (*renderpasses).clear(); }

        DECLARE_PRIVATE_RESOURCE(Project)
    };
}


