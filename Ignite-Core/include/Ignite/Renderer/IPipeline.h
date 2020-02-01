#pragma once
#include "NonCopyable.h"
#include <string>
#include <memory>

namespace Ignite
{
	class IGraphicsContext;

	class IPipeline : NonCopyable
	{
	protected:
		IPipeline(const std::string& vertexShader, const std::string& fragmentShader);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IPipeline() = default;
		const IGraphicsContext* m_context;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		
		std::string m_vertexShader;
		std::string m_fragmentShader;
		
		static std::unique_ptr<IPipeline> Create(const std::string& vertexShader, const std::string& fragmentShader);
	};
}