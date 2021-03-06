#pragma once
//#include "settings.hpp"
#include "anvLog.hpp"
#include "anvpch.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace AnvilEngine{

/**
 * 
 * @brief Basically, all this is is just geting the required vulkan stuf
 * out of the way. later on i will make a class specificaly for vulkan. 
 * right now i just want to get vulkan running on the window and render a
 * basic triangle. "small" things first, ya know? 
 * 
 */

    class AnvDevice
    {

        public:
            VkInstance m_instance;
            VkDevice m_device;
            VkPhysicalDevice m_physicalDevice;
            VkQueue m_graphicsQueue;
            VkQueue m_presentQueue;
            VkSurfaceKHR m_surface;

            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            VkFormat swapChainImageFormat;
            VkExtent2D swapChainExtent;
            VkCommandPool commandPool;
            
            const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
            const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};

            VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
            VkDebugUtilsMessengerCreateInfoEXT debCreateInfo{};

            struct QueueFamilyIndices {
                uint32_t graphicsFamily = 0;
                uint32_t presentFamily = 0;

                bool graphicsFamilyHasValue = false;
                bool presentFamilyHasValue = false;

                bool isComplete() {
                    return graphicsFamilyHasValue && presentFamilyHasValue;
                }
            };

            struct SwapChainSupportDetails {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> presentModes;
            };

#ifdef NDEBUG
            const bool enableValidationLayers = false;
#else
            const bool enableValidationLayers = true;
#endif

            VkQueue graphicsQueue() { return m_graphicsQueue; }
            SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_physicalDevice); }

            

            VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                const VkAllocationCallbacks* pAllocator, 
                VkDebugUtilsMessengerEXT* pDebugMessenger);

            void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                VkDebugUtilsMessengerEXT debugMessenger, 
                const VkAllocationCallbacks* pAllocator);

            bool checkValidationLayerSupport();

            std::vector<const char*> getRequiredExtensions();
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData);

            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debCreateInfo);

            void SetupDebugMessenger();

            void CreateInstance();

            void PickPhysicalDevice();

            //bool isDeviceSuitable(VkPhysicalDevice device);

            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

            bool CheckDeviceExtentionSupport(VkPhysicalDevice device);

            void CreateLogicalDevice();

            AnvDevice(GLFWwindow* window) { InitVulkan(window); };
            

            void InitVulkan(GLFWwindow* window);

            void CreateSurface(GLFWwindow* window);

            QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

            void CreateCommandPool();

            std::vector<VkImageView> swapChainImageViews;

            auto IsDeviceSuitable(VkPhysicalDevice device)
            {
                QueueFamilyIndices indices = findQueueFamilies(device);

                //bool extensionsSupported = AnvDevice.CheckDeviceExtentionSupport(device);

                bool swapChainAdequate = false;
                
                SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
                swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
                

                return indices.isComplete() && swapChainAdequate;
            }

            VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

            uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

            VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

            VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

            VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

            SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device)
            {
                SwapChainSupportDetails details;
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

                uint32_t formatCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

                if (formatCount != 0) {
                    details.formats.resize(formatCount);
                    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
                }

                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

                if (presentModeCount != 0) {
                    details.presentModes.resize(presentModeCount);
                    vkGetPhysicalDeviceSurfacePresentModesKHR(
                        device,
                        m_surface,
                        &presentModeCount,
                        details.presentModes.data());
                }
                return details;
            }

            void CreateBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer &buffer,
            VkDeviceMemory &bufferMemory);
            VkCommandBuffer BeginSingleTimeCommands();
            void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
            void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
            void CopyBufferToImage(
                VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

            void CreateImageViews();

            void CreateImageWithInfo(
                const VkImageCreateInfo &imageInfo,
                VkMemoryPropertyFlags properties,
                VkImage &image,
                VkDeviceMemory &imageMemory);

            void Clean();
    };

    

} //AnvilEngine