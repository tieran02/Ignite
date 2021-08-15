#pragma once
#include "NonCopyable.h"
#include "Ignite/CreateProperties.h"
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
		INDEX,
		UNIFORM,
		STORAGE,
		TRANSFER
	};

	enum class BUFFER_VISIBILITY
	{
		HOST,
		STAGED
	};
	
	struct BufferCreateInfo : public CreateProperties
	{
	public:
		BufferCreateInfo(const std::string& name, BUFFER_TYPE type, BUFFER_VISIBILITY visibility, const void* sourceData, size_t size)
			: m_name(name),
			m_type(type),
			m_visibility(visibility),
			m_sourceData(sourceData),
			m_size(size),
			CreateProperties(CreatePropertyType::Buffer)
		{
		}

		const std::string& GetName() const { return m_name; }
		BUFFER_TYPE GetBufferType() const { return m_type; }
		BUFFER_VISIBILITY GetBufferVisibility() const { return m_visibility; }
		const void* GetData() const { return m_sourceData; }
		size_t GetSize() const { return m_size; }
	private:
		const std::string m_name;
		BUFFER_TYPE m_type;
		BUFFER_VISIBILITY m_visibility;
		const void* m_sourceData;
		size_t m_size;

	};
	
	class GraphicsContext;
	class Buffer : NonCopyable
	{		
	protected:
		Buffer(const BufferCreateInfo& bufferInfo);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		Buffer(Buffer&& other) noexcept
			: m_deleted(other.m_deleted),
			  m_bufferInfo(std::move(other.m_bufferInfo))
		{
		}

		virtual ~Buffer() = default;

		virtual void Free() = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void* Map() const = 0;
		virtual void Unmap() const = 0;
		virtual void Flush() const = 0;

		virtual void CopyToBuffer(void* data, size_t size) = 0;

		static std::unique_ptr<Buffer> Create(const BufferCreateInfo& bufferInfo);
		
		const BufferCreateInfo& GetBufferInfo() const { return m_bufferInfo; }
	protected:
		bool m_deleted;
		const BufferCreateInfo m_bufferInfo;
	};
}
