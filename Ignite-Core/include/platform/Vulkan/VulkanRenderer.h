#pragma once
#include "Ignite/IRenderer.h"

namespace Ignite {
    class VulkanRenderer : public IRenderer {
	friend class IRenderer;
    protected:
		VulkanRenderer(IWindow* window);

        void Init() override;
		void Cleanup() override;
	public:
		~VulkanRenderer() override;

        void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        void SetClearColor(const glm::vec4 &color) override;
        
        void Clear() override;
    };
}
