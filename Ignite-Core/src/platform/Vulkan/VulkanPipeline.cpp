#include "igpch.h"
#include "platform/Vulkan/VulkanPipeline.h"
#include "platform/Vulkan/VulkanContext.h"
#include "Ignite/Log.h"
#include "Ignite/Utils.h"

namespace  Ignite
{
	VulkanPipeline::VulkanPipeline(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader, const PipelineInputLayout& inputLayout)
	: IPipeline(name, vertexShader, fragmentShader, inputLayout)
	{
		Init();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		Cleanup();
	}

	void VulkanPipeline::Init()
	{
		createPipeline();
	}

	void VulkanPipeline::Cleanup()
	{
		if(m_deleted)
			return;
		
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		
		vkDeviceWaitIdle(vulkanContext->Device().LogicalDevice());
	

		LOG_CORE_INFO("Cleaning up Vulkan Pipeline: " + m_name);
		vkDestroyPipeline(vulkanContext->Device().LogicalDevice(), m_pipeline, nullptr);
		vkDestroyPipelineLayout(vulkanContext->Device().LogicalDevice(), m_pipelineLayout, nullptr);
		m_deleted = true;
	}

	void VulkanPipeline::Free()
	{
		Cleanup();
	}

	void VulkanPipeline::Recreate()
	{
		Cleanup();
		createPipeline();
	}

	void VulkanPipeline::Bind() const
	{
		//TODO when binding a pipeline we need to create a new command to start the render pass
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);
		CORE_ASSERT(vulkanContext, "Failed to bind pipeline, vulkan context is null");
		CORE_ASSERT(vulkanContext->CommandBuffers().size(), "Failed to bind pipeline, vulkan command buffers are empty");
		
		for (size_t i = 0; i < vulkanContext->CommandBuffers().size(); i++) {

			//bind pipeline
			vkCmdBindPipeline(vulkanContext->CommandBuffers()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

			////draw test
			//vkCmdDraw(vulkanContext->CommandBuffers()[i], 3, 1, 0, 0);
		}

		s_currentBoundPipeline = (IPipeline*)this;
	}

	void VulkanPipeline::Unbind() const
	{
		//TODO when unbinding a pipeline we need to create a new command to end the render pass

	}

	VkShaderModule VulkanPipeline::createShaderModule(const VulkanDevice& device, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		VK_CHECK_RESULT(vkCreateShaderModule(device.LogicalDevice(), &createInfo, nullptr, &shaderModule));
		return shaderModule;
	}

	VkVertexInputBindingDescription VulkanPipeline::getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = m_inputLayout.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> VulkanPipeline::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributes(m_inputLayout.GetElements().size());
		//convert input element to attribute description
		int location = 0;
		for (auto& inputElement : m_inputLayout)
		{
			VkVertexInputAttributeDescription attributeDescription;
			attributeDescription.binding = 0; //vertex data is always going to be on binding 0
			attributeDescription.location = location;
			attributeDescription.offset = inputElement.Offset;

			//convert type to vulkan type
			switch (inputElement.Type)
			{
			case PipelineDataType::eNone:
				attributeDescription.format = VK_FORMAT_UNDEFINED;
				break;
			case PipelineDataType::eFloat:
				attributeDescription.format = VK_FORMAT_R32_SFLOAT;
				break;
			case PipelineDataType::eFloat2:
				attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
				break;
			case PipelineDataType::eFloat3:
				attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case PipelineDataType::eFloat4:
				attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			case PipelineDataType::eMat3:
				LOG_CORE_FATAL("Mat3 value is currently unsupported in vertex attribute arrays");
				break;
			case PipelineDataType::eMat4:
				LOG_CORE_FATAL("Mat4 value is currently unsupported in vertex attribute arrays");
				break;
			case PipelineDataType::eInt:
				attributeDescription.format = VK_FORMAT_R32_SINT;
				break;
			case PipelineDataType::eInt2:
				attributeDescription.format = VK_FORMAT_R32G32_SINT;
				break;
			case PipelineDataType::eInt3:
				attributeDescription.format = VK_FORMAT_R32G32B32_SINT;
				break;
			case PipelineDataType::eInt4:
				attributeDescription.format = VK_FORMAT_R32G32B32A32_SINT;
				break;
			case PipelineDataType::eBool:
				LOG_CORE_FATAL("Boolean value is currently unsupported in vertex attribute arrays");
				break;
			default: ;
			}

			attributes[location++] = attributeDescription;
		}
		return attributes;
	}

	void VulkanPipeline::createPipeline()
	{
		const VulkanContext* vulkanContext = reinterpret_cast<const VulkanContext*>(m_context);

		//read files
		auto vertShaderCode = Utils::ReadFile(m_vertexShader);
		auto fragShaderCode = Utils::ReadFile(m_fragmentShader);

		///
		///Shader modules
		///

		//create shader modules
		VkShaderModule vertShaderModule = createShaderModule(vulkanContext->Device(), vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(vulkanContext->Device(), fragShaderCode);

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

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		///
		///Fixed functions
		///

		//vertex input
		auto bindingDescription = getBindingDescription();
		auto attributeDescriptions = getAttributeDescriptions();
		
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

		//input assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		///
		///Viewports and scissors
		///

		//viewport
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)vulkanContext->Swapchain().Width();
		viewport.height = (float)vulkanContext->Swapchain().Height();
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		//scissor
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = VkExtent2D{ vulkanContext->Swapchain().Width() ,vulkanContext->Swapchain().Height() };

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		///
		///Rasterizer
		///

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		///
		///Multisampling
		///

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		///
		///Depth and stencil testing (NOT IN USE YES)
		///

		///
		///Color blending
		///

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		//depth testing
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

		///
		///Dynamic states
		///
		VkDynamicState dynamicStates[]
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		///
		///Pipeline layout
		///

		// push constraints for materials
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(MaterialProperties);

		std::array<VkDescriptorSetLayout, 2> setLayouts = { vulkanContext->SceneDescriptorSetLayout(), vulkanContext->MaterialDescriptorSetLayout() };
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = setLayouts.size(); // Optional
		pipelineLayoutInfo.pSetLayouts = setLayouts.data(); // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Optional

		VK_CHECK_RESULT(vkCreatePipelineLayout(vulkanContext->Device().LogicalDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout));

		//create pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = m_pipelineLayout;

		pipelineInfo.renderPass = vulkanContext->Renderpass().Renderpass();
		pipelineInfo.subpass = 0;
		
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(vulkanContext->Device().LogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));

		vkDestroyShaderModule(vulkanContext->Device().LogicalDevice(), fragShaderModule, nullptr);
		vkDestroyShaderModule(vulkanContext->Device().LogicalDevice(), vertShaderModule, nullptr);

		LOG_CORE_INFO("Created Vulkan Pipeline: " + m_name);
		m_deleted = false;
	}

}
