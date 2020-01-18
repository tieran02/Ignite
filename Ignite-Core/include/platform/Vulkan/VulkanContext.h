#pragma once

#include <Ignite/IGraphicsContext.h>
#include <platform\Vulkan\VulkanDevice.h>

#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
		CORE_ASSERT(res == VK_SUCCESS, "Fatal : VkResult is " + std::to_string(static_cast<int>(res)) + " in " + __FILE__ + " at line " + std::to_string(__LINE__));\
}

namespace Ignite
{
    class VulkanContext : public IGraphicsContext
    {
	friend class IGraphicsContext;
    protected:
        VulkanContext();

		void Init() override;
		void Cleanup() override;
	public:
        ~VulkanContext();

        void SwapBuffers() override;
	private:
		std::unique_ptr<VulkanDevice> m_vulkanDevice;
    };
}