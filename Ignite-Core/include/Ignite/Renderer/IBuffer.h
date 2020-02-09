#pragma once
#include "NonCopyable.h"
#include "glm/mat4x4.hpp"

namespace Ignite
{
	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
	
	class IGraphicsContext;
	class IBuffer : NonCopyable
	{		
	protected:
		IBuffer();
		virtual void Init(void* data, size_t size) = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IBuffer() = default;
		const IGraphicsContext* m_context;

		virtual void Free() = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<IBuffer> Create(void* data, size_t size);
	protected:
		bool m_deleted;
	};

	class IVertexBuffer :public IBuffer
	{
	protected:
		IVertexBuffer();
		void Init(void* data, size_t size) override = 0;
		void Cleanup() override = 0;
	public:
		virtual ~IVertexBuffer() = default;
		void Bind() const override = 0;
		void Unbind() const override = 0;

		static std::shared_ptr<IVertexBuffer> Create(float* data, size_t size);
	};

	class IIndexBuffer :public IBuffer
	{
	protected:
		IIndexBuffer();
		void Init(void* data, size_t size) override = 0;
		void Cleanup() override = 0;
	public:
		virtual ~IIndexBuffer() = default;
		void Bind() const override = 0;
		void Unbind() const override = 0;

		static std::shared_ptr<IIndexBuffer> Create(uint16_t* data, size_t size);
	};
}
