/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4CONSOLE_RENDER_BUFFER_H__
#define __T3D_GL4CONSOLE_RENDER_BUFFER_H__


#include "T3DGL4ConsolePrerequisites.h"


namespace Tiny3D
{
    class GL4ConsoleVertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static GL4ConsoleVertexDeclarationPtr create();
        ~GL4ConsoleVertexDeclaration() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleVertexDeclaration() = default;
    };

    class GL4ConsoleVertexBuffer : public RHIVertexBuffer
    {
    public:
        static GL4ConsoleVertexBufferPtr create();
        ~GL4ConsoleVertexBuffer() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleVertexBuffer() = default;
    };

    class GL4ConsoleIndexBuffer : public RHIIndexBuffer
    {
    public:
        static GL4ConsoleIndexBufferPtr create();
        ~GL4ConsoleIndexBuffer() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleIndexBuffer() = default;
    };

    class GL4ConsoleConstantBuffer : public RHIConstantBuffer
    {
    public:
        static GL4ConsoleConstantBufferPtr create();
        ~GL4ConsoleConstantBuffer() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleConstantBuffer() = default;
    };

    class GL4ConsolePixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static GL4ConsolePixelBuffer1DPtr create();
        ~GL4ConsolePixelBuffer1D() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsolePixelBuffer1D() = default;
    };

    class GL4ConsolePixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static GL4ConsolePixelBuffer2DPtr create();
        ~GL4ConsolePixelBuffer2D() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsolePixelBuffer2D() = default;
    };

    class GL4ConsolePixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;
    };
}


#endif    /*__T3D_GL4CONSOLE_RENDER_BUFFER_H__*/
