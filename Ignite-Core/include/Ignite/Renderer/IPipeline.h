#pragma once
#include "NonCopyable.h"
#include <string>
#include <memory>
#include "PipelineInputLayout.h"

namespace Ignite
{
	class IGraphicsContext;

	struct PipelineInfo
	{
	public:
		PipelineInfo(const std::string& name, PipelineInputLayout& input_layout, const std::string& vertex_shader_path,
			const std::string& fragment_shader_path, const std::string& geometry_shader_path = "")
			: m_name(name),
			m_inputLayout(input_layout),
			m_vertexShaderPath(vertex_shader_path),
			m_fragmentShaderPath(fragment_shader_path),
			m_geometryShaderPath(geometry_shader_path)
		{
		}
		const std::string& GetName() const { return m_name; }
		const PipelineInputLayout& GetLayout() const { return m_inputLayout; }
		const std::string& GetVertexShaderPath() const { return m_vertexShaderPath; }
		const std::string& GetFragmentShaderPath() const { return m_fragmentShaderPath; }
		const std::string& GetGeometryShaderPath() const { return m_geometryShaderPath; }
	private:
		const std::string m_name;
		const PipelineInputLayout m_inputLayout;
		const std::string m_vertexShaderPath;
		const std::string m_fragmentShaderPath;
		const std::string m_geometryShaderPath;
	};
	
	class IPipeline : NonCopyable
	{
	protected:
		IPipeline(const PipelineInfo& info);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~IPipeline() = default;
		const IGraphicsContext* m_context;

		virtual void Free() = 0;
		virtual void Recreate() = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
			
		static std::shared_ptr<IPipeline> Create(const PipelineInfo& info);
		static const IPipeline* CurrentBoundPipeline() { return s_currentBoundPipeline; }
	protected:
		const PipelineInfo m_pipelineInfo;

		bool m_deleted;
		static IPipeline* s_currentBoundPipeline;
	};
}
