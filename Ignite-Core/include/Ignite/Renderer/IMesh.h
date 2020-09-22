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
		MeshCreateInfo(MeshData&& mesh_data, const IMaterial* material)
			: meshData(std::move(mesh_data)),
			m_material(material)
		{
		}

		const MeshData& GetMeshData() const { return meshData; }
		const IMaterial* GetMaterial() const { return m_material; }
		
	private:
		const MeshData meshData;
		const IMaterial* m_material;
	};
	
	class IMesh
	{
	protected:
		IMesh(const IMaterial* material);
		virtual void Init(const MeshData& meshData) = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IMesh() = default;
		virtual void BindDescriptors() const = 0;
		const IGraphicsContext* m_context;

		static std::shared_ptr<IMesh> Create(const MeshCreateInfo& meshInfo);

		const IVertexBuffer* VertexBuffer() const { return m_vertexBuffer.get(); }
		const IIndexBuffer* IndexBuffer() const { return m_IndexBuffer.get(); }
		const IMaterial* Material() const { return m_matieral; }
		uint32_t IndexCount() const { return m_indexCount; }
	protected:
		std::shared_ptr<IVertexBuffer> m_vertexBuffer;
		std::shared_ptr<IIndexBuffer> m_IndexBuffer;
		uint32_t m_indexCount;
		const IMaterial* m_matieral;
	};
}
