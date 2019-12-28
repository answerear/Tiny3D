/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "Render/T3DRendererCapabilities.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    String RendererCapabilities::GPUVendorStrings[GPUVendor::GPU_VENDOR_COUNT];

    //--------------------------------------------------------------------------

    RendererCapabilities::RendererCapabilities()
    {

    }

    //--------------------------------------------------------------------------

    RendererCapabilities::~RendererCapabilities()
    {

    }

    //--------------------------------------------------------------------------

    void RendererCapabilities::initVendorStrings()
    {
        if (GPUVendorStrings[0].empty())
        {
            // Always lower case!
            GPUVendorStrings[GPUVendor::UNKNOWN] = "unknown";
            GPUVendorStrings[GPUVendor::NVIDIA] = "nvidia";
            GPUVendorStrings[GPUVendor::AMD] = "amd";
            GPUVendorStrings[GPUVendor::INTEL] = "intel";
            GPUVendorStrings[GPUVendor::IMAGINATION_TECHNOLOGIES] = "imagination technologies";
            GPUVendorStrings[GPUVendor::APPLE] = "apple";    // iOS Simulator
            GPUVendorStrings[GPUVendor::NOKIA] = "nokia";
            GPUVendorStrings[GPUVendor::MS_SOFTWARE] = "microsoft"; // Microsoft software device
            GPUVendorStrings[GPUVendor::MS_WARP] = "ms warp";
            GPUVendorStrings[GPUVendor::ARM] = "arm";
            GPUVendorStrings[GPUVendor::QUALCOMM] = "qualcomm";
            GPUVendorStrings[GPUVendor::MOZILLA] = "mozilla";
            GPUVendorStrings[GPUVendor::WEBKIT] = "webkit";
        }
    }
}
