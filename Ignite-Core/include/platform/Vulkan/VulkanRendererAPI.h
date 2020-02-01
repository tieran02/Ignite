#pragma once
#include "Ignite/IRendererAPI.h"

namespace Ignite {
    class VulkanRendererAPI : public IRendererAPI {
	friend class IRendererAPI;
    protected:
		VulkanRendererAPI();

        void Init() override;
		void Cleanup() override;
	public:
		~VulkanRendererAPI() override;

        void BeginScene() override;
        void EndScene() override;
    	
        void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        void SetClearColor(const glm::vec4 &color) override;
        
        void Clear() override;
    private:
        glm::vec4 m_clearColour{ 0.0f,0.0f,0.0f,1.0f };
    };
}
