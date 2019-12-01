#pragma once

#include <Ignite/GraphicsContext.h>

namespace Ignite
{
    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext();
        ~VulkanContext();

        void Init() override;

        void SwapBuffers() override;
    };
}