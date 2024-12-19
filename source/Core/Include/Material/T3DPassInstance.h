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
#include "Material/T3DShaderVariantInstance.h"


namespace Tiny3D
{
    class T3D_ENGINE_API PassInstance : public Object
    {
    public:
        static PassInstancePtr create(TechniqueInstance *parent, PassPtr pass);

        virtual ~PassInstance() = default;

        PassInstancePtr clone(TechniqueInstance *parent) const;

        TechniqueInstance *getTechInstance() const { return mTechInstance; }

        PassPtr getPass() const { return mPass; }

        const ShaderKeyword &getCurrentKeyword() const { return mCurrentKeyword; }

        TResult switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys);

        TResult switchKeyword(const ShaderKeyword &keyword);

        ShaderVariantInstance *getCurrentVertexShader() const { return mCurrentVS; }

        ShaderVariantInstance *getCurrentHullShader() const { return mCurrentHS; }

        ShaderVariantInstance *getCurrentDomainShader() const { return mCurrentDS; }

        ShaderVariantInstance *getCurrentGeometryShader() const { return mCurrentGS; }

        ShaderVariantInstance *getCurrentPixelShader() const { return mCurrentPS; }

        void setBool(const String &name, bool value);

        void setBoolArray(const String &name, const BoolArray &values);

        void setFloat(const String &name, float32_t value);

        void setFloatArray(const String &name, const FloatArray &values);

        void setInteger(const String &name, int32_t value);

        void setIntArray(const String &name, const IntArray &values);

        void setColor(const String &name, const ColorRGBA &value);

        void setColorArray(const String &name, const ColorArray &values);
        
        void setVector(const String &name, const Vector4 &value);

        void setVectorArray(const String &name, const Vector4Array &values);

        void setMatrix(const String &name, const Matrix4 &value);

        void setMatrixArray(const String &name, const Matrix4Array &values);

        void setData(const String &name, const void *data, uint32_t dataSize);

        void setTexture(const String &name, const UUID &uuid);
        
    protected:
        PassInstance() = default;
        
        PassInstance(TechniqueInstance *parent, PassPtr pass);

        TResult cloneProperties(TechniqueInstance *parent, const PassInstance * const src);

        TResult switchShaderVariants(const ShaderKeyword &keyword, const ShaderVariants &shaderVariants, ShaderVariantInstancePtr &currentVariant);

        template <typename Value_t, typename ShaderSetValue_t>
        void setValue(ShaderVariantInstance *shader, const String &name, const Value_t &value, ShaderSetValue_t shaderSetValue)
        {
            if (shader != nullptr)
            {
                (shader->*shaderSetValue)(name, value);
            }
        }

        void setData(ShaderVariantInstance *shader, const String &name, const void *data, size_t size)
        {
            if (shader != nullptr)
            {
                shader->setData(name, data, size);
            }
        }

        void setTexture(ShaderVariantInstance *shader, const String &name, const UUID &uuid)
        {
            if (shader != nullptr)
            {
                shader->setTexture(name, uuid);
            }
        }
        
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

    //--------------------------------------------------------------------------

    inline void PassInstance::setBool(const String &name, bool value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setBool);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setBool);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setBool);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setBool);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setBool);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setBoolArray(const String &name, const BoolArray &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setBoolArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setBoolArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setBoolArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setBoolArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setBoolArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setFloat(const String &name, float32_t value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setFloat);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setFloat);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setFloat);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setFloat);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setFloat);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setFloatArray(const String &name, const FloatArray &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setFloatArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setFloatArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setFloatArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setFloatArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setFloatArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setInteger(const String &name, int32_t value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setInteger);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setInteger);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setInteger);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setInteger);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setInteger);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setIntArray(const String &name, const IntArray &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setIntArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setIntArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setIntArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setIntArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setIntArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setColor(const String &name, const ColorRGBA &value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setColor);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setColor);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setColor);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setColor);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setColor);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setColorArray(const String &name, const ColorArray &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setColorArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setColorArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setColorArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setColorArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setColorArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setVector(const String &name, const Vector4 &value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setVector);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setVector);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setVector);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setVector);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setVector);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setVectorArray(const String &name, const Vector4Array &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setVectorArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setVectorArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setVectorArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setVectorArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setVectorArray);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setMatrix(const String &name, const Matrix4 &value)
    {
        setValue(mCurrentVS, name, value, &ShaderVariantInstance::setMatrix);
        setValue(mCurrentPS, name, value, &ShaderVariantInstance::setMatrix);
        setValue(mCurrentGS, name, value, &ShaderVariantInstance::setMatrix);
        setValue(mCurrentHS, name, value, &ShaderVariantInstance::setMatrix);
        setValue(mCurrentDS, name, value, &ShaderVariantInstance::setMatrix);
    }

    //--------------------------------------------------------------------------
    
    inline void PassInstance::setMatrixArray(const String &name, const Matrix4Array &values)
    {
        setValue(mCurrentVS, name, values, &ShaderVariantInstance::setMatrixArray);
        setValue(mCurrentPS, name, values, &ShaderVariantInstance::setMatrixArray);
        setValue(mCurrentGS, name, values, &ShaderVariantInstance::setMatrixArray);
        setValue(mCurrentHS, name, values, &ShaderVariantInstance::setMatrixArray);
        setValue(mCurrentDS, name, values, &ShaderVariantInstance::setMatrixArray);
    }
    
    //--------------------------------------------------------------------------

    inline void PassInstance::setData(const String &name, const void *data, uint32_t dataSize)
    {
        setData(mCurrentVS, name, data, dataSize);
        setData(mCurrentPS, name, data, dataSize);
        setData(mCurrentGS, name, data, dataSize);
        setData(mCurrentHS, name, data, dataSize);
        setData(mCurrentDS, name, data, dataSize);
    }

    //--------------------------------------------------------------------------

    inline void PassInstance::setTexture(const String &name, const UUID &uuid)
    {
        setTexture(mCurrentVS, name, uuid);
        setTexture(mCurrentPS, name, uuid);
        setTexture(mCurrentGS, name, uuid);
        setTexture(mCurrentHS, name, uuid);
        setTexture(mCurrentDS, name, uuid);
    }

    //--------------------------------------------------------------------------
}


#endif  /*__T3D_PASS_INSTANCE_H__*/
