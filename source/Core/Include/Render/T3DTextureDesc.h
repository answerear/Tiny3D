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


#ifndef __T3D_TEXTURE_DESC_H__
#define __T3D_TEXTURE_DESC_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DMultisamplerDesc.h"


namespace Tiny3D
{

    
    TSTRUCT()
    struct T3D_ENGINE_API TextureDesc
    {
        /// 纹理宽度
        TPROPERTY()
        int32_t             Width {0};
        /// 纹理高度
        TPROPERTY()
        int32_t             Height {0};
        /// 纹理色深
        TPROPERTY()
        int32_t             ColorDepth {0};
        /// 抗锯齿采样参数
        TPROPERTY()
        MultisamplerDesc    MSAA {};
        /// Mipmap
        TPROPERTY()
        int32_t             Mipmap {0};
        /// 纹理名称
        TPROPERTY()
        String              Name {};
        /// 纹理格式
        TPROPERTY()
        PixelFormat         Format {PixelFormat::E_PF_UNKNOWN};
    };
}


#endif  /*__T3D_TEXTURE_DESC_H__*/
