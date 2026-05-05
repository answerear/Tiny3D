/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "T3DVKContext.h"
#include "T3DVKRenderWindow.h"
#include "T3DVKError.h"
#include "T3DVKRenderBuffer.h"
#include "T3DVKMapping.h"
#include "T3DVKRenderState.h"
#include "T3DVKShader.h"
#include <vector>
#include <set>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKContextPtr VKContext::create()
    {
        VKContextPtr ctx = T3D_NEW VKContext();
        if (ctx && T3D_FAILED(ctx->init()))
        {
            ctx = nullptr;
        }
        return ctx;
    }

    //--------------------------------------------------------------------------

    VKContext::VKContext()
    {
    }

    //--------------------------------------------------------------------------

    VKContext::~VKContext()
    {
        mCurrentRenderTarget = nullptr;

        if (mVkDevice != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(mVkDevice);

            for (size_t i = 0; i < mVkInFlightFences.size(); ++i)
            {
                vkDestroyFence(mVkDevice, mVkInFlightFences[i], nullptr);
                vkDestroySemaphore(mVkDevice, mVkRenderFinishedSemaphores[i], nullptr);
                vkDestroySemaphore(mVkDevice, mVkImageAvailableSemaphores[i], nullptr);
            }
            mVkInFlightFences.clear();
            mVkRenderFinishedSemaphores.clear();
            mVkImageAvailableSemaphores.clear();
            mVkCommandBuffers.clear();

            if (mVkDescriptorPool != VK_NULL_HANDLE)
                vkDestroyDescriptorPool(mVkDevice, mVkDescriptorPool, nullptr);
            if (mVkDescriptorSetLayout != VK_NULL_HANDLE)
                vkDestroyDescriptorSetLayout(mVkDevice, mVkDescriptorSetLayout, nullptr);
            if (mVkPipelineCache != VK_NULL_HANDLE)
                vkDestroyPipelineCache(mVkDevice, mVkPipelineCache, nullptr);
            if (mVkPipelineLayout != VK_NULL_HANDLE)
                vkDestroyPipelineLayout(mVkDevice, mVkPipelineLayout, nullptr);
            if (mVkRenderPass != VK_NULL_HANDLE)
                vkDestroyRenderPass(mVkDevice, mVkRenderPass, nullptr);
            if (mVkCommandPool != VK_NULL_HANDLE)
                vkDestroyCommandPool(mVkDevice, mVkCommandPool, nullptr);

            vkDestroyDevice(mVkDevice, nullptr);
            mVkDevice = VK_NULL_HANDLE;
        }

#if defined (T3D_DEBUG)
        if (mVkDebugMessenger != VK_NULL_HANDLE)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mVkInstance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr)
            {
                func(mVkInstance, mVkDebugMessenger, nullptr);
            }
            mVkDebugMessenger = VK_NULL_HANDLE;
        }
#endif

        if (mVkInstance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(mVkInstance, nullptr);
            mVkInstance = VK_NULL_HANDLE;
        }
    }

    //--------------------------------------------------------------------------

    TResult VKContext::init()
    {
        TResult ret = T3D_OK;

        do
        {
            // 1. Create VkInstance
            VkApplicationInfo appInfo {};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "Tiny3D";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "Tiny3D";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            std::vector<const char *> extensions;
            extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
            extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

            std::vector<const char *> validationLayers;

#if defined (T3D_DEBUG)
            validationLayers.push_back("VK_LAYER_KHRONOS_validation");
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

            VkInstanceCreateInfo createInfo {};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            VkResult vkResult = vkCreateInstance(&createInfo, nullptr, &mVkInstance);
            if (vkResult != VK_SUCCESS)
            {
                ret = T3D_ERR_VK_CREATE_INSTANCE;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create VkInstance failed ! VK ERROR [%d]", vkResult);
                break;
            }

#if defined (T3D_DEBUG)
            // Setup debug messenger
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
            debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugCreateInfo.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) -> VkBool32
            {
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Validation: %s", pCallbackData->pMessage);
                return VK_FALSE;
            };

            auto createDebugFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mVkInstance, "vkCreateDebugUtilsMessengerEXT");
            if (createDebugFunc != nullptr)
            {
                createDebugFunc(mVkInstance, &debugCreateInfo, nullptr, &mVkDebugMessenger);
            }
