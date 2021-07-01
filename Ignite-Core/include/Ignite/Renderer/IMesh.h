#pragma once
#include "IBuffer.h"
#include "ITexture2D.h"
#include "MeshData.h"
#include "Vertex.h"
#include "Ignite/SceneObject.h"

namespace Ignite
{
	struct MeshData;
	class PipelineInputLayout;

	struct MeshCreateInfo : SceneObjectCreateInfo
	{
	public:
		MeshCreateInfo(const std::string& name,MeshData&& mesh_data, const IMaterial* material)
			: SceneObjectCreateInfo(SceneObjectType::MESH),
			m_name(name),
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
	
	class IMesh : public SceneObject
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

		const IBuffer* VertexBuffer() const { return m_vertexBuffer; }
		const IBuffer* IndexBuffer() const { return m_IndexBuffer; }
		const MeshCreateInfo& MeshInfo() const { return m_meshInfo; }
		const IMaterial* Material() const { return m_meshInfo.GetMaterial(); }
		uint32_t IndexCount() const { return m_indexCount; }
	protected:
		const IBuffer* m_vertexBuffer;
		const IBuffer* m_IndexBuffer;
		uint32_t m_indexCount;

		MeshCreateInfo m_meshInfo;
	};
}
