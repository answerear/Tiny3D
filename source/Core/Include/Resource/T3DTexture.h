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

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "Resource/T3DResource.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DRenderBufferDesc.h"
#include "Render/T3DRenderConstant.h"
#include "Render/T3DSamplerState.h"


namespace Tiny3D
{
    /**
     * \brief 纹理资源基类，持有采样器状态及像素缓冲访问接口
     */
    TCLASS()
    class T3D_ENGINE_API Texture : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        /// 析构函数
        ~Texture() override = default;

        /**
         * \brief 获取资源类型
         * \return 固定返回 Type::kTexture
         */
        Type getType() const override;

        /**
         * \brief 获取具体纹理维度类型
         * \return 由子类返回 TEXTURE_TYPE 枚举值
         */
        virtual TEXTURE_TYPE getTextureType() const = 0;

        /**
         * \brief 获取 GPU 像素缓冲对象
         * \return 已创建渲染资源时返回 PixelBuffer；否则可能为 nullptr
         */
        virtual PixelBuffer *getPixelBuffer() const = 0;

        /// 获取采样器状态对象
        const SamplerStatePtr &getSamplerState() const { return mSamplerState; }

        /// 获取采样器描述
        TPROPERTY(RTTRFuncName="SamplerDesc", RTTRFuncType="getter")
        const SamplerDesc &getSamplerDesc() const { return mSamplerState->getStateDesc(); }

        /**
         * \brief 设置采样器描述
         * \param [in] desc : 新采样器参数
         * \remarks 与当前描述 CRC 不同时通过 RenderStateManager 加载或复用 SamplerState
         */
        TPROPERTY(RTTRFuncName="SamplerDesc", RTTRFuncType="setter")
        void setSamplerDesc(const SamplerDesc &desc);

        /**
         * \brief 获取创建时声明的 CPU 访问许可（CPUAccessMode 组合）
         * \return 默认 kCPUNone；带 kCPURead 才允许 map
         */
        uint32_t getCPUAccessMode() const { return mCPUAccessMode; }

        /**
         * \brief 发起本纹理的 GPU→CPU 读回，只录制 Copy
         * \param [in] region : 待读回的子区域
         * \return 成功返回读回票据，失败返回 ReadbackHandle::invalid()
         * \remarks 必须在 Application::onRender 里调用，配对的 unmap 放 onPostRender。
         *          创建时必须声明 kCPURead，否则被拒。
         */
        ReadbackHandle map(const ReadbackRegion &region);

        /**
         * \brief 消费 map 的结果
         * \param [in] handle : map 返回的票据
         * \param [out] dst : 紧凑排布的像素数据，调用方负责 release()
         * \return 调用成功返回 T3D_OK
         * \warning 阻塞等待 GPU，只用于验证 / 截帧，不要放进游戏热路径
         */
        TResult unmap(ReadbackHandle handle, Buffer &dst);
        
    private:
        /// RTTR 默认构造入口
        Texture() : Texture("") {}
        
    protected:
        /**
         * \brief 以名称构造纹理
         * \param [in] name : 纹理名称
         */
        Texture(const String &name);
        
        /**
         * \brief 从源纹理拷贝通用属性
         * \param [in] src : 源 Texture 对象
         */
        void cloneProperties(const Resource * const src) override;

