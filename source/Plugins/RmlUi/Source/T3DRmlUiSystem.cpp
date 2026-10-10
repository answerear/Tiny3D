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

#include "T3DRmlUiSystem.h"

#include "T3DRmlCanvas.h"
#include "T3DRmlFileInterface.h"
#include "T3DRmlRenderInterface.h"
#include "T3DRmlSystemInterface.h"

#include "Component/T3DCamera.h"
#include "Device/T3DDeviceInfo.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "Render/T3DRenderWindow.h"
#include "Render/T3DViewport.h"
#include "Render/T3DPixelBuffer.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIRenderer.h"

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/ElementDocument.h>

#include <algorithm>


namespace Tiny3D
{
    namespace
    {
        const char *kFontPath = "assets/samples/ui/NotoSansSC-Regular.otf";
        const char *kFontFamily = "Noto Sans SC";

        bool targetHasStencil(RenderTarget *target)
        {
            if (target == nullptr)
            {
                return false;
            }

            const RenderTexturePtr depth = target->getDepthStencil();
            if (depth != nullptr && depth->getPixelBuffer() != nullptr)
            {
                const PixelFormat format = static_cast<PixelBuffer2D *>(depth->getPixelBuffer())->getDescriptor().format;
                return format == PixelFormat::E_PF_D24_UNORM_S8_UINT
                    || format == PixelFormat::E_PF_D32_FLOAT_S8X24_UINT;
            }

            // 窗口自带的深度不挂在 RenderTarget 上。D3D11 / GL4 / GLES3 窗口是 D24S8。
            // Vulkan 窗口是 D32，没有模板。
            if (target->getType() != RenderTarget::Type::E_RT_WINDOW)
            {
                return false;
            }

            const RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
            if (renderer == nullptr)
            {
                return false;
            }

            const String &name = renderer->getName();
            return name == RHIRenderer::DIRECT3D11
                || name == RHIRenderer::OPENGL4
                || name == RHIRenderer::OPENGLES3;
        }
    }

    RmlUiSystem *RmlUiSystem::sInstance = nullptr;

    //--------------------------------------------------------------------------

    RmlUiSystemPtr RmlUiSystem::create()
    {
        return T3D_NEW RmlUiSystem();
    }

    //--------------------------------------------------------------------------

