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
#include "Render/T3DSamplerState.h"


namespace Tiny3D
{
    /**
     * \brief 纹理
     */
    TCLASS()
    class T3D_ENGINE_API Texture : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        ~Texture() override = default;

        Type getType() const override;

        virtual TEXTURE_TYPE getTextureType() const = 0;

        virtual PixelBuffer *getPixelBuffer() const = 0;

        const SamplerStatePtr &getSamplerState() const { return mSamplerState; }

        TPROPERTY(RTTRFuncName="SamplerDesc", RTTRFuncType="getter")
        const SamplerDesc &getSamplerDesc() const { return mSamplerState->getStateDesc(); }

        TPROPERTY(RTTRFuncName="SamplerDesc", RTTRFuncType="setter")
        void setSamplerDesc(const SamplerDesc &desc);
        
    private:
        Texture() : Texture("") {}
        
    protected:
        Texture(const String &name);
        
        void cloneProperties(const Resource * const src) override;

    protected:
        /// 纹理采样信息
        SamplerStatePtr     mSamplerState {nullptr};
    };

    TCLASS()
    class T3D_ENGINE_API Texture1D : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        static Texture1DPtr create(const String &name, uint32_t width, PixelFormat format, uint32_t mipmaps, const Buffer &data);

        TEXTURE_TYPE getTextureType() const override;

        const uint8_t * const getData() const { return mDesc.buffer.Data; }

        size_t getDataSize() const { return mDesc.buffer.DataSize; }

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="getter")
        uint32_t getWidth() const { return mDesc.width; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="getter")
        PixelFormat getPixelFormat() const { return mDesc.format; }

        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="getter")
        uint32_t getMipmaps() const { return mDesc.mipmaps; }

        // PixelBuffer1DPtr getPixelBuffer() const { return mPixelBuffer; }

        PixelBuffer *getPixelBuffer() const override;

    private:
        Texture1D() : Texture("") {}
        
    protected:
        Texture1D(const String &name, uint32_t width, PixelFormat format, uint32_t mipmaps, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad(Archive *archive) override;

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="setter")
        void setPixelFormat(PixelFormat format) { mDesc.format = format; }
        
        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="setter")
        void setMipmaps(uint32_t mipmaps) { mDesc.mipmaps = mipmaps; }

        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="getter")
        const Buffer &getBuffer() const { return mDesc.buffer; }

        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="setter")
        void setBuffer(const Buffer &data) { mDesc.buffer = data; }
        
    protected:
        /// 像素缓冲区描述
        PixelBuffer1DDesc   mDesc {};
        /// 像素缓冲区对象
        PixelBuffer1DPtr    mPixelBuffer {nullptr};
    };

    TCLASS()
    class T3D_ENGINE_API Texture2D : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        static Texture2DPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        static Texture2DPtr create(const String &name, Image *iamge, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality);

        TEXTURE_TYPE getTextureType() const override;

        const uint8_t * const getData() const { return mDesc.buffer.Data; }

        size_t getDataSize() const { return mDesc.buffer.DataSize; }

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="getter")
        uint32_t getWidth() const { return mDesc.width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="getter")
        uint32_t getHeight() const { return mDesc.height; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="getter")
        PixelFormat getPixelFormat() const { return mDesc.format; }

        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="getter")
        uint32_t getMipmaps() const { return mDesc.mipmaps; }

        TPROPERTY(RTTRFuncName="MSAA", RTTRFuncType="getter")
        const MultisamplerDesc &getMSAADesc() const { return mDesc.sampleDesc; }

        // PixelBuffer2DPtr getPixelBuffer() const { return mPixelBuffer; }

        PixelBuffer *getPixelBuffer() const override;
        
    protected:
        Texture2D() : Texture("") {}
        
        Texture2D(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        Texture2D(const String &name, Image *image, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad(Archive *archive) override;

        TResult onUnload() override;

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="setter")
        void setHeight(uint32_t height) { mDesc.height = height; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="setter")
        void setPixelFormat(PixelFormat format) { mDesc.format = format; }

        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="setter")
        void setMipmaps(uint32_t mipmaps) { mDesc.mipmaps = mipmaps; }

        TPROPERTY(RTTRFuncName="MSAA", RTTRFuncType="setter")
        void setMSAADesc(const MultisamplerDesc &desc) { mDesc.sampleDesc = desc; }

        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="getter")
        const Buffer &getBuffer() const { return mDesc.buffer; }

        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="setter")
        void setBuffer(const Buffer &data) { mDesc.buffer = data; }
        
    protected:
        /// 像素缓冲区描述
        PixelBuffer2DDesc   mDesc {};
        /// 像素缓冲区对象
        PixelBuffer2DPtr    mPixelBuffer {nullptr};
        /// 图像数据对象
        ImagePtr            mImage {nullptr};
    };

    TCLASS()
    class T3D_ENGINE_API Texture2DArray : public Texture2D
    {
        TRTTI_ENABLE(Texture2D)
        TRTTI_FRIEND

    public:
        static Texture2DArrayPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        TEXTURE_TYPE getTextureType() const override;

        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="getter")
        uint32_t getArraySize() const { return mDesc.arraySize; }
        
    private:
        Texture2DArray() {};
        
    protected:
        Texture2DArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad(Archive *archive) override;

        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="setter")
        void setArraySize(uint32_t arraySize) { mDesc.arraySize = arraySize; }
    };

    TCLASS()
    class T3D_ENGINE_API Texture3D : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        static Texture3DPtr create(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t mipmaps, const Buffer &data);

        TEXTURE_TYPE getTextureType() const override;

        const uint8_t * const getData() const { return mDesc.buffer.Data; }

        size_t getDataSize() const { return mDesc.buffer.DataSize; }

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="getter")
        uint32_t getWidth() const { return mDesc.width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="getter")
        uint32_t getHeight() const { return mDesc.height; }

        TPROPERTY(RTTRFuncName="Depth", RTTRFuncType="getter")
        uint32_t getDepth() const { return mDesc.depth; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="getter")
        PixelFormat getPixelFormat() const { return mDesc.format; }

        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="getter")
        uint32_t getMipmaps() const { return mDesc.mipmaps; }

        // PixelBuffer3DPtr getPixelBuffer() const { return mPixelBuffer; }

        PixelBuffer *getPixelBuffer() const override;
        
    private:
        Texture3D() : Texture("") {}
        
    protected:
        Texture3D(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t mipmaps, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad(Archive *archive) override;

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="setter")
        void setHeight(uint32_t height) { mDesc.height = height; }

        TPROPERTY(RTTRFuncName="Depth", RTTRFuncType="setter")
        void setDepth(uint32_t depth) { mDesc.depth = depth; }
        
        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="setter")
        void setPixelFormat(PixelFormat format) { mDesc.format = format; }

        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="setter")
        void setMipmaps(uint32_t mipmaps) { mDesc.mipmaps = mipmaps; }

        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="getter")
        const Buffer &getBuffer() const { return mDesc.buffer; }

        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="setter")
        void setBuffer(const Buffer &data) { mDesc.buffer = data; }
        
    protected:
        /// 像素缓冲区描述
        PixelBuffer3DDesc   mDesc {};
        /// 像素缓冲区对象
        PixelBuffer3DPtr    mPixelBuffer {nullptr};
    };

    TCLASS()
    class T3D_ENGINE_API Cubemap : public Texture2D
    {
        TRTTI_ENABLE(Texture2D)
        TRTTI_FRIEND

    public:
        static CubemapPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        TEXTURE_TYPE getTextureType() const override;

    protected:
        Cubemap() {}
        
        Cubemap(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad(Archive *archive) override;
    };

    TCLASS()
    class T3D_ENGINE_API CubemapArray : public Cubemap
    {
        TRTTI_ENABLE(Cubemap)
        TRTTI_FRIEND

    public:
        static CubemapArrayPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        TEXTURE_TYPE getTextureType() const override;

        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="getter")
        uint32_t getArraySize() const { return mDesc.arraySize / 6; }
        
    protected:
        CubemapArray() {}
        
        CubemapArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad(Archive *archive) override;
        
        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="setter")
        void setArraySize(uint32_t arraySize) { mDesc.arraySize = arraySize * 6; }
    };
}


#endif    /*__T3D_TEXTURE_H__*/
