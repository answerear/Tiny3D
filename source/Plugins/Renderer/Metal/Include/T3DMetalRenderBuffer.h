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

#ifndef __T3D_METAL_RENDER_BUFFER_H__
#define __T3D_METAL_RENDER_BUFFER_H__


#include "T3DMetalPrerequisites.h"
#include "Render/T3DVertexAttribute.h"


namespace Tiny3D
{
    class MetalVertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static MetalVertexDeclarationPtr create();

        ~MetalVertexDeclaration() override;

        void *getNativeObject() const override;

        void setAttributes(const VertexAttributes &attributes) { mAttributes = attributes; }

        const VertexAttributes &getAttributes() const { return mAttributes; }

    protected:
        MetalVertexDeclaration() = default;

        VertexAttributes mAttributes;
    };

    class MetalVertexBuffer : public RHIVertexBuffer
    {
    public:
        static MetalVertexBufferPtr create();

        ~MetalVertexBuffer() override;

        void *getNativeObject() const override;

        void setNativeObject(void *buffer);

    protected:
        MetalVertexBuffer() = default;

        void *mNative {nullptr};
    };

    class MetalIndexBuffer : public RHIIndexBuffer
    {
    public:
        static MetalIndexBufferPtr create();

        ~MetalIndexBuffer() override;

        void *getNativeObject() const override;

        void setNativeObject(void *buffer);

        void setIndexType(IndexType type) { mIndexType = type; }

        IndexType getIndexType() const { return mIndexType; }

    protected:
        MetalIndexBuffer() = default;

        void        *mNative {nullptr};
        IndexType   mIndexType {IndexType::E_IT_16BITS};
    };

    class MetalConstantBuffer : public RHIConstantBuffer
    {
    public:
        static MetalConstantBufferPtr create();

        ~MetalConstantBuffer() override;

        void *getNativeObject() const override;

        void setNativeObject(void *buffer);

    protected:
        MetalConstantBuffer() = default;

        void *mNative {nullptr};
    };

    class MetalPixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static MetalPixelBuffer1DPtr create();

        ~MetalPixelBuffer1D() override;

        void *getNativeObject() const override;

        void setNativeObject(void *texture);

    protected:
        MetalPixelBuffer1D() = default;

        void *mNative {nullptr};
    };

    class MetalPixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static MetalPixelBuffer2DPtr create();

        ~MetalPixelBuffer2D() override;

        void *getNativeObject() const override;

        void setNativeObject(void *texture);

        void *getResolveNative() const { return mResolveNative; }

        void setResolveNative(void *texture);

    protected:
        MetalPixelBuffer2D() = default;

        void *mNative {nullptr};
        void *mResolveNative {nullptr};
    };

    class MetalPixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        static MetalPixelBuffer3DPtr create();

        ~MetalPixelBuffer3D() override;

        void *getNativeObject() const override;

        void setNativeObject(void *texture);

    protected:
        MetalPixelBuffer3D() = default;

        void *mNative {nullptr};
    };

    class MetalPixelBufferCubemap : public RHIPixelBufferCubemap
    {
    public:
        static MetalPixelBufferCubemapPtr create();

        ~MetalPixelBufferCubemap() override;

        void *getNativeObject() const override;

        void setNativeObject(void *texture);

    protected:
        MetalPixelBufferCubemap() = default;

        void *mNative {nullptr};
    };
}


#endif    /*__T3D_METAL_RENDER_BUFFER_H__*/
