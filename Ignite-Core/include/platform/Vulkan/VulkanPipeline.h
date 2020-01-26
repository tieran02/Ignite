#pragma once
#include "Ignite/IPipeline.h"

namespace  Ignite
{
	class VulkanContext;
	
	class VulkanPipeline : public IPipeline
	{
		friend class IPipeline;
	protected:
		VulkanPipeline(const std::string& vertexShader, const std::string& fragmentShader);
	public:
		~VulkanPipeline() override;
	protected:
		void Init() override;
		void Cleanup() override;
	public:
		void Bind() const override;
		void Unbind() const override;
	};
}