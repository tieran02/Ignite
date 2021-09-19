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

	vkDeviceWaitIdle(device->LogicalDevice());
	// Release all Vulkan resources required for rendering imGui
	vkDestroyBuffer(device->LogicalDevice(), vertexBuffer, nullptr);
	vkFreeMemory(device->LogicalDevice(), vertexMemory, nullptr);
	vkDestroyBuffer(device->LogicalDevice(), indexBuffer, nullptr);
	vkFreeMemory(device->LogicalDevice(), indexMemory, nullptr);
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

void VulkanImGUI::NewFrame(bool updateFrameGraph)
{
	ImGui::NewFrame();

	// Init imGui windows and elements

	ImGui::ShowDemoWindow();

	// Render to generate draw buffers
	ImGui::Render();
}

void VulkanImGUI::UpdateBuffers()
{
	ImDrawData* imDrawData = ImGui::GetDrawData();

	// Note: Alignment is done inside buffer creation
	
	VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
	VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

	if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
		return;
	}
	
	// Update buffers only if vertex or index count has been changed compared to current buffer size

	// Vertex buffer
	if ((vertexBuffer == VK_NULL_HANDLE) || (vertexCount != imDrawData->TotalVtxCount))
	{
		if(vertexMapped)
		{
			vkUnmapMemory(device->LogicalDevice(), vertexMemory);
			vertexMapped = nullptr;
		}

		if(vertexBuffer != VK_NULL_HANDLE)
			vkDestroyBuffer(device->LogicalDevice(), vertexBuffer, nullptr);
		Ignite::VulkanResources::CreateBuffer(device->LogicalDevice(), device->PhysicalDevice(), vertexBufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		                                      vertexBuffer, vertexMemory);

		vertexCount = imDrawData->TotalVtxCount;
		vkMapMemory(device->LogicalDevice(), vertexMemory, 0, vertexBufferSize, 0, &vertexMapped);
	}

	if ((indexBuffer == VK_NULL_HANDLE) || (indexCount != imDrawData->TotalIdxCount))
	{
		if (indexMapped)
		{
			vkUnmapMemory(device->LogicalDevice(), indexMemory);
			indexMapped = nullptr;
		}

		if (indexBuffer != VK_NULL_HANDLE)
			vkDestroyBuffer(device->LogicalDevice(), indexBuffer, nullptr);
		Ignite::VulkanResources::CreateBuffer(device->LogicalDevice(), device->PhysicalDevice(), indexBufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			indexBuffer, indexMemory);

		indexCount = imDrawData->TotalIdxCount;
		vkMapMemory(device->LogicalDevice(), indexMemory, 0, indexBufferSize, 0, &indexMapped);
	}

	// Upload data
	ImDrawVert* vtxDst = (ImDrawVert*)vertexMapped;
	ImDrawIdx* idxDst = (ImDrawIdx*)indexMapped;

	VkDeviceSize actualVertexSize = 0;
	VkDeviceSize actualIndexSize = 0;
	for (int n = 0; n < imDrawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = imDrawData->CmdLists[n];
		
		memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtxDst += cmd_list->VtxBuffer.Size;
		idxDst += cmd_list->IdxBuffer.Size;
	}

	// Flush to make writes visible to GPU
	VkMappedMemoryRange vertexMappedRange = {};
	vertexMappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	vertexMappedRange.memory = vertexMemory;
	vertexMappedRange.offset = 0;
	vertexMappedRange.size = VK_WHOLE_SIZE;
	vkFlushMappedMemoryRanges(device->LogicalDevice(), 1, &vertexMappedRange);

	VkMappedMemoryRange indexMappedRange = {};
	indexMappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	indexMappedRange.memory = indexMemory;
	indexMappedRange.offset = 0;
	indexMappedRange.size = VK_WHOLE_SIZE;
	vkFlushMappedMemoryRanges(device->LogicalDevice(), 1, &indexMappedRange);
}

void VulkanImGUI::DrawFrame()
{
	const std::vector<VkCommandBuffer>& commandBuffers = context->CommandBuffers();

	for (auto& commandBuffer : commandBuffers)
	{
		ImGuiIO& io = ImGui::GetIO();

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		VkViewport viewport{};
		viewport.width = ImGui::GetIO().DisplaySize.x;
		viewport.height = ImGui::GetIO().DisplaySize.y;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		// UI scale and translate via push constants
		pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
		pushConstBlock.translate = glm::vec2(-1.0f);
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &pushConstBlock);

		// Render commands
		ImDrawData* imDrawData = ImGui::GetDrawData();
		int32_t vertexOffset = 0;
		int32_t indexOffset = 0;

		if (imDrawData->CmdListsCount > 0) {

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

			for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
			{
				const ImDrawList* cmd_list = imDrawData->CmdLists[i];
				for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
					VkRect2D scissorRect;
					scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
					scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
					scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
					scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
					vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
					vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
					indexOffset += pcmd->ElemCount;
				}
				vertexOffset += cmd_list->VtxBuffer.Size;
			}
		}
	}
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

	Ignite::BufferCreateInfo imageBufferInfo{ "imGUI_font", Ignite::BUFFER_TYPE::TRANSFER, Ignite::BUFFER_VISIBILITY::HOST, fontData,uploadSize };
	
	//image staging buffer
	Ignite::VulkanBuffer imageBuffer = Ignite::VulkanBuffer(imageBufferInfo,*context);

    //copy data to the buffer
    void* data = imageBuffer.Map();
	memcpy(data, fontData, static_cast<size_t>(uploadSize));
	imageBuffer.Unmap();

	Ignite::VulkanResources::CopyBufferToImage(device->LogicalDevice(),
		device->PhysicalDevice(),
		context->CommandPool(),
		context->Device().GraphicsQueue(),
		imageBuffer.PlatformBuffer(),
		fontImage, 
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight),
		1,
		VK_FORMAT_R8G8B8A8_UNORM);

	/*Ignite::VulkanResources::TransitionImageLayout(device->LogicalDevice(), context->CommandPool(), device->GraphicsQueue(),
	                                               fontImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);*/

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
	VkShaderModule vertShaderModule = Ignite::VulkanResources::CreateShaderModule(device->LogicalDevice(), vertShaderCode);
	VkShaderModule fragShaderModule = Ignite::VulkanResources::CreateShaderModule(device->LogicalDevice(), fragShaderCode);

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

	vkDestroyShaderModule(device->LogicalDevice(), vertShaderModule, nullptr);
	vkDestroyShaderModule(device->LogicalDevice(), fragShaderModule, nullptr);
}
