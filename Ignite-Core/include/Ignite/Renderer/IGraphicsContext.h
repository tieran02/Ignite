#pragma once
#include "NonCopyable.h"
#include "IBuffer.h"
#include "IMaterial.h"

namespace Ignite
{
	struct MeshCreateInfo;
	struct Texture2DCreateInfo;
	struct PipelineCreateInfo;
	class IPipeline;
	class IWindow;
	class IMesh;

	class IGraphicsContext : NonCopyable
    {
		friend class IBuffer;
		friend class IVertexBuffer;
		friend class IIndexBuffer;
		friend class IMaterial;
	protected:
		IGraphicsContext() = default;
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
    public:
		virtual ~IGraphicsContext() = default;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<IGraphicsContext> Create();

		const IPipeline* CreatePipeline(const PipelineCreateInfo& pipelineInfo);
		const ITexture2D* CreateTexture2D(const Texture2DCreateInfo& textureInfo);
		const IMesh* CreateMesh(const MeshCreateInfo& meshInfo);
		const IMaterial* CreateMaterial(const MaterialCreateInfo& materialInfo);
		const IBuffer* CreateBuffer(const BufferCreateInfo& bufferInfo);
		
		const std::unordered_map<std::string, std::unique_ptr<IPipeline>>& Pipelines() const { return m_pipelines; }
		const std::unordered_map<std::string, std::unique_ptr<IMaterial>>& Materials() const { return m_materials; }
		const std::unordered_map<std::string, std::unique_ptr<IBuffer>>& Buffers() const { return m_buffers; }
		const std::unordered_map<std::string, std::unique_ptr<ITexture2D>>& Texture2Ds() const { return m_texture2Ds; }
		const std::unordered_map<std::string, std::unique_ptr<IMesh>>& Meshes() const { return m_meshes; }
	protected:
		std::unordered_map<std::string, std::unique_ptr<IPipeline>> m_pipelines;
		std::unordered_map<std::string, std::unique_ptr<IMaterial>> m_materials;
		std::unordered_map<std::string, std::unique_ptr<IBuffer>> m_buffers;
		std::unordered_map<std::string, std::unique_ptr<ITexture2D>> m_texture2Ds;
		std::unordered_map<std::string, std::unique_ptr<IMesh>> m_meshes;
    };
}
