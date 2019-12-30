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


#include "T3DD3D11Prerequisites.h"
#include "T3DD3D11Capabilities.h"
#include "T3DD3D11Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11CapabilitiesPtr D3D11Capabilities::create()
    {
        D3D11CapabilitiesPtr cap = new D3D11Capabilities();
        if (cap == nullptr || cap->init() != T3D_OK)
        {
            cap = nullptr;
        }
        return cap;
    }

    //--------------------------------------------------------------------------

    D3D11Capabilities::D3D11Capabilities()
    {

    }

    //--------------------------------------------------------------------------

    D3D11Capabilities::~D3D11Capabilities()
    {

    }

    //--------------------------------------------------------------------------

    TResult D3D11Capabilities::init()
    {
        TResult ret = T3D_OK;

        ID3D11Device *pD3DDevice = D3D11_RENDERER.getD3DDevice();

        mDriverVersion = D3D11_RENDERER.getDriverVersion();
        mDeviceName = D3D11_RENDERER.getDeviceName();
        mRendererName = D3D11_RENDERER.getName();

        mNumTextureUnits = 16;
        mNumVertexTextureUnits = 4;
        mStencilBufferBitDepth = 8;

#ifdef D3D_FEATURE_LEVEL_9_3
        int numMultiRenderTargets = (mFeatureLevel > D3D_FEATURE_LEVEL_9_3) ? D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT :      // 8
            (mFeatureLevel == D3D_FEATURE_LEVEL_9_3) ? 4/*D3D_FL9_3_SIMULTANEOUS_RENDER_TARGET_COUNT*/ :    // 4
            1/*D3D_FL9_1_SIMULTANEOUS_RENDER_TARGET_COUNT*/;
#else
        int numMultiRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;     // 8
#endif

        mNumMultiRenderTargets = std::min(numMultiRenderTargets, 
            (int)T3D_MAX_MULTIPLE_RENDER_TARGETS);

//         mNumVertexAttributes = ;

        setCapability(Capabilities::PRIMITIVE_RESTART);
        setCapability(Capabilities::ANISOTROPY);

        D3D_FEATURE_LEVEL featureLevel = D3D11_RENDERER.getFeatureLevel();

        uint32_t fmtSupport;
        HRESULT hr = S_OK;
        hr = pD3DDevice->CheckFormatSupport(DXGI_FORMAT_R32_UINT, &fmtSupport);
        if (featureLevel >= D3D_FEATURE_LEVEL_9_2 || SUCCEEDED(hr)
            && 0 != (fmtSupport & D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER))
        {
            setCapability(Capabilities::INDEX_32BITS);
        }

        if (featureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            setCapability(Capabilities::TWO_SIDED_STENCIL);
        }

        mCategoryRelevant[CapabilitiesCategory::DIRECTX] = true;

        return ret;
    }
}
