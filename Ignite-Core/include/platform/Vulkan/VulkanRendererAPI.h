#pragma once
#include "Ignite/Renderer/IRendererAPI.h"
#include <vulkan/vulkan.h>

namespace Ignite {
    class VulkanRendererAPI : public IRendererAPI {
	friend class IRendererAPI;
    protected:
		VulkanRendererAPI();

        void Init() override;
		void Cleanup() override;
	public:
		~VulkanRendererAPI() override;

        void BeginScene(const Camera& camera,const std::vector<LightData>& lights) override;
        void EndScene() override;
    	
        void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void DrawIndexed(const IBuffer* vertexBuffer, const IBuffer* indexBuffer, uint32_t indexCount, const glm::mat4& transform) override;

        void SetModelUniformBufferObject(const ModelUniformBuffer& ubo) override;
        void SetClearColor(const glm::vec4 &color) override;
        
        void Clear() override;
    protected:
        glm::vec4 m_clearColour{ 0.0f,0.0f,0.0f,1.0f };
        void bindDescriptors();
    };
}
