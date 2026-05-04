/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4_CONTEXT_BASE_H__
#define __T3D_GL4_CONTEXT_BASE_H__


#include "T3DGL4PrerequisitesBase.h"


namespace Tiny3D
{
    class GL4ContextBase : public RHIContext
    {
    public:
        virtual ~GL4ContextBase();

        TResult init();

        // Shader compilation and reflection (shared by Console and Runtime)
        TResult compileShader(ShaderVariant *shader) override;
        TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;
        TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams) override;

    protected:
        GL4ContextBase();

        TResult initDummyContext();
        void destroyDummyContext();

        //-------------------------------------------------------------------
        // glslang CPU-side reflection cache
        //-------------------------------------------------------------------
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

#if defined(T3D_OS_WINDOWS)
        HWND  mDummyHWND {nullptr};
        HDC   mDummyHDC {nullptr};
        HGLRC mDummyHGLRC {nullptr};
#elif defined(T3D_OS_LINUX)
        Display      *mDummyDisplay {nullptr};
        ::Window      mDummyWindow {0};
        GLXContext    mDummyGLXContext {nullptr};
#endif
        bool mGLADLoaded {false};
    };
}


#endif  /*__T3D_GL4_CONTEXT_BASE_H__*/
