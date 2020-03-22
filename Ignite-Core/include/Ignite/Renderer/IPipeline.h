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
		IPipeline(const std::string& name, const PipelineInputLayout& inputLayout, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader);
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
		
		static std::shared_ptr<IPipeline> Create(const std::string& name, const PipelineInputLayout& inputLayout, const std::string& vertexShader, const std::string& fragmentShader,const std::string& geometryShader = "");
		static const IPipeline* CurrentBoundPipeline() { return s_currentBoundPipeline; }
	protected:
		const std::string m_name;
		const std::string m_vertexShader;
		const std::string m_fragmentShader;
		const std::string m_geometryShader;
		const PipelineInputLayout m_inputLayout;

		bool m_deleted;
		static IPipeline* s_currentBoundPipeline;
	};
}
