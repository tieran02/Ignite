#pragma once
#include "ignite/RendererAPI.h"

namespace Ignite {
    class VulkanRenderAPI : public RendererAPI {
    public:
        VulkanRenderAPI();
        ~VulkanRenderAPI();
        
        void Init() override;

        void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        void SetClearColor(const glm::vec4 &color) override;
        
        void Clear() override;
    };
}
