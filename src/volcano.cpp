#include "volcano.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>


void Volcano::run()
{
  initWindow();
  initVulkan();
  loop();
  onExit();
}

void Volcano::loop()
{
  while (!glfwWindowShouldClose(m_Window))
  {
    glfwPollEvents();
  }
}


void Volcano::initWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_Window = glfwCreateWindow(WINDOW_LENGTH, WINDOW_HEIGHT, APP_NAME, nullptr, nullptr);
 }

void Volcano::initVulkan()
{
  createInstance();
  setupDebugMessenger();
  createSurface();
  selectPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
 }

void Volcano::createInstance()
{
  if (validationLayersOn && !checkValidationLayerSupport())
  {
    throw std::runtime_error("Validation layers requested but not available");
  }

  VkApplicationInfo vkAppInfo{};
  vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  vkAppInfo.pApplicationName = APP_NAME;
  vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  vkAppInfo.pEngineName = "NoneRN";
  vkAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  vkAppInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo vkCreateInfo{};
  vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkCreateInfo.pApplicationInfo = &vkAppInfo;

  auto extensions = getRequiredExtensions();
  vkCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  vkCreateInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

  if (validationLayersOn)
  {
    vkCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
    vkCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();

    populateDebugMesssengerCreateInfo(debugCreateInfo);
    vkCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
  }
  else
  {
    vkCreateInfo.enabledLayerCount = 0;
    vkCreateInfo.pNext = nullptr;
  }


  if (vkCreateInstance(&vkCreateInfo, nullptr, &m_VulkanInstance) != VK_SUCCESS)
  {
    throw std::runtime_error("Cannot create Vulkan Instance");
  }
}

void Volcano::createSurface()
{
  if (glfwCreateWindowSurface(m_VulkanInstance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
  {
      throw std::runtime_error("Failed to create window surface");
  }

}


void Volcano::selectPhysicalDevice()
{
  uint32_t supportedDevicesCount = 0;

  vkEnumeratePhysicalDevices(m_VulkanInstance, &supportedDevicesCount, nullptr);

  if (supportedDevicesCount == 0)
    throw std::runtime_error("No supported Vulkan GPUs");

  std::vector<VkPhysicalDevice> devices(supportedDevicesCount);
  vkEnumeratePhysicalDevices(m_VulkanInstance, &supportedDevicesCount, devices.data());

  for (const auto& device : devices)
  {
    if (isDeviceSuitable(device))
    {
      m_PhysicalDevice = device;
      break;
    }
  }

  if (m_PhysicalDevice == VK_NULL_HANDLE)
  {
    throw std::runtime_error("Failed to find suitable GPU!");
  }

}

bool Volcano::isDeviceSuitable(VkPhysicalDevice pDevice)
{
  QueueFamilyIndices indices = findQueueFamilies(pDevice);

  bool extensionsSupported = checkDeviceExtensionsSupport(pDevice);

  bool swapChainAdequate = false;
  if (extensionsSupported)
  {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(pDevice);
    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsSupported && swapChainAdequate;
}


VkSurfaceFormatKHR Volcano::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
  for (const auto& availableFormat : availableFormats)
  {
    if (availableFormat.format == VK_FORMAT_B8G8R8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      return availableFormat;
    }
  }

  return availableFormats[0];
}


void Volcano::createSwapChain()
{
  SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

  if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
  {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = m_Surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;





  QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0; // Optional
      createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;
  
  if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain))
  {
    throw std::runtime_error("Failed to create Swapchain!");
  }


  vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
  m_SwapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());



}

VkPresentModeKHR Volcano::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
  for (const auto& availablePresentMode : availablePresentModes)
  {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      return availablePresentMode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;

}

VkExtent2D Volcano::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
  {
    return capabilities.currentExtent;
  }
  else 
  {
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);

    VkExtent2D actualExtent = 
    {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height= std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
  }
  
}

QueueFamilyIndices Volcano::findQueueFamilies(VkPhysicalDevice device)
{
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;

  for (const auto& queueFamily : queueFamilies)
  {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

    if (presentSupport)
      indices.presentFamily = i;

    if (indices.isComplete())
    {
      break;
    }

    i++;
  }
  return indices;
}



bool Volcano::checkDeviceExtensionsSupport(VkPhysicalDevice pDevice)
{
  uint32_t extensionCount; 
  vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extensionCount, availableExtensions.data());
  std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

  for (const auto& extension : availableExtensions)
  {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}


bool Volcano::checkValidationLayerSupport()
{
  uint32_t  layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* layer : m_ValidationLayers )
  {
    uint32_t layerFound = false;

    for (const auto& VkLayerProperties : availableLayers)
    {
      if (strcmp(layer, VkLayerProperties.layerName) == 0) 
      {
      layerFound = true;
      break;
      }
    }
    if (!layerFound)
      return false;
  }
  return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData)
{

  std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

std::vector<const char*> Volcano::getRequiredExtensions()
{
  uint32_t glfwExtensionsCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

  if (validationLayersOn)
  {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  return extensions;
}


 void Volcano::createLogicalDevice() 
{
  QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
  createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

  if (validationLayersOn) {
      createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
      createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
  } else {
      createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
      throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
  vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
}


SwapChainSupportDetails Volcano::querySwapChainSupport(VkPhysicalDevice pDevice)
{
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice, m_Surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, m_Surface, &formatCount, details.formats.data());

  if (formatCount != 0)
  {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, m_Surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, m_Surface, &presentModeCount, details.presentModes.data());


  return details;
}

void Volcano::setupDebugMessenger()
{
  if (!validationLayersOn) return;

  VkDebugUtilsMessengerCreateInfoEXT vkCreateInfo;
  populateDebugMesssengerCreateInfo(vkCreateInfo);

  if (CreateDebugUtilsMessengerEXT(m_VulkanInstance, &vkCreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
  {
     throw std::runtime_error("Failed to set up debug messenger");
  }
}

void Volcano::populateDebugMesssengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr;



}


VkResult Volcano::CreateDebugUtilsMessengerEXT
  (
   VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
  const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger
  )
  {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func == nullptr)
    {
      std::cout << "NULLPTR" << std::endl;
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

  }


 void Volcano::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) 
    {
        func(instance, debugMessenger, pAllocator);
    }
}


void Volcano::onExit()
{
  if (validationLayersOn)
  {
    DestroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
  }


  vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
  vkDestroyDevice(m_Device, nullptr);
  vkDestroySurfaceKHR(m_VulkanInstance, m_Surface, nullptr);
  vkDestroyInstance(m_VulkanInstance, nullptr);
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}

