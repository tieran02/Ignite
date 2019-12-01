#pragma once
#include "Ignite/Renderer.h"

namespace Ignite {
    class VulkanRenderer : public Renderer {
	friend class Renderer;
    protected:
		VulkanRenderer();

        void Init() override;
		void Cleanup() override;
	public:
		~VulkanRenderer() override;

        void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        void SetClearColor(const glm::vec4 &color) override;
        
        void Clear() override;
    };
}
