/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "Component/T3DCamera.h"

#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "Kernel/T3DGameObject.h"
// #include "RHI/T3DRHIContext.h"
// #include "Render/T3DRenderResourceManager.h"
#include "Render/T3DRenderWindow.h"
// #include "Render/T3DVertexDeclaration.h"
// #include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DTextureManager.h"
// #include "Render/T3DVertexDeclaration.h"
// #include "Render/T3DVertexBuffer.h"
// #include "Render/T3DIndexBuffer.h"



namespace Tiny3D
{
    //--------------------------------------------------------------------------

    CameraPtr Camera::create()
    {
        return new Camera();
    }

    //--------------------------------------------------------------------------
    
    Camera::~Camera() 
    {
        if (mRenderTexture != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mRenderTexture);
            mRenderTexture = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    void Camera::setRenderTarget(RenderTargetPtr target)
    {
        if (target != mRenderarget)
        {
            if (mRenderTexture != nullptr)
            {
                T3D_TEXTURE_MGR.unload(mRenderTexture);
                mRenderTexture = nullptr;
            }

            if (target != nullptr)
            {
                switch (target->getType())
                {
                case RenderTarget::Type::E_RT_WINDOW:
                    {
                        RenderWindowPtr renderWindow = target->getRenderWindow();
                    
                        // 新建个 render texture ，相机先渲染到 render texture 上，然后再画到屏幕上
                        setupRenderTexture(renderWindow);
                    
                        // 新建个 quad ，用于最后 blit render texture 到 render windows 上用
                        setupQuad(renderWindow);
                    }
                    break;
                case RenderTarget::Type::E_RT_TEXTURE:
                    {
                        // 渲染目标本来就是渲染纹理，则不创建了渲染纹理，直接绘制到渲染纹理上
                    }
                    break;
                }
            }
        }
        
        mRenderarget = target;
    }

    //--------------------------------------------------------------------------

    ComponentPtr Camera::clone() const
    {
        CameraPtr camera = new Camera();
        TResult ret = camera->cloneProperties(this);
        if (T3D_FAILED(ret))
        {
            camera = nullptr;
        }
        return camera;
    }

    //--------------------------------------------------------------------------

    void Camera::setupRenderTexture(RenderWindow *window)
    {
        String name = getGameObject()->getName() + "##RT";
        const RenderWindowDesc &desc = window->getDescriptor();
        PixelFormat format;
        switch (desc.ColorDepth)
        {
        case 8:
            format = PixelFormat::E_PF_PALETTE8;
            break;
        case 16:
            format = PixelFormat::E_PF_B5G6R5;
            break;
        case 24:
            format = PixelFormat::E_PF_B8G8R8;
            break;
        case 32:
            format = PixelFormat::E_PF_B8G8R8A8;
            break;
        default:
            format = PixelFormat::E_PF_B8G8R8;
            break;
        }
        mRenderTexture = T3D_TEXTURE_MGR.createRenderTexture(name, desc.Width, desc.Height, format, 1, desc.MSAA.Count, desc.MSAA.Quality);
    }

    //--------------------------------------------------------------------------

    void Camera::setupQuad(RenderWindow *window)
    {
        // if (mVB == nullptr)
        // {
        //     // 没有创建过才创建
        //     struct Vertex
        //     {
        //         Vector3 position;
        //         Vector2 uv;
        //     };
        //
        //     Vertex vertices[4] =
        //     {
        //         { Vector3(-1.0f, 1.0f, 0.5f), Vector2(0.0f, 0.0f) },
        //         { Vector3(-1.0f, -1.0f, 0.5f), Vector2(0.0f, 1.0f) },
        //         { Vector3(1.0f, 1.0f, 0.5f), Vector2(1.0f, 0.0f) },
        //         { Vector3(1.0f, -1.0f, 0.5f), Vector2(1.0f, 1.0f) }
        //     };
        //     
        //     // vertex input layout
        //     VertexDeclarationPtr decl = VertexDeclaration::create();
        //     decl->addAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
        //     decl->addAttribute(0, 12, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
        //     T3D_RENDER_BUFFER_MGR.addVertexDeclaration(decl);
        //
        //     // vertex buffer
        //     Buffer buffer;
        //     buffer.setData(&vertices, sizeof(vertices));
        //     T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(Vertex), 4, buffer, MemoryType::kVRAM, Usage::kImmutable, kCPUNone);
        //
        //     
        // }
    }

    //--------------------------------------------------------------------------
}
