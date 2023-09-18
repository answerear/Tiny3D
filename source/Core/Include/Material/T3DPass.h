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

#ifndef __T3D_PASS_H__
#define __T3D_PASS_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderState.h"


namespace Tiny3D
{
    /**
     * \brief 渲染 pass
     */
    TCLASS()
    class T3D_ENGINE_API Pass
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        static PassPtr create(const String &name);

        ~Pass() override = default;

        PassPtr clone() const;

        bool addTag(const String &key, const String &value);

        void removeTag(const String &key);
        
        bool getTag(const String &key, String &value) const;

        ShaderPtr getShader() const
        {
            return mShader;
        }

        void setShader(ShaderPtr shader)
        {
            mShader = shader;
        }

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="getter")
        const ShaderLabTags &getTags() const
        {
            return mTags;
        }

        TPROPERTY(RTTRFuncName="Shader", RTTRFuncType="getter")
        const String &getShaderName() const
        {
            return mShaderName;
        }

        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="getter")
        const RenderState &getRenderState() const
        {
            return mRenderState;
        }

        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="setter")
        void setRenderState(const RenderState &state)
        {
            mRenderState = state;
        }
        
    private:
        Pass() : Pass("") {}

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="setter")
        void setTags(const ShaderLabTags &tags)
        {
            mTags = tags;
        }

        TPROPERTY(RTTRFuncName="Shader", RTTRFuncType="setter")
        void setShaderName(const String &name) 
        {
            mShaderName = name;
        }
        
    protected:
        Pass(const String &name);

        /// Pass 名称
        String          mName {};
        /// Shader 名称
        String          mShaderName {};
        /// Tags
        ShaderLabTags   mTags {};
        /// 渲染状态
        RenderState     mRenderState {};
        /// 着色器对象
        ShaderPtr       mShader {nullptr};
    };
}


#endif    /*__T3D_PASS_H__*/