    protected:
        /// 纹理采样器状态
        SamplerStatePtr     mSamplerState {nullptr};
        /**
         * \brief CPU 访问许可（CPUAccessMode 组合），创建时指定
         * \remarks kCPURead 只表示「允许对这块 GPU 数据做读回」，读回走引擎内部的
         *          staging 中转。资源本身仍然是 IMMUTABLE / DEFAULT，不会带上
         *          原生的 CPU_ACCESS_READ，也不影响它作为贴图 / RT 使用。
         */
        uint32_t            mCPUAccessMode {kCPUNone};
    };

    /**
     * \brief 一维纹理资源
     */
    TCLASS()
    class T3D_ENGINE_API Texture1D : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 1D 纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度（texel）
         * \param [in] format : 像素格式
         * \param [in] mipmaps : mipmap 级数
         * \param [in] data : 像素数据缓冲
         * \return 新建的 Texture1D 智能指针
         */
        static Texture1DPtr create(const String &name, uint32_t width, PixelFormat format, uint32_t mipmaps, const Buffer &data);

        /// 析构；像素数据若未交给 PixelBuffer 则由本对象释放
        ~Texture1D() override;

        /**
         * \brief 获取纹理维度类型
         * \return TEXTURE_TYPE::TT_1D
         */
        TEXTURE_TYPE getTextureType() const override;

        /// 获取 CPU 端像素数据指针
        const uint8_t * const getData() const { return mDesc.buffer.Data; }

        /// 获取 CPU 端像素数据字节数
        size_t getDataSize() const { return mDesc.buffer.DataSize; }

        /// 获取纹理宽度
        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="getter")
        uint32_t getWidth() const { return mDesc.width; }

        /// 获取像素格式
        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="getter")
        PixelFormat getPixelFormat() const { return mDesc.format; }

        /// 获取 mipmap 级数
        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="getter")
        uint32_t getMipmaps() const { return mDesc.mipmaps; }

        /**
         * \brief 获取 1D PixelBuffer
         * \return mPixelBuffer 裸指针
         */
        PixelBuffer *getPixelBuffer() const override;

    private:
        /// RTTR 默认构造入口
        Texture1D() : Texture("") {}
        
    protected:
        /**
         * \brief 构造 1D 纹理并填充描述
         * \param [in] data : 像素数据；所有权在 PixelBuffer 创建前由本对象持有
         */
        Texture1D(const String &name, uint32_t width, PixelFormat format, uint32_t mipmaps, const Buffer &data);
        
        /**
         * \brief 克隆 1D 纹理
         * \return 新 Texture1D 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 创建 GPU 1D PixelBuffer
         * \return 失败时返回对应错误码
         */
        TResult onCreate() override;

        /**
         * \brief 加载后创建 GPU 1D PixelBuffer
         * \param [in] archive : 来源档案（基类回调使用）
         * \return 失败时返回对应错误码
         */
        TResult onLoad(Archive *archive) override;

        /// 设置宽度
        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        /// 设置像素格式
        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="setter")
        void setPixelFormat(PixelFormat format) { mDesc.format = format; }
        
        /// 设置 mipmap 级数
        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="setter")
        void setMipmaps(uint32_t mipmaps) { mDesc.mipmaps = mipmaps; }

        /// 获取像素数据缓冲
        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="getter")
        const Buffer &getBuffer() const { return mDesc.buffer; }

        /**
         * \brief 设置像素数据（浅拷贝）
         * \param [in] data : 新数据；与旧 Data 不同时先 release 旧块
         */
        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="setter")
        void setBuffer(const Buffer &data);

    protected:
        /// 1D 像素缓冲描述
        PixelBuffer1DDesc   mDesc {};
        /// GPU 1D 像素缓冲
        PixelBuffer1DPtr    mPixelBuffer {nullptr};
    };

    /**
     * \brief 二维纹理资源
     */
    TCLASS()
    class T3D_ENGINE_API Texture2D : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        /**
         * \brief 用原始像素数据创建 2D 纹理
         * \param [in] MSAACount : MSAA 采样数
         * \param [in] MSAAQuality : MSAA 质量
         * \param [in] accMode : CPU 访问许可，传 kCPURead 才允许 map 读回
         */
        static Texture2DPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data, uint32_t accMode = kCPUNone);

        /**
         * \brief 从 Image 创建 2D 纹理
         * \param [in] iamge : 源 Image 对象，不可为 nullptr
         * \param [in] accMode : CPU 访问许可，传 kCPURead 才允许 map 读回
         * \remarks 引用 Image 像素指针并持有 mImage 智能指针
         */
        static Texture2DPtr create(const String &name, Image *iamge, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, uint32_t accMode = kCPUNone);

        /// 析构；像素数据若未交给 PixelBuffer 则由本对象释放
        ~Texture2D() override;

        /**
         * \brief 获取纹理维度类型
         * \return TEXTURE_TYPE::TT_2D
         */
        TEXTURE_TYPE getTextureType() const override;

        /// 获取 CPU 端像素数据指针
        const uint8_t * const getData() const { return mDesc.buffer.Data; }

        /// 获取 CPU 端像素数据字节数
        size_t getDataSize() const { return mDesc.buffer.DataSize; }

        /// 获取纹理宽度
        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="getter")
        uint32_t getWidth() const { return mDesc.width; }

        /// 获取纹理高度
        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="getter")
        uint32_t getHeight() const { return mDesc.height; }

        /// 获取像素格式
        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="getter")
        PixelFormat getPixelFormat() const { return mDesc.format; }

        /// 获取 mipmap 级数
        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="getter")
        uint32_t getMipmaps() const { return mDesc.mipmaps; }

        /// 获取 MSAA 描述
        TPROPERTY(RTTRFuncName="MSAA", RTTRFuncType="getter")
        const MultisamplerDesc &getMSAADesc() const { return mDesc.sampleDesc; }

        /**
         * \brief 获取 2D PixelBuffer
         * \return mPixelBuffer 裸指针
         */
        PixelBuffer *getPixelBuffer() const override;
        
    protected:
        /// RTTR 默认构造入口
        Texture2D() : Texture("") {}
        
        /**
         * \brief 用 Buffer 构造 2D 纹理
         * \param [in] shaderReadable : 是否允许 Shader 读取（写入 mDesc.shaderReadable）
         */
        Texture2D(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data, bool shaderReadable = false, uint32_t accMode = kCPUNone);

        /**
         * \brief 从 Image 构造 2D 纹理
         * \param [in] image : 源 Image，不可为 nullptr
         */
        Texture2D(const String &name, Image *image, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, bool shaderReadable = false, uint32_t accMode = kCPUNone);
        
        /**
         * \brief 克隆 2D 纹理
         * \return 新 Texture2D 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 创建 GPU 2D PixelBuffer
         * \return 失败时返回对应错误码
         */
        TResult onCreate() override;

        /**
         * \brief 加载后创建 GPU 2D PixelBuffer
         * \param [in] archive : 来源档案
         * \return 失败时返回对应错误码
         */
        TResult onLoad(Archive *archive) override;

        /**
         * \brief 卸载并释放关联 Image 引用
         * \return 基类 onUnload 的返回值
         */
        TResult onUnload() override;

        /// 设置宽度
        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        /// 设置高度
        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="setter")
        void setHeight(uint32_t height) { mDesc.height = height; }

        /// 设置像素格式
        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="setter")
        void setPixelFormat(PixelFormat format) { mDesc.format = format; }

        /// 设置 mipmap 级数
        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="setter")
        void setMipmaps(uint32_t mipmaps) { mDesc.mipmaps = mipmaps; }

        /// 设置 MSAA 描述
        TPROPERTY(RTTRFuncName="MSAA", RTTRFuncType="setter")
        void setMSAADesc(const MultisamplerDesc &desc) { mDesc.sampleDesc = desc; }

        /// 获取像素数据缓冲
        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="getter")
        const Buffer &getBuffer() const { return mDesc.buffer; }

        /**
         * \brief 设置像素数据（浅拷贝）
         * \param [in] data : 新数据；与旧 Data 不同时先 release 旧块
         */
        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="setter")
        void setBuffer(const Buffer &data);

    protected:
        /// 2D 像素缓冲描述
        PixelBuffer2DDesc   mDesc {};
        /// GPU 2D 像素缓冲
        PixelBuffer2DPtr    mPixelBuffer {nullptr};
        /// 从 Image 创建时持有的源 Image
        ImagePtr            mImage {nullptr};
    };

    /**
     * \brief 二维纹理数组资源
     */
    TCLASS()
    class T3D_ENGINE_API Texture2DArray : public Texture2D
    {
        TRTTI_ENABLE(Texture2D)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 2D 纹理数组
         * \param [in] arraySize : 数组层数
         */
        static Texture2DArrayPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        /**
         * \brief 获取纹理维度类型
         * \return TEXTURE_TYPE::TT_2D_ARRAY
         */
        TEXTURE_TYPE getTextureType() const override;

        /// 获取数组层数
        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="getter")
        uint32_t getArraySize() const { return mDesc.arraySize; }
        
    private:
        /// RTTR 默认构造入口
        Texture2DArray() {};
        
    protected:
        /**
         * \brief 构造 2D 纹理数组
         * \param [in] arraySize : 层数，写入 mDesc.arraySize
         */
        Texture2DArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        /**
         * \brief 克隆 2D 纹理数组
         * \return 新 Texture2DArray 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 创建 GPU 资源（转调 Texture2D::onCreate）
         * \return 失败时返回对应错误码
         */
        TResult onCreate() override;

        /**
         * \brief 加载后创建 GPU 资源（转调 Texture2D::onLoad）
         * \param [in] archive : 来源档案
         * \return 失败时返回对应错误码
         */
        TResult onLoad(Archive *archive) override;

        /// 设置数组层数
        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="setter")
        void setArraySize(uint32_t arraySize) { mDesc.arraySize = arraySize; }
    };

    /**
     * \brief 三维纹理资源
     */
    TCLASS()
    class T3D_ENGINE_API Texture3D : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 3D 纹理
         * \param [in] depth : 深度（texel）
         */
        static Texture3DPtr create(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t mipmaps, const Buffer &data);

        /// 析构；像素数据若未交给 PixelBuffer 则由本对象释放
        ~Texture3D() override;

        /**
         * \brief 获取纹理维度类型
         * \return TEXTURE_TYPE::TT_3D
         */
        TEXTURE_TYPE getTextureType() const override;

        /// 获取 CPU 端像素数据指针
        const uint8_t * const getData() const { return mDesc.buffer.Data; }

        /// 获取 CPU 端像素数据字节数
        size_t getDataSize() const { return mDesc.buffer.DataSize; }

        /// 获取纹理宽度
        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="getter")
        uint32_t getWidth() const { return mDesc.width; }

        /// 获取纹理高度
        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="getter")
        uint32_t getHeight() const { return mDesc.height; }

        /// 获取纹理深度
        TPROPERTY(RTTRFuncName="Depth", RTTRFuncType="getter")
        uint32_t getDepth() const { return mDesc.depth; }

        /// 获取像素格式
        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="getter")
        PixelFormat getPixelFormat() const { return mDesc.format; }

        /// 获取 mipmap 级数
        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="getter")
        uint32_t getMipmaps() const { return mDesc.mipmaps; }

        /**
         * \brief 获取 3D PixelBuffer
         * \return mPixelBuffer 裸指针；当前 onCreate/onLoad 未创建缓冲时为 nullptr
         */
        PixelBuffer *getPixelBuffer() const override;
        
    private:
        /// RTTR 默认构造入口
        Texture3D() : Texture("") {}
        
    protected:
        /**
         * \brief 构造 3D 纹理
         */
        Texture3D(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t mipmaps, const Buffer &data);
        
        /**
         * \brief 克隆 3D 纹理
         * \return 新 Texture3D 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 资源创建回调
         * \return 当前实现未创建 PixelBuffer，返回 Texture::onCreate 结果
         */
        TResult onCreate() override;

        /**
         * \brief 资源加载回调
         * \param [in] archive : 来源档案
         * \return 当前实现未创建 PixelBuffer，返回 Texture::onLoad 结果
         */
        TResult onLoad(Archive *archive) override;

        /// 设置宽度
        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        /// 设置高度
        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="setter")
        void setHeight(uint32_t height) { mDesc.height = height; }

        /// 设置深度
        TPROPERTY(RTTRFuncName="Depth", RTTRFuncType="setter")
        void setDepth(uint32_t depth) { mDesc.depth = depth; }
        
        /// 设置像素格式
        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="setter")
        void setPixelFormat(PixelFormat format) { mDesc.format = format; }

        /// 设置 mipmap 级数
        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="setter")
        void setMipmaps(uint32_t mipmaps) { mDesc.mipmaps = mipmaps; }

        /// 获取像素数据缓冲
        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="getter")
        const Buffer &getBuffer() const { return mDesc.buffer; }

        /**
         * \brief 设置像素数据（浅拷贝）
         * \param [in] data : 新数据；与旧 Data 不同时先 release 旧块
         */
        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="setter")
        void setBuffer(const Buffer &data);

    protected:
        /// 3D 像素缓冲描述
        PixelBuffer3DDesc   mDesc {};
        /// GPU 3D 像素缓冲
        PixelBuffer3DPtr    mPixelBuffer {nullptr};
    };

    /**
     * \brief 立方体贴图资源（6 面，顺序 +X/-X/+Y/-Y/+Z/-Z）
     */
    TCLASS()
    class T3D_ENGINE_API Cubemap : public Texture2D
    {
        TRTTI_ENABLE(Texture2D)
        TRTTI_FRIEND

    public:
        /// 立方体面数，排列顺序 +X, -X, +Y, -Y, +Z, -Z
        static const uint32_t FACE_COUNT = 6;

        /**
         * \brief 创建立方体贴图
         * \remarks 构造时将 mDesc.arraySize 设为 6
         */
        static CubemapPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        /// 析构；若数据已交给 mCubePixelBuffer 则从 mDesc 摘除指针避免双重释放
        ~Cubemap() override;

        /**
         * \brief 获取纹理维度类型
         * \return TEXTURE_TYPE::TT_CUBE
         */
        TEXTURE_TYPE getTextureType() const override;

        /**
         * \brief 获取立方体 PixelBuffer
         * \return mCubePixelBuffer 裸指针（非基类 mPixelBuffer）
         */
        PixelBuffer *getPixelBuffer() const override;

    protected:
        /// RTTR 默认构造入口
        Cubemap() {}
        
        /**
         * \brief 构建立方体贴图
         */
        Cubemap(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        /**
         * \brief 克隆立方体贴图
         * \return 新 Cubemap 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 跳过 Texture2D::onCreate，创建立方体 PixelBuffer
         * \return createCubePixelBuffer 的返回值
         */
        TResult onCreate() override;

        /**
         * \brief 加载后创建立方体 PixelBuffer
         * \param [in] archive : 来源档案
         * \return createCubePixelBuffer 的返回值
         */
        TResult onLoad(Archive *archive) override;

        /**
         * \brief 创建立方体 PixelBuffer 并设置 Clamp 采样
         * \return 调用成功返回 T3D_OK
         * \remarks 普通 Cubemap 将 arraySize 修正为 6；Sampler 默认 Clamp 避免面接缝采样错误
         */
        TResult createCubePixelBuffer();

    protected:
        /// 立方体贴图 GPU 像素缓冲（替代基类 mPixelBuffer）
        PixelBufferCubemapPtr   mCubePixelBuffer {nullptr};
    };

    /**
     * \brief 立方体贴图数组资源
     */
    TCLASS()
    class T3D_ENGINE_API CubemapArray : public Cubemap
    {
        TRTTI_ENABLE(Cubemap)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建立方体贴图数组
         * \param [in] arraySize : 立方体个数（内部存储为 arraySize * 6 面）
         */
        static CubemapArrayPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        /**
         * \brief 获取纹理维度类型
         * \return TEXTURE_TYPE::TT_CUBE_ARRAY
         */
        TEXTURE_TYPE getTextureType() const override;

        /// 获取立方体个数（mDesc.arraySize / 6）
        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="getter")
        uint32_t getArraySize() const { return mDesc.arraySize / 6; }
        
    protected:
        /// RTTR 默认构造入口
        CubemapArray() {}
        
        /**
         * \brief 构建立方体贴图数组
         * \param [in] arraySize : 立方体个数
         */
        CubemapArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        /**
         * \brief 克隆立方体贴图数组
         * \return 新 CubemapArray 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 创建 GPU 资源（转调 Cubemap::onCreate）
         * \return 失败时返回对应错误码
         */
        TResult onCreate() override;

        /**
         * \brief 加载后创建 GPU 资源（转调 Cubemap::onLoad）
         * \param [in] archive : 来源档案
         * \return 失败时返回对应错误码
         */
        TResult onLoad(Archive *archive) override;
        
        /**
         * \brief 设置立方体个数
         * \param [in] arraySize : 立方体个数（内部乘以 6 写入 mDesc.arraySize）
         */
        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="setter")
        void setArraySize(uint32_t arraySize) { mDesc.arraySize = arraySize * 6; }
    };
}


#endif    /*__T3D_TEXTURE_H__*/
