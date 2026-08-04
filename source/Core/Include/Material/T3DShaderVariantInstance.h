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
    /**
     * \brief ShaderVariant 的运行时实例，管理常量缓冲区、纹理 LUT 与 RHI 绑定资源
     */
    class T3D_ENGINE_API ShaderVariantInstance : public Object
    {
    public:
        /**
         * \brief 创建 ShaderVariantInstance 并按反射信息初始化常量缓冲与纹理 LUT
         * \param [in] parent : 所属的 PassInstance
         * \param [in] shaderVariant : 对应的 ShaderVariant 定义
         * \return 新建的 ShaderVariantInstance 智能指针
         */
        static ShaderVariantInstancePtr create(PassInstance *parent, ShaderVariantPtr shaderVariant);

        /// 析构函数
        ~ShaderVariantInstance() override = default;

        /**
         * \brief 克隆本实例的属性到新的 ShaderVariantInstance
         * \param [in] parent : 新实例所属的 PassInstance
         * \return 克隆成功返回新实例；cloneProperties 失败时返回 nullptr
         */
        ShaderVariantInstancePtr clone(PassInstance *parent) const;

        /// 获取所属的 PassInstance
        PassInstance *getPassInstance() const { return mPassInstance; }

        /// 获取对应的 ShaderVariant 定义
        ShaderVariantPtr getShaderVariant() const { return mShaderVariant; }

        /// 获取 RHI 常量缓冲区列表
        const ConstantBuffers &getConstantBuffers() const { return mConstantBuffers; }

        /// 获取 RHI 采样器列表
        const Samplers &getSamplers() const { return mSamplers; }

        /// 获取 RHI 像素缓冲区列表
        const PixelBuffers &getPixelBuffers() const { return mPixelBuffers; }

        /// 获取采样器起始槽位（反射结果中的最小 binding）
        uint32_t getSamplerStartSlot() const { return mSamplerStartSlot; }

        /// 获取像素缓冲区起始槽位
        uint32_t getPixelBufferStartSlot() const { return mPixelBufferStartSlot; }

        /**
         * \brief 将全部引擎层常量缓冲数据写入 RHI 常量缓冲，并返回最小 binding 槽位
         * \param [out] startSlot : 输出最小 binding 槽位；无缓冲时为 uint32_t 最大值
         * \return 调用成功返回 T3D_OK
         */
        TResult updateConstantBuffers(uint32_t &startSlot);

        /**
         * \brief 按名称写入 bool 常量到对应 cbuffer 偏移
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setBool(const String &name, bool value);

        /**
         * \brief 按名称读取 bool 常量
         * \param [in] name : 常量名称
         * \return 常量值；未找到时返回 false
         */
        bool getBool(const String &name) const;

        /**
         * \brief 判断是否存在指定名称的常量
         * \param [in] name : 常量名称
         * \return 存在时返回 true
         */
        bool hasBool(const String &name) const;

        /**
         * \brief 按名称写入 bool 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         * \note 目标缓冲区大小须与数组字节数一致，否则静默跳过
         */
        void setBoolArray(const String &name, const BoolArray &values);

        /**
         * \brief 按名称读取 bool 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 未使用的占位参数
         * \return 数组视图；未找到时返回空数组
         */
        BoolArray getBoolArray(const String &name, const BoolArray &values) const;

        /**
         * \brief 按名称写入 int 常量
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setInteger(const String &name, int32_t value);

        /**
         * \brief 按名称读取 int 常量
         * \param [in] name : 常量名称
         * \return 常量值；未找到时返回 0
         */
        int32_t getInteger(const String &name) const;

        /**
         * \brief 按名称写入 int 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         */
        void setIntArray(const String &name, const IntArray &values);

        /**
         * \brief 按名称读取 int 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 未使用的占位参数
         * \return 数组视图；未找到时返回空数组
         */
        IntArray getIntArray(const String &name, const IntArray &values) const;

        /**
         * \brief 判断是否存在指定名称的 int 常量
         * \param [in] name : 常量名称
         * \return 存在时返回 true
         */
        bool hasInteger(const String &name) const;

        /**
         * \brief 按名称写入 float 常量
         * \param [in] name : 常量名称
         * \param [in] value : 常量值
         */
        void setFloat(const String &name, float32_t value);

        /**
         * \brief 按名称读取 float 常量
         * \param [in] name : 常量名称
         * \return 常量值；未找到时返回 0
         */
        float32_t getFloat(const String &name) const;

        /**
         * \brief 判断是否存在指定名称的 float 常量
         * \param [in] name : 常量名称
         * \return 存在时返回 true
         */
        bool hasFloat(const String &name) const;

        /**
         * \brief 按名称写入 float 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 常量值数组
         */
        void setFloatArray(const String &name, const FloatArray &values);

        /**
         * \brief 按名称读取 float 数组常量
         * \param [in] name : 常量名称
         * \return 数组视图；未找到时返回空数组
         */
        FloatArray getFloatArray(const String &name) const;

        /**
         * \brief 按名称写入颜色常量
         * \param [in] name : 常量名称
         * \param [in] value : 颜色值
         */
        void setColor(const String &name, const ColorRGBA &value);

        /**
         * \brief 按名称读取颜色常量
         * \param [in] name : 常量名称
         * \return 颜色值
         */
        ColorRGBA getColor(const String &name) const;

        /**
         * \brief 判断是否存在指定名称的颜色常量
         * \param [in] name : 常量名称
         * \return 存在时返回 true
         */
        bool hasColor(const String &name) const;

        /**
         * \brief 按名称写入颜色数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 颜色数组
         */
        void setColorArray(const String &name, const ColorArray &values);

        /**
         * \brief 按名称读取颜色数组常量
         * \param [in] name : 常量名称
         * \return 数组视图；未找到时返回空数组
         */
        ColorArray getColorArray(const String &name) const;

        /**
         * \brief 按名称写入 Vector4 常量
         * \param [in] name : 常量名称
         * \param [in] value : 向量值
         */
        void setVector(const String &name, const Vector4 &value);

        /**
         * \brief 按名称读取 Vector4 常量
         * \param [in] name : 常量名称
         * \return 向量值
         */
        Vector4 getVector(const String &name) const;

        /**
         * \brief 判断是否存在指定名称的 Vector4 常量
         * \param [in] name : 常量名称
         * \return 存在时返回 true
         */
        bool hasVector(const String &name) const;

        /**
         * \brief 按名称写入 Vector4 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 向量数组
         */
        void setVectorArray(const String &name, const Vector4Array &values);

        /**
         * \brief 按名称读取 Vector4 数组常量
         * \param [in] name : 常量名称
         * \return 数组视图；未找到时返回空数组
         */
        Vector4Array getVectorArray(const String &name) const;

        /**
         * \brief 按名称写入 Matrix4 常量
         * \param [in] name : 常量名称
         * \param [in] value : 矩阵值
         */
        void setMatrix(const String &name, const Matrix4 &value);

        /**
         * \brief 按名称读取 Matrix4 常量
         * \param [in] name : 常量名称
         * \return 矩阵值
         */
        Matrix4 getMatrix(const String &name) const;

        /**
         * \brief 判断是否存在指定名称的 Matrix4 常量
         * \param [in] name : 常量名称
         * \return 存在时返回 true
         */
        bool hasMatrix(const String &name) const;

        /**
         * \brief 按名称写入 Matrix4 数组常量
         * \param [in] name : 常量名称
         * \param [in] values : 矩阵数组
         */
        void setMatrixArray(const String &name, const Matrix4Array &values);

        /**
         * \brief 按名称读取 Matrix4 数组常量
         * \param [in] name : 常量名称
         * \return 数组视图；未找到时返回空数组
         */
        Matrix4Array getMatrixArray(const String &name) const;

        /**
         * \brief 按名称写入原始常量数据
         * \param [in] name : 常量名称
         * \param [in] data : 数据指针
         * \param [in] dataSize : 数据字节数
         * \return 写入成功返回 true；名称未找到或大小不匹配时返回 false
         */
        bool setData(const String &name, const void *data, uint32_t dataSize);

        /**
         * \brief 按名称读取原始常量数据
         * \param [in] name : 常量名称
         * \param [out] data : 输出缓冲区
         * \param [in] dataSize : 缓冲区字节数
         * \return 读取成功返回 true；名称未找到或大小不匹配时返回 false
         */
        bool getData(const String &name, void *data, uint32_t dataSize);

        /**
         * \brief 按名称设置纹理，并同步更新 PixelBuffer 与 Sampler
         * \param [in] name : 采样器/纹理名称
         * \param [in] uuid : 纹理资源 UUID
         * \note UUID 对应纹理不存在时静默跳过
         */
        void setTexture(const String &name, const UUID &uuid);

        /**
         * \brief 按名称获取已设置的 Texture 对象
         * \param [in] name : 采样器/纹理名称
         * \return Texture 指针；未找到时返回 nullptr
         */
        Texture *getTexture(const String &name) const;

        /**
         * \brief 判断是否存在指定名称的纹理绑定
         * \param [in] name : 采样器/纹理名称
         * \return 存在时返回 true
         */
        bool hasTexture(const String &name) const;
        
    protected:
        /// 默认构造，供 clone 使用
        ShaderVariantInstance() = default;
        
        /**
         * \brief 构造实例并按 ShaderVariant 反射信息分配常量缓冲与纹理 LUT
         * \param [in] parent : 所属的 PassInstance
         * \param [in] shaderVariant : 对应的 ShaderVariant 定义
         */
        ShaderVariantInstance(PassInstance *parent, ShaderVariantPtr shaderVariant);

        /**
         * \brief 从源实例复制常量缓冲、纹理 LUT 与 RHI 资源
         * \param [in] parent : 新实例所属的 PassInstance；为 nullptr 时使用源实例的 parent
         * \param [in] src : 源 ShaderVariantInstance
         * \return 调用成功返回 T3D_OK
         */
        TResult cloneProperties(PassInstance *parent, const ShaderVariantInstance * const src);

        /**
         * \brief 按常量名称查找其在 cbuffer 中的数据指针与大小
         * \param [in] name : 常量名称
         * \param [out] data : 输出数据指针（指向 cbuffer 内偏移处）
         * \param [out] dataSize : 输出变量字节数
         * \return 找到时返回 true，否则返回 false
         */
        bool getConstantBufferInfo(const String &name, uint8_t *&data, uint32_t &dataSize) const;
        
        /**
         * \brief 按名称将原始数据 memcpy 到 cbuffer 对应偏移
         * \param [in] name : 常量名称
         * \param [in] data : 源数据指针
         * \param [in] dataSize : 数据字节数
         * \return 写入成功返回 true；未找到或大小不匹配时返回 false
         */
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

        /**
         * \brief 按名称从 cbuffer 对应偏移 memcpy 读取原始数据
         * \param [in] name : 常量名称
         * \param [out] data : 目标缓冲区
         * \param [in] dataSize : 缓冲区字节数
         * \return 读取成功返回 true；未找到或大小不匹配时返回 false
         */
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
        /// 常量名称 → 引擎层 Buffer 的快速查找表
        using ConstBuffersLUT = TUnorderedMap<String, Buffer>;

        /// 纹理 LUT 条目，记录采样器/像素缓冲/纹理数组索引
        struct TexLUTItem
        {
            uint32_t samplerIndex {0};
            uint32_t pixelBufferIndex {0};
            uint32_t textureIndex {0};
        };
        
        /// 纹理名称 → TexLUTItem 的快速查找表
        using TexturesLUT = TUnorderedMap<String, TexLUTItem>;
        
        /// 所属的 PassInstance
        PassInstance *mPassInstance {nullptr};
        /// 对应的 ShaderVariant 定义
        ShaderVariantPtr mShaderVariant {nullptr};
        
        /// 常量名称 → 引擎层 Buffer 的 LUT
        ConstBuffersLUT mConstBuffersLUT {};
        /// 引擎层常量缓冲数据（用于读写）
        ConstBuffers mConstBuffers {};

        /// 纹理名称 → 索引信息的 LUT
        TexturesLUT mTexturesLUT {};
        /// 引擎层纹理对象数组
        Textures mTextures {};

        /// 采样器起始槽位
        uint32_t mSamplerStartSlot {0};
        /// 像素缓冲区起始槽位
        uint32_t mPixelBufferStartSlot {0};
        
        /// RHI 常量缓冲区
        ConstantBuffers mConstantBuffers {};
        /// RHI 采样器
        Samplers mSamplers {};
        /// RHI 像素缓冲区
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
        uint8_t *dst = nullptr;
        uint32_t dstSize = 0;
        if (getConstantBufferInfo(name, dst, dstSize))
        {
            if (dstSize == values.size() * sizeof(bool))
            {
                std::copy(values.begin(), values.end(), (bool*)dst);
            }
        }
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
