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

#ifndef __T3D_PASS_INSTANCE_H__
#define __T3D_PASS_INSTANCE_H__


#include "T3DTypedef.h"
#include "Material/T3DShaderKeyword.h"


namespace Tiny3D
{
    class T3D_ENGINE_API PassInstance : public Object
    {
    public:
        static PassInstancePtr create(TechniqueInstance *parent, PassPtr pass);

        virtual ~PassInstance() = default;

        TechniqueInstance *getTechInstance() const { return mTechInstance; }

        PassPtr getPass() const { return mPass; }

        TResult switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys);
        
        ShaderVariantInstance *getCurrentVertexShader() const { return mCurrentVS; }

        ShaderVariantInstance *getCurrentHullShader() const { return mCurrentHS; }

        ShaderVariantInstance *getCurrentDomainShader() const { return mCurrentDS; }

        ShaderVariantInstance *getCurrentGeometryShader() const { return mCurrentGS; }

        ShaderVariantInstance *getCurrentPixelShader() const { return mCurrentPS; }
        
    protected:
        PassInstance(TechniqueInstance *parent, PassPtr pass);

        TResult switchShaderVariants(const ShaderKeyword &keyword, const ShaderVariants &shaderVariants, ShaderVariantInstancePtr &currentVariant);
        
    protected:
        /// pass instance 所属的 technique instancee
        TechniqueInstance   *mTechInstance {nullptr};
        
        /// pass 实例对应的 pass
        PassPtr             mPass {nullptr};
        
        /// 当前生效的关键字 
        ShaderKeyword       mCurrentKeyword {};
        
        /// 当前生效的 Vertex Shader 变体
        ShaderVariantInstancePtr    mCurrentVS {nullptr};
        /// 当前生效的 Pixel Shader 变体
        ShaderVariantInstancePtr    mCurrentPS {nullptr};
        /// 当前生效的 Geometry Shader 变体
        ShaderVariantInstancePtr    mCurrentGS {nullptr};
        /// 当前生效的 Hull Shader 变体
        ShaderVariantInstancePtr    mCurrentHS {nullptr};
        /// 当前生效的 Domain Shader 变体
        ShaderVariantInstancePtr    mCurrentDS {nullptr};
    };
}


#endif  /*__T3D_PASS_INSTANCE_H__*/
