#include "igpch.h"
#include "platform/ImGUI/VulkanImGUI.h"


#include "imgui.h"
#include "Ignite/Log.h"
#include "Ignite/Utils.h"
#include "Ignite/Renderer/Renderer.h"
#include "platform/Vulkan/VulkanContext.h"
#include "platform/Vulkan/VulkanResources.h"
#include "platform/Vulkan/VulkanBuffer.h"

VulkanImGUI::~VulkanImGUI()
{
	ImGui::DestroyContext();

	// Release all Vulkan resources required for rendering imGui
	vertexBuffer->Free();
	indexBuffer->Free();
	vkDestroyImage(device->LogicalDevice(), fontImage, nullptr);
	vkDestroyImageView(device->LogicalDevice(), fontView, nullptr);
	vkFreeMemory(device->LogicalDevice(), fontMemory, nullptr);
	vkDestroySampler(device->LogicalDevice(), sampler, nullptr);
	vkDestroyPipelineCache(device->LogicalDevice(), pipelineCache, nullptr);
	vkDestroyPipeline(device->LogicalDevice(), pipeline, nullptr);
	vkDestroyPipelineLayout(device->LogicalDevice(), pipelineLayout, nullptr);
	vkDestroyDescriptorPool(device->LogicalDevice(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device->LogicalDevice(), descriptorSetLayout, nullptr);
}

VulkanImGUI::VulkanImGUI()
{
	context = dynamic_cast<const Ignite::VulkanContext*>(Ignite::Renderer::GraphicsContext());
	CORE_ASSERT(context, "VulkanImGUI Failed to find context");
	device = &context->Device();

	ImGui::CreateContext();
}

void VulkanImGUI::Init(float width, float height)
{
	// Color scheme
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	// Dimensions
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(width, height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	VkRenderPass renderPass = context->Renderpass().Renderpass();
	VkQueue copyQueue = device->GraphicsQueue();
	
	initResources(renderPass, copyQueue,"resources/shaders/uiVert.spv", "resources/shaders/uiFrag.spv");
}

void VulkanImGUI::initResources(VkRenderPass renderPass, VkQueue copyQueue, const std::string& vertShaderPath, const std::string& fragShaderPath)
{
	ImGuiIO& io = ImGui::GetIO();

	// Create font texture
	unsigned char* fontData;
	int texWidth, texHeight;
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
	VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

	//Create Image
	Ignite::VulkanResources::CreateImage(device->LogicalDevice(),
		device->PhysicalDevice(),
		texWidth,
		texHeight,
		1,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		fontImage,
		fontMemory);

	//create imageview
	fontView = Ignite::VulkanResources::CreateImageView(device->LogicalDevice(),
		fontImage,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1);

	//image staging buffer
	Ignite::VulkanBaseBuffer imageBuffer(context);
	imageBuffer.CreateHostVisable(fontData, uploadSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	//copy data to the buffer
	void* data;
	vkMapMemory(device->LogicalDevice(), imageBuffer.DeviceMemory(), 0, uploadSize, 0, &data);
	memcpy(data, fontData, static_cast<size_t>(uploadSize));
	vkUnmapMemory(device->LogicalDevice(), imageBuffer.DeviceMemory());

	Ignite::VulkanResources::CopyBufferToImage(device->LogicalDevice(),
		device->PhysicalDevice(),
		context->CommandPool(),
		context->Device().GraphicsQueue(),
		imageBuffer.Buffer(),
		fontImage, 
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight),
		1,
		VK_FORMAT_R8G8B8A8_UNORM);

	Ignite::VulkanResources::TransitionImageLayout(device->LogicalDevice(), context->CommandPool(), device->GraphicsQueue(),
	                                               fontImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);

	imageBuffer.Free();

	// Font texture Sampler
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	VK_CHECK_RESULT(vkCreateSampler(device->LogicalDevice(), &samplerInfo, nullptr, &sampler));

	// Descriptor pool
	VkDescriptorPoolSize descriptorPoolSize{};
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize.descriptorCount = 1;
	
	std::vector<VkDescriptorPoolSize> poolSizes = 
	{
		descriptorPoolSize
	};
	
	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = 2;
	VK_CHECK_RESULT(vkCreateDescriptorPool(device->LogicalDevice(), &descriptorPoolInfo, nullptr, &descriptorPool));

	// Descriptor set layout
	VkDescriptorSetLayoutBinding setLayoutBinding{};
	setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	setLayoutBinding.binding = 0;
	setLayoutBinding.descriptorCount = 1;
	
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = 
	{
		setLayoutBinding
	};

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device->LogicalDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

	// Descriptor set
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	VK_CHECK_RESULT(vkAllocateDescriptorSets(device->LogicalDevice(), &descriptorSetAllocateInfo, &descriptorSet));
	
	VkDescriptorImageInfo fontDescriptor{};
	fontDescriptor.sampler = sampler;
	fontDescriptor.imageView = fontView;
	fontDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.pImageInfo = &fontDescriptor;
	writeDescriptorSet.descriptorCount = 1;
	std::vector<VkWriteDescriptorSet> writeDescriptorSets = 
	{
			writeDescriptorSet
	};
	vkUpdateDescriptorSets(device->LogicalDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

	// Pipeline cache
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	VK_CHECK_RESULT(vkCreatePipelineCache(device->LogicalDevice(), &pipelineCacheCreateInfo, nullptr, &pipelineCache));

	// Pipeline layout
		// Push constants for UI rendering parameters
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstBlock);

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
	VK_CHECK_RESULT(vkCreatePipelineLayout(device->LogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

	// Setup graphics pipeline for UI rendering
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
	inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyState.flags = 0;
	inputAssemblyState.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizationState{};
	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationState.cullMode = VK_CULL_MODE_NONE;
	rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.flags = 0;
	rasterizationState.depthClampEnable = VK_FALSE;
	rasterizationState.lineWidth = 1.0f;

	// Enable blending
	VkPipelineColorBlendAttachmentState blendAttachmentState{};
	blendAttachmentState.blendEnable = VK_TRUE;
	blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendState{};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = &blendAttachmentState;

	VkPipelineDepthStencilStateCreateInfo depthStencilState{};
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthTestEnable = VK_FALSE;
	depthStencilState.depthWriteEnable = VK_FALSE;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;
	viewportState.flags = 0;

	VkPipelineMultisampleStateCreateInfo multisampleState{};
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleState.flags = 0;

	std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = dynamicStateEnables.data();
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynamicState.flags = 0;

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.flags = 0;
	pipelineCreateInfo.basePipelineIndex = -1;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
	pipelineCreateInfo.pRasterizationState = &rasterizationState;
	pipelineCreateInfo.pColorBlendState = &colorBlendState;
	pipelineCreateInfo.pMultisampleState = &multisampleState;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pDepthStencilState = &depthStencilState;
	pipelineCreateInfo.pDynamicState = &dynamicState;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	pipelineCreateInfo.pStages = shaderStages.data();

	// Vertex bindings an attributes based on ImGui vertex definition
	VkVertexInputBindingDescription vInputBindDescription{};
	vInputBindDescription.binding = 0;
	vInputBindDescription.stride = sizeof(ImDrawVert);
	vInputBindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
		vInputBindDescription
	};

	VkVertexInputAttributeDescription vPositionAttribDescription{};
	vPositionAttribDescription.location = 0;
	vPositionAttribDescription.binding = 0;
	vPositionAttribDescription.format = VK_FORMAT_R32G32_SFLOAT;
	vPositionAttribDescription.offset = offsetof(ImDrawVert, pos);

	VkVertexInputAttributeDescription vUVAttribDescription{};
	vUVAttribDescription.location = 1;
	vUVAttribDescription.binding = 0;
	vUVAttribDescription.format = VK_FORMAT_R32G32_SFLOAT;
	vUVAttribDescription.offset = offsetof(ImDrawVert, uv);

	VkVertexInputAttributeDescription vColorAttribDescription{};
	vColorAttribDescription.location = 2;
	vColorAttribDescription.binding = 0;
	vColorAttribDescription.format = VK_FORMAT_R8G8B8A8_UNORM;
	vColorAttribDescription.offset = offsetof(ImDrawVert, col);
	
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
		vPositionAttribDescription,
		vUVAttribDescription,
		vColorAttribDescription
	};

	VkPipelineVertexInputStateCreateInfo vertexInputState{};
	vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
	vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
	vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

	pipelineCreateInfo.pVertexInputState = &vertexInputState;

	//read files
	std::vector<char> vertShaderCode = Ignite::Utils::ReadFile(vertShaderPath);
	std::vector<char> fragShaderCode = Ignite::Utils::ReadFile(fragShaderPath);

	//create shader modules
	VkShaderModule vertShaderModule = Ignite::VulkanResources::CreateShaderModule(device->LogicalDevice(), vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT);
	VkShaderModule fragShaderModule = Ignite::VulkanResources::CreateShaderModule(device->LogicalDevice(), fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT);

	//vertex shader stage
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	//fragment shader stage
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	shaderStages[0] = vertShaderStageInfo;
	shaderStages[1] = fragShaderStageInfo;

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(device->LogicalDevice(), pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline));
}
