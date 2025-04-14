#pragma once
#include "../resource_manager.hpp"

#include <functional>

#include "list.hpp"
#include "renderpass.hpp"

namespace gflow::parser
{
    class ProjectImageSource final : public Resource
    {
        EXPORT(std::string, imageID);
        EXPORT_ENUM(source, gflow::parser::EnumContexts::ImageSource);
        EXPORT(gflow::parser::FilePath, path);
        EXPORT(gflow::parser::Color, color);
        EXPORT(bool, matchScreen);
        EXPORT(gflow::parser::Vec2, size);
        gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

    public:
        DECLARE_PRIVATE_RESOURCE(ProjectImageSource)

        template <typename T>
        friend class List;
    };

    class ProjectRenderpassDrawCall final : public Resource
    {
        EXPORT_RESOURCE(Pipeline, pipeline, false, true);
        EXPORT(int, vertexCount);

    public:
        Pipeline* getPipeline() { return *pipeline; }
        void setPipeline(Pipeline* pipeline) { this->pipeline.setData(pipeline); }

        DECLARE_PRIVATE_RESOURCE(ProjectRenderpassDrawCall);

        template <typename T>
        friend class List;
    };

    class ProjectRenderpassSubpass final : public Resource
    {
        EXPORT_RESOURCE_LIST(ProjectRenderpassDrawCall, drawcalls);

    public:
        ProjectRenderpassDrawCall* addDrawCall() { return *(*drawcalls).emplace_back(); }

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
        EXPORT_RESOURCE_LIST(ProjectImageSource, images);
        EXPORT_RESOURCE_LIST(ProjectRenderpass, renderpasses);

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
    public:
        [[nodiscard]] std::string getName() const { return *name; }

        ProjectRenderpass* addRenderpass() { return *(*renderpasses).emplace_back(); }

        void clear() { (*renderpasses).clear(); }

        DECLARE_PRIVATE_RESOURCE(Project)
    };

    inline gflow::parser::DataUsage ProjectImageSource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "imageID" || variable == "source")
            return gflow::parser::USED;

        switch ((*source).id)
        {
        case 0: // ImageSource::File
            if (variable == "color")
                return gflow::parser::USED;
            if (variable == "matchScreen")
                return gflow::parser::USED;
            if (variable == "size" && !*matchScreen)
                return gflow::parser::USED;
            break;
        case 1: // ImageSource::Flat Color
            if (variable == "path")
                return gflow::parser::USED;
            break;
        }
        return NOT_USED;
    }

    inline DataUsage Project::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "name")
            return READ_ONLY;
        if (variable == "images")
            return USED;
        return NOT_USED;
    }
}