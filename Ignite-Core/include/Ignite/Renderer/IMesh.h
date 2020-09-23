#pragma once
#include "IBuffer.h"
#include "ITexture2D.h"
#include "MeshData.h"
#include "Vertex.h"

namespace Ignite
{
	struct MeshData;
	class PipelineInputLayout;

	struct MeshCreateInfo
	{
	public:
		MeshCreateInfo(const std::string& name,MeshData&& mesh_data, const IMaterial* material)
			: m_name(name),
			m_meshData(std::move(mesh_data)),
			m_material(material)
		{
		}

		const std::string& GetName() const { return m_name; }
		const MeshData& GetMeshData() const { return m_meshData; }
		const IMaterial* GetMaterial() const { return m_material; }
		
	private:
		const std::string m_name;
		const MeshData m_meshData;
		const IMaterial* m_material;
	};
	
	class IMesh
	{
	protected:
		IMesh(const MeshCreateInfo& meshInfo);
		virtual void Init(const MeshData& meshData) = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IMesh() = default;
		virtual void BindDescriptors() const = 0;
		const IGraphicsContext* m_context;

		static std::unique_ptr<IMesh> Create(const MeshCreateInfo& meshInfo);

		const IBuffer* VertexBuffer() const { return m_vertexBuffer.get(); }
		const IBuffer* IndexBuffer() const { return m_IndexBuffer.get(); }
		const MeshCreateInfo& MeshInfo() const { return m_meshInfo; }
		const IMaterial* Material() const { return m_meshInfo.GetMaterial(); }
		uint32_t IndexCount() const { return m_indexCount; }
	protected:
		std::shared_ptr<IBuffer> m_vertexBuffer;
		std::shared_ptr<IBuffer> m_IndexBuffer;
		uint32_t m_indexCount;

		MeshCreateInfo m_meshInfo;
	};
}
