#include "MyVulkanInit.h"
using MyVlk = MyVulkanInit;


void MyVlk::transitionImageLayout(
	uint32_t imageIndex,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkAccessFlags2 srcAccessMask,
	VkAccessFlags2 dstAccessMask,
	VkPipelineStageFlags2 srcStageMask,
	VkPipelineStageFlags2 dstStageMask) 
{
	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.srcStageMask = srcStageMask;
	barrier.srcAccessMask = srcAccessMask;
	barrier.dstStageMask = dstStageMask;
	barrier.dstAccessMask = dstAccessMask;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = swapChainImages[imageIndex];
	
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
			
	VkDependencyInfo dependencyInfo{};
	dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfo.dependencyFlags = {};
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(commandBuffers[currentFrame], &dependencyInfo); //this is it!!!!!!!!
	
		





	
	};
	
void MyVlk::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,VkMemoryPropertyFlags properties,VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("YOU FAILED TO allocate VERTEX MEMORY jesse");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);

}

void MyVlk::createIndexBuffer() {
	VkDeviceSize bufferSize = indexIndices.size() * sizeof(indexIndices[0]);
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingMemory);


	void* data;
	vkMapMemory(device, stagingMemory, 0, bufferSize, 0, &data); //creates pointer from cpu to gpu
	memcpy(data, indexIndices.data(), (size_t)bufferSize);				   //actually copies to gpu
	vkUnmapMemory(device, stagingMemory);			//uncreates tthe pointer and becomes and implication for cpu to release resoursces or smthn


	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		indexBuffer,
		indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);


}
void MyVlk::createVertexBuffer() {
	VkDeviceSize bufferSize = vertices.size() * sizeof(vertices[0]);
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingMemory);


	void* data;
	vkMapMemory(device, stagingMemory, 0, bufferSize, 0, &data); //creates pointer from cpu to gpu
	memcpy(data, vertices.data(), (size_t)bufferSize);				   //actually copies to gpu
	vkUnmapMemory(device, stagingMemory);			//uncreates tthe pointer and becomes and implication for cpu to release resoursces or smthn


	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer,
		vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);

}

void MyVlk::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

	//gotta create a commandbuffer to send commands in to copy from slower gpu buffer to another high perfoormance gpu buffer
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);


}

uint32_t MyVlk::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}

	}
	throw std::runtime_error(" failed to find suitable memory type LOOOOOOOLLL!!!!");

}

void MyVlk::createCommandBuffers() {

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers yooo skyler white");
	}



}






void MyVlk::recordCommandBuffer(VkCommandBuffer cmdBfr, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(cmdBfr, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin reeecooording command buffers YOOOOOOOOOOOOOOOO");
	}

	transitionImageLayout(imageIndex, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		0,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
	);

	
	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	VkRenderingAttachmentInfo attachInfo{};
	attachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	attachInfo.imageView = swapChainImageViews[imageIndex];
	attachInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachInfo.clearValue = clearColor;
	

	VkRenderingInfo renderInfo{};
	renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderInfo.renderArea.offset = { 0,0 };
	renderInfo.renderArea.extent = swapChainExtent;
	renderInfo.layerCount = 1,
	renderInfo.colorAttachmentCount = 1,
	renderInfo.pColorAttachments = &attachInfo;

	vkCmdBeginRendering(cmdBfr, &renderInfo);

	
	vkCmdBindPipeline(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	
	vkCmdBindVertexBuffers(cmdBfr, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBfr, indexBuffer, 0, VK_INDEX_TYPE_UINT16);


	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmdBfr, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(cmdBfr, 0, 1, &scissor);




	//vkCmdDraw(cmdBfr, static_cast<uint32_t>(vertices.size()),1,0,0);
	vkCmdDrawIndexed(cmdBfr, static_cast<uint32_t>(indexIndices.size()), 1, 0, 0, 0);

	vkCmdEndRendering(cmdBfr);

	transitionImageLayout(imageIndex,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);


	if (vkEndCommandBuffer(cmdBfr) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}


void MyVlk::drawFrame() {
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	auto result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR){
		recreateSwapChain();
		return;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to create swapchain! LMAOOOOOO");
	}


	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);//lottashit happening in this func 
	/////////RELATED!!!!!!
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphore[] = { imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer");
	}



	//RELATED!!!!!!!!

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR  swapChains[] = { swapChain };

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; //optional lmao


	result = vkQueuePresentKHR(presentQueue, &presentInfo);
	if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("FAIL AT vkQueuePresentKHR");
	}



	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void MyVlk::createSyncObjects() {
	VkSemaphoreCreateInfo semaphInfo{};
	semaphInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	for (uint32_t i{}; i < MAX_FRAMES_IN_FLIGHT; i++) {
		
		if (vkCreateSemaphore(device, &semaphInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores and fences yo");
		}

	}


}

void MyVlk::initVulkan() {
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createGraphicsPipeline();
	createCommandPool();
	createVertexBuffer();
	createIndexBuffer();
	createCommandBuffers();
	createSyncObjects();
	
}

void MyVlk::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(device);//boottleneck according to indian

}
//
void MyVlk::cleanup() {

	for (uint32_t i{}; i < MAX_FRAMES_IN_FLIGHT;i++) {
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);

	}

	for (uint32_t i{}; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyFence(device, inFlightFences[i], nullptr);

	}
	
	cleanupSwapChain();
	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);

	vkDestroyCommandPool(device, commandPool, nullptr); //maybe wrong ordcer??
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyPipeline(device, graphicsPipeline, nullptr);

	

	//vkDestroyRenderPass(device, renderPass, nullptr);

	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);//ska den vara eftger destroyinstance??
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);

	glfwTerminate();
}