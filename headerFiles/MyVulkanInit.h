#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <optional>
#include <set>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <fstream>
extern const std::vector<const char*> validationLayers;
extern const std::vector<const char*> deviceExtensions;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

class MyVulkanInit {

public:
	uint32_t WIDTH = 800;
	uint32_t HEIGHT = 800;
	void run();
	


private:
	GLFWwindow* window;
	VkInstance instance;
	VkDevice device;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass renderPass;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkPipelineLayout pipelineLayout;

	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector <VkSemaphore> imageAvailableSemaphores;
	std::vector <VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkImage> swapChainImages;
	std::vector<VkDynamicState> dynamicStates;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}


	};
	struct swapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilites; //min max number of images in swap chain etc.
		std::vector<VkSurfaceFormatKHR> formats;  // pixel format, color space
		std::vector<VkPresentModeKHR> presentModes;//available present modes 

	};

	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;


		static VkVertexInputBindingDescription  getBindingDescription(){
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2>  attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);//THe pos is actully 0 here so the offsset is nothing

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);



			return attributeDescriptions;
		}


	};
	//pos, color
	const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {0.5f, 0.3f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}

	};
	

	uint32_t currentFrame = 0;
	bool framebufferResized = false;
	
	bool checkValidationLayerSupport();
	void initWindow();
	void createInstance();
	void pickPhysicalDevice();

	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	void createLogicalDevice();
	void createSurface();
	void createVertexBuffer();
	
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabil);

	void transitionImageLayout(
		uint32_t imageIndex,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkAccessFlags2 srcAccessMask,
		VkAccessFlags2 dstAccessMask,
		VkPipelineStageFlags2 srcStageMask,
		VkPipelineStageFlags2 dstStageMask);

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void createSwapChain();
	void recreateSwapChain();
	void cleanupSwapChain();
	void createImageViews();
	void createGraphicsPipeline();
	static std::vector<char> readFile(const std::string& fileName);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	//void createRenderPass();

	void createCommandPool();
	void createCommandBuffers();
	void recordCommandBuffer(VkCommandBuffer cmdBfr, uint32_t imageindex);
	void drawFrame();
	void createSyncObjects();
	void initVulkan();
	void mainLoop();
	void cleanup();

};