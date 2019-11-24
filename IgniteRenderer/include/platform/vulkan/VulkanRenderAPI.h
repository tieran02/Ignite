//
// Created by Tieran on 24/11/2019.
//

#ifndef IGNITERENDERER_VULKANRENDERAPI_H
#define IGNITERENDERER_VULKANRENDERAPI_H

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

#endif //IGNITERENDERER_VULKANRENDERAPI_H
