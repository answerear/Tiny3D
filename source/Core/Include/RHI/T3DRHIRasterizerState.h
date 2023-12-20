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


#ifndef __T3D_RHI_RASTERIZER_STATE_H__
#define __T3D_RHI_RASTERIZER_STATE_H__


#include "RHI/T3DRHIResource.h"


namespace Tiny3D
{
    /**
     * \brief 渲染硬件层的光栅化状态
     */
    class T3D_ENGINE_API RHIRasterizerState : public RHIResource
    {
    public:
        ResourceType getResourceType() const override
        {
            return ResourceType::kRasterizerState;
        }
        
    protected:
        RHIRasterizerState();
    };
}


#endif  /*__T3D_RHI_RASTERIZER_STATE_H__*/
