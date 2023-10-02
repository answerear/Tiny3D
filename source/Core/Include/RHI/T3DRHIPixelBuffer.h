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


#ifndef __T3D_RHI_PIXEL_BUFFER_H__
#define __T3D_RHI_PIXEL_BUFFER_H__


#include "RHI/T3DRHIResource.h"


namespace Tiny3D
{
    /**
     * @brief   像素缓冲区
     * @sa  class HardwareBuffer
     */
    class T3D_ENGINE_API RHIPixelBuffer : public RHIResource
    {
    public:
        ResourceType getResourceType() const override { return ResourceType::kPixelBuffer; }
        
    protected:
        RHIPixelBuffer();
    };
}


#endif  /*__T3D_RHI_PIXEL_BUFFER_H__*/
