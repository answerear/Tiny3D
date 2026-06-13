/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGLES3ContextBase.h"
#include "T3DGLES3Error.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>


namespace Tiny3D
{
    #define LOG_TAG_GLES3CONTEXTBASE        "GLES3ContextBase"

    //--------------------------------------------------------------------------

    GLES3ContextBase::GLES3ContextBase()
    {
    }

    //--------------------------------------------------------------------------

    GLES3ContextBase::~GLES3ContextBase()
    {
        if (mGlslangInitialized)
        {
            glslang::FinalizeProcess();
            mGlslangInitialized = false;
        }
        destroyEGLContext();
    }

    //--------------------------------------------------------------------------

    TResult GLES3ContextBase::initEGLContext()
    {
        TResult ret = T3D_OK;

        do
        {
            if (!eglBindAPI(EGL_OPENGL_ES_API))
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "eglBindAPI(EGL_OPENGL_ES_API) failed !");
                ret = T3D_ERR_GLES3_EGL_INIT;
                break;
            }

            mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (mEGLDisplay == EGL_NO_DISPLAY)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "eglGetDisplay failed !");
                ret = T3D_ERR_GLES3_EGL_INIT;
                break;
            }

            EGLint major, minor;
            if (!eglInitialize(mEGLDisplay, &major, &minor))
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "eglInitialize failed !");
                ret = T3D_ERR_GLES3_EGL_INIT;
                break;
            }

            T3D_LOG_INFO(LOG_TAG_GLES3CONTEXTBASE, "EGL version: %d.%d", major, minor);

            EGLint configAttribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, 24,
                EGL_STENCIL_SIZE, 8,
                EGL_NONE
            };

            EGLint numConfigs = 0;
            if (!eglChooseConfig(mEGLDisplay, configAttribs, &mEGLConfig, 1, &numConfigs) || numConfigs == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "eglChooseConfig failed (numConfigs=%d) !", numConfigs);
                ret = T3D_ERR_GLES3_EGL_CONFIG;
                break;
            }

            ret = createEGLContext();
            if (T3D_FAILED(ret))
            {
                break;
            }

            if (!mGlslangInitialized)
            {
                glslang::InitializeProcess();
                mGlslangInitialized = true;
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3ContextBase::createEGLContext()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mEGLContext != EGL_NO_CONTEXT)
            {
                break;
            }

            int versions[][2] = { {3, 2}, {3, 1}, {3, 0} };
            bool contextCreated = false;

            for (auto& ver : versions)
            {
                EGLint ctxAttribs[] = {
                    EGL_CONTEXT_MAJOR_VERSION, ver[0],
                    EGL_CONTEXT_MINOR_VERSION, ver[1],
                    EGL_NONE
                };

                mEGLContext = eglCreateContext(mEGLDisplay, mEGLConfig, EGL_NO_CONTEXT, ctxAttribs);
                if (mEGLContext != EGL_NO_CONTEXT)
                {
                    mGLESMajor = ver[0];
                    mGLESMinor = ver[1];
                    contextCreated = true;
                    T3D_LOG_INFO(LOG_TAG_GLES3CONTEXTBASE, "Created GLES %d.%d context", ver[0], ver[1]);
                    break;
                }
            }

            if (!contextCreated)
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "Failed to create any GLES 3.x context !");
                ret = T3D_ERR_GLES3_CREATE_CONTEXT;
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void GLES3ContextBase::destroyEGLContext()
    {
        if (mEGLDisplay != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if (mEGLContext != EGL_NO_CONTEXT)
            {
                eglDestroyContext(mEGLDisplay, mEGLContext);
                mEGLContext = EGL_NO_CONTEXT;
            }

            eglTerminate(mEGLDisplay);
            mEGLDisplay = EGL_NO_DISPLAY;
        }
    }

    //--------------------------------------------------------------------------

    TResult GLES3ContextBase::compileShader(ShaderVariant *shader)
    {
        size_t bytesLength = 0;
        const char *bytes = shader->getBytesCode(bytesLength);
        shader->setBytesCode(bytes, bytesLength);

        return glslangCompileAndReflect(shader);
    }

    //--------------------------------------------------------------------------

    TResult GLES3ContextBase::glslangCompileAndReflect(ShaderVariant *shader)
    {
        TResult ret = T3D_OK;

        do
        {
            EShLanguage glslangStage;
            switch (shader->getShaderStage())
            {
            case SHADER_STAGE::kVertex:   glslangStage = EShLangVertex; break;
            case SHADER_STAGE::kPixel:    glslangStage = EShLangFragment; break;
            case SHADER_STAGE::kGeometry: glslangStage = EShLangGeometry; break;
            default:
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "glslangCompileAndReflect: unsupported shader stage !");
                ret = T3D_ERR_GLES3_SHADER_REFLECTION;
                break;
            }

            if (T3D_FAILED(ret))
                break;

            size_t bytesLength = 0;
            const char *source = shader->getBytesCode(bytesLength);

            glslang::TShader glslangShader(glslangStage);
            int sourceLen = static_cast<int>(bytesLength);
            glslangShader.setStringsWithLengths(&source, &sourceLen, 1);

            int profileVersion = 300;
            if (mGLESMinor >= 2) profileVersion = 320;
            else if (mGLESMinor >= 1) profileVersion = 310;

            const TBuiltInResource *resources = GetDefaultResources();
            if (!glslangShader.parse(resources, profileVersion, EEsProfile, false, false, EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "glslang parse error:\n%s", glslangShader.getInfoLog());
                ret = T3D_ERR_GLES3_SHADER_REFLECTION;
                break;
            }

            glslang::TProgram program;
            program.addShader(&glslangShader);

            if (!program.link(EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "glslang link error:\n%s", program.getInfoLog());
                ret = T3D_ERR_GLES3_SHADER_REFLECTION;
                break;
            }

            if (!program.buildReflection(EShReflectionAllBlockVariables))
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "glslang buildReflection failed !");
                ret = T3D_ERR_GLES3_SHADER_REFLECTION;
                break;
            }

            GlslangReflectionData data;

            int numBlocks = program.getNumUniformBlocks();
            data.blocks.resize(numBlocks);
            for (int i = 0; i < numBlocks; ++i)
            {
                const auto &block = program.getUniformBlock(i);
                data.blocks[i].name = block.name.c_str();
                data.blocks[i].size = block.size;
            }

            int numUniforms = program.getNumUniformVariables();
            data.uniforms.resize(numUniforms);
            for (int i = 0; i < numUniforms; ++i)
            {
                const auto &uniform = program.getUniform(i);
                data.uniforms[i].name = uniform.name.c_str();
                data.uniforms[i].glDefineType = uniform.glDefineType;
                data.uniforms[i].offset = uniform.offset;
                data.uniforms[i].size = uniform.size;
                data.uniforms[i].blockIndex = uniform.index;
                data.uniforms[i].arrayStride = uniform.arrayStride;
            }

            mReflectionCache[shader] = std::move(data);

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3ContextBase::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mReflectionCache.find(shader);
            if (itr == mReflectionCache.end())
            {
                T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE, "reflectShaderAllBindings: no cached reflection data (compileShader not called?)");
                ret = T3D_ERR_GLES3_SHADER_REFLECTION;
                break;
            }

            const GlslangReflectionData &data = itr->second;

            for (int blockIdx = 0; blockIdx < static_cast<int>(data.blocks.size()); ++blockIdx)
            {
                for (const auto &uniform : data.uniforms)
                {
                    if (uniform.blockIndex != blockIdx)
                        continue;

                    uint32_t dataSize = 0;
                    ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_FLOAT;

                    switch (uniform.glDefineType)
                    {
                    case GL_FLOAT:       dataSize = sizeof(float) * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT; break;
                    case GL_FLOAT_VEC4:  dataSize = sizeof(float) * 4 * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_VECTOR4; break;
                    case GL_FLOAT_MAT4:  dataSize = sizeof(float) * 16 * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_MATRIX4; break;
                    case GL_INT:         dataSize = sizeof(int) * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY : ShaderConstantParam::DATA_TYPE::DT_INTEGER; break;
                    case GL_BOOL:        dataSize = sizeof(int) * uniform.size; dataType = (uniform.size > 1) ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY : ShaderConstantParam::DATA_TYPE::DT_BOOL; break;
                    default:             dataSize = uniform.arrayStride > 0 ? uniform.arrayStride * uniform.size : 4 * uniform.size; dataType = ShaderConstantParam::DATA_TYPE::DT_STRUCT; break;
                    }

                    String cbufferName = data.blocks[blockIdx].name;
                    String cname = uniform.name;

                    if (StringUtil::startsWith(cbufferName, "type_"))
                    {
                        cbufferName = cbufferName.substr(5);
                    }

                    String::size_type dotPos = cname.find('.');
                    if (dotPos != String::npos)
                    {
                        cname = cname.substr(dotPos + 1);
                    }

                    String::size_type bracketPos = cname.find('[');
                    if (bracketPos != String::npos)
                    {
                        cname = cname.substr(0, bracketPos);
                    }

                    ShaderConstantParamPtr param = ShaderConstantParam::create(cbufferName, cname, blockIdx, dataSize, uniform.offset, dataType);
                    constantParams.emplace(param->getName(), param);
                }
            }

            uint32_t samplerIndex = 0;
            for (const auto &uniform : data.uniforms)
            {
                if (uniform.blockIndex >= 0)
                    continue;

                bool isSampler = false;
                TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;

                switch (uniform.glDefineType)
                {
                case GL_SAMPLER_2D:         isSampler = true; texType = TEXTURE_TYPE::TT_2D; break;
                case GL_SAMPLER_3D:         isSampler = true; texType = TEXTURE_TYPE::TT_3D; break;
                case GL_SAMPLER_CUBE:       isSampler = true; texType = TEXTURE_TYPE::TT_CUBE; break;
                case GL_SAMPLER_2D_SHADOW:  isSampler = true; texType = TEXTURE_TYPE::TT_2D; break;
                default: break;
                }

                if (isSampler)
                {
                    String name = uniform.name;

                    const String kSpirvPrefix = "SPIRV_Cross_Combined";
                    if (StringUtil::startsWith(name, kSpirvPrefix, false))
                    {
                        String remainder = name.substr(kSpirvPrefix.size());
                        String::size_type samplerPos = remainder.find("sampler");
                        if (samplerPos != String::npos && samplerPos > 0)
                        {
                            name = remainder.substr(0, samplerPos);
                        }
                    }

                    ShaderSamplerParamPtr param;
                    const auto it = samplerParams.find(name);
                    if (it == samplerParams.end())
                    {
                        param = ShaderSamplerParam::create(name);
                        samplerParams.emplace(name, param);
                    }
                    else
                    {
                        param = it->second;
                    }

                    param->setTexBinding(samplerIndex);
                    param->setSamplerBinding(samplerIndex);
                    param->setTextureType(texType);

                    samplerIndex++;
                }
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3ContextBase::reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
