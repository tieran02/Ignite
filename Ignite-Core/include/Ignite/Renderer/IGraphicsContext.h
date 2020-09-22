#pragma once
#include "NonCopyable.h"
#include "IBuffer.h"
#include "IMaterial.h"

namespace Ignite
{
	struct Texture2DCreateInfo;
	struct PipelineCreateInfo;
	class IPipeline;
	class IWindow;

	class IGraphicsContext : NonCopyable
    {
		friend class IPipeline;
		friend class IBuffer;
		friend class IVertexBuffer;
		friend class IIndexBuffer;
		friend class ITexture2D;
		friend class IMesh;
		friend class IMaterial;
	protected:
		IGraphicsContext() = default;
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
    public:
		virtual ~IGraphicsContext() = default;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<IGraphicsContext> Create();

		IPipeline* CreatePipeline(const PipelineCreateInfo& pipelineInfo);
		ITexture2D* CreateTexture2D(const Texture2DCreateInfo& textureInfo);
		
		const std::unordered_map<std::string, std::unique_ptr<IPipeline>>& Pipelines() const { return m_pipelines; }
		const std::vector<std::shared_ptr<IBuffer>>& Buffers() const { return m_buffers; }
		const std::unordered_map<std::string, std::shared_ptr<ITexture2D>>& Texture2Ds() const { return m_texture2Ds; }
		const std::vector<std::shared_ptr<IMesh>>& Models() const { return m_models; }
	protected:
		std::unordered_map<std::string,std::unique_ptr<IPipeline>> m_pipelines;
		std::unordered_map<std::string, std::shared_ptr<IMaterial>> m_materials;
		std::vector<std::shared_ptr<IBuffer>> m_buffers;
		std::unordered_map<std::string, std::shared_ptr<ITexture2D>> m_texture2Ds;
		std::vector<std::shared_ptr<IMesh>> m_models;
    };
}
