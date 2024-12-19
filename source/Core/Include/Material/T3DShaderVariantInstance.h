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

#ifndef __T3D_SHADER_VARIANT_INSTANCE_H__
#define __T3D_SHADER_VARIANT_INSTANCE_H__


#include "T3DShaderVariant.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ShaderVariantInstance : public Object
    {
    public:
        static ShaderVariantInstancePtr create(PassInstance *parent, ShaderVariantPtr shaderVariant);

        ~ShaderVariantInstance() override = default;

        ShaderVariantInstancePtr clone(PassInstance *parent) const;

        PassInstance *getPassInstance() const { return mPassInstance; }

        ShaderVariantPtr getShaderVariant() const { return mShaderVariant; }

        const ConstantBuffers &getConstantBuffers() const { return mConstantBuffers; }

        const Samplers &getSamplers() const { return mSamplers; }

        const PixelBuffers &getPixelBuffers() const { return mPixelBuffers; }

        uint32_t getSamplerStartSlot() const { return mSamplerStartSlot; }

        uint32_t getPixelBufferStartSlot() const { return mPixelBufferStartSlot; }

        TResult updateConstantBuffers(uint32_t &startSlot);

        void setBool(const String &name, bool value);

        bool getBool(const String &name) const;

        bool hasBool(const String &name) const;

        void setBoolArray(const String &name, const BoolArray &values);

        BoolArray getBoolArray(const String &name, const BoolArray &values) const;

        void setInteger(const String &name, int32_t value);

        int32_t getInteger(const String &name) const;

        void setIntArray(const String &name, const IntArray &values);

        IntArray getIntArray(const String &name, const IntArray &values) const;

        bool hasInteger(const String &name) const;

        void setFloat(const String &name, float32_t value);

        float32_t getFloat(const String &name) const;

        bool hasFloat(const String &name) const;

        void setFloatArray(const String &name, const FloatArray &values);

        FloatArray getFloatArray(const String &name) const;

        void setColor(const String &name, const ColorRGBA &value);

        ColorRGBA getColor(const String &name) const;

        bool hasColor(const String &name) const;

        void setColorArray(const String &name, const ColorArray &values);

        ColorArray getColorArray(const String &name) const;

        void setVector(const String &name, const Vector4 &value);

        Vector4 getVector(const String &name) const;

        bool hasVector(const String &name) const;

        void setVectorArray(const String &name, const Vector4Array &values);

        Vector4Array getVectorArray(const String &name) const;

        void setMatrix(const String &name, const Matrix4 &value);

        Matrix4 getMatrix(const String &name) const;

        bool hasMatrix(const String &name) const;

        void setMatrixArray(const String &name, const Matrix4Array &values);

        Matrix4Array getMatrixArray(const String &name) const;

        bool setData(const String &name, const void *data, uint32_t dataSize);

        bool getData(const String &name, void *data, uint32_t dataSize);

        void setTexture(const String &name, const UUID &uuid);

        Texture *getTexture(const String &name) const;

        bool hasTexture(const String &name) const;
        
    protected:
        ShaderVariantInstance() = default;
        
        ShaderVariantInstance(PassInstance *parent, ShaderVariantPtr shaderVariant);

        TResult cloneProperties(PassInstance *parent, const ShaderVariantInstance * const src);

        bool getConstantBufferInfo(const String &name, uint8_t *&data, uint32_t &dataSize) const;
        
        bool setValue(const String &name, const void *data, uint32_t dataSize)
        {
            uint8_t *dst = nullptr;
            uint32_t dstSize = 0;
            if (getConstantBufferInfo(name, dst, dstSize))
            {
                if (dstSize != dataSize)
                    return false;
                memcpy(dst, data, dataSize);
                return true;
            }
            return false;
        }

        bool getValue(const String &name, void *data, uint32_t dataSize) const
        {
            uint8_t *src = nullptr;
            uint32_t srcSize = 0;
            if (getConstantBufferInfo(name, src, srcSize))
            {
                if (srcSize != dataSize)
                    return false;
                memcpy(data, src, dataSize);
                return true;
            }
            return false;
        }
        
    protected:
        /// String : 常量名称
       /// Buffer : 常量数据对象
        using ConstBuffersLUT = TUnorderedMap<String, Buffer>;

        struct TexLUTItem
        {
            uint32_t samplerIndex {0};
            uint32_t pixelBufferIndex {0};
        };
        
        /// String : 纹理采样器名称
       /// uint32_t : 纹理所在纹理数组的索引 
        using TexturesLUT = TUnorderedMap<String, TexLUTItem>;
        
        /// shader variant instance 所属的 pass instance
        PassInstance *mPassInstance {nullptr};
        /// shader variant 实例对应的 shader variant
        ShaderVariantPtr mShaderVariant {nullptr};
        
        /// 常量缓冲区 LUT ，方便根据名称快速找到来进行读写
        ConstBuffersLUT mConstBuffersLUT {};
        /// 引擎层使用的常量缓冲区，也就是实际数据
        ConstBuffers mConstBuffers {};

        /// 纹理 LUT
        TexturesLUT mTexturesLUT {};
        /// 引擎层使用的纹理，也就是实际纹理数据
        Textures mTextures {};

        /// 采样器起始索引
        uint32_t mSamplerStartSlot {0};
        /// 像素缓冲区起始索引
        uint32_t mPixelBufferStartSlot {0};
        
        /// RHI 使用的常量缓冲区
        ConstantBuffers mConstantBuffers {};
        /// RHI 使用的采样器
        Samplers mSamplers {};
        /// RHI 使用的像素缓冲区
        PixelBuffers mPixelBuffers {};
    };

    //--------------------------------------------------------------------------

    inline void ShaderVariantInstance::setBool(const String &name, bool value)
    {
        setValue(name, &value, sizeof(value));
    }

    //--------------------------------------------------------------------------
    
    inline bool ShaderVariantInstance::getBool(const String &name) const
    {
        bool value = false;
        getValue(name, &value, sizeof(value));
        return value;
    }

    //--------------------------------------------------------------------------
    
    inline bool ShaderVariantInstance::hasBool(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        return getConstantBufferInfo(name, data, dataSize);
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setBoolArray(const String &name, const BoolArray &values)
    {
        const auto &val = values[0];
        setValue(name, &val, static_cast<uint32_t>(values.size() * sizeof(bool)));
    }

    //--------------------------------------------------------------------------
    
    inline BoolArray ShaderVariantInstance::getBoolArray(const String &name, const BoolArray &values) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        if (getConstantBufferInfo(name, data, dataSize))
        {
            return BoolArray((bool*)data, (bool*)(data+dataSize));
        }
        return BoolArray();
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setInteger(const String &name, int32_t value)
    {
        setValue(name, &value, sizeof(value));
    }

    //--------------------------------------------------------------------------
    
    inline int32_t ShaderVariantInstance::getInteger(const String &name) const
    {
        int32_t value = 0;
        getValue(name, &value, sizeof(value));
        return value;
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setIntArray(const String &name, const IntArray &values)
    {
        setValue(name, values.data(), static_cast<uint32_t>(values.size() * sizeof(int32_t)));
    }

    //--------------------------------------------------------------------------
    
    inline IntArray ShaderVariantInstance::getIntArray(const String &name, const IntArray &values) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        if (getConstantBufferInfo(name, data, dataSize))
        {
            return IntArray((int32_t*)data, (int32_t *)(data+dataSize));
        }
        return IntArray();
    }

    //--------------------------------------------------------------------------
    
    inline bool ShaderVariantInstance::hasInteger(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        return getConstantBufferInfo(name, data, dataSize);
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setFloat(const String &name, float32_t value)
    {
        setValue(name, &value, sizeof(value));
    }

    //--------------------------------------------------------------------------
    
    inline float32_t ShaderVariantInstance::getFloat(const String &name) const
    {
        float32_t value = 0;
        getValue(name, &value, sizeof(value));
        return value;
    }

    //--------------------------------------------------------------------------
    
    inline bool ShaderVariantInstance::hasFloat(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        return getConstantBufferInfo(name, data, dataSize);
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setFloatArray(const String &name, const FloatArray &values)
    {
        setValue(name, values.data(), static_cast<uint32_t>(values.size() * sizeof(float32_t)));
    }

    //--------------------------------------------------------------------------
    
    inline FloatArray ShaderVariantInstance::getFloatArray(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        if (getConstantBufferInfo(name, data, dataSize))
        {
            return FloatArray((float32_t*)data, (float32_t *)(data+dataSize));
        }
        return FloatArray();
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setColor(const String &name, const ColorRGBA &value)
    {
        setValue(name, &value, sizeof(value));
    }

    //--------------------------------------------------------------------------
    
    inline ColorRGBA ShaderVariantInstance::getColor(const String &name) const
    {
        ColorRGBA value;
        getValue(name, &value, sizeof(value));
        return value;
    }

    //--------------------------------------------------------------------------
    
    inline bool ShaderVariantInstance::hasColor(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        return getConstantBufferInfo(name, data, dataSize);
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setColorArray(const String &name, const ColorArray &values)
    {
        setValue(name, values.data(), static_cast<uint32_t>(values.size() * sizeof(ColorRGBA)));
    }

    //--------------------------------------------------------------------------
    
    inline ColorArray ShaderVariantInstance::getColorArray(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        if (getConstantBufferInfo(name, data, dataSize))
        {
            return ColorArray((ColorRGBA*)data, (ColorRGBA*)(data+dataSize));
        }
        return ColorArray();
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setVector(const String &name, const Vector4 &value)
    {
        setValue(name, &value, sizeof(value));
    }

    //--------------------------------------------------------------------------
    
    inline Vector4 ShaderVariantInstance::getVector(const String &name) const
    {
        Vector4 value;
        getValue(name, &value, sizeof(value));
        return value;
    }

    //--------------------------------------------------------------------------
    
    inline bool ShaderVariantInstance::hasVector(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        return getConstantBufferInfo(name, data, dataSize);
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setVectorArray(const String &name, const Vector4Array &values)
    {
        setValue(name, values.data(), static_cast<uint32_t>(values.size() * sizeof(Vector4)));
    }

    //--------------------------------------------------------------------------
    
    inline Vector4Array ShaderVariantInstance::getVectorArray(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        if (getConstantBufferInfo(name, data, dataSize))
        {
            return Vector4Array((Vector4*)data, (Vector4*)(data+dataSize));
        }
        return Vector4Array();
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setMatrix(const String &name, const Matrix4 &value)
    {
        setValue(name, &value, sizeof(value));
    }

    //--------------------------------------------------------------------------
    
    inline Matrix4 ShaderVariantInstance::getMatrix(const String &name) const
    {
        Matrix4 value;
        getValue(name, &value, sizeof(value));
        return value;
    }

    //--------------------------------------------------------------------------
    
    inline bool ShaderVariantInstance::hasMatrix(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        return getConstantBufferInfo(name, data, dataSize);
    }

    //--------------------------------------------------------------------------
    
    inline void ShaderVariantInstance::setMatrixArray(const String &name, const Matrix4Array &values)
    {
        setValue(name, values.data(), static_cast<uint32_t>(values.size() * sizeof(Matrix4)));
    }

    //--------------------------------------------------------------------------
    
    inline Matrix4Array ShaderVariantInstance::getMatrixArray(const String &name) const
    {
        uint8_t *data = nullptr;
        uint32_t dataSize = 0;
        if (getConstantBufferInfo(name, data, dataSize))
        {
            return Matrix4Array((Matrix4*)data, (Matrix4*)(data+dataSize));
        }
        return Matrix4Array();
    }

    //--------------------------------------------------------------------------

    inline bool ShaderVariantInstance::setData(const String &name, const void *data, uint32_t dataSize)
    {
        return setValue(name, data, dataSize);
    }

    //--------------------------------------------------------------------------

    inline bool ShaderVariantInstance::getData(const String &name, void *data, uint32_t dataSize)
    {
        return getValue(name, data, dataSize);
    }

    //--------------------------------------------------------------------------

    inline bool ShaderVariantInstance::hasTexture(const String &name) const
    {
        const auto itr = mTexturesLUT.find(name);
        return (itr != mTexturesLUT.end());
    }

    //--------------------------------------------------------------------------
}


#endif  /*__T3D_SHADER_VARIANT_INSTANCE_H__*/
