#pragma once
#include "NonCopyable.h"
#include <string>
#include <memory>
#include "PipelineInputLayout.h"

namespace Ignite
{
	class IGraphicsContext;

	class IPipeline : NonCopyable
	{
	protected:
		IPipeline(const std::string& name, const PipelineInputLayout& inputLayout, const std::string& vertexShader, const std::string& fragmentShader);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IPipeline() = default;
		const IGraphicsContext* m_context;

		virtual void Free() = 0;
		virtual void Recreate() = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		
		const std::string& Name() const { return m_name; }
		
		static std::shared_ptr<IPipeline> Create(const std::string& name, const PipelineInputLayout& inputLayout, const std::string& vertexShader, const std::string& fragmentShader);
		static const IPipeline* CurrentBoundPipeline() { return s_currentBoundPipeline; }
	protected:
		std::string m_name;
		std::string m_vertexShader;
		std::string m_fragmentShader;
		PipelineInputLayout m_inputLayout;

		bool m_deleted;
		static IPipeline* s_currentBoundPipeline;
	};
}
