#pragma once
#include "NonCopyable.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Ignite
{
	struct SceneUniformBuffer
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::vec3 view_pos;
	};

	struct ModelUniformBuffer
	{
		//TODO this is temp
		alignas(16) glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	};

	enum class BUFFER_TYPE
	{
		BUFFER,
		VERTEX,
		INDEX
	};
	
	struct BufferCreateInfo
	{
	public:
		BufferCreateInfo(BUFFER_TYPE m_type, const void* m_data, size_t m_size)
			: m_type(m_type),
			m_data(m_data),
			m_size(m_size)
		{
		}
		
		BUFFER_TYPE GetBufferType() const { return m_type; }
		const void* GetData() const { return m_data; }
		size_t GetSize() const { return m_size; }
	private:
		BUFFER_TYPE m_type;
		const void* m_data;
		size_t m_size;

	};
	
	class IGraphicsContext;
	class IBuffer : NonCopyable
	{		
	protected:
		IBuffer(const BufferCreateInfo& bufferInfo);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IBuffer() = default;
		const IGraphicsContext* m_context;

		virtual void Free() = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<IBuffer> Create(const BufferCreateInfo& bufferInfo);
		
		const BufferCreateInfo& GetBufferInfo() const { return m_bufferInfo; }
	protected:
		bool m_deleted;
		const BufferCreateInfo m_bufferInfo;
	};
}
