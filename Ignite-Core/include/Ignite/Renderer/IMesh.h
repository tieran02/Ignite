#pragma once
#include "IBuffer.h"
#include "ITexture2D.h"
#include "Vertex.h"

namespace Ignite
{
	struct MeshData;
	class PipelineInputLayout;

	class IMesh
	{
	protected:
		IMesh(const std::shared_ptr<IMaterial>& material);
		virtual void Init(const MeshData& meshData) = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IMesh() = default;
		virtual void BindDescriptors() const = 0;
		const IGraphicsContext* m_context;

		static std::shared_ptr<IMesh> Create(const MeshData& meshData, const std::shared_ptr<IMaterial>& material);

		const IVertexBuffer* VertexBuffer() const { return m_vertexBuffer.get(); }
		const IIndexBuffer* IndexBuffer() const { return m_IndexBuffer.get(); }
		const IMaterial* Material() const { return m_matieral.get(); }
		uint32_t IndexCount() const { return m_indexCount; }
	protected:
		std::shared_ptr<IVertexBuffer> m_vertexBuffer;
		std::shared_ptr<IIndexBuffer> m_IndexBuffer;
		uint32_t m_indexCount;
		std::shared_ptr<IMaterial> m_matieral;
	};
}
