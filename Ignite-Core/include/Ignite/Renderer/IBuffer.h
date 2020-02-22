#pragma once
#include "NonCopyable.h"
#include "glm/mat4x4.hpp"

namespace Ignite
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		glm::vec3 light_dir;
	};
	
	class IGraphicsContext;
	class IBuffer : NonCopyable
	{		
	protected:
		IBuffer();
		virtual void Init(const void* data, size_t size) = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IBuffer() = default;
		const IGraphicsContext* m_context;

		virtual void Free() = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<IBuffer> Create(const void* data, size_t size);
	protected:
		bool m_deleted;
	};

	class IVertexBuffer :public IBuffer
	{
	protected:
		IVertexBuffer();
		void Init(const void* data, size_t size) override = 0;
		void Cleanup() override = 0;
	public:
		virtual ~IVertexBuffer() = default;
		void Bind() const override = 0;
		void Unbind() const override = 0;

		static std::shared_ptr<IVertexBuffer> Create(const void* data, size_t size);
	};

	class IIndexBuffer :public IBuffer
	{
	protected:
		IIndexBuffer();
		void Init(const void* data, size_t size) override = 0;
		void Cleanup() override = 0;
	public:
		virtual ~IIndexBuffer() = default;
		void Bind() const override = 0;
		void Unbind() const override = 0;

		static std::shared_ptr<IIndexBuffer> Create(const uint32_t* data, size_t size);
	};
}
