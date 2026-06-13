/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GLES3_CONTEXT_BASE_H__
#define __T3D_GLES3_CONTEXT_BASE_H__


#include "T3DGLES3PrerequisitesBase.h"


namespace Tiny3D
{
    class GLES3ContextBase : public RHIContext
    {
    public:
        virtual ~GLES3ContextBase();

        TResult compileShader(ShaderVariant *shader) override;
        TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;
        TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams) override;

        EGLDisplay getEGLDisplay() const { return mEGLDisplay; }
        EGLContext getEGLContext() const { return mEGLContext; }
        EGLConfig  getEGLConfig()  const { return mEGLConfig; }

        EGLSurface getMainSurface() const { return mMainSurface; }
        void setMainSurface(EGLSurface surface) { mMainSurface = surface; }

        TResult createEGLContext();

    protected:
        GLES3ContextBase();

        TResult initEGLContext();
        void destroyEGLContext();

        struct GlslangUniformInfo
        {
            String name;
            int glDefineType;
            int offset;
            int size;
            int blockIndex;
            int arrayStride;
        };

        struct GlslangBlockInfo
        {
            String name;
            int size;
        };

        struct GlslangReflectionData
        {
            TArray<GlslangBlockInfo> blocks;
            TArray<GlslangUniformInfo> uniforms;
        };

        TResult glslangCompileAndReflect(ShaderVariant *shader);

        TMap<ShaderVariant*, GlslangReflectionData> mReflectionCache;
        bool mGlslangInitialized {false};

        EGLDisplay  mEGLDisplay {EGL_NO_DISPLAY};
        EGLContext  mEGLContext {EGL_NO_CONTEXT};
        EGLConfig   mEGLConfig {nullptr};
        EGLSurface  mMainSurface {EGL_NO_SURFACE};

        int mGLESMajor {3};
        int mGLESMinor {0};
    };
}


#endif  /*__T3D_GLES3_CONTEXT_BASE_H__*/
