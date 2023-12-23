#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32



#define WINDOW_LENGTH 900
#define WINDOW_HEIGHT 600
#define APP_NAME "Volcano" 


#ifdef NDEBUG
  const bool validationLayersOn = false;
#else
  const bool validationLayersOn = true;
#endif

struct SwapChainSupportDetails
{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};


struct QueueFamilyIndices
{
  //std::optional is a wrapper that contains no value until assigned
  //Query if it has a value set or now with has_value()
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete()
  {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }

};

class Volcano {
public:
  void run();

private:
  void initWindow();
  void initVulkan();
  void loop();
  void onExit();

  void createInstance();
  bool checkValidationLayerSupport();
  void selectPhysicalDevice();
  void createLogicalDevice();
  std::vector<const char*> getRequiredExtensions();
  void setupDebugMessenger();
  static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
  void populateDebugMesssengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  VkResult CreateDebugUtilsMessengerEXT
  (
   VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
  const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger
  );
  
  bool isDeviceSuitable(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  void createSurface();

  bool checkDeviceExtensionsSupport(VkPhysicalDevice pDevice);

  void createSwapChain();
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice pDevice);

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

  void createImageViews();

  //Drawing
  void createFrameBuffers();
  void createCommandPool();
  void createCommandBuffer();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  void createSyncObjects();


  //Pipeline Methods
  void createGraphicalPipeline();
  VkShaderModule createShaderModule(const std::vector<char>& code);
  void createRenderPass();
 
  //Rendering {FFS FINALLY}
  void drawFrame();

  GLFWwindow *m_Window;
  VkInstance m_VulkanInstance;
  VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
  VkDevice m_Device;
  VkQueue m_GraphicsQueue;
  VkSurfaceKHR m_Surface;
  VkQueue m_PresentQueue;
  VkSwapchainKHR m_SwapChain; 
  std::vector<VkImage> m_SwapChainImages;
  VkFormat m_SwapChainImageFormat;
  VkExtent2D m_SwapChainExtent;
  std::vector<VkImageView> m_SwapChainImageViews;
  VkRenderPass m_RenderPass;
  VkPipelineLayout m_PipelineLayout;
  VkPipeline m_GraphicsPipeline;
  VkCommandPool m_CommandPool;
  VkCommandBuffer m_CommandBuffer;

  VkSemaphore m_ImageAvailableSemaphore;
  VkSemaphore m_RenderFinishedSemaphore;
  VkFence m_InFlightFence;
  


  std::vector<VkFramebuffer> m_SwapChainFrameBuffer;
  const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
  const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
  VkDebugUtilsMessengerEXT m_DebugMessenger;



};
