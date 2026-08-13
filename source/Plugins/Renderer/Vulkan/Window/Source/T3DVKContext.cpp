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
#include "spirv_reflect.h"
#include <vector>
#include <set>
#include <map>


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    // Engine enum -> Vulkan enum mapping helpers
    //--------------------------------------------------------------------------

    static VkCullModeFlags toVkCullMode(CullingMode mode)
    {
        switch (mode)
        {
        case CullingMode::kNone:  return VK_CULL_MODE_NONE;
        case CullingMode::kFront: return VK_CULL_MODE_FRONT_BIT;
        case CullingMode::kBack:  return VK_CULL_MODE_BACK_BIT;
        default:                  return VK_CULL_MODE_BACK_BIT;
        }
    }

    static VkPolygonMode toVkPolygonMode(PolygonMode mode)
    {
        switch (mode)
        {
        case PolygonMode::kPoint:     return VK_POLYGON_MODE_POINT;
        case PolygonMode::kWireframe: return VK_POLYGON_MODE_LINE;
        case PolygonMode::kSolid:     return VK_POLYGON_MODE_FILL;
        default:                      return VK_POLYGON_MODE_FILL;
        }
    }

    static VkCompareOp toVkCompareOp(CompareFunction func)
    {
        switch (func)
        {
        case CompareFunction::kAlwaysFail:   return VK_COMPARE_OP_NEVER;
        case CompareFunction::kAlwaysPass:   return VK_COMPARE_OP_ALWAYS;
        case CompareFunction::kLess:         return VK_COMPARE_OP_LESS;
        case CompareFunction::kLessEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareFunction::kEqual:        return VK_COMPARE_OP_EQUAL;
        case CompareFunction::kNotEqual:     return VK_COMPARE_OP_NOT_EQUAL;
        case CompareFunction::kGreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareFunction::kGreater:      return VK_COMPARE_OP_GREATER;
        default:                             return VK_COMPARE_OP_LESS;
        }
    }

    static VkBlendFactor toVkBlendFactor(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::kOne:              return VK_BLEND_FACTOR_ONE;
        case BlendFactor::kZero:             return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::kDstColor:         return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::kSrcColor:         return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::kOneMinusDstColor: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::kOneMinusSrcColor: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::kDstAlpha:         return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::kSrcAlpha:         return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::kOneMinusDstAlpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::kOneMinusSrcAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        default:                             return VK_BLEND_FACTOR_ONE;
        }
    }

    static VkBlendOp toVkBlendOp(BlendOperation op)
    {
        switch (op)
        {
        case BlendOperation::kAdd:            return VK_BLEND_OP_ADD;
        case BlendOperation::kSubtract:       return VK_BLEND_OP_SUBTRACT;
        case BlendOperation::kReverseSubtract:return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOperation::kMin:            return VK_BLEND_OP_MIN;
        case BlendOperation::kMax:            return VK_BLEND_OP_MAX;
        default:                              return VK_BLEND_OP_ADD;
        }
    }

    static VkStencilOp toVkStencilOp(StencilOp op)
    {
        switch (op)
        {
        case StencilOp::kKeep:    return VK_STENCIL_OP_KEEP;
        case StencilOp::kZero:    return VK_STENCIL_OP_ZERO;
        case StencilOp::kReplace: return VK_STENCIL_OP_REPLACE;
        case StencilOp::kInc:     return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case StencilOp::kIncWrap: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case StencilOp::kDec:     return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case StencilOp::kDecWrap: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        case StencilOp::kInvert:  return VK_STENCIL_OP_INVERT;
        default:                  return VK_STENCIL_OP_KEEP;
        }
    }

    static VkColorComponentFlags toVkColorWriteMask(uint8_t mask)
    {
        VkColorComponentFlags flags = 0;
        if (mask & kWriteMaskRed)   flags |= VK_COLOR_COMPONENT_R_BIT;
        if (mask & kWriteMaskGreen) flags |= VK_COLOR_COMPONENT_G_BIT;
        if (mask & kWriteMaskBlue)  flags |= VK_COLOR_COMPONENT_B_BIT;
        if (mask & kWriteMaskAlpha) flags |= VK_COLOR_COMPONENT_A_BIT;
        return flags;
    }

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

            // Destroy cached pipelines
            for (auto &pair : mPipelineCache)
                vkDestroyPipeline(mVkDevice, pair.second, nullptr);
            mPipelineCache.clear();

            // Destroy cached framebuffers
            for (auto &pair : mFramebufferCache)
                vkDestroyFramebuffer(mVkDevice, pair.second.framebuffer, nullptr);
            mFramebufferCache.clear();

            // Destroy cached render passes
            for (auto &pair : mRenderPassCache)
                vkDestroyRenderPass(mVkDevice, pair.second, nullptr);
            mRenderPassCache.clear();

            // Destroy cached pipeline layouts
            for (auto &pair : mPipelineLayoutCache)
                vkDestroyPipelineLayout(mVkDevice, pair.second, nullptr);
            mPipelineLayoutCache.clear();

            // Destroy cached descriptor set layouts
            for (auto &pair : mDescriptorSetLayoutCache)
                vkDestroyDescriptorSetLayout(mVkDevice, pair.second, nullptr);
            mDescriptorSetLayoutCache.clear();

            if (mVkDescriptorPools[0] != VK_NULL_HANDLE)
            {
                for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
                {
                    if (mVkDescriptorPools[i] != VK_NULL_HANDLE)
                        vkDestroyDescriptorPool(mVkDevice, mVkDescriptorPools[i], nullptr);
                }
            }
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
            appInfo.apiVersion = VK_API_VERSION_1_1;

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

            VkPhysicalDeviceFeatures supportedFeatures {};
            vkGetPhysicalDeviceFeatures(mVkPhysicalDevice, &supportedFeatures);

            VkPhysicalDeviceFeatures deviceFeatures {};
            // Scene 线框模式需要 VK_POLYGON_MODE_LINE；未开启 fillModeNonSolid 时非法
            deviceFeatures.fillModeNonSolid = supportedFeatures.fillModeNonSolid;

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

            // 9. Create descriptor pool (per-frame)
            {
                VkDescriptorPoolSize poolSizes[] = {
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_DESCRIPTOR_SETS * 4 },
                    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_DESCRIPTOR_SETS * 4 },
                    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, MAX_DESCRIPTOR_SETS * 2 },
                    { VK_DESCRIPTOR_TYPE_SAMPLER, MAX_DESCRIPTOR_SETS * 2 },
                };

                VkDescriptorPoolCreateInfo poolInfo {};
                poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                poolInfo.maxSets = MAX_DESCRIPTOR_SETS;
                poolInfo.poolSizeCount = 4;
                poolInfo.pPoolSizes = poolSizes;

                for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
                {
                    vkResult = vkCreateDescriptorPool(mVkDevice, &poolInfo, nullptr, &mVkDescriptorPools[i]);
                    if (vkResult != VK_SUCCESS)
                    {
                        ret = T3D_ERR_VK_CREATE_PIPELINE;
                        T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create VkDescriptorPool[%u] failed ! VK ERROR [%d]", i, vkResult);
                        break;
                    }
                }
                if (T3D_FAILED(ret))
                    break;
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::swapBackBuffer(VKRenderWindow *renderWindow)
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
    }

    //--------------------------------------------------------------------------

    TResult VKContext::beginRender()
    {
        if (mCurrentRenderWindow == nullptr)
            return T3D_OK;

        TResult ret = T3D_OK;

        // Wait for this frame's fence
        vkWaitForFences(mVkDevice, 1, &mVkInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(mVkDevice, 1, &mVkInFlightFences[mCurrentFrame]);

        // Reset descriptor pool to reclaim all descriptor sets from previous frame
        vkResetDescriptorPool(mVkDevice, mVkDescriptorPools[mCurrentFrame], 0);

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
    }

    //--------------------------------------------------------------------------

    TResult VKContext::endRender()
    {
        if (mCurrentRenderWindow == nullptr)
            return T3D_OK;

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
    }

    //--------------------------------------------------------------------------

    VkRenderPass VKContext::getOrCreateRenderPass(VkFormat colorFormat, VkFormat depthFormat, bool hasColor, bool depthOnly)
    {
        // Compute a hash from the parameters
        size_t hash = 0;
        hash ^= std::hash<uint32_t>{}((uint32_t)colorFormat) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)depthFormat) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)hasColor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)depthOnly) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        auto it = mRenderPassCache.find(hash);
        if (it != mRenderPassCache.end())
            return it->second;

        std::vector<VkAttachmentDescription> attachments;
        VkAttachmentReference colorAttachmentRef {};
        VkAttachmentReference depthAttachmentRef {};

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        if (hasColor && !depthOnly)
        {
            VkAttachmentDescription colorAttachment {};
            colorAttachment.format = colorFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments.push_back(colorAttachment);

            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
        }

        if (depthFormat != VK_FORMAT_UNDEFINED)
        {
            VkAttachmentDescription depthAttachment {};
            depthAttachment.format = depthFormat;
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = depthOnly ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachments.push_back(depthAttachment);

            depthAttachmentRef.attachment = (uint32_t)(attachments.size() - 1);
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
        }

        if (depthOnly)
        {
            subpass.colorAttachmentCount = 0;
            subpass.pColorAttachments = nullptr;
        }

        VkSubpassDependency dependency {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkResult result = vkCreateRenderPass(mVkDevice, &renderPassInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS)
        {
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create render pass for RT ! VK ERROR [%d]", result);
            return VK_NULL_HANDLE;
        }

        mRenderPassCache[hash] = renderPass;
        return renderPass;
    }

    //--------------------------------------------------------------------------

    VkFramebuffer VKContext::getOrCreateFramebuffer(VkRenderPass renderPass, VkExtent2D &outExtent)
    {
        if (mCurrentRenderTarget == nullptr)
            return VK_NULL_HANDLE;

        // Use render target pointer as cache key
        size_t hash = std::hash<uintptr_t>{}((uintptr_t)mCurrentRenderTarget.get());
        hash ^= std::hash<uint64_t>{}((uint64_t)renderPass) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        auto it = mFramebufferCache.find(hash);
        if (it != mFramebufferCache.end())
        {
            outExtent = it->second.extent;
            return it->second.framebuffer;
        }

        std::vector<VkImageView> attachments;
        uint32_t width = 0, height = 0;

        bool depthOnly = (mCurrentRenderTarget->getNumOfRenderTextures() == 0);

        if (!depthOnly)
        {
            // Color attachment
            const RenderTexturePtr &colorRT = mCurrentRenderTarget->getRenderTexture();
            VKPixelBuffer2D *vkColorPB = static_cast<VKPixelBuffer2D *>(colorRT->getPixelBuffer()->getRHIResource().get());
            attachments.push_back(vkColorPB->VkRTView);

            PixelBuffer2D *pb2d = static_cast<PixelBuffer2D *>(colorRT->getPixelBuffer());
            width = pb2d->getDescriptor().width;
            height = pb2d->getDescriptor().height;
        }

        // Depth attachment
        RenderTexturePtr depthStencilRT = mCurrentRenderTarget->getDepthStencil();
        if (depthStencilRT != nullptr)
        {
            VKPixelBuffer2D *vkDSPB = static_cast<VKPixelBuffer2D *>(depthStencilRT->getPixelBuffer()->getRHIResource().get());
            attachments.push_back(vkDSPB->VkDSView);

            if (depthOnly)
            {
                PixelBuffer2D *pb2d = static_cast<PixelBuffer2D *>(depthStencilRT->getPixelBuffer());
                width = pb2d->getDescriptor().width;
                height = pb2d->getDescriptor().height;
            }
        }

        if (attachments.empty() || width == 0 || height == 0)
            return VK_NULL_HANDLE;

        VkFramebufferCreateInfo fbInfo {};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = renderPass;
        fbInfo.attachmentCount = (uint32_t)attachments.size();
        fbInfo.pAttachments = attachments.data();
        fbInfo.width = width;
        fbInfo.height = height;
        fbInfo.layers = 1;

        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkResult result = vkCreateFramebuffer(mVkDevice, &fbInfo, nullptr, &framebuffer);
        if (result != VK_SUCCESS)
        {
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create framebuffer for RT ! VK ERROR [%d]", result);
            return VK_NULL_HANDLE;
        }

        outExtent = { width, height };
        mFramebufferCache[hash] = { framebuffer, outExtent };
        return framebuffer;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::beginPass()
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        TResult ret = T3D_OK;

        VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

        if (mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            if (mCurrentRenderWindow == nullptr)
                return T3D_OK;

            // Transition swapchain image from TRANSFER_DST to COLOR_ATTACHMENT_OPTIMAL
            insertImageBarrier(cmdBuf, mCurrentRenderWindow->VkSwapChainImages[mCurrentImageIndex],
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

            // Transition depth image to DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            insertImageBarrier(cmdBuf, mCurrentRenderWindow->VkDepthImage,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

            // Begin render pass
            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = mVkRenderPass;
            renderPassInfo.framebuffer = mCurrentRenderWindow->VkFramebuffers[mCurrentImageIndex];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = mCurrentRenderWindow->VkSwapChainExtent;

            // Provide clear values for LOAD_OP_CLEAR attachments (color + depth)
            VkClearValue clearValues[2] {};
            clearValues[0].color = {{mClearColor.red(), mClearColor.green(), mClearColor.blue(), 1.0f}};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = 2;
            renderPassInfo.pClearValues = clearValues;

            mCurrentPassRenderPass = mVkRenderPass;
            mCurrentPassExtent = mCurrentRenderWindow->VkSwapChainExtent;

            vkCmdBeginRenderPass(cmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // Set viewport and scissor (negative height flips Y to match OpenGL/D3D convention)
            VkViewport viewport {};
            viewport.x = 0.0f;
            viewport.y = (float)mCurrentRenderWindow->VkSwapChainExtent.height;
            viewport.width = (float)mCurrentRenderWindow->VkSwapChainExtent.width;
            viewport.height = -(float)mCurrentRenderWindow->VkSwapChainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(cmdBuf, 0, 1, &viewport);

            VkRect2D scissor {};
            scissor.offset = {0, 0};
            scissor.extent = mCurrentRenderWindow->VkSwapChainExtent;
            vkCmdSetScissor(cmdBuf, 0, 1, &scissor);
            mPendingScissorValid = false;
        }
        else
        {
            // Render to texture
            bool depthOnly = (mCurrentRenderTarget->getNumOfRenderTextures() == 0);
            VkFormat colorFormat = VK_FORMAT_UNDEFINED;
            VkFormat depthFormat = VK_FORMAT_UNDEFINED;
            bool hasColor = !depthOnly;

            if (hasColor)
            {
                const RenderTexturePtr &colorRT = mCurrentRenderTarget->getRenderTexture();
                VKPixelBuffer2D *vkColorPB = static_cast<VKPixelBuffer2D *>(colorRT->getPixelBuffer()->getRHIResource().get());
                PixelBuffer2D *pb2d = static_cast<PixelBuffer2D *>(colorRT->getPixelBuffer());
                colorFormat = VKMapping::get(pb2d->getDescriptor().format);

                // Transition color to COLOR_ATTACHMENT_OPTIMAL
                insertImageBarrier(cmdBuf, vkColorPB->VkTex,
                    vkColorPB->VkCurrentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
                vkColorPB->VkCurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            RenderTexturePtr depthStencilRT = mCurrentRenderTarget->getDepthStencil();
            if (depthStencilRT != nullptr)
            {
                VKPixelBuffer2D *vkDSPB = static_cast<VKPixelBuffer2D *>(depthStencilRT->getPixelBuffer()->getRHIResource().get());
                PixelBuffer2D *dsPB2D = static_cast<PixelBuffer2D *>(depthStencilRT->getPixelBuffer());
                depthFormat = VKMapping::get(dsPB2D->getDescriptor().format);

                VkImageAspectFlags dsAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
                if (hasStencilComponent(depthFormat))
                    dsAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;

                // Transition depth to DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                insertImageBarrier(cmdBuf, vkDSPB->VkTex,
                    vkDSPB->VkCurrentLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                    dsAspect);
                vkDSPB->VkCurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }

            // Get or create render pass and framebuffer
            VkRenderPass renderPass = getOrCreateRenderPass(colorFormat, depthFormat, hasColor, depthOnly);
            if (renderPass == VK_NULL_HANDLE)
                return T3D_ERR_VK_CREATE_RENDER_PASS;

            VkExtent2D extent {};
            VkFramebuffer framebuffer = getOrCreateFramebuffer(renderPass, extent);
            if (framebuffer == VK_NULL_HANDLE)
                return T3D_ERR_VK_CREATE_FRAMEBUFFER;

            mCurrentPassRenderPass = renderPass;
            mCurrentPassExtent = extent;

            // Begin render pass
            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = framebuffer;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = extent;

            // Provide clear values for LOAD_OP_CLEAR attachments
            VkClearValue clearValues[2] {};
            uint32_t clearValueCount = 0;
            if (hasColor && !depthOnly)
            {
                clearValues[clearValueCount].color = {{mClearColor.red(), mClearColor.green(), mClearColor.blue(), 1.0f}};
                clearValueCount++;
            }
            if (depthFormat != VK_FORMAT_UNDEFINED)
            {
                clearValues[clearValueCount].depthStencil = {1.0f, 0};
                clearValueCount++;
            }
            renderPassInfo.clearValueCount = clearValueCount;
            renderPassInfo.pClearValues = clearValues;

            vkCmdBeginRenderPass(cmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // Set viewport and scissor (negative height flips Y to match OpenGL/D3D convention)
            VkViewport viewport {};
            viewport.x = 0.0f;
            viewport.y = (float)extent.height;
            viewport.width = (float)extent.width;
            viewport.height = -(float)extent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(cmdBuf, 0, 1, &viewport);

            VkRect2D scissor {};
            scissor.offset = {0, 0};
            scissor.extent = extent;
            vkCmdSetScissor(cmdBuf, 0, 1, &scissor);
            mPendingScissorValid = false;
        }

        mRenderPassActive = true;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::endPass()
    {
        if (!mRenderPassActive)
            return T3D_OK;

        VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

        vkCmdEndRenderPass(cmdBuf);

        if (mCurrentRenderTarget != nullptr && mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            // Transition swapchain image back to TRANSFER_DST for potential blit operations
            insertImageBarrier(cmdBuf, mCurrentRenderWindow->VkSwapChainImages[mCurrentImageIndex],
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        }
        else if (mCurrentRenderTarget != nullptr && mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_TEXTURE)
        {
            bool depthOnly = (mCurrentRenderTarget->getNumOfRenderTextures() == 0);

            if (!depthOnly)
            {
                // Transition color render texture to SHADER_READ_ONLY for sampling
                const RenderTexturePtr &colorRT = mCurrentRenderTarget->getRenderTexture();
                VKPixelBuffer2D *vkColorPB = static_cast<VKPixelBuffer2D *>(colorRT->getPixelBuffer()->getRHIResource().get());
                insertImageBarrier(cmdBuf, vkColorPB->VkTex,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                vkColorPB->VkCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            // Transition depth to SHADER_READ_ONLY (for shadow map sampling)
            RenderTexturePtr depthStencilRT = mCurrentRenderTarget->getDepthStencil();
            if (depthStencilRT != nullptr)
            {
                VKPixelBuffer2D *vkDSPB = static_cast<VKPixelBuffer2D *>(depthStencilRT->getPixelBuffer()->getRHIResource().get());
                PixelBuffer2D *dsPB2D = static_cast<PixelBuffer2D *>(depthStencilRT->getPixelBuffer());
                VkFormat depthFormat = VKMapping::get(dsPB2D->getDescriptor().format);

                VkImageAspectFlags dsAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
                if (hasStencilComponent(depthFormat))
                    dsAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;

                insertImageBarrier(cmdBuf, vkDSPB->VkTex,
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    dsAspect);
                vkDSPB->VkCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
        }

        mRenderPassActive = false;
        mVkCurrentPipeline = VK_NULL_HANDLE;

        return T3D_OK;
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

        TResult ret = T3D_OK;

        do
        {
            const RenderWindowDesc &desc = renderWindow->getDescriptor();

            // Create Win32 surface
            SysWMInfo info;
            renderWindow->getSystemInfo(info);

            VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {};
            surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surfaceCreateInfo.hwnd = (HWND)info.hWnd;
            surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

            VkResult vkResult = vkCreateWin32SurfaceKHR(mVkInstance, &surfaceCreateInfo, nullptr, &vkRenderWindow->VkSurface);
            if (vkResult != VK_SUCCESS)
            {
                ret = T3D_ERR_VK_CREATE_SURFACE;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Create Win32 surface failed ! VK ERROR [%d]", vkResult);
                break;
            }

            // Verify present support
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(mVkPhysicalDevice, mPresentQueueFamily, vkRenderWindow->VkSurface, &presentSupport);
            if (!presentSupport)
            {
                ret = T3D_ERR_VK_CREATE_SURFACE;
                T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Queue family does not support present !");
                break;
            }

            ret = createRenderWindow(vkRenderWindow, desc.Width, desc.Height, desc.MSAA.Count, desc.MSAA.Quality);
        } while (false);

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

            // Create image view(s)
            if (isDepthFormat(format))
            {
                // Attachment view: full depth+stencil aspect for framebuffer
                vkPixelBuffer->VkDSView = createVkImageView(vkPixelBuffer->VkTex, format, aspect);
                // Sampling view: depth-only aspect (Vulkan spec requires single aspect for descriptor sampling)
                vkPixelBuffer->VkTexView = createVkImageView(vkPixelBuffer->VkTex, format, VK_IMAGE_ASPECT_DEPTH_BIT);
            }
            else
            {
                vkPixelBuffer->VkTexView = createVkImageView(vkPixelBuffer->VkTex, format, aspect);
                vkPixelBuffer->VkRTView = vkPixelBuffer->VkTexView;
            }
        } while (false);

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

    TResult VKContext::setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        mPendingScissor.offset.x = x;
        mPendingScissor.offset.y = y;
        mPendingScissor.extent.width = width;
        mPendingScissor.extent.height = height;
        mPendingScissorValid = true;

        // 已在 render pass 内则可立即写入；否则等 render() 再应用
        if (mRenderPassActive)
        {
            VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];
            vkCmdSetScissor(cmdBuf, 0, 1, &mPendingScissor);
        }

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

                // Transition to TRANSFER_DST using tracked layout
                insertImageBarrier(cmdBuf, vkPB->VkTex,
                    vkPB->VkCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    0, VK_ACCESS_TRANSFER_WRITE_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
                vkPB->VkCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

                VkClearColorValue clearColor = {{color.red(), color.green(), color.blue(), 1.0f}};
                VkImageSubresourceRange range {};
                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                range.levelCount = 1;
                range.layerCount = 1;
                vkCmdClearColorImage(cmdBuf, vkPB->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &range);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::clearDepth(Real depth)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        if (mCurrentRenderTarget->getDepthStencil() == nullptr)
            return T3D_OK;

        RenderTexturePtr depthStencil = mCurrentRenderTarget->getDepthStencil();

        if (depthStencil == nullptr)
            return T3D_OK;

        VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];
        VKPixelBuffer2D *vkDS = static_cast<VKPixelBuffer2D *>(depthStencil->getPixelBuffer()->getRHIResource().get());

        // D24S8 requires both DEPTH+STENCIL in barrier aspectMask
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

        insertImageBarrier(cmdBuf, vkDS->VkTex,
            vkDS->VkCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            0, VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            aspect);
        vkDS->VkCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        VkClearDepthStencilValue clearValue = {depth, 0};
        VkImageSubresourceRange range {};
        range.aspectMask = aspect;
        range.levelCount = 1;
        range.layerCount = 1;
        vkCmdClearDepthStencilImage(cmdBuf, vkDS->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &range);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::clearDepthStencil(Real depth, uint32_t stencil)
    {
        if (mCurrentRenderTarget == nullptr)
            return T3D_OK;

        if (mCurrentRenderTarget->getDepthStencil() == nullptr)
            return T3D_OK;

        RenderTexturePtr depthStencilRT = mCurrentRenderTarget->getDepthStencil();

        if (depthStencilRT == nullptr)
            return T3D_OK;

        VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];
        VKPixelBuffer2D *vkDS = static_cast<VKPixelBuffer2D *>(depthStencilRT->getPixelBuffer()->getRHIResource().get());

        insertImageBarrier(cmdBuf, vkDS->VkTex,
            vkDS->VkCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            0, VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        vkDS->VkCurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        VkClearDepthStencilValue clearValue = {depth, stencil};
        VkImageSubresourceRange range {};
        range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        range.levelCount = 1;
        range.layerCount = 1;
        vkCmdClearDepthStencilImage(cmdBuf, vkDS->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &range);

        return T3D_OK;
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

        const SamplerDesc &desc = state->getStateDesc();

        VkSamplerCreateInfo samplerInfo {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VKMapping::get(desc.MagFilter);
        samplerInfo.minFilter = VKMapping::get(desc.MinFilter);
        samplerInfo.mipmapMode = VKMapping::getMipmapMode(desc.MipFilter);
        samplerInfo.addressModeU = VKMapping::get(desc.AddressU);
        samplerInfo.addressModeV = VKMapping::get(desc.AddressV);
        samplerInfo.addressModeW = VKMapping::get(desc.AddressW);
        samplerInfo.mipLodBias = desc.MipLODBias;
        samplerInfo.anisotropyEnable = (desc.MaxAnisotropy > 1) ? VK_TRUE : VK_FALSE;
        samplerInfo.maxAnisotropy = (float)desc.MaxAnisotropy;
        samplerInfo.compareEnable = desc.IsComparison ? VK_TRUE : VK_FALSE;
        samplerInfo.compareOp = VKMapping::get(desc.CompareFunc);
        samplerInfo.minLod = desc.MinLOD;
        samplerInfo.maxLod = desc.MaxLOD;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        VkResult vkResult = vkCreateSampler(mVkDevice, &samplerInfo, nullptr, &vkState->VkSamp);
        if (vkResult != VK_SUCCESS)
        {
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create VkSampler ! VK ERROR [%d]", vkResult);
            vkState = nullptr;
        }
        else
        {
            vkState->VkDev = mVkDevice;
        }

        return vkState;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setBlendState(BlendState *state)
    {
        if (state != nullptr)
        {
            mCurrentBlendDesc = state->getStateDesc();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setDepthStencilState(DepthStencilState *state)
    {
        if (state != nullptr)
        {
            mCurrentDepthStencilDesc = state->getStateDesc();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setRasterizerState(RasterizerState *state)
    {
        if (state != nullptr)
        {
            mCurrentRasterizerDesc = state->getStateDesc();
        }
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
        if (decl == nullptr)
            return T3D_OK;

        mVertexBindings.clear();
        mVertexAttributes.clear();

        const VertexAttributes &attrs = decl->getAttributes();

        // Collect unique bindings (slots)
        std::set<uint32_t> slots;
        for (size_t i = 0; i < attrs.size(); ++i)
        {
            slots.insert(attrs[i].getSlot());
        }

        // Create binding descriptions
        for (uint32_t slot : slots)
        {
            VkVertexInputBindingDescription binding {};
            binding.binding = slot;
            binding.stride = decl->getVertexSize(slot);
            binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            mVertexBindings.push_back(binding);
        }

        // Create attribute descriptions
        for (size_t i = 0; i < attrs.size(); ++i)
        {
            VkVertexInputAttributeDescription attr {};
            attr.location = (uint32_t)i;
            attr.binding = attrs[i].getSlot();
            attr.format = VKMapping::get(attrs[i].getType());
            attr.offset = attrs[i].getOffset();
            mVertexAttributes.push_back(attr);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexBufferPtr VKContext::createVertexBuffer(VertexBuffer *buffer)
    {
        VKVertexBufferPtr vkBuffer = VKVertexBuffer::create();

        do
        {
            VkDeviceSize bufferSize = buffer->getBufferSize();

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
        if (buffers.empty())
            return T3D_OK;

        VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

        std::vector<VkBuffer> vkBuffers(buffers.size());
        std::vector<VkDeviceSize> vkOffsets(buffers.size());

        for (size_t i = 0; i < buffers.size(); ++i)
        {
            VKVertexBuffer *vkVB = static_cast<VKVertexBuffer *>(buffers[i]->getRHIResource().get());
            vkBuffers[i] = vkVB->VkBuf;
            vkOffsets[i] = (i < offsets.size()) ? (VkDeviceSize)offsets[i] : 0;
        }

        vkCmdBindVertexBuffers(cmdBuf, startSlot, (uint32_t)vkBuffers.size(), vkBuffers.data(), vkOffsets.data());

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIIndexBufferPtr VKContext::createIndexBuffer(IndexBuffer *buffer)
    {
        VKIndexBufferPtr vkBuffer = VKIndexBuffer::create();

        do
        {
            VkDeviceSize bufferSize = buffer->getBufferSize();

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
        if (buffer == nullptr)
            return T3D_OK;

        VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];
        VKIndexBuffer *vkIB = static_cast<VKIndexBuffer *>(buffer->getRHIResource().get());
        VkIndexType indexType = VKMapping::get(buffer->getIndexType());

        vkCmdBindIndexBuffer(cmdBuf, vkIB->VkBuf, 0, indexType);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIConstantBufferPtr VKContext::createConstantBuffer(ConstantBuffer *buffer)
    {
        VKConstantBufferPtr vkBuffer = VKConstantBuffer::create();

        do
        {
            VkDeviceSize bufferSize = buffer->getBufferSize();

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

            TResult ret = T3D_OK;

            do
            {
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
                    vkBuffer->VkCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    vkDestroyBuffer(mVkDevice, stagingBuffer, nullptr);
                    vkFreeMemory(mVkDevice, stagingBufferMemory, nullptr);
                }
            } while (false);

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

    RHIPixelBufferCubemapPtr VKContext::createPixelBufferCubemap(PixelBufferCubemap *buffer)
    {
        VKPixelBufferCubemapPtr vkBuffer = VKPixelBufferCubemap::create();

        do
        {
            const auto &desc = buffer->getDescriptor();
            const uint32_t faceCount = PixelBufferCubemap::FACE_COUNT;
            VkFormat format = VKMapping::get(desc.format);

            TResult ret = T3D_OK;

            do
            {
                // 6 层 + CUBE_COMPATIBLE，才能建出 VK_IMAGE_VIEW_TYPE_CUBE
                VkImageCreateInfo imageInfo {};
                imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                imageInfo.extent = { desc.width, desc.height, 1 };
                imageInfo.mipLevels = 1;
                imageInfo.arrayLayers = faceCount;
                imageInfo.format = format;
                imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                VkResult vkResult = vkCreateImage(mVkDevice, &imageInfo, nullptr, &vkBuffer->VkTex);
                if (vkResult != VK_SUCCESS)
                {
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create cubemap image ! VK ERROR [%d]", vkResult);
                    ret = T3D_ERR_VK_CREATE_IMAGE;
                    break;
                }

                VkMemoryRequirements memRequirements;
                vkGetImageMemoryRequirements(mVkDevice, vkBuffer->VkTex, &memRequirements);

                VkMemoryAllocateInfo allocInfo {};
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize = memRequirements.size;
                allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                vkResult = vkAllocateMemory(mVkDevice, &allocInfo, nullptr, &vkBuffer->VkTexMemory);
                if (vkResult != VK_SUCCESS)
                {
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to allocate cubemap image memory ! VK ERROR [%d]", vkResult);
                    ret = T3D_ERR_VK_ALLOCATE_MEMORY;
                    break;
                }

                vkBindImageMemory(mVkDevice, vkBuffer->VkTex, vkBuffer->VkTexMemory, 0);

                VkImageViewCreateInfo viewInfo {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image = vkBuffer->VkTex;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
                viewInfo.format = format;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = faceCount;

                vkResult = vkCreateImageView(mVkDevice, &viewInfo, nullptr, &vkBuffer->VkTexView);
                if (vkResult != VK_SUCCESS)
                {
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create cubemap image view ! VK ERROR [%d]", vkResult);
                    ret = T3D_ERR_VK_CREATE_IMAGE_VIEW;
                    break;
                }

                if (buffer->getBuffer().Data != nullptr)
                {
                    VkDeviceSize imageSize = buffer->getBuffer().DataSize;
                    const size_t bpp = Image::getBPP(desc.format) / 8;
                    const VkDeviceSize faceSize = (VkDeviceSize)desc.width * desc.height * bpp;

                    VkBuffer stagingBuffer;
                    VkDeviceMemory stagingBufferMemory;
                    createVkBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        stagingBuffer, stagingBufferMemory);

                    void *data = nullptr;
                    vkMapMemory(mVkDevice, stagingBufferMemory, 0, imageSize, 0, &data);
                    memcpy(data, buffer->getBuffer().Data, (size_t)imageSize);
                    vkUnmapMemory(mVkDevice, stagingBufferMemory);

                    transitionCubemapLayout(vkBuffer->VkTex, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

                    VkCommandBuffer cmdBuf = beginSingleTimeCommands();
                    VkBufferImageCopy regions[PixelBufferCubemap::FACE_COUNT] = {};
                    for (uint32_t face = 0; face < faceCount; ++face)
                    {
                        regions[face].bufferOffset = face * faceSize;
                        regions[face].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        regions[face].imageSubresource.mipLevel = 0;
                        regions[face].imageSubresource.baseArrayLayer = face;
                        regions[face].imageSubresource.layerCount = 1;
                        regions[face].imageExtent = { desc.width, desc.height, 1 };
                    }
                    vkCmdCopyBufferToImage(cmdBuf, stagingBuffer, vkBuffer->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, faceCount, regions);
                    endSingleTimeCommands(cmdBuf);

                    transitionCubemapLayout(vkBuffer->VkTex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                    vkBuffer->VkCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    vkDestroyBuffer(mVkDevice, stagingBuffer, nullptr);
                    vkFreeMemory(mVkDevice, stagingBufferMemory, nullptr);
                }
            } while (false);

            if (T3D_FAILED(ret))
            {
                vkBuffer = nullptr;
                break;
            }
        } while (false);

        return vkBuffer;
    }

    //--------------------------------------------------------------------------

    void VKContext::transitionCubemapLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
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
        barrier.subresourceRange.layerCount = PixelBufferCubemap::FACE_COUNT;

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

    RHIShaderPtr VKContext::createVertexShader(ShaderVariant *shader)
    {
        VKVertexShaderPtr vkShader = VKVertexShader::create();

        do
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

                // Extract entry point name and binding info from SPIR-V via spirv-reflect
                SpvReflectShaderModule spvModule;
                SpvReflectResult spvResult = spvReflectCreateShaderModule(bytecodeLength, bytecode, &spvModule);
                if (spvResult == SPV_REFLECT_RESULT_SUCCESS)
                {
                    if (spvModule.entry_point_count > 0)
                    {
                        vkShader->EntryPoint = spvModule.entry_points[0].name;
                    }

                    // Extract descriptor bindings
                    uint32_t bindingCount = 0;
                    spvReflectEnumerateDescriptorBindings(&spvModule, &bindingCount, nullptr);
                    if (bindingCount > 0)
                    {
                        std::vector<SpvReflectDescriptorBinding*> spvBindings(bindingCount);
                        spvReflectEnumerateDescriptorBindings(&spvModule, &bindingCount, spvBindings.data());

                        for (uint32_t i = 0; i < bindingCount; ++i)
                        {
                            VKShaderBindingInfo info {};
                            info.binding = spvBindings[i]->binding;
                            info.set = spvBindings[i]->set;
                            info.descriptorType = (VkDescriptorType)spvBindings[i]->descriptor_type;
                            info.descriptorCount = spvBindings[i]->count;
                            info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                            vkShader->Bindings.push_back(info);
                        }
                    }

                    spvReflectDestroyShaderModule(&spvModule);
                }
            } while (false);

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
        if (shader == nullptr)
        {
            mCurrentVSModule = VK_NULL_HANDLE;
            mCurrentVSEntryPoint = "main";
            mCurrentVSBindings.clear();
            return T3D_OK;
        }

        VKVertexShader *vkShader = static_cast<VKVertexShader *>(shader->getRHIShader());
        if (vkShader != nullptr)
        {
            mCurrentVSModule = vkShader->VkModule;
            mCurrentVSEntryPoint = vkShader->EntryPoint;

            // Convert binding info to VkDescriptorSetLayoutBinding
            mCurrentVSBindings.clear();
            for (const auto &b : vkShader->Bindings)
            {
                VkDescriptorSetLayoutBinding layoutBinding {};
                layoutBinding.binding = b.binding;
                layoutBinding.descriptorType = b.descriptorType;
                layoutBinding.descriptorCount = b.descriptorCount;
                layoutBinding.stageFlags = b.stageFlags;
                mCurrentVSBindings.push_back(layoutBinding);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        mCurrentVSCBStartSlot = startSlot;
        mCurrentVSConstantBuffers.clear();
        mCurrentVSConstantBufferSizes.clear();

        for (size_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i] != nullptr)
            {
                VKConstantBuffer *vkCB = static_cast<VKConstantBuffer *>(buffers[i]->getRHIResource().get());
                mCurrentVSConstantBuffers.push_back(vkCB->VkBuf);
                mCurrentVSConstantBufferSizes.push_back((VkDeviceSize)buffers[i]->getBufferSize());
            }
            else
            {
                mCurrentVSConstantBuffers.push_back(VK_NULL_HANDLE);
                mCurrentVSConstantBufferSizes.push_back(0);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        // VS texture bindings not commonly used, but store for completeness
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
            TResult ret = T3D_OK;

            do
            {
                size_t bytecodeLength = 0;
                const char *bytecode = shader->getBytesCode(bytecodeLength);

                // Use spirv-reflect to remap PS bindings before creating VkShaderModule
                SpvReflectShaderModule spvModule;
                SpvReflectResult spvResult = spvReflectCreateShaderModule(bytecodeLength, bytecode, &spvModule);
                if (spvResult != SPV_REFLECT_RESULT_SUCCESS)
                {
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create spirv-reflect module for pixel shader !");
                    ret = T3D_ERR_VK_CREATE_SHADER_MODULE;
                    break;
                }

                // Extract entry point name
                if (spvModule.entry_point_count > 0)
                {
                    vkShader->EntryPoint = spvModule.entry_points[0].name;
                }

                // Remap all PS descriptor bindings by +PS_BINDING_OFFSET
                uint32_t bindingCount = 0;
                spvReflectEnumerateDescriptorBindings(&spvModule, &bindingCount, nullptr);
                if (bindingCount > 0)
                {
                    std::vector<SpvReflectDescriptorBinding*> spvBindings(bindingCount);
                    spvReflectEnumerateDescriptorBindings(&spvModule, &bindingCount, spvBindings.data());

                    for (uint32_t i = 0; i < bindingCount; ++i)
                    {
                        uint32_t newBinding = spvBindings[i]->binding + PS_BINDING_OFFSET;
                        spvReflectChangeDescriptorBindingNumbers(&spvModule, spvBindings[i],
                            newBinding, SPV_REFLECT_SET_NUMBER_DONT_CHANGE);
                    }

                    // Re-enumerate after remap to get updated binding info
                    spvReflectEnumerateDescriptorBindings(&spvModule, &bindingCount, nullptr);
                    spvBindings.resize(bindingCount);
                    spvReflectEnumerateDescriptorBindings(&spvModule, &bindingCount, spvBindings.data());

                    for (uint32_t i = 0; i < bindingCount; ++i)
                    {
                        VKShaderBindingInfo info {};
                        info.binding = spvBindings[i]->binding;
                        info.set = spvBindings[i]->set;
                        info.descriptorType = (VkDescriptorType)spvBindings[i]->descriptor_type;
                        info.descriptorCount = spvBindings[i]->count;
                        info.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                        vkShader->Bindings.push_back(info);
                    }
                }

                // Create VkShaderModule from modified SPIR-V bytecode
                VkShaderModuleCreateInfo createInfo {};
                createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                createInfo.codeSize = spvReflectGetCodeSize(&spvModule);
                createInfo.pCode = spvReflectGetCode(&spvModule);

                VkResult vkResult = vkCreateShaderModule(mVkDevice, &createInfo, nullptr, &vkShader->VkModule);
                if (vkResult != VK_SUCCESS)
                {
                    T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create pixel shader module ! VK ERROR [%d]", vkResult);
                    ret = T3D_ERR_VK_CREATE_SHADER_MODULE;
                    spvReflectDestroyShaderModule(&spvModule);
                    break;
                }

                spvReflectDestroyShaderModule(&spvModule);
            } while (false);

            if (T3D_FAILED(ret))
            {
                vkShader = nullptr;
                break;
            }
        } while (false);

        return vkShader;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::setPixelShader(ShaderVariant *shader)
    {
        if (shader == nullptr)
        {
            mCurrentPSModule = VK_NULL_HANDLE;
            mCurrentPSEntryPoint = "main";
            mCurrentPSBindings.clear();
            return T3D_OK;
        }

        VKPixelShader *vkShader = static_cast<VKPixelShader *>(shader->getRHIShader());
        if (vkShader != nullptr)
        {
            mCurrentPSModule = vkShader->VkModule;
            mCurrentPSEntryPoint = vkShader->EntryPoint;

            // Convert binding info to VkDescriptorSetLayoutBinding
            mCurrentPSBindings.clear();
            for (const auto &b : vkShader->Bindings)
            {
                VkDescriptorSetLayoutBinding layoutBinding {};
                layoutBinding.binding = b.binding;
                layoutBinding.descriptorType = b.descriptorType;
                layoutBinding.descriptorCount = b.descriptorCount;
                layoutBinding.stageFlags = b.stageFlags;
                mCurrentPSBindings.push_back(layoutBinding);
            }
        }

        return T3D_OK;
    }
    TResult VKContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        mCurrentPSCBStartSlot = startSlot;
        mCurrentPSConstantBuffers.clear();
        mCurrentPSConstantBufferSizes.clear();

        for (size_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i] != nullptr)
            {
                VKConstantBuffer *vkCB = static_cast<VKConstantBuffer *>(buffers[i]->getRHIResource().get());
                mCurrentPSConstantBuffers.push_back(vkCB->VkBuf);
                mCurrentPSConstantBufferSizes.push_back((VkDeviceSize)buffers[i]->getBufferSize());
            }
            else
            {
                mCurrentPSConstantBuffers.push_back(VK_NULL_HANDLE);
                mCurrentPSConstantBufferSizes.push_back(0);
            }
        }

        return T3D_OK;
    }

    TResult VKContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        mCurrentPSTexStartSlot = startSlot;
        mCurrentPSImageViews.clear();

        for (size_t i = 0; i < buffers.size(); ++i)
        {
            RHIResource *rhiResource = buffers[i] != nullptr ? buffers[i]->getRHIResource().get() : nullptr;
            if (rhiResource != nullptr
                && rhiResource->getResourceType() == RHIResource::ResourceType::kPixelBufferCubemap)
            {
                mCurrentPSImageViews.push_back(static_cast<VKPixelBufferCubemap *>(rhiResource)->VkTexView);
            }
            else if (rhiResource != nullptr)
            {
                mCurrentPSImageViews.push_back(static_cast<VKPixelBuffer2D *>(rhiResource)->VkTexView);
            }
            else
            {
                mCurrentPSImageViews.push_back(VK_NULL_HANDLE);
            }
        }

        return T3D_OK;
    }

    TResult VKContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        mCurrentPSSamplerStartSlot = startSlot;
        mCurrentPSSamplers.clear();

        for (size_t i = 0; i < samplers.size(); ++i)
        {
            if (samplers[i] != nullptr)
            {
                VKSamplerState *vkSampler = static_cast<VKSamplerState *>(samplers[i]->getRHIResource().get());
                mCurrentPSSamplers.push_back(vkSampler->VkSamp);
            }
            else
            {
                mCurrentPSSamplers.push_back(VK_NULL_HANDLE);
            }
        }

        return T3D_OK;
    }

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

    VkDescriptorSetLayout VKContext::getOrCreateDescriptorSetLayout(size_t &outHash)
    {
        // Merge VS and PS bindings, combining stageFlags for same binding number
        std::map<uint32_t, VkDescriptorSetLayoutBinding> mergedBindings;

        for (const auto &b : mCurrentVSBindings)
        {
            auto it = mergedBindings.find(b.binding);
            if (it != mergedBindings.end())
                it->second.stageFlags |= b.stageFlags;
            else
                mergedBindings[b.binding] = b;
        }

        for (const auto &b : mCurrentPSBindings)
        {
            auto it = mergedBindings.find(b.binding);
            if (it != mergedBindings.end())
                it->second.stageFlags |= b.stageFlags;
            else
                mergedBindings[b.binding] = b;
        }

        // Compute hash
        size_t hash = 0;
        for (const auto &pair : mergedBindings)
        {
            hash ^= std::hash<uint32_t>{}(pair.second.binding) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<uint32_t>{}((uint32_t)pair.second.descriptorType) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<uint32_t>{}(pair.second.stageFlags) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        outHash = hash;

        // Check cache
        auto it = mDescriptorSetLayoutCache.find(hash);
        if (it != mDescriptorSetLayoutCache.end())
            return it->second;

        // Create new descriptor set layout
        std::vector<VkDescriptorSetLayoutBinding> bindingsVec;
        for (const auto &pair : mergedBindings)
            bindingsVec.push_back(pair.second);

        VkDescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (uint32_t)bindingsVec.size();
        layoutInfo.pBindings = bindingsVec.data();

        VkDescriptorSetLayout dsl = VK_NULL_HANDLE;
        VkResult result = vkCreateDescriptorSetLayout(mVkDevice, &layoutInfo, nullptr, &dsl);
        if (result != VK_SUCCESS)
        {
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create descriptor set layout ! VK ERROR [%d]", result);
            return VK_NULL_HANDLE;
        }

        mDescriptorSetLayoutCache[hash] = dsl;
        return dsl;
    }

    //--------------------------------------------------------------------------

    VkPipelineLayout VKContext::getOrCreatePipelineLayout(VkDescriptorSetLayout dsl, size_t dslHash)
    {
        auto it = mPipelineLayoutCache.find(dslHash);
        if (it != mPipelineLayoutCache.end())
            return it->second;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &dsl;

        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkResult result = vkCreatePipelineLayout(mVkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
        if (result != VK_SUCCESS)
        {
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create pipeline layout ! VK ERROR [%d]", result);
            return VK_NULL_HANDLE;
        }

        mPipelineLayoutCache[dslHash] = pipelineLayout;
        return pipelineLayout;
    }

    //--------------------------------------------------------------------------

    VkPipeline VKContext::getOrCreatePipeline()
    {
        // Compute a hash for the current pipeline state
        size_t hash = 0;
        hash ^= std::hash<uint64_t>{}((uint64_t)mCurrentVSModule) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint64_t>{}((uint64_t)mCurrentPSModule) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)mVkPrimitiveTopology) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<size_t>{}(mVertexAttributes.size()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<size_t>{}(mVertexBindings.size()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        // Include render pass in hash since pipeline must be compatible
        VkRenderPass currentRP = mCurrentPassRenderPass != VK_NULL_HANDLE ? mCurrentPassRenderPass : mVkRenderPass;
        hash ^= std::hash<uint64_t>{}((uint64_t)currentRP) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        // Include render state in hash (excluding depth bias values which are dynamic)
        hash ^= std::hash<uint32_t>{}((uint32_t)mCurrentRasterizerDesc.CullMode) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)mCurrentRasterizerDesc.FillMode) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)mCurrentRasterizerDesc.FrontAnticlockwise) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)mCurrentDepthStencilDesc.DepthTestEnable) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)mCurrentDepthStencilDesc.DepthWriteEnable) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)mCurrentDepthStencilDesc.DepthFunc) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)mCurrentDepthStencilDesc.StencilEnable) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        // Blend state for RT[0]
        const auto &rt0Blend = mCurrentBlendDesc.RenderTargetStates[0];
        hash ^= std::hash<uint32_t>{}((uint32_t)rt0Blend.BlendEnable) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)rt0Blend.SrcBlend) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)rt0Blend.DestBlend) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)rt0Blend.BlendOp) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}((uint32_t)rt0Blend.ColorMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        // Check cache
        auto it = mPipelineCache.find(hash);
        if (it != mPipelineCache.end())
            return it->second;

        // Determine if this is a depth-only pass (no pixel shader)
        bool hasPixelShader = (mCurrentPSModule != VK_NULL_HANDLE);

        // Create new pipeline
        VkPipelineShaderStageCreateInfo shaderStages[2] {};
        uint32_t stageCount = 1;

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = mCurrentVSModule;
        shaderStages[0].pName = mCurrentVSEntryPoint.c_str();

        if (hasPixelShader)
        {
            shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStages[1].module = mCurrentPSModule;
            shaderStages[1].pName = mCurrentPSEntryPoint.c_str();
            stageCount = 2;
        }

        // Vertex input state
        VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)mVertexBindings.size();
        vertexInputInfo.pVertexBindingDescriptions = mVertexBindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)mVertexAttributes.size();
        vertexInputInfo.pVertexAttributeDescriptions = mVertexAttributes.data();

        // Input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = mVkPrimitiveTopology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Viewport state (dynamic)
        VkPipelineViewportStateCreateInfo viewportState {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Rasterization — read from cached desc
        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = toVkPolygonMode(mCurrentRasterizerDesc.FillMode);
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = toVkCullMode(mCurrentRasterizerDesc.CullMode);
        rasterizer.frontFace = mCurrentRasterizerDesc.FrontAnticlockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
        // Always enable depth bias (dynamic state); when not needed, values are set to 0
        rasterizer.depthBiasEnable = VK_TRUE;
        rasterizer.depthBiasConstantFactor = 0.0f;  // dynamic
        rasterizer.depthBiasSlopeFactor = 0.0f;     // dynamic
        rasterizer.depthBiasClamp = 0.0f;           // dynamic

        // Multisampling
        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Depth stencil — read from cached desc
        VkPipelineDepthStencilStateCreateInfo depthStencil {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = mCurrentDepthStencilDesc.DepthTestEnable ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable = mCurrentDepthStencilDesc.DepthWriteEnable ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp = toVkCompareOp(mCurrentDepthStencilDesc.DepthFunc);
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = mCurrentDepthStencilDesc.StencilEnable ? VK_TRUE : VK_FALSE;
        if (mCurrentDepthStencilDesc.StencilEnable)
        {
            depthStencil.front.failOp = toVkStencilOp(mCurrentDepthStencilDesc.FrontFace.StencilFailOp);
            depthStencil.front.passOp = toVkStencilOp(mCurrentDepthStencilDesc.FrontFace.StencilPassOp);
            depthStencil.front.depthFailOp = toVkStencilOp(mCurrentDepthStencilDesc.FrontFace.StencilDepthFailOp);
            depthStencil.front.compareOp = toVkCompareOp(mCurrentDepthStencilDesc.FrontFace.StencilFunc);
            depthStencil.front.compareMask = mCurrentDepthStencilDesc.StencilReadMask;
            depthStencil.front.writeMask = mCurrentDepthStencilDesc.StencilWriteMask;
            depthStencil.front.reference = mCurrentDepthStencilDesc.StencilRef;

            depthStencil.back.failOp = toVkStencilOp(mCurrentDepthStencilDesc.BackFace.StencilFailOp);
            depthStencil.back.passOp = toVkStencilOp(mCurrentDepthStencilDesc.BackFace.StencilPassOp);
            depthStencil.back.depthFailOp = toVkStencilOp(mCurrentDepthStencilDesc.BackFace.StencilDepthFailOp);
            depthStencil.back.compareOp = toVkCompareOp(mCurrentDepthStencilDesc.BackFace.StencilFunc);
            depthStencil.back.compareMask = mCurrentDepthStencilDesc.StencilReadMask;
            depthStencil.back.writeMask = mCurrentDepthStencilDesc.StencilWriteMask;
            depthStencil.back.reference = mCurrentDepthStencilDesc.StencilRef;
        }

        // Color blending — read from cached desc
        VkPipelineColorBlendAttachmentState colorBlendAttachment {};
        colorBlendAttachment.colorWriteMask = toVkColorWriteMask(rt0Blend.ColorMask);
        colorBlendAttachment.blendEnable = rt0Blend.BlendEnable ? VK_TRUE : VK_FALSE;
        if (rt0Blend.BlendEnable)
        {
            colorBlendAttachment.srcColorBlendFactor = toVkBlendFactor(rt0Blend.SrcBlend);
            colorBlendAttachment.dstColorBlendFactor = toVkBlendFactor(rt0Blend.DestBlend);
            colorBlendAttachment.colorBlendOp = toVkBlendOp(rt0Blend.BlendOp);
            colorBlendAttachment.srcAlphaBlendFactor = toVkBlendFactor(rt0Blend.SrcBlendAlpha);
            colorBlendAttachment.dstAlphaBlendFactor = toVkBlendFactor(rt0Blend.DstBlendAlpha);
            colorBlendAttachment.alphaBlendOp = toVkBlendOp(rt0Blend.BlendOpAlpha);
        }

        VkPipelineColorBlendStateCreateInfo colorBlending {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;

        // Depth-only pass: no color attachments in render pass
        bool isDepthOnlyPass = (mCurrentRenderTarget != nullptr
            && mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_TEXTURE
            && mCurrentRenderTarget->getNumOfRenderTextures() == 0);

        if (isDepthOnlyPass)
        {
            colorBlending.attachmentCount = 0;
            colorBlending.pAttachments = nullptr;
        }
        else
        {
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
        }

        // Dynamic state — viewport, scissor, and depth bias
        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_BIAS };
        VkPipelineDynamicStateCreateInfo dynamicState {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 3;
        dynamicState.pDynamicStates = dynamicStates;

        // Create pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = stageCount;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;

        // Dynamic descriptor set layout and pipeline layout
        size_t dslHash = 0;
        VkDescriptorSetLayout dsl = getOrCreateDescriptorSetLayout(dslHash);
        VkPipelineLayout pipelineLayout = getOrCreatePipelineLayout(dsl, dslHash);
        pipelineInfo.layout = pipelineLayout;

        // Use current render pass (set by beginPass)
        pipelineInfo.renderPass = currentRP;
        pipelineInfo.subpass = 0;

        VkPipeline pipeline = VK_NULL_HANDLE;
        VkResult result = vkCreateGraphicsPipelines(mVkDevice, mVkPipelineCache, 1, &pipelineInfo, nullptr, &pipeline);
        if (result != VK_SUCCESS)
        {
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "Failed to create graphics pipeline ! VK ERROR [%d]", result);
            return VK_NULL_HANDLE;
        }

        mPipelineCache[hash] = pipeline;
        return pipeline;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::bindDescriptorSet(VkCommandBuffer cmdBuf, VkPipelineLayout pipelineLayout)
    {
        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = mVkDescriptorPools[mCurrentFrame];
        allocInfo.descriptorSetCount = 1;

        size_t dslHash = 0;
        VkDescriptorSetLayout dsl = getOrCreateDescriptorSetLayout(dslHash);
        allocInfo.pSetLayouts = &dsl;

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        VkResult result = vkAllocateDescriptorSets(mVkDevice, &allocInfo, &descriptorSet);
        if (result != VK_SUCCESS)
            return T3D_ERR_VK_ALLOCATE_DESCRIPTOR_SET;

        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;

        // Reserve space to avoid reallocation invalidating pointers
        size_t totalBindings = mCurrentVSBindings.size() + mCurrentPSBindings.size();
        bufferInfos.reserve(totalBindings);
        imageInfos.reserve(totalBindings);

        // --- VS bindings (reflection-driven) ---
        uint32_t vsUboIdx = 0;
        for (const auto &b : mCurrentVSBindings)
        {
            if (b.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            {
                if (vsUboIdx < mCurrentVSConstantBuffers.size() && mCurrentVSConstantBuffers[vsUboIdx] != VK_NULL_HANDLE)
                {
                    VkDescriptorBufferInfo bufInfo {};
                    bufInfo.buffer = mCurrentVSConstantBuffers[vsUboIdx];
                    bufInfo.offset = 0;
                    bufInfo.range = mCurrentVSConstantBufferSizes[vsUboIdx];
                    bufferInfos.push_back(bufInfo);

                    VkWriteDescriptorSet write {};
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = descriptorSet;
                    write.dstBinding = b.binding;
                    write.dstArrayElement = 0;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    write.descriptorCount = 1;
                    write.pBufferInfo = &bufferInfos.back();
                    writes.push_back(write);
                }
                vsUboIdx++;
            }
        }

        // --- PS bindings (reflection-driven) ---
        // PS bindings have been remapped by +PS_BINDING_OFFSET in createPixelShader(),
        // but mCurrentPSTexStartSlot/mCurrentPSSamplerStartSlot store the original
        // (un-offset) binding numbers from shader reflection. So we subtract
        // PS_BINDING_OFFSET first to recover the original binding, then subtract startSlot.
        uint32_t psUboIdx = 0;
        for (const auto &b : mCurrentPSBindings)
        {
            switch (b.descriptorType)
            {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            {
                if (psUboIdx < mCurrentPSConstantBuffers.size() && mCurrentPSConstantBuffers[psUboIdx] != VK_NULL_HANDLE)
                {
                    VkDescriptorBufferInfo bufInfo {};
                    bufInfo.buffer = mCurrentPSConstantBuffers[psUboIdx];
                    bufInfo.offset = 0;
                    bufInfo.range = mCurrentPSConstantBufferSizes[psUboIdx];
                    bufferInfos.push_back(bufInfo);

                    VkWriteDescriptorSet write {};
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = descriptorSet;
                    write.dstBinding = b.binding;
                    write.dstArrayElement = 0;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    write.descriptorCount = 1;
                    write.pBufferInfo = &bufferInfos.back();
                    writes.push_back(write);
                }
                psUboIdx++;
                break;
            }
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            {
                uint32_t texIdx = (b.binding - PS_BINDING_OFFSET) - mCurrentPSTexStartSlot;
                if (texIdx < mCurrentPSImageViews.size() && mCurrentPSImageViews[texIdx] != VK_NULL_HANDLE)
                {
                    VkDescriptorImageInfo imgInfo {};
                    imgInfo.imageView = mCurrentPSImageViews[texIdx];
                    imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imgInfo.sampler = VK_NULL_HANDLE;
                    imageInfos.push_back(imgInfo);

                    VkWriteDescriptorSet write {};
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = descriptorSet;
                    write.dstBinding = b.binding;
                    write.dstArrayElement = 0;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    write.descriptorCount = 1;
                    write.pImageInfo = &imageInfos.back();
                    writes.push_back(write);
                }
                break;
            }
            case VK_DESCRIPTOR_TYPE_SAMPLER:
            {
                uint32_t sampIdx = (b.binding - PS_BINDING_OFFSET) - mCurrentPSSamplerStartSlot;
                if (sampIdx < mCurrentPSSamplers.size() && mCurrentPSSamplers[sampIdx] != VK_NULL_HANDLE)
                {
                    VkDescriptorImageInfo imgInfo {};
                    imgInfo.imageView = VK_NULL_HANDLE;
                    imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imgInfo.sampler = mCurrentPSSamplers[sampIdx];
                    imageInfos.push_back(imgInfo);

                    VkWriteDescriptorSet write {};
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = descriptorSet;
                    write.dstBinding = b.binding;
                    write.dstArrayElement = 0;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                    write.descriptorCount = 1;
                    write.pImageInfo = &imageInfos.back();
                    writes.push_back(write);
                }
                break;
            }
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            {
                uint32_t texIdx = (b.binding - PS_BINDING_OFFSET) - mCurrentPSTexStartSlot;
                uint32_t sampIdx = (b.binding - PS_BINDING_OFFSET) - mCurrentPSSamplerStartSlot;
                if (texIdx < mCurrentPSImageViews.size() && mCurrentPSImageViews[texIdx] != VK_NULL_HANDLE)
                {
                    VkDescriptorImageInfo imgInfo {};
                    imgInfo.imageView = mCurrentPSImageViews[texIdx];
                    imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imgInfo.sampler = (sampIdx < mCurrentPSSamplers.size()) ? mCurrentPSSamplers[sampIdx] : VK_NULL_HANDLE;
                    imageInfos.push_back(imgInfo);

                    VkWriteDescriptorSet write {};
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = descriptorSet;
                    write.dstBinding = b.binding;
                    write.dstArrayElement = 0;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write.descriptorCount = 1;
                    write.pImageInfo = &imageInfos.back();
                    writes.push_back(write);
                }
                break;
            }
            default:
                break;
            }
        }

        if (!writes.empty())
        {
            vkUpdateDescriptorSets(mVkDevice, (uint32_t)writes.size(), writes.data(), 0, nullptr);
        }

        vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
            0, 1, &descriptorSet, 0, nullptr);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        if (!mRenderPassActive)
        {
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "render() called outside an active render pass !");
            return T3D_ERR_INVALID_PARAM;
        }

        VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

        // Bind pipeline
        VkPipeline pipeline = getOrCreatePipeline();
        if (pipeline == VK_NULL_HANDLE)
            return T3D_ERR_VK_CREATE_PIPELINE;

        if (pipeline != mVkCurrentPipeline)
        {
            vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            mVkCurrentPipeline = pipeline;
        }

        // Set dynamic depth bias
        vkCmdSetDepthBias(cmdBuf,
            mCurrentRasterizerDesc.DepthBias,
            mCurrentRasterizerDesc.DepthBiasClamp,
            mCurrentRasterizerDesc.SlopeScaledDepthBias);

        if (mPendingScissorValid)
        {
            vkCmdSetScissor(cmdBuf, 0, 1, &mPendingScissor);
        }

        // Allocate and bind descriptor set (reflection-driven)
        size_t dslHash = 0;
        VkDescriptorSetLayout dsl = getOrCreateDescriptorSetLayout(dslHash);
        if (dsl != VK_NULL_HANDLE)
        {
            VkPipelineLayout pipelineLayout = getOrCreatePipelineLayout(dsl, dslHash);
            bindDescriptorSet(cmdBuf, pipelineLayout);
        }

        vkCmdDrawIndexed(cmdBuf, indexCount, 1, startIndex, baseVertex, 0);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::render(uint32_t vertexCount, uint32_t startVertex)
    {
        if (!mRenderPassActive)
        {
            T3D_LOG_ERROR(LOG_TAG_VKRENDERER, "render() called outside an active render pass !");
            return T3D_ERR_INVALID_PARAM;
        }

        VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

        // Bind pipeline
        VkPipeline pipeline = getOrCreatePipeline();
        if (pipeline == VK_NULL_HANDLE)
            return T3D_ERR_VK_CREATE_PIPELINE;

        if (pipeline != mVkCurrentPipeline)
        {
            vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            mVkCurrentPipeline = pipeline;
        }

        // Set dynamic depth bias
        vkCmdSetDepthBias(cmdBuf,
            mCurrentRasterizerDesc.DepthBias,
            mCurrentRasterizerDesc.DepthBiasClamp,
            mCurrentRasterizerDesc.SlopeScaledDepthBias);

        if (mPendingScissorValid)
        {
            vkCmdSetScissor(cmdBuf, 0, 1, &mPendingScissor);
        }

        // Allocate and bind descriptor set (reflection-driven)
        size_t dslHash = 0;
        VkDescriptorSetLayout dsl = getOrCreateDescriptorSetLayout(dslHash);
        if (dsl != VK_NULL_HANDLE)
        {
            VkPipelineLayout pipelineLayout = getOrCreatePipelineLayout(dsl, dslHash);
            bindDescriptorSet(cmdBuf, pipelineLayout);
        }

        vkCmdDraw(cmdBuf, vertexCount, 1, startVertex, 0);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContext::reset()
    {
        mCurrentRenderTarget = nullptr;
        mVkCurrentPipeline = VK_NULL_HANDLE;

        // Reset render state caches to defaults
        mCurrentRasterizerDesc = RasterizerDesc{};
        mCurrentBlendDesc = BlendDesc{};
        mCurrentDepthStencilDesc = DepthStencilDesc{};

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

        TResult ret = T3D_OK;

        do
        {
            VkCommandBuffer cmdBuf = mVkCommandBuffers[mCurrentFrame];

            RenderTexture *srcRT = static_cast<RenderTexture *>(src);

            // Get source VkImage
            VKPixelBuffer2D *vkSrcPB = static_cast<VKPixelBuffer2D *>(srcRT->getPixelBuffer()->getRHIResource().get());
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

            // Transition src to TRANSFER_SRC (using tracked layout)
            insertImageBarrier(cmdBuf, srcImage,
                vkSrcPB->VkCurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            // dst is already in TRANSFER_DST (set by beginRender for back buffer)

            // Compute blit regions
            PixelBuffer2D *srcPB2D = static_cast<PixelBuffer2D *>(srcRT->getPixelBuffer());
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

            // Restore src to SHADER_READ_ONLY for subsequent sampling
            insertImageBarrier(cmdBuf, srcImage,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkSrcPB->VkCurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } while (false);

        return ret;
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
    }

    //--------------------------------------------------------------------------

    TResult VKContext::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)
    {
        TResult ret = T3D_OK;

        do
        {
            VkBuffer vkBuf = (VkBuffer)renderBuffer->getRHIResource()->getNativeObject();
            // Find the device memory associated with this buffer
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
        } while (false);

        return ret;
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
