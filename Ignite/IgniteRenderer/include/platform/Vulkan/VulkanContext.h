#pragma once

#include <Ignite/GraphicsContext.h>

namespace Ignite
{
    class VulkanContext : public GraphicsContext
    {
	friend class GraphicsContext;
    protected:
        VulkanContext();

		void Init() override;
		void Cleanup() override;
	public:
        ~VulkanContext();

        void SwapBuffers() override;
    };
}