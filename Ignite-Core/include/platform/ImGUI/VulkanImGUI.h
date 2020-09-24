#pragma once
#include <vulkan/vulkan.h>

#include "glm/vec2.hpp"
#include "platform/ImGUI/ImGUI.h"

namespace Ignite {
	class VulkanDevice;
	class VulkanContext;
	class VulkanBuffer;
}

class VulkanImGUI : public ImGUI
{
	friend class ImGUI;
public:
	struct PushConstBlock {
		glm::vec2 scale;
		glm::vec2 translate;
	} pushConstBlock;

	
	~VulkanImGUI() override;

	void Init(float width, float height) override;
	void NewFrame(bool updateFrameGraph) override;
	void UpdateBuffers() override;
	void DrawFrame() override;
protected:
	VulkanImGUI();
private:
	// Vulkan resources for rendering the UI
	VkSampler sampler;
	
	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexMemory = VK_NULL_HANDLE;
	VkDeviceMemory indexMemory = VK_NULL_HANDLE;
	void* vertexMapped{nullptr};
	void* indexMapped{ nullptr };
	
	int32_t vertexCount = 0;
	int32_t indexCount = 0;
	VkDeviceMemory fontMemory = VK_NULL_HANDLE;
	VkImage fontImage = VK_NULL_HANDLE;
	VkImageView fontView = VK_NULL_HANDLE;
	VkPipelineCache pipelineCache;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;
	const Ignite::VulkanDevice* device;
	const Ignite::VulkanContext* context;

	void initResources(VkRenderPass renderPass, VkQueue copyQueue, const std::string& vertShaderPath, const std::string& fragShaderPath);
};

