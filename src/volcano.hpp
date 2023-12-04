#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
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
  std::vector<const char*> getRequiredExtensions();
  void setupDebugMessenger();
  static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
  void populateDebugMesssengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  VkResult CreateDebugUtilsMessengerEXT
  (
   VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
  const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger
  );
 

  GLFWwindow *m_Window;
  VkInstance m_VulkanInstance;
  const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
  VkDebugUtilsMessengerEXT m_DebugMessenger;



};