#endif

            // 2. Pick physical device
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, nullptr);
            if (deviceCount == 0)
            {
                ret = T3D_ERR_VK_CREATE_DEVICE;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "No Vulkan-capable GPU found !");
                break;
            }

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, devices.data());
            mVkPhysicalDevice = devices[0]; // Pick first device

            // Find queue families
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

            for (uint32_t i = 0; i < queueFamilyCount; ++i)
            {
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    mGraphicsQueueFamily = i;
                    mPresentQueueFamily = i; // Assume same for now
                    break;
                }
            }

            if (mGraphicsQueueFamily == UINT32_MAX)
            {
                ret = T3D_ERR_VK_CREATE_DEVICE;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "No graphics queue family found !");
                break;
            }

            // 3. Create logical device
            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
            std::set<uint32_t> uniqueQueueFamilies = { mGraphicsQueueFamily, mPresentQueueFamily };

            float queuePriority = 1.0f;
            for (uint32_t queueFamily : uniqueQueueFamilies)
            {
                VkDeviceQueueCreateInfo queueCreateInfo {};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfos.push_back(queueCreateInfo);
            }

            VkPhysicalDeviceFeatures deviceFeatures {};

            const char *deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

            VkDeviceCreateInfo deviceCreateInfo {};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
            deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
            deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
            deviceCreateInfo.enabledExtensionCount = 1;
            deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();

            vkResult = vkCreateDevice(mVkPhysicalDevice, &deviceCreateInfo, nullptr, &mVkDevice);
            if (vkResult != VK_SUCCESS)
            {
                ret = T3D_ERR_VK_CREATE_DEVICE;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create VkDevice failed ! VK ERROR [%d]", vkResult);
                break;
            }

            // 4. Get queues
            vkGetDeviceQueue(mVkDevice, mGraphicsQueueFamily, 0, &mVkGraphicsQueue);
            vkGetDeviceQueue(mVkDevice, mPresentQueueFamily, 0, &mVkPresentQueue);

            // 5. Create command pool
            VkCommandPoolCreateInfo poolInfo {};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = mGraphicsQueueFamily;

            vkResult = vkCreateCommandPool(mVkDevice, &poolInfo, nullptr, &mVkCommandPool);
            if (vkResult != VK_SUCCESS)
            {
                ret = T3D_ERR_VK_CREATE_COMMAND_POOL;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create VkCommandPool failed ! VK ERROR [%d]", vkResult);
                break;
            }

            // 6. Allocate command buffers (per frame)
            mVkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
            {
                VkCommandBufferAllocateInfo allocInfo {};
                allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocInfo.commandPool = mVkCommandPool;
                allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

                vkResult = vkAllocateCommandBuffers(mVkDevice, &allocInfo, mVkCommandBuffers.data());
                if (vkResult != VK_SUCCESS)
                {
                    ret = T3D_ERR_VK_ALLOCATE_COMMAND_BUFFER;
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Allocate VkCommandBuffers failed ! VK ERROR [%d]", vkResult);
                    break;
                }
            }

            // 7. Create sync objects (per frame)
            mVkImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            mVkRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            mVkInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

            {
                VkSemaphoreCreateInfo semaphoreInfo {};
                semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

                VkFenceCreateInfo fenceInfo {};
                fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

                for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
                {
                    if (vkCreateSemaphore(mVkDevice, &semaphoreInfo, nullptr, &mVkImageAvailableSemaphores[i]) != VK_SUCCESS ||
                        vkCreateSemaphore(mVkDevice, &semaphoreInfo, nullptr, &mVkRenderFinishedSemaphores[i]) != VK_SUCCESS ||
                        vkCreateFence(mVkDevice, &fenceInfo, nullptr, &mVkInFlightFences[i]) != VK_SUCCESS)
                    {
                        ret = T3D_ERR_VK_CREATE_SEMAPHORE;
                        T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create sync objects for frame %d failed !", i);
                        break;
                    }
                }
                if (T3D_FAILED(ret))
                    break;
            }

            // 8. Create pipeline cache
            VkPipelineCacheCreateInfo pipelineCacheInfo {};
            pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
            vkCreatePipelineCache(mVkDevice, &pipelineCacheInfo, nullptr, &mVkPipelineCache);

            // 9. Create descriptor set layout (basic: one UBO + one sampler)
            VkDescriptorSetLayoutBinding uboLayoutBinding {};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;

            VkDescriptorSetLayoutBinding samplerLayoutBinding {};
            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorSetLayoutBinding bindings[] = { uboLayoutBinding, samplerLayoutBinding };

            VkDescriptorSetLayoutCreateInfo layoutInfo {};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = 2;
            layoutInfo.pBindings = bindings;

            vkCreateDescriptorSetLayout(mVkDevice, &layoutInfo, nullptr, &mVkDescriptorSetLayout);

            // 10. Create pipeline layout
            VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &mVkDescriptorSetLayout;

            vkResult = vkCreatePipelineLayout(mVkDevice, &pipelineLayoutInfo, nullptr, &mVkPipelineLayout);
            if (vkResult != VK_SUCCESS)
            {
                ret = T3D_ERR_VK_CREATE_PIPELINE;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create VkPipelineLayout failed ! VK ERROR [%d]", vkResult);
                break;
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::swapBackBuffer(VKRenderWindow *renderWindow)
    {
        auto lambda = [this](const VKRenderWindowPtr &renderWindow)
        {
            TResult ret = T3D_OK;

            // Present
            VkSemaphore waitSemaphores[] = { mVkRenderFinishedSemaphores[mCurrentFrame] };

            VkPresentInfoKHR presentInfo {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = waitSemaphores;

            VkSwapchainKHR swapChains[] = { renderWindow->VkSwapChain };
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &mCurrentImageIndex;

            vkQueuePresentKHR(mVkPresentQueue, &presentInfo);

            // Advance frame index
            mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, VKRenderWindowPtr(renderWindow));
    }

    //--------------------------------------------------------------------------

    TResult VKContext::beginRender()
    {
        if (mCurrentRenderWindow == nullptr)
            return T3D_OK;

        auto lambda = [this]()
        {
            TResult ret = T3D_OK;

            // Wait for this frame's fence
            vkWaitForFences(mVkDevice, 1, &mVkInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
            vkResetFences(mVkDevice, 1, &mVkInFlightFences[mCurrentFrame]);

            // Acquire next image
            vkAcquireNextImageKHR(mVkDevice, mCurrentRenderWindow->VkSwapChain, UINT64_MAX,
                mVkImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &mCurrentImageIndex);
            mCurrentRenderWindow->VkCurrentImageIndex = mCurrentImageIndex;

            // Reset and begin command buffer
            VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];
            vkResetCommandBuffer(cmdBuf, 0);

            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            vkBeginCommandBuffer(cmdBuf, &beginInfo);

            // Transition swapchain image from UNDEFINED to TRANSFER_DST
            insertImageBarrier(cmdBuf, mCurrentRenderWindow->VkSwapChainImages[mCurrentImageIndex],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                0, VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------

    TResult VKContext::endRender()
    {
        if (mCurrentRenderWindow == nullptr)
            return T3D_OK;

        auto lambda = [this]()
        {
            TResult ret = T3D_OK;

            VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

            // Transition swapchain image to PRESENT_SRC
            insertImageBarrier(cmdBuf, mCurrentRenderWindow->VkSwapChainImages[mCurrentImageIndex],
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_ACCESS_TRANSFER_WRITE_BIT, 0,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

            vkEndCommandBuffer(cmdBuf);

            // Submit
            VkSubmitInfo submitInfo {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore waitSemaphores[] = { mVkImageAvailableSemaphores[mCurrentFrame] };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &cmdBuf;

            VkSemaphore signalSemaphores[] = { mVkRenderFinishedSemaphores[mCurrentFrame] };
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            vkQueueSubmit(mVkGraphicsQueue, 1, &submitInfo, mVkInFlightFences[mCurrentFrame]);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------

    void VKContext::insertImageBarrier(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkImageAspectFlags aspect)
    {
        VkImageMemoryBarrier barrier {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspect;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = srcAccess;
        barrier.dstAccessMask = dstAccess;

        vkCmdPipelineBarrier(cmdBuf, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    //--------------------------------------------------------------------------

    RHIRenderTargetPtr VKContext::createRenderWindow(RenderWindow *renderWindow)
    {
        VKRenderWindowPtr vkRenderWindow = VKRenderWindow::create(renderWindow);

        auto lambda = [this](const RenderWindowPtr &pRenderWindow, const VKRenderWindowPtr &pVKRenderWindow)
        {
            TResult ret = T3D_OK;

            do
            {
                const RenderWindowDesc &desc = pRenderWindow->getDescriptor();

                // Create Win32 surface
                SysWMInfo info;
                pRenderWindow->getSystemInfo(info);

                VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {};
                surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                surfaceCreateInfo.hwnd = (HWND)info.hWnd;
                surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

                VkResult vkResult = vkCreateWin32SurfaceKHR(mVkInstance, &surfaceCreateInfo, nullptr, &pVKRenderWindow->VkSurface);
                if (vkResult != VK_SUCCESS)
                {
                    ret = T3D_ERR_VK_CREATE_SURFACE;
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create Win32 surface failed ! VK ERROR [%d]", vkResult);
                    break;
                }

                // Verify present support
                VkBool32 presentSupport = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(mVkPhysicalDevice, mPresentQueueFamily, pVKRenderWindow->VkSurface, &presentSupport);
                if (!presentSupport)
                {
                    ret = T3D_ERR_VK_CREATE_SURFACE;
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Queue family does not support present !");
                    break;
                }

                ret = createRenderWindow(pVKRenderWindow, desc.Width, desc.Height, desc.MSAA.Count, desc.MSAA.Quality);
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, RenderWindowPtr(renderWindow), VKRenderWindowPtr(vkRenderWindow));

        if (T3D_FAILED(ret))
        {
            vkRenderWindow = nullptr;
        }
        else
        {
            mCurrentRenderWindow = vkRenderWindow;
        }

        return vkRenderWindow;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::createRenderWindow(VKRenderWindow *vkRenderWindow, uint32_t w, uint32_t h, uint32_t MSAACount, uint32_t MSAAQuality)
    {
        TResult ret = T3D_OK;

        do
        {
            // Query swap chain support
            VkSurfaceCapabilitiesKHR capabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkPhysicalDevice, vkRenderWindow->VkSurface, &capabilities);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, vkRenderWindow->VkSurface, &formatCount, nullptr);
            std::vector<VkSurfaceFormatKHR> formats(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, vkRenderWindow->VkSurface, &formatCount, formats.data());

            // Choose format
            VkSurfaceFormatKHR surfaceFormat = formats[0];
            for (const auto &f : formats)
            {
                if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    surfaceFormat = f;
                    break;
                }
            }

            // Choose extent
            VkExtent2D extent;
            if (capabilities.currentExtent.width != UINT32_MAX)
            {
                extent = capabilities.currentExtent;
            }
            else
            {
                extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, w));
                extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, h));
            }

            uint32_t imageCount = capabilities.minImageCount + 1;
            if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
            {
                imageCount = capabilities.maxImageCount;
            }

            // Create swap chain
            VkSwapchainCreateInfoKHR swapCreateInfo {};
            swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapCreateInfo.surface = vkRenderWindow->VkSurface;
            swapCreateInfo.minImageCount = imageCount;
            swapCreateInfo.imageFormat = surfaceFormat.format;
            swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
            swapCreateInfo.imageExtent = extent;
            swapCreateInfo.imageArrayLayers = 1;
            swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapCreateInfo.preTransform = capabilities.currentTransform;
            swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
            swapCreateInfo.clipped = VK_TRUE;
            swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

            VkResult vkResult = vkCreateSwapchainKHR(mVkDevice, &swapCreateInfo, nullptr, &vkRenderWindow->VkSwapChain);
            if (vkResult != VK_SUCCESS)
            {
                ret = T3D_ERR_VK_CREATE_SWAPCHAIN;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create swapchain failed ! VK ERROR [%d]", vkResult);
                break;
            }

            vkRenderWindow->VkSwapChainImageFormat = surfaceFormat.format;
            vkRenderWindow->VkSwapChainExtent = extent;

            // Get swap chain images
            vkGetSwapchainImagesKHR(mVkDevice, vkRenderWindow->VkSwapChain, &imageCount, nullptr);
            vkRenderWindow->VkSwapChainImages.resize(imageCount);
            vkGetSwapchainImagesKHR(mVkDevice, vkRenderWindow->VkSwapChain, &imageCount, vkRenderWindow->VkSwapChainImages.data());

            // Create image views
            vkRenderWindow->VkSwapChainImageViews.resize(imageCount);
            for (uint32_t i = 0; i < imageCount; ++i)
            {
                vkRenderWindow->VkSwapChainImageViews[i] = createVkImageView(vkRenderWindow->VkSwapChainImages[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
            }

            // Create render pass (if not already created)
            if (mVkRenderPass == VK_NULL_HANDLE)
            {
                VkAttachmentDescription colorAttachment {};
                colorAttachment.format = surfaceFormat.format;
                colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                VkAttachmentDescription depthAttachment {};
                depthAttachment.format = VK_FORMAT_D32_SFLOAT;
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkAttachmentReference colorAttachmentRef {};
                colorAttachmentRef.attachment = 0;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentReference depthAttachmentRef {};
                depthAttachmentRef.attachment = 1;
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass {};
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments = &colorAttachmentRef;
                subpass.pDepthStencilAttachment = &depthAttachmentRef;

                VkSubpassDependency dependency {};
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstSubpass = 0;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                dependency.srcAccessMask = 0;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };

                VkRenderPassCreateInfo renderPassInfo {};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.attachmentCount = 2;
                renderPassInfo.pAttachments = attachments;
                renderPassInfo.subpassCount = 1;
                renderPassInfo.pSubpasses = &subpass;
                renderPassInfo.dependencyCount = 1;
                renderPassInfo.pDependencies = &dependency;

                vkResult = vkCreateRenderPass(mVkDevice, &renderPassInfo, nullptr, &mVkRenderPass);
                if (vkResult != VK_SUCCESS)
                {
                    ret = T3D_ERR_VK_CREATE_RENDER_PASS;
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create render pass failed ! VK ERROR [%d]", vkResult);
                    break;
                }
            }

            // Create depth image
            createVkImage(extent.width, extent.height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                vkRenderWindow->VkDepthImage, vkRenderWindow->VkDepthImageMemory);
            vkRenderWindow->VkDepthImageView = createVkImageView(vkRenderWindow->VkDepthImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

            // Create framebuffers
            vkRenderWindow->VkFramebuffers.resize(imageCount);
            for (uint32_t i = 0; i < imageCount; ++i)
            {
                VkImageView attachments[] = { vkRenderWindow->VkSwapChainImageViews[i], vkRenderWindow->VkDepthImageView };

                VkFramebufferCreateInfo fbInfo {};
                fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                fbInfo.renderPass = mVkRenderPass;
                fbInfo.attachmentCount = 2;
                fbInfo.pAttachments = attachments;
                fbInfo.width = extent.width;
                fbInfo.height = extent.height;
                fbInfo.layers = 1;

                vkResult = vkCreateFramebuffer(mVkDevice, &fbInfo, nullptr, &vkRenderWindow->VkFramebuffers[i]);
                if (vkResult != VK_SUCCESS)
                {
                    ret = T3D_ERR_VK_CREATE_FRAMEBUFFER;
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create framebuffer failed ! VK ERROR [%d]", vkResult);
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::resizeRenderWindow(VKRenderWindow *rw, uint32_t w, uint32_t h)
    {
        auto lambda = [this](const VKRenderWindowPtr &rw, uint32_t w, uint32_t h)
        {
            TResult ret = T3D_OK;

            do
            {
                vkDeviceWaitIdle(mVkDevice);

                // Cleanup old resources
                for (auto fb : rw->VkFramebuffers)
                    vkDestroyFramebuffer(mVkDevice, fb, nullptr);
                rw->VkFramebuffers.clear();

                for (auto iv : rw->VkSwapChainImageViews)
                    vkDestroyImageView(mVkDevice, iv, nullptr);
                rw->VkSwapChainImageViews.clear();

                if (rw->VkDepthImageView != VK_NULL_HANDLE)
                    vkDestroyImageView(mVkDevice, rw->VkDepthImageView, nullptr);
                if (rw->VkDepthImage != VK_NULL_HANDLE)
                    vkDestroyImage(mVkDevice, rw->VkDepthImage, nullptr);
                if (rw->VkDepthImageMemory != VK_NULL_HANDLE)
                    vkFreeMemory(mVkDevice, rw->VkDepthImageMemory, nullptr);

                VkSwapchainKHR oldSwapchain = rw->VkSwapChain;
                rw->VkSwapChain = VK_NULL_HANDLE;
                if (oldSwapchain != VK_NULL_HANDLE)
                    vkDestroySwapchainKHR(mVkDevice, oldSwapchain, nullptr);

                // Recreate
                ret = createRenderWindow(rw, w, h, 1, 0);
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, VKRenderWindowPtr(rw), w, h);
    }

    //--------------------------------------------------------------------------

    TResult VKContext::resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h)
    {
        // TODO: implement when render texture resize is needed
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h)
    {
        // TODO: dispatch to resizeRenderWindow or resizeRenderTexture
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr VKContext::createRenderTexture(PixelBuffer2D *buffer)
    {
        VKPixelBuffer2DPtr vkPixelBuffer = VKPixelBuffer2D::create();

        auto lambda = [this](const PixelBuffer2DPtr &buffer, const VKPixelBuffer2DPtr &vkPixelBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                if (buffer == nullptr)
                {
                    ret = T3D_ERR_INVALID_POINTER;
                    break;
                }

                const auto &desc = buffer->getDescriptor();
                VkFormat format = VKMapping::get(desc.format);

                // Determine usage and aspect based on format
                VkImageUsageFlags usage;
                VkImageAspectFlags aspect;

                if (isDepthFormat(format))
                {
                    usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                    aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
                    if (hasStencilComponent(format))
                        aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
                else
                {
                    usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                    aspect = VK_IMAGE_ASPECT_COLOR_BIT;
                }

                // Create image for render texture
                ret = createVkImage(desc.width, desc.height, format, VK_IMAGE_TILING_OPTIMAL,
                    usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    vkPixelBuffer->VkTex, vkPixelBuffer->VkTexMemory);
                if (T3D_FAILED(ret))
                    break;

                // Create image view
                vkPixelBuffer->VkTexView = createVkImageView(vkPixelBuffer->VkTex, format, aspect);

                if (isDepthFormat(format))
                    vkPixelBuffer->VkDSView = vkPixelBuffer->VkTexView;
                else
                    vkPixelBuffer->VkRTView = vkPixelBuffer->VkTexView;
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DPtr(buffer), VKPixelBuffer2DPtr(vkPixelBuffer));
        if (T3D_FAILED(ret))
        {
            vkPixelBuffer = nullptr;
        }

        return vkPixelBuffer;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setRenderTarget(RenderTarget *renderTarget)
    {
        mCurrentRenderTarget = renderTarget;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setRenderTarget(RenderWindow *renderWindow, RenderTexture *depthStencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setRenderTarget(const RenderTexturePtr *renderTexture, uint32_t numOfTextures, RenderTexture *depthStencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::resetRenderTarget()
    {
        mCurrentRenderTarget = nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setViewport(const Viewport &viewport)
    {
        // Viewport is set during command buffer recording in render()
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::clearColor(const ColorRGB &color)
    {
        TResult ret = T3D_OK;

        if (mCurrentRenderTarget == nullptr)
            return ret;

        mClearColor = color;

        RenderTargetPtr renderTarget = mCurrentRenderTarget;

        auto lambda = [this](const ColorRGB &color, const RenderTargetPtr &renderTarget)
        {
            TResult ret = T3D_OK;

            if (renderTarget == nullptr)
                return ret;

            VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

            if (renderTarget->getType() == RenderTarget::Type::E_RT_WINDOW)
            {
                // Clear back buffer — it's already in TRANSFER_DST layout
                VKRenderWindow *vkWin = static_cast<VKRenderWindow *>(renderTarget->getRenderWindow()->getRHIRenderWindow());
                VkImage image = vkWin->VkSwapChainImages[vkWin->VkCurrentImageIndex];

                VkClearColorValue clearColor = {{color.red(), color.green(), color.blue(), 1.0f}};
                VkImageSubresourceRange range {};
                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                range.levelCount = 1;
                range.layerCount = 1;
                vkCmdClearColorImage(cmdBuf, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &range);
            }
            else
            {
                // Clear render texture(s)
                for (uint32_t i = 0; i < renderTarget->getNumOfRenderTextures(); ++i)
                {
                    const RenderTexturePtr &rt = renderTarget->getRenderTexture(i);
                    VKPixelBuffer2D *vkPB = static_cast<VKPixelBuffer2D *>(rt->getPixelBuffer()->getRHIResource().get());

                    // Transition to TRANSFER_DST
                    insertImageBarrier(cmdBuf, vkPB->VkTex,
                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        0, VK_ACCESS_TRANSFER_WRITE_BIT,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

                    VkClearColorValue clearColor = {{color.red(), color.green(), color.blue(), 1.0f}};
                    VkImageSubresourceRange range {};
                    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    range.levelCount = 1;
                    range.layerCount = 1;
                    vkCmdClearColorImage(cmdBuf, vkPB->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &range);
                }
            }

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, color, renderTarget);
    }

    //--------------------------------------------------------------------------

    TResult VKContext::clearDepth(Real depth)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        if (mCurrentRenderTarget->getDepthStencil() == nullptr)
            return T3D_OK;

        RenderTexturePtr depthStencil = mCurrentRenderTarget->getDepthStencil();

        auto lambda = [this](Real depth, const RenderTexturePtr &depthStencil)
        {
            if (depthStencil == nullptr)
                return T3D_OK;

            VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];
            VKPixelBuffer2D *vkDS = static_cast<VKPixelBuffer2D *>(depthStencil->getPixelBuffer()->getRHIResource().get());

            // D24S8 requires both DEPTH+STENCIL in barrier aspectMask
            VkImageAspectFlags aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

            insertImageBarrier(cmdBuf, vkDS->VkTex,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                0, VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                aspect);

            VkClearDepthStencilValue clearValue = {depth, 0};
            VkImageSubresourceRange range {};
            range.aspectMask = aspect;
            range.levelCount = 1;
            range.layerCount = 1;
            vkCmdClearDepthStencilImage(cmdBuf, vkDS->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &range);

            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, depth, depthStencil);
    }

    //--------------------------------------------------------------------------

    TResult VKContext::clearDepthStencil(Real depth, uint32_t stencil)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        if (mCurrentRenderTarget->getDepthStencil() == nullptr)
            return T3D_OK;

        RenderTexturePtr depthStencilRT = mCurrentRenderTarget->getDepthStencil();

        auto lambda = [this](Real depth, uint32_t stencil, const RenderTexturePtr &ds)
        {
            if (ds == nullptr)
                return T3D_OK;

            VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];
            VKPixelBuffer2D *vkDS = static_cast<VKPixelBuffer2D *>(ds->getPixelBuffer()->getRHIResource().get());

            insertImageBarrier(cmdBuf, vkDS->VkTex,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                0, VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

            VkClearDepthStencilValue clearValue = {depth, stencil};
            VkImageSubresourceRange range {};
            range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            range.levelCount = 1;
            range.layerCount = 1;
            vkCmdClearDepthStencilImage(cmdBuf, vkDS->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &range);

            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, depth, stencil, depthStencilRT);
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr VKContext::createBlendState(BlendState *state)
    {
        // Vulkan blend state is part of pipeline creation, store desc for later
        VKBlendStatePtr vkState = VKBlendState::create();
        return vkState;
    }

    //--------------------------------------------------------------------------

    RHIDepthStencilStatePtr VKContext::createDepthStencilState(DepthStencilState *state)
    {
        VKDepthStencilStatePtr vkState = VKDepthStencilState::create();
        return vkState;
    }

    //--------------------------------------------------------------------------

    RHIRasterizerStatePtr VKContext::createRasterizerState(RasterizerState *state)
    {
        VKRasterizerStatePtr vkState = VKRasterizerState::create();
        return vkState;
    }

    //--------------------------------------------------------------------------

    RHISamplerStatePtr VKContext::createSamplerState(SamplerState *state)
    {
        VKSamplerStatePtr vkState = VKSamplerState::create();
        return vkState;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setBlendState(BlendState *state)
    {
        // State is recorded for pipeline creation at render time
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setDepthStencilState(DepthStencilState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setRasterizerState(RasterizerState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr VKContext::createVertexDeclaration(VertexDeclaration *decl)
    {
        VKVertexDeclarationPtr vkDecl = VKVertexDeclaration::create();
        return vkDecl;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVertexDeclaration(VertexDeclaration *decl)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexBufferPtr VKContext::createVertexBuffer(VertexBuffer *buffer)
    {
        VKVertexBufferPtr vkBuffer = VKVertexBuffer::create();

        do
        {
            VkDeviceSize bufferSize = buffer->getBufferSize();

            auto lambda = [this](VkDeviceSize bufferSize, const VKVertexBufferPtr &vkBuffer, const VertexBufferPtr &buffer)
            {
                TResult ret = T3D_OK;

                do
                {
                    VkResult vkResult;
                    ret = createVkBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        vkBuffer->VkBuf, vkBuffer->VkBufMemory);
                    if (T3D_FAILED(ret))
                    {
                        T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create vertex buffer !");
                        break;
                    }

                    // Copy data if available
                    if (buffer->getBuffer().Data != nullptr)
                    {
                        void *data = nullptr;
                        vkResult = vkMapMemory(mVkDevice, vkBuffer->VkBufMemory, 0, bufferSize, 0, &data);
                        if (vkResult == VK_SUCCESS)
                        {
                            memcpy(data, buffer->getBuffer().Data, (size_t)bufferSize);
                            vkUnmapMemory(mVkDevice, vkBuffer->VkBufMemory);
                        }
                    }
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, bufferSize, vkBuffer, VertexBufferPtr(buffer));
            if (T3D_FAILED(ret))
            {
                vkBuffer = nullptr;
                break;
            }
        } while (false);

        return vkBuffer;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)
    {
        // Vertex buffers are bound during command buffer recording
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIIndexBufferPtr VKContext::createIndexBuffer(IndexBuffer *buffer)
    {
        VKIndexBufferPtr vkBuffer = VKIndexBuffer::create();

        do
        {
            VkDeviceSize bufferSize = buffer->getBufferSize();

            auto lambda = [this](VkDeviceSize bufferSize, const VKIndexBufferPtr &vkBuffer, const IndexBufferPtr &buffer)
            {
                TResult ret = T3D_OK;

                do
                {
                    ret = createVkBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        vkBuffer->VkBuf, vkBuffer->VkBufMemory);
                    if (T3D_FAILED(ret))
                    {
                        T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create index buffer !");
                        break;
                    }

                    if (buffer->getBuffer().Data != nullptr)
                    {
                        void *data = nullptr;
                        VkResult vkResult = vkMapMemory(mVkDevice, vkBuffer->VkBufMemory, 0, bufferSize, 0, &data);
                        if (vkResult == VK_SUCCESS)
                        {
                            memcpy(data, buffer->getBuffer().Data, (size_t)bufferSize);
                            vkUnmapMemory(mVkDevice, vkBuffer->VkBufMemory);
                        }
                    }
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, bufferSize, vkBuffer, IndexBufferPtr(buffer));
            if (T3D_FAILED(ret))
            {
                vkBuffer = nullptr;
                break;
            }
        } while (false);

        return vkBuffer;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setIndexBuffer(IndexBuffer *buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIConstantBufferPtr VKContext::createConstantBuffer(ConstantBuffer *buffer)
    {
        VKConstantBufferPtr vkBuffer = VKConstantBuffer::create();

        do
        {
            VkDeviceSize bufferSize = buffer->getBufferSize();

            auto lambda = [this](VkDeviceSize bufferSize, const VKConstantBufferPtr &vkBuffer, const ConstantBufferPtr &buffer)
            {
                TResult ret = T3D_OK;

                do
                {
                    ret = createVkBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        vkBuffer->VkBuf, vkBuffer->VkBufMemory);
                    if (T3D_FAILED(ret))
                    {
                        T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create constant buffer !");
                        break;
                    }

                    if (buffer->getBuffer().Data != nullptr)
                    {
                        void *data = nullptr;
                        VkResult vkResult = vkMapMemory(mVkDevice, vkBuffer->VkBufMemory, 0, bufferSize, 0, &data);
                        if (vkResult == VK_SUCCESS)
                        {
                            memcpy(data, buffer->getBuffer().Data, (size_t)bufferSize);
                            vkUnmapMemory(mVkDevice, vkBuffer->VkBufMemory);
                        }
                    }
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, bufferSize, vkBuffer, ConstantBufferPtr(buffer));
            if (T3D_FAILED(ret))
            {
                vkBuffer = nullptr;
                break;
            }
        } while (false);

        return vkBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer1DPtr VKContext::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        VKPixelBuffer1DPtr vkBuffer = VKPixelBuffer1D::create();
        return vkBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr VKContext::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        VKPixelBuffer2DPtr vkBuffer = VKPixelBuffer2D::create();

        do
        {
            const auto &desc = buffer->getDescriptor();

            auto lambda = [this](const VKPixelBuffer2DPtr &vkBuffer, const PixelBuffer2DPtr &buffer)
            {
                TResult ret = T3D_OK;

                do
                {
                    const auto &desc = buffer->getDescriptor();
                    VkFormat format = VKMapping::get(desc.format);

                    ret = createVkImage(desc.width, desc.height, format, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        vkBuffer->VkTex, vkBuffer->VkTexMemory);
                    if (T3D_FAILED(ret))
                    {
                        T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create 2D texture !");
                        break;
                    }

                    vkBuffer->VkTexView = createVkImageView(vkBuffer->VkTex, format, VK_IMAGE_ASPECT_COLOR_BIT);

                    // Upload data if available
                    if (buffer->getBuffer().Data != nullptr)
                    {
                        VkDeviceSize imageSize = buffer->getBuffer().DataSize;
                        VkBuffer stagingBuffer;
                        VkDeviceMemory stagingBufferMemory;
                        createVkBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            stagingBuffer, stagingBufferMemory);

                        void *data = nullptr;
                        vkMapMemory(mVkDevice, stagingBufferMemory, 0, imageSize, 0, &data);
                        memcpy(data, buffer->getBuffer().Data, (size_t)imageSize);
                        vkUnmapMemory(mVkDevice, stagingBufferMemory);

                        transitionImageLayout(vkBuffer->VkTex, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

                        VkCommandBuffer cmdBuf = beginSingleTimeCommands();
                        VkBufferImageCopy region {};
                        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        region.imageSubresource.layerCount = 1;
                        region.imageExtent = { desc.width, desc.height, 1 };
                        vkCmdCopyBufferToImage(cmdBuf, stagingBuffer, vkBuffer->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
                        endSingleTimeCommands(cmdBuf);

                        transitionImageLayout(vkBuffer->VkTex, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                        vkDestroyBuffer(mVkDevice, stagingBuffer, nullptr);
                        vkFreeMemory(mVkDevice, stagingBufferMemory, nullptr);
                    }
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, vkBuffer, PixelBuffer2DPtr(buffer));
            if (T3D_FAILED(ret))
            {
                vkBuffer = nullptr;
                break;
            }
        } while (false);

        return vkBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer3DPtr VKContext::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createVertexShader(ShaderVariant *shader)
    {
        VKVertexShaderPtr vkShader = VKVertexShader::create();

        do
        {
            auto lambda = [this](const ShaderVariantPtr &shader, const VKVertexShaderPtr &vkShader)
            {
                TResult ret = T3D_OK;

                do
                {
                    size_t bytecodeLength = 0;
                    const char *bytecode = shader->getBytesCode(bytecodeLength);

                    VkShaderModuleCreateInfo createInfo {};
                    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                    createInfo.codeSize = bytecodeLength;
                    createInfo.pCode = reinterpret_cast<const uint32_t *>(bytecode);

                    VkResult vkResult = vkCreateShaderModule(mVkDevice, &createInfo, nullptr, &vkShader->VkModule);
                    if (vkResult != VK_SUCCESS)
                    {
                        T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create vertex shader module ! VK ERROR [%d]", vkResult);
                        ret = T3D_ERR_VK_CREATE_SHADER_MODULE;
                        break;
                    }
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), vkShader);
            if (T3D_FAILED(ret))
            {
                vkShader = nullptr;
                break;
            }
        } while (false);

        return vkShader;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVertexShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createPixelShader(ShaderVariant *shader)
    {
        VKPixelShaderPtr vkShader = VKPixelShader::create();

        do
        {
            auto lambda = [this](const ShaderVariantPtr &shader, const VKPixelShaderPtr &vkShader)
            {
                TResult ret = T3D_OK;

                do
                {
                    size_t bytecodeLength = 0;
                    const char *bytecode = shader->getBytesCode(bytecodeLength);

                    VkShaderModuleCreateInfo createInfo {};
                    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                    createInfo.codeSize = bytecodeLength;
                    createInfo.pCode = reinterpret_cast<const uint32_t *>(bytecode);

                    VkResult vkResult = vkCreateShaderModule(mVkDevice, &createInfo, nullptr, &vkShader->VkModule);
                    if (vkResult != VK_SUCCESS)
                    {
                        T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create pixel shader module ! VK ERROR [%d]", vkResult);
                        ret = T3D_ERR_VK_CREATE_SHADER_MODULE;
                        break;
                    }
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), vkShader);
            if (T3D_FAILED(ret))
            {
                vkShader = nullptr;
                break;
            }
        } while (false);

        return vkShader;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setPixelShader(ShaderVariant *shader) { return T3D_OK; }
    TResult VKContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createHullShader(ShaderVariant *shader)
    {
        VKHullShaderPtr vkShader = VKHullShader::create();
        // Same pattern as vertex/pixel shader
        return vkShader;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setHullShader(ShaderVariant *shader) { return T3D_OK; }
    TResult VKContext::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setHSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createDomainShader(ShaderVariant *shader)
    {
        VKDomainShaderPtr vkShader = VKDomainShader::create();
        return vkShader;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setDomainShader(ShaderVariant *shader) { return T3D_OK; }
    TResult VKContext::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setDSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createGeometryShader(ShaderVariant *shader)
    {
        VKGeometryShaderPtr vkShader = VKGeometryShader::create();
        return vkShader;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setGeometryShader(ShaderVariant *shader) { return T3D_OK; }
    TResult VKContext::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setGSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------

    RHIShaderPtr VKContext::createComputeShader(ShaderVariant *shader)
    {
        VKComputeShaderPtr vkShader = VKComputeShader::create();
        return vkShader;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setComputeShader(ShaderVariant *shader) { return T3D_OK; }
    TResult VKContext::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult VKContext::setCSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }

    //--------------------------------------------------------------------------

    TResult VKContext::setPrimitiveType(PrimitiveType primitive)
    {
        mVkPrimitiveTopology = VKMapping::get(primitive);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        auto lambda = [this](uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
        {
            vkCmdDrawIndexed(mVkCommandBuffers[mCurrentFrame], indexCount, 1, startIndex, baseVertex, 0);
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, indexCount, startIndex, baseVertex);
    }

    //--------------------------------------------------------------------------

    TResult VKContext::render(uint32_t vertexCount, uint32_t startVertex)
    {
        auto lambda = [this](uint32_t vertexCount, uint32_t startVertex)
        {
            vkCmdDraw(mVkCommandBuffers[mCurrentFrame], vertexCount, 1, startVertex, 0);
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, vertexCount, startVertex);
    }

    //--------------------------------------------------------------------------

    TResult VKContext::reset()
    {
        mCurrentRenderTarget = nullptr;
        mVkCurrentPipeline = VK_NULL_HANDLE;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        // Delegate: get render texture from src and call blit(Texture*, RenderTarget*)
        if (src == nullptr || dst == nullptr)
            return T3D_OK;

        if (src->getType() == RenderTarget::Type::E_RT_TEXTURE && src->getNumOfRenderTextures() > 0)
        {
            return blit(static_cast<Texture *>(src->getRenderTexture().get()), dst, srcOffset, size, dstOffset);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
            return T3D_OK;

        auto lambda = [this](const RenderTexturePtr &srcTex, const RenderTargetPtr &dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
        {
            TResult ret = T3D_OK;

            do
            {
                VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

                // Get source VkImage
                VKPixelBuffer2D *vkSrcPB = static_cast<VKPixelBuffer2D *>(srcTex->getPixelBuffer()->getRHIResource().get());
                VkImage srcImage = vkSrcPB->VkTex;

                // Get destination VkImage
                VkImage dstImage = VK_NULL_HANDLE;
                uint32_t dstWidth = 0, dstHeight = 0;

                if (dst->getType() == RenderTarget::Type::E_RT_WINDOW)
                {
                    VKRenderWindow *vkWin = static_cast<VKRenderWindow *>(dst->getRenderWindow()->getRHIRenderWindow());
                    dstImage = vkWin->VkSwapChainImages[vkWin->VkCurrentImageIndex];
                    dstWidth = vkWin->VkSwapChainExtent.width;
                    dstHeight = vkWin->VkSwapChainExtent.height;
                }
                else if (dst->getType() == RenderTarget::Type::E_RT_TEXTURE && dst->getNumOfRenderTextures() > 0)
                {
                    VKPixelBuffer2D *vkDstPB = static_cast<VKPixelBuffer2D *>(dst->getRenderTexture()->getPixelBuffer()->getRHIResource().get());
                    dstImage = vkDstPB->VkTex;
                    PixelBuffer2D *dstPB2D = static_cast<PixelBuffer2D *>(dst->getRenderTexture()->getPixelBuffer());
                    dstWidth = dstPB2D->getDescriptor().width;
                    dstHeight = dstPB2D->getDescriptor().height;
                }

                if (srcImage == VK_NULL_HANDLE || dstImage == VK_NULL_HANDLE)
                    break;

                // Transition src to TRANSFER_SRC
                insertImageBarrier(cmdBuf, srcImage,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

                // dst is already in TRANSFER_DST (set by beginRender for back buffer)

                // Compute blit regions
                PixelBuffer2D *srcPB2D = static_cast<PixelBuffer2D *>(srcTex->getPixelBuffer());
                uint32_t srcW = srcPB2D->getDescriptor().width;
                uint32_t srcH = srcPB2D->getDescriptor().height;

                VkImageBlit blitRegion {};
                blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blitRegion.srcSubresource.layerCount = 1;
                blitRegion.srcOffsets[0] = {(int32_t)srcOffset.x(), (int32_t)srcOffset.y(), 0};

                if (size.x() > 0 && size.y() > 0)
                {
                    blitRegion.srcOffsets[1] = {(int32_t)(srcOffset.x() + size.x()), (int32_t)(srcOffset.y() + size.y()), 1};
                }
                else
                {
                    blitRegion.srcOffsets[1] = {(int32_t)srcW, (int32_t)srcH, 1};
                }

                blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blitRegion.dstSubresource.layerCount = 1;
                blitRegion.dstOffsets[0] = {(int32_t)dstOffset.x(), (int32_t)dstOffset.y(), 0};

                if (size.x() > 0 && size.y() > 0)
                {
                    blitRegion.dstOffsets[1] = {(int32_t)(dstOffset.x() + size.x()), (int32_t)(dstOffset.y() + size.y()), 1};
                }
                else
                {
                    blitRegion.dstOffsets[1] = {(int32_t)dstWidth, (int32_t)dstHeight, 1};
                }

                vkCmdBlitImage(cmdBuf, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blitRegion, VK_FILTER_LINEAR);
            } while (false);

            return ret;
        };

        RenderTexture *srcRT = static_cast<RenderTexture *>(src);
        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderTexturePtr(srcRT), RenderTargetPtr(dst), srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult VKContext::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        // TODO: implement when needed
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        // TODO: implement when needed
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        auto lambda = [this](RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
        {
            VkBuffer srcBuf = (VkBuffer)src->getRHIResource()->getNativeObject();
            VkBuffer dstBuf = (VkBuffer)dst->getRHIResource()->getNativeObject();

            VkCommandBuffer cmdBuf = beginSingleTimeCommands();
            VkBufferCopy copyRegion {};
            copyRegion.srcOffset = srcOffset;
            copyRegion.dstOffset = dstOffset;
            copyRegion.size = (size == 0) ? src->getBufferSize() : size;
            vkCmdCopyBuffer(cmdBuf, srcBuf, dstBuf, 1, &copyRegion);
            endSingleTimeCommands(cmdBuf);

            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, src, dst, srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult VKContext::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)
    {
        auto lambda = [this](RenderBuffer *renderBuffer, Buffer buffer, bool discardWholeBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                VkBuffer vkBuf = (VkBuffer)renderBuffer->getRHIResource()->getNativeObject();
                // Find the device memory associated with this buffer
                // For simplicity, we use the VkBufMemory from the typed buffer
                VkDeviceMemory memory = VK_NULL_HANDLE;

                auto rhiRes = renderBuffer->getRHIResource();
                if (auto *vb = dynamic_cast<VKVertexBuffer *>(rhiRes.get()))
                    memory = vb->VkBufMemory;
                else if (auto *ib = dynamic_cast<VKIndexBuffer *>(rhiRes.get()))
                    memory = ib->VkBufMemory;
                else if (auto *cb = dynamic_cast<VKConstantBuffer *>(rhiRes.get()))
                    memory = cb->VkBufMemory;

                if (memory == VK_NULL_HANDLE)
                {
                    ret = T3D_ERR_VK_MAP_MEMORY;
                    break;
                }

                void *data = nullptr;
                VkResult vkResult = vkMapMemory(mVkDevice, memory, 0, buffer.DataSize, 0, &data);
                if (vkResult != VK_SUCCESS)
                {
                    ret = T3D_ERR_VK_MAP_MEMORY;
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to map buffer memory ! VK ERROR [%d]", vkResult);
                    break;
                }

                memcpy(data, buffer.Data, buffer.DataSize);
                vkUnmapMemory(mVkDevice, memory);
                buffer.release();
            } while (false);

            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, renderBuffer, buffer, discardWholeBuffer);
    }

    //--------------------------------------------------------------------------
    // Helper functions
    //--------------------------------------------------------------------------

    uint32_t VKContext::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        return UINT32_MAX;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
    {
        VkBufferCreateInfo bufferInfo {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult vkResult = vkCreateBuffer(mVkDevice, &bufferInfo, nullptr, &buffer);
        if (vkResult != VK_SUCCESS)
        {
            return T3D_ERR_VK_CREATE_BUFFER;
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(mVkDevice, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        vkResult = vkAllocateMemory(mVkDevice, &allocInfo, nullptr, &bufferMemory);
        if (vkResult != VK_SUCCESS)
        {
            vkDestroyBuffer(mVkDevice, buffer, nullptr);
            buffer = VK_NULL_HANDLE;
            return T3D_ERR_VK_ALLOCATE_MEMORY;
        }

        vkBindBufferMemory(mVkDevice, buffer, bufferMemory, 0);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::createVkImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory)
    {
        VkImageCreateInfo imageInfo {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult vkResult = vkCreateImage(mVkDevice, &imageInfo, nullptr, &image);
        if (vkResult != VK_SUCCESS)
        {
            return T3D_ERR_VK_CREATE_IMAGE;
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(mVkDevice, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        vkResult = vkAllocateMemory(mVkDevice, &allocInfo, nullptr, &imageMemory);
        if (vkResult != VK_SUCCESS)
        {
            vkDestroyImage(mVkDevice, image, nullptr);
            image = VK_NULL_HANDLE;
            return T3D_ERR_VK_ALLOCATE_MEMORY;
        }

        vkBindImageMemory(mVkDevice, image, imageMemory, 0);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    VkImageView VKContext::createVkImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView = VK_NULL_HANDLE;
        vkCreateImageView(mVkDevice, &viewInfo, nullptr, &imageView);
        return imageView;
    }

    //--------------------------------------------------------------------------

    VkCommandBuffer VKContext::beginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = mVkCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(mVkDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    //--------------------------------------------------------------------------

    void VKContext::endSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(mVkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(mVkGraphicsQueue);

        vkFreeCommandBuffers(mVkDevice, mVkCommandPool, 1, &commandBuffer);
    }

    //--------------------------------------------------------------------------

    void VKContext::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = 0;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        endSingleTimeCommands(commandBuffer);
    }

    //--------------------------------------------------------------------------

    bool VKContext::isDepthFormat(VkFormat format) const
    {
        return format == VK_FORMAT_D16_UNORM
            || format == VK_FORMAT_D32_SFLOAT
            || format == VK_FORMAT_D16_UNORM_S8_UINT
            || format == VK_FORMAT_D24_UNORM_S8_UINT
            || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
    }

    //--------------------------------------------------------------------------

    bool VKContext::hasStencilComponent(VkFormat format) const
    {
        return format == VK_FORMAT_D16_UNORM_S8_UINT
            || format == VK_FORMAT_D24_UNORM_S8_UINT
            || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
    }
}
