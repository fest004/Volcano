#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include <iostream>


#define WINDOW_LENGTH 900
#define WINDOW_HEIGHT 600
#define APP_NAME "Volcano" 


#ifdef NDEBUG
  const bool validationLayersOn = false;
#else
  const bool validationLayersOn = true;
#endif


struct QueueFamilyIndices
{
  //std::optional is a wrapper that contains no value until assigned
  //Query if it has a value set or now with has_value()
  std::optional<uint32_t> graphicsFamily;

  bool isComplete()
  {
    return graphicsFamily.has_value();
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
void CreateLogicalDevice();
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
 

  GLFWwindow *m_Window;
  VkInstance m_VulkanInstance;
  VkPhysicalDevice m_PhysicalDevice;
  VkDevice m_Device;
  VkQueue m_GraphicsQueue;
  const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
  VkDebugUtilsMessengerEXT m_DebugMessenger;



};
