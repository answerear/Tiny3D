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

        SamplerStatePtr getSamplerState() const { return mSamplerState; }

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

        PixelBuffer1DPtr getPixelBuffer() const { return mPixelBuffer; }

    private:
        Texture1D() : Texture("") {}
        
    protected:
        Texture1D(const String &name, uint32_t width, PixelFormat format, uint32_t mipmaps, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad() override;

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

        PixelBuffer2DPtr getPixelBuffer() const { return mPixelBuffer; }
        
    private:
        Texture2D() : Texture("") {}
        
    protected:
        Texture2D(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad() override;

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
    };

    TCLASS()
    class T3D_ENGINE_API Texture2DArray : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        static Texture2DArrayPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

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

        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="getter")
        uint32_t getArraySize() const { return mDesc.arraySize; }

        TPROPERTY(RTTRFuncName="MSAA", RTTRFuncType="getter")
        const MultisamplerDesc &getMSAADesc() const { return mDesc.sampleDesc; }

        PixelBuffer2DPtr getPixelBuffer() const { return mPixelBuffer; }
        
    private:
        Texture2DArray() : Texture("") {}
        
    protected:
        Texture2DArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad() override;

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="setter")
        void setHeight(uint32_t height) { mDesc.height = height; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="setter")
        void setPixelFormat(PixelFormat format) { mDesc.format = format; }

        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="setter")
        void setMipmaps(uint32_t mipmaps) { mDesc.mipmaps = mipmaps; }

        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="setter")
        void setArraySize(uint32_t arraySize) { mDesc.arraySize = arraySize; }

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

        PixelBuffer3DPtr getPixelBuffer() const { return mPixelBuffer; }
        
    private:
        Texture3D() : Texture("") {}
        
    protected:
        Texture3D(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t mipmaps, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad() override;

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="setter")
        void setHeight(uint32_t height) { mDesc.height = height; }

        TPROPERTY(RTTRFuncName="Depth", RTTRFuncType="getter")
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
    class T3D_ENGINE_API Cubemap : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        static CubemapPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

        TEXTURE_TYPE getTextureType() const override;

        const uint8_t * const getData() const { return mDesc.buffer.Data; }

        size_t getDataSize() const { return mDesc.buffer.DataSize; }

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="getter")
        uint32_t getWidth() const { return mDesc.width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="getter")
        uint32_t getHeight() const { return mDesc.height; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="getter")
        PixelFormat getPixelFormat() const { return mDesc.format; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="getter")
        uint32_t getMipmaps() const { return mDesc.mipmaps; }

        TPROPERTY(RTTRFuncName="MSAA", RTTRFuncType="getter")
        const MultisamplerDesc &getMSAADesc() const { return mDesc.sampleDesc; }

        PixelBuffer2DPtr getPixelBuffer() const { return mPixelBuffer; }
        
    private:
        Cubemap() : Texture("") {}
        
    protected:
        Cubemap(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad() override;

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
    };

    TCLASS()
    class T3D_ENGINE_API CubemapArray : public Texture
    {
        TRTTI_ENABLE(Texture)
        TRTTI_FRIEND

    public:
        static CubemapArrayPtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);

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

        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="getter")
        uint32_t getArraySize() const { return mDesc.arraySize / 6; }

        TPROPERTY(RTTRFuncName="MSAA", RTTRFuncType="getter")
        const MultisamplerDesc &getMSAADesc() const { return mDesc.sampleDesc; }

        PixelBuffer2DPtr getPixelBuffer() const { return mPixelBuffer; }
        
    private:
        CubemapArray() : Texture("") {}
        
    protected:
        CubemapArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data);
        
        ResourcePtr clone() const override;

        TResult onCreate() override;

        TResult onLoad() override;

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(uint32_t width) { mDesc.width = width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="setter")
        void setHeight(uint32_t height) { mDesc.height = height; }

        TPROPERTY(RTTRFuncName="Format", RTTRFuncType="setter")
        void setPixelFormat(PixelFormat format) { mDesc.format = format; }

        TPROPERTY(RTTRFuncName="Mipmaps", RTTRFuncType="setter")
        void setMipmaps(uint32_t mipmaps) { mDesc.mipmaps = mipmaps; }

        TPROPERTY(RTTRFuncName="ArraySize", RTTRFuncType="setter")
        void setArraySize(uint32_t arraySize) { mDesc.arraySize = arraySize * 6; }

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
    };
}


#endif    /*__T3D_TEXTURE_H__*/
