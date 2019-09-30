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


#ifndef __T3D_TEXTURE_UNIT_H__
#define __T3D_TEXTURE_UNIT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "T3DObject.h"
#include "Resource/T3DTexture.h"
#include "Kernel/T3DCommon.h"


namespace Tiny3D
{
    class T3D_ENGINE_API TextureUnit : public Object
    {
    public:
        enum TexgtureAddressMode
        {
            E_TAM_UNKNOWN = 0,
            E_TAM_WRAP,
            E_TAM_MIRROR,
            E_TAM_CLAMP,
            E_TAM_BORDER,
        };

        /**
         * @brief 描述纹理绑定到渲染流水线中可编程流水线阶段上
         */
        enum BindingType
        {
            E_BT_FRAGMENT = 0,
            E_BT_VERTEX,
            E_BT_GEOMETRY,
            E_BT_TESSELATION_HULL,
            E_BT_TESSELATION_DOMAIN,
            E_BT_COMPUTE
        };

        /**
         * @brief 描述纹理内容的标识
         */
        enum ContentType
        {
            E_CONTENT_NAMED = 0,    /// 通过名字标识纹理
            E_CONTENT_SHADOW,       /// 阴影纹理
        };

        struct UVWAddressMode
        {
            TexgtureAddressMode u, v, w;
        };

        TextureUnit();

        virtual ~TextureUnit();

    protected:
        Pass        *mParent;           /// 当前纹理单元所属的pass对象

        String      mName;              /// 纹理单元名称
        String      mNameAlias;         /// 纹理单元别名

        TextureType mTextureType;       /// 纹理类型
        PixelFormat mTextureFormat;     /// 纹理格式

        bool        mCubic;             /// 是否cubemap

        uint16_t    mCurrentFrame;      /// 当前帧计数
        Real        mAnimDuration;      /// 动画持续时长

        uint32_t    mTexCoordSetIndex;  /// 当前材质使用的纹理层级索引

        ColorARGB   mBorderColor;       /// border 模式下的边缘颜色

        FilterOptions mMinFilter;

        UVWAddressMode  mAddressMode;   /// 3个维度的纹理寻址模式
        BindingType     mBindingType;   /// 纹理绑定到渲染流水线可编程阶段
        ContentType     mContentType;   /// 纹理标识方式，普通加载的还是自动创建的

        StringArray     mFrames;
    };
}


#endif  /*__T3D_TEXTURE_UNIT_H__*/