    const String &RmlUiSystem::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult RmlUiSystem::startup()
    {
        if (mInitialised)
        {
            return T3D_OK;
        }

        mFile = T3D_NEW RmlFileInterface();
        mSystem = T3D_NEW RmlSystemInterface();
        mRender = T3D_NEW RmlRenderInterface();
        Rml::SetFileInterface(mFile);
        Rml::SetSystemInterface(mSystem);
        Rml::SetRenderInterface(mRender);

        if (!Rml::Initialise())
        {
            T3D_LOG_ERROR(LOG_TAG_RMLUI, "Rml::Initialise() failed.");
            T3D_DELETE mRender;
            T3D_DELETE mSystem;
            T3D_DELETE mFile;
            mRender = nullptr;
            mSystem = nullptr;
            mFile = nullptr;
            return T3D_ERR_FAIL;
        }

        mInitialised = true;
        sInstance = this;
        const Rml::String version = Rml::GetVersion();
        T3D_LOG_INFO(LOG_TAG_RMLUI, "RmlUi %s initialised.", version.c_str());
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::shutdown()
    {
        if (!mInitialised)
        {
            return;
        }

        // context 还在时渲染接口必须活着。RHI 在这之后才销毁。
        T3D_LOG_INFO(LOG_TAG_RMLUI, "Rml::Shutdown() before RHI destroy.");
        releaseAll();
        Rml::Shutdown();
        T3D_DELETE mRender;
        T3D_DELETE mSystem;
        T3D_DELETE mFile;
        mRender = nullptr;
        mSystem = nullptr;
        mFile = nullptr;
        mInitialised = false;
        mFontLoaded = false;
        if (sInstance == this)
        {
            sInstance = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::ensureFont()
    {
        if (mFontLoaded)
        {
            return;
        }

        // 样例档案在 applicationDidFinishLaunching 才挂上，startup 时还读不到字体。
        mFontLoaded = Rml::LoadFontFace(kFontPath, kFontFamily, Rml::Style::FontStyle::Normal,
            Rml::Style::FontWeight::Normal, true);
        if (!mFontLoaded && !mFontWarned)
        {
            mFontWarned = true;
            T3D_LOG_WARNING(LOG_TAG_RMLUI, "LoadFontFace failed: %s", kFontPath);
        }
    }

    //--------------------------------------------------------------------------

    bool RmlUiSystem::targetPixelSize(RenderTarget *target, int32_t &width, int32_t &height)
    {
        width = 0;
        height = 0;
        if (target == nullptr)
        {
            return false;
        }

        if (target->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            const RenderWindowPtr window = target->getRenderWindow();
            if (window == nullptr)
            {
                return false;
            }
            width = window->getDescriptor().Width;
            height = window->getDescriptor().Height;
        }
        else
        {
            const RenderTexturePtr texture = target->getRenderTexture();
            if (texture == nullptr)
            {
                return false;
            }
            width = static_cast<int32_t>(texture->getWidth());
            height = static_cast<int32_t>(texture->getHeight());
        }

        return width > 0 && height > 0;
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::syncCanvas(RmlCanvas *canvas)
    {
        Rml::Context *context = canvas->getContext();
        GameObject *owner = canvas->getGameObject();
        if (context == nullptr || owner == nullptr)
        {
            return;
        }

        CameraPtr camera = owner->getComponent<Camera>();
        if (camera == nullptr || camera->getRenderTarget() == nullptr)
        {
            return;
        }

        int32_t targetWidth = 0;
        int32_t targetHeight = 0;
        if (!targetPixelSize(camera->getRenderTarget(), targetWidth, targetHeight))
        {
            return;
        }

        const Viewport &viewport = camera->getViewport();
        const int32_t width = static_cast<int32_t>(static_cast<Real>(targetWidth) * viewport.Width);
        const int32_t height = static_cast<int32_t>(static_cast<Real>(targetHeight) * viewport.Height);
        if (width <= 0 || height <= 0)
        {
            return;
        }

        Real dpRatio = canvas->getDpRatio();
        if (dpRatio <= Real(0))
        {
#if defined(T3D_OS_ANDROID)
            dpRatio = DeviceInfo::getInstance().getScreenDPI() / Real(160);
#else
            dpRatio = DeviceInfo::getInstance().getScreenDPI() / Real(96);
#endif
            if (dpRatio <= Real(0))
            {
                dpRatio = Real(1);
            }
        }

        const Rml::Vector2i dimensions = context->GetDimensions();
        if (dimensions.x != width || dimensions.y != height)
        {
            context->SetDimensions(Rml::Vector2i(width, height));
        }
        if (context->GetDensityIndependentPixelRatio() != dpRatio)
        {
            context->SetDensityIndependentPixelRatio(dpRatio);
        }
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::loadDocuments(RmlCanvas *canvas)
    {
        if (canvas->documentsLoaded())
        {
            return;
        }

        Rml::Context *context = canvas->getContext();
        if (context == nullptr)
        {
            return;
        }

        bool loaded = !canvas->getDocuments().empty();
        for (const String &path : canvas->getDocuments())
        {
            Rml::ElementDocument *document = context->LoadDocument(path);
            if (document == nullptr)
            {
                loaded = false;
                T3D_LOG_WARNING(LOG_TAG_RMLUI, "LoadDocument failed: %s", path.c_str());
                continue;
            }
            document->Show();
        }

        canvas->setDocumentsLoaded(loaded);
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::update()
    {
        if (!mInitialised)
        {
            return;
        }

        ensureFont();
        for (RmlCanvas *canvas : mCanvases)
        {
            if (canvas == nullptr || !canvas->isActiveAndEnabled() || canvas->getContext() == nullptr)
            {
                continue;
            }

            syncCanvas(canvas);
            loadDocuments(canvas);
            canvas->getContext()->Update();
        }
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::render(RHIContext *ctx, Camera *camera, RenderTarget *target, UIRenderPhase phase)
    {
        if (!mInitialised || ctx == nullptr || camera == nullptr || target == nullptr)
        {
            return;
        }

        GameObject *owner = camera->getGameObject();
        TArray<RmlCanvas *> canvases;
        for (RmlCanvas *canvas : mCanvases)
        {
            if (canvas == nullptr || canvas->getContext() == nullptr || canvas->getGameObject() != owner)
            {
                continue;
            }
            if (!canvas->isActiveAndEnabled() || canvas->getRenderPhase() != phase)
            {
                continue;
            }
            canvases.push_back(canvas);
        }
        if (canvases.empty())
        {
            return;
        }

        std::stable_sort(canvases.begin(), canvases.end(), [](RmlCanvas *lhs, RmlCanvas *rhs)
        {
            return lhs->getSortOrder() < rhs->getSortOrder();
        });

        int32_t targetWidth = 0;
        int32_t targetHeight = 0;
        if (!targetPixelSize(target, targetWidth, targetHeight))
        {
            return;
        }

        const Viewport &viewport = camera->getViewport();
        const int32_t originX = static_cast<int32_t>(static_cast<Real>(targetWidth) * viewport.Left);
        const int32_t originY = static_cast<int32_t>(static_cast<Real>(targetHeight) * viewport.Top);
        const int32_t width = static_cast<int32_t>(static_cast<Real>(targetWidth) * viewport.Width);
        const int32_t height = static_cast<int32_t>(static_cast<Real>(targetHeight) * viewport.Height);
        if (width <= 0 || height <= 0)
        {
            return;
        }

        ctx->setRenderTarget(target);
        ctx->setViewport(viewport);
        ctx->beginPass();
        if (mRender->beginFrame(ctx, width, height, originX, originY, targetHasStencil(target)))
        {
            for (RmlCanvas *canvas : canvases)
            {
                canvas->getContext()->Render();
            }
            mRender->endFrame();
        }
        ctx->endPass();
    }

    //--------------------------------------------------------------------------

    bool RmlUiSystem::isPointerOverUI() const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    bool RmlUiSystem::wantsKeyboard() const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::releaseAll()
    {
        TArray<RmlCanvas *> canvases = mCanvases;
        mCanvases.clear();
        for (RmlCanvas *canvas : canvases)
        {
            if (canvas != nullptr)
            {
                canvas->releaseContext();
            }
        }
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::registerCanvas(RmlCanvas *canvas)
    {
        if (canvas == nullptr)
        {
            return;
        }
        if (std::find(mCanvases.begin(), mCanvases.end(), canvas) == mCanvases.end())
        {
            mCanvases.push_back(canvas);
        }
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::unregisterCanvas(RmlCanvas *canvas)
    {
        auto found = std::find(mCanvases.begin(), mCanvases.end(), canvas);
        if (found != mCanvases.end())
        {
            mCanvases.erase(found);
        }
    }

    //--------------------------------------------------------------------------
}
