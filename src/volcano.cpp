#include "volcano.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstring>
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
  selectPhysicalDevice();
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

bool Volcano::isDeviceSuitable(VkPhysicalDevice device)
{
  QueueFamilyIndices indices = findQueueFamilies(device);

  return indices.isComplete();
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

    if (indices.isComplete())
    {
      break;
    }

    i++;
  }
  return indices;
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

void Volcano::CreateLogicalDevice()
{
  QueueFamilyIndices indices;


  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value(); 
  queueCreateInfo.queueCount = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = 0;

  if (validationLayersOn)
  {
    createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
    createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
  }
  else
  {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
  {
    throw std::runtime_error("Unable to create logic device");
  }

  vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
 
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


  vkDestroyDevice(m_Device, nullptr);
  vkDestroyInstance(m_VulkanInstance, nullptr);
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}

