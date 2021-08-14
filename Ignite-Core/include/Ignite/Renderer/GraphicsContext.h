#pragma once
#include "NonCopyable.h"

namespace Ignite
{
	struct MeshCreateInfo;
	struct Texture2DCreateInfo;
	struct PipelineCreateInfo;
	struct BufferCreateInfo;
	struct MaterialCreateInfo;
	class Pipeline;
	class IWindow;
	class Mesh;
	class Texture2D;
	class Material;
	class Buffer;

	class GraphicsContext : NonCopyable
    {
	protected:
		GraphicsContext() = default;
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
    public:
		virtual ~GraphicsContext() = default;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<GraphicsContext> Create();

		const Pipeline* CreatePipeline(const PipelineCreateInfo& pipelineInfo);
		const Texture2D* CreateTexture2D(const Texture2DCreateInfo& textureInfo);
		const Mesh* CreateMesh(const MeshCreateInfo& meshInfo);
		const Material* CreateMaterial(const MaterialCreateInfo& materialInfo);
		const Buffer* CreateBuffer(const BufferCreateInfo& bufferInfo);
		
		const std::unordered_map<std::string, std::unique_ptr<Pipeline>>& Pipelines() const { return m_pipelines; }
		const std::unordered_map<std::string, std::unique_ptr<Material>>& Materials() const { return m_materials; }
		const std::unordered_map<std::string, std::unique_ptr<Buffer>>& Buffers() const { return m_buffers; }
		const std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Texture2Ds() const { return m_texture2Ds; }
		const std::unordered_map<std::string, std::unique_ptr<Mesh>>& Meshes() const { return m_meshes; }
	protected:
		std::unordered_map<std::string, std::unique_ptr<Pipeline>> m_pipelines;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
		std::unordered_map<std::string, std::unique_ptr<Buffer>> m_buffers;
		std::unordered_map<std::string, std::unique_ptr<Texture2D>> m_texture2Ds;
		std::unordered_map<std::string, std::unique_ptr<Mesh>> m_meshes;
    };
}
