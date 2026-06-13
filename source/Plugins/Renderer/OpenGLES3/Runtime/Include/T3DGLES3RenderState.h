/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GLES3_RENDER_STATE_H__
#define __T3D_GLES3_RENDER_STATE_H__


#include "T3DGLES3Prerequisites.h"


namespace Tiny3D
{
    struct GLES3BlendStateData
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

    class GLES3BlendState : public RHIBlendState
    {
    public:
        static GLES3BlendStatePtr create();
        ~GLES3BlendState() override = default;
        void *getNativeObject() const override;

        GLES3BlendStateData data {};
    protected:
        GLES3BlendState() = default;
    };

    struct GLES3DepthStencilStateData
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

    class GLES3DepthStencilState : public RHIDepthStencilState
    {
    public:
        static GLES3DepthStencilStatePtr create();
        ~GLES3DepthStencilState() override = default;
        void *getNativeObject() const override;

        GLES3DepthStencilStateData data {};
    protected:
        GLES3DepthStencilState() = default;
    };

    struct GLES3RasterizerStateData
    {
        GLenum      cullMode {GL_BACK};
        bool        cullEnabled {true};
        bool        scissorEnabled {false};
        bool        frontCCW {true};
        bool        multisampleEnabled {false};
        GLfloat     depthBias {0.0f};
        GLfloat     slopeScaledDepthBias {0.0f};
    };

    class GLES3RasterizerState : public RHIRasterizerState
    {
    public:
        static GLES3RasterizerStatePtr create();
        ~GLES3RasterizerState() override = default;
        void *getNativeObject() const override;

        GLES3RasterizerStateData data {};
    protected:
        GLES3RasterizerState() = default;
    };

    class GLES3SamplerState : public RHISamplerState
    {
    public:
        static GLES3SamplerStatePtr create();
        ~GLES3SamplerState() override;
        void *getNativeObject() const override;

        GLuint GLSampler {0};
    protected:
        GLES3SamplerState() = default;
    };
}


#endif  /*__T3D_GLES3_RENDER_STATE_H__*/
