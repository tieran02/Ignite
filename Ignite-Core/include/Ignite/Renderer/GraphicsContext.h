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
	class DescriptorSetLayout;
	class BaseMaterial;
	class UUID;

	class GraphicsContext : NonCopyable
    {
	protected:
		GraphicsContext() = default;
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
    public:
		virtual ~GraphicsContext() = default;
        virtual void SwapBuffers() = 0;
		virtual void CreateDescriptorSetLayouts() = 0;

        static std::unique_ptr<GraphicsContext> Create();

		Ref<Pipeline>	CreatePipeline(const PipelineCreateInfo& pipelineInfo);
		Ref<Texture2D>	CreateTexture2D(const Texture2DCreateInfo& textureInfo);
		Ref<Mesh>		CreateMesh(const MeshCreateInfo& meshInfo);
		Ref<Material>	CreateMaterial(const MaterialCreateInfo& materialInfo);
		Ref<Buffer>		CreateBuffer(const BufferCreateInfo& bufferInfo);
		
		const std::unordered_map<std::string, Ref<Pipeline>>& Pipelines() const { return m_pipelines; }
		const std::unordered_map<std::string, Ref<Material>>& Materials() const { return m_materials; }
		const std::unordered_map<std::string, Ref<Buffer>>& Buffers() const { return m_buffers; }
		const std::unordered_map<std::string, Ref<Texture2D>>& Texture2Ds() const { return m_texture2Ds; }
		const std::unordered_map<std::string, Ref<Mesh>>& Meshes() const { return m_meshes; }

		std::unordered_map<UUID, Ref<DescriptorSetLayout>>& DescriptorSetLayouts() { return m_descriptorSetLayouts; }
		const std::unordered_map<UUID, Ref<DescriptorSetLayout>>& DescriptorSetLayouts() const { return m_descriptorSetLayouts; }
		std::unordered_map<UUID, Ref<BaseMaterial>>& BaseMaterials() { return m_baseMaterials; }
		const std::unordered_map<UUID, Ref<BaseMaterial>>& BaseMaterials() const { return m_baseMaterials; }
	protected:
		std::unordered_map<std::string, Ref<Pipeline>> m_pipelines;
		std::unordered_map<std::string, Ref<Material>> m_materials;
		std::unordered_map<std::string, Ref<Buffer>> m_buffers;
		std::unordered_map<std::string, Ref<Texture2D>> m_texture2Ds;
		std::unordered_map<std::string, Ref<Mesh>> m_meshes;

		std::unordered_map<UUID, Ref<DescriptorSetLayout>> m_descriptorSetLayouts;
		std::unordered_map<UUID, Ref<BaseMaterial>> m_baseMaterials;
    };
}
