/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4_RENDER_BUFFER_H__
#define __T3D_GL4_RENDER_BUFFER_H__


#include "T3DGL4Prerequisites.h"


namespace Tiny3D
{
    class GL4VertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static GL4VertexDeclarationPtr create();
        ~GL4VertexDeclaration() override;
        void *getNativeObject() const override;

        GLuint GLVAO {0};

        /// 声明中的一个属性
        struct Attribute
        {
            /// HLSL 语义名（不含序号），与 D3D11 输入布局一致
            String      semanticName;
            uint32_t    semanticIndex {0};
            uint32_t    slot {0};
            uint32_t    offset {0};
            VertexAttribute::Type type {VertexAttribute::Type::E_VAT_FLOAT4};
            bool        perInstance {false};
            uint32_t    instanceStepRate {0};
        };
        /// 创建时从 VertexDeclaration 拷贝，draw 时据此配置 VAO
        TArray<Attribute> Attributes;

        /// 以下成员只在 RHI 线程访问。
        /// 各属性当前映射到的 attribute location，VS 不读取的属性为 -1
        TArray<GLint> AttribLocations;
        /// 当前在 VAO 上启用的 location，换布局时先全部关闭
        TArray<GLuint> EnabledLocations;
        /// VAO 当前按哪个 VS 输入签名、哪个版本的顶点缓冲表配置，两者都没变时 draw 不用重设
        uint32_t AppliedSignature {0};
        uint32_t AppliedBufferRevision {0};
        /// 已与本声明对照过 VS 输入的 program，每对只校验一次
        TSet<GLuint> ValidatedPrograms;
    protected:
        GL4VertexDeclaration() = default;
    };

    class GL4VertexBuffer : public RHIVertexBuffer
    {
    public:
        static GL4VertexBufferPtr create();
        ~GL4VertexBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GL4VertexBuffer() = default;
    };

    class GL4IndexBuffer : public RHIIndexBuffer
    {
    public:
        static GL4IndexBufferPtr create();
        ~GL4IndexBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GL4IndexBuffer() = default;
    };

    class GL4ConstantBuffer : public RHIConstantBuffer
    {
    public:
        static GL4ConstantBufferPtr create();
        ~GL4ConstantBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GL4ConstantBuffer() = default;
    };

    class GL4PixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static GL4PixelBuffer1DPtr create();
        ~GL4PixelBuffer1D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
        GLenum GLInternalFormat {GL_RGBA8};
        GLenum GLTarget {GL_TEXTURE_1D};
    protected:
        GL4PixelBuffer1D() = default;
    };

    class GL4PixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static GL4PixelBuffer2DPtr create();
        ~GL4PixelBuffer2D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
        GLenum GLInternalFormat {GL_RGBA8};
        GLenum GLTarget {GL_TEXTURE_2D};
        bool GLHasStencil {false};
        uint32_t GLAttachedColorCount {1};
        GLuint GLFBO {0};
        /// MSAA resolve 纹理（非多采样，用于 shader 读取）
        GLuint GLResolveTex {0};
        /// MSAA resolve FBO
        GLuint GLResolveFBO {0};
        /// MSAA 采样数（1 = 无 MSAA）
        uint32_t GLMSAACount {1};
    protected:
        GL4PixelBuffer2D() = default;
    };

    class GL4PixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        static GL4PixelBuffer3DPtr create();
        ~GL4PixelBuffer3D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
        GLenum GLInternalFormat {GL_RGBA8};
    protected:
        GL4PixelBuffer3D() = default;
    };

    class GL4PixelBufferCubemap : public RHIPixelBufferCubemap
    {
    public:
        static GL4PixelBufferCubemapPtr create();
        ~GL4PixelBufferCubemap() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
        GLenum GLInternalFormat {GL_RGBA8};
        GLenum GLTarget {GL_TEXTURE_CUBE_MAP};
    protected:
        GL4PixelBufferCubemap() = default;
    };

    class GL4StructuredBuffer : public RHIStructuredBuffer
    {
    public:
        static GL4StructuredBufferPtr create();
        ~GL4StructuredBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
        /// Append/Consume 或 hasCounter 时的 4 字节计数器
        GLuint GLCounterBuffer {0};
        /// kTyped 时关联的 GL_TEXTURE_BUFFER 纹理对象
        GLuint GLTextureBuffer {0};
        GLenum GLInternalFormat {GL_RGBA8};
        StructuredBufferKind Kind {StructuredBufferKind::kStructured};
        uint32_t ElementCount {0};
        uint32_t ElementSize {0};
        bool HasCounter {false};
    protected:
        GL4StructuredBuffer() = default;
    };
}


#endif  /*__T3D_GL4_RENDER_BUFFER_H__*/
