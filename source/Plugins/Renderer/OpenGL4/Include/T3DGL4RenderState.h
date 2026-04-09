/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4_RENDER_STATE_H__
#define __T3D_GL4_RENDER_STATE_H__


#include "T3DGL4Prerequisites.h"


namespace Tiny3D
{
    struct GL4BlendStateData
    {
        bool        enabled {false};
        GLenum      srcRGB {GL_ONE};
        GLenum      dstRGB {GL_ZERO};
        GLenum      srcAlpha {GL_ONE};
        GLenum      dstAlpha {GL_ZERO};
        GLenum      opRGB {GL_FUNC_ADD};
        GLenum      opAlpha {GL_FUNC_ADD};
        GLboolean   colorMask[4] {GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
    };

    class GL4BlendState : public RHIBlendState
    {
    public:
        static GL4BlendStatePtr create();
        ~GL4BlendState() override = default;
        void *getNativeObject() const override;

        GL4BlendStateData data {};
    protected:
        GL4BlendState() = default;
    };

    struct GL4DepthStencilStateData
    {
        bool        depthTestEnabled {true};
        bool        depthWriteEnabled {true};
        GLenum      depthFunc {GL_LESS};
        bool        stencilEnabled {false};
        uint8_t     stencilReadMask {0xFF};
        uint8_t     stencilWriteMask {0xFF};
        GLenum      frontStencilFail {GL_KEEP};
        GLenum      frontDepthFail {GL_KEEP};
        GLenum      frontStencilPass {GL_KEEP};
        GLenum      frontStencilFunc {GL_ALWAYS};
        GLenum      backStencilFail {GL_KEEP};
        GLenum      backDepthFail {GL_KEEP};
        GLenum      backStencilPass {GL_KEEP};
        GLenum      backStencilFunc {GL_ALWAYS};
    };

    class GL4DepthStencilState : public RHIDepthStencilState
    {
    public:
        static GL4DepthStencilStatePtr create();
        ~GL4DepthStencilState() override = default;
        void *getNativeObject() const override;

        GL4DepthStencilStateData data {};
    protected:
        GL4DepthStencilState() = default;
    };

    struct GL4RasterizerStateData
    {
        GLenum      fillMode {GL_FILL};
        GLenum      cullMode {GL_BACK};
        bool        cullEnabled {true};
        bool        scissorEnabled {false};
        bool        depthClipEnabled {true};
        bool        frontCCW {true};
        GLfloat     depthBias {0.0f};
        GLfloat     slopeScaledDepthBias {0.0f};
    };

    class GL4RasterizerState : public RHIRasterizerState
    {
    public:
        static GL4RasterizerStatePtr create();
        ~GL4RasterizerState() override = default;
        void *getNativeObject() const override;

        GL4RasterizerStateData data {};
    protected:
        GL4RasterizerState() = default;
    };

    class GL4SamplerState : public RHISamplerState
    {
    public:
        static GL4SamplerStatePtr create();
        ~GL4SamplerState() override;
        void *getNativeObject() const override;

        GLuint GLSampler {0};
    protected:
        GL4SamplerState() = default;
    };
}


#endif  /*__T3D_GL4_RENDER_STATE_H__*/
