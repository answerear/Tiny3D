/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DNullGL4Context.h"
#include "T3DNullGL4Error.h"
#include "T3DNullGL4Window.h"
#include "T3DNullGL4RenderBuffer.h"
#include "T3DNullGL4RenderState.h"
#include "T3DNullGL4Shader.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullGL4ContextPtr NullGL4Context::create()
    {
        return T3D_NEW NullGL4Context();
    }

    //--------------------------------------------------------------------------

    NullGL4Context::NullGL4Context()
    {
    }

    //--------------------------------------------------------------------------

    NullGL4Context::~NullGL4Context()
    {
        if (mGlslangInitialized)
        {
            glslang::FinalizeProcess();
            mGlslangInitialized = false;
        }
        destroyDummyContext();
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Context::init()
    {
        TResult ret = initDummyContext();
        if (T3D_OK == ret && !mGlslangInitialized)
        {
            glslang::InitializeProcess();
            mGlslangInitialized = true;
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Context::initDummyContext()
    {
        TResult ret = T3D_OK;

        do
        {
#if defined(T3D_OS_WINDOWS)
            const wchar_t *className = L"Tiny3DNullGL4DummyWindow";
            WNDCLASSW wc = {};
            wc.lpfnWndProc = DefWindowProcW;
            wc.hInstance = GetModuleHandleW(nullptr);
            wc.lpszClassName = className;
            RegisterClassW(&wc);

            mDummyHWND = CreateWindowExW(
                0, className, L"", WS_POPUP,
                0, 0, 1, 1,
                nullptr, nullptr, wc.hInstance, nullptr);
            if (mDummyHWND == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "Failed to create dummy window !");
                ret = T3D_ERR_NULLGL4_CREATE_CONTEXT;
                break;
            }

            mDummyHDC = GetDC(mDummyHWND);
            if (mDummyHDC == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "Failed to get dummy HDC !");
                ret = T3D_ERR_NULLGL4_CREATE_CONTEXT;
                break;
            }

            PIXELFORMATDESCRIPTOR pfd = {};
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 32;
            pfd.cDepthBits = 24;
            pfd.cStencilBits = 8;
            pfd.iLayerType = PFD_MAIN_PLANE;

            int pixelFormat = ChoosePixelFormat(mDummyHDC, &pfd);
            if (pixelFormat == 0)
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "ChoosePixelFormat failed !");
                ret = T3D_ERR_NULLGL4_SET_PIXEL_FORMAT;
                break;
            }

            if (!SetPixelFormat(mDummyHDC, pixelFormat, &pfd))
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "SetPixelFormat failed !");
                ret = T3D_ERR_NULLGL4_SET_PIXEL_FORMAT;
                break;
            }

            mDummyHGLRC = wglCreateContext(mDummyHDC);
            if (mDummyHGLRC == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "wglCreateContext failed !");
                ret = T3D_ERR_NULLGL4_CREATE_CONTEXT;
                break;
            }

            if (!wglMakeCurrent(mDummyHDC, mDummyHGLRC))
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "wglMakeCurrent failed !");
                ret = T3D_ERR_NULLGL4_CREATE_CONTEXT;
                break;
            }

            {
                static HMODULE hOpenGL32 = LoadLibrary(TEXT("opengl32.dll"));
                auto combinedLoader = [](const char *name) -> void*
                {
                    void *proc = (void *)wglGetProcAddress(name);
                    if (proc == nullptr
                        || proc == (void *)0x1 || proc == (void *)0x2
                        || proc == (void *)0x3 || proc == (void *)-1)
                    {
                        static HMODULE hGL = LoadLibrary(TEXT("opengl32.dll"));
                        proc = (void *)GetProcAddress(hGL, name);
                    }
                    return proc;
                };

                if (!gladLoadGLLoader((GLADloadproc)+combinedLoader))
                {
                    T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "gladLoadGLLoader failed !");
                    ret = T3D_ERR_NULLGL4_GLAD_LOAD;
                    break;
                }
            }

#elif defined(T3D_OS_LINUX)
            mDummyDisplay = XOpenDisplay(nullptr);
            if (mDummyDisplay == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "XOpenDisplay failed !");
                ret = T3D_ERR_NULLGL4_CREATE_CONTEXT;
                break;
            }

            int screen = DefaultScreen(mDummyDisplay);
            int attribs[] = {
                GLX_RGBA,
                GLX_RED_SIZE, 8,
                GLX_GREEN_SIZE, 8,
                GLX_BLUE_SIZE, 8,
                GLX_ALPHA_SIZE, 8,
                GLX_DEPTH_SIZE, 24,
                GLX_STENCIL_SIZE, 8,
                GLX_DOUBLEBUFFER,
                None
            };

            XVisualInfo *vi = glXChooseVisual(mDummyDisplay, screen, attribs);
            if (vi == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "glXChooseVisual failed !");
                ret = T3D_ERR_NULLGL4_CREATE_CONTEXT;
                break;
            }

            ::Window rootWin = RootWindow(mDummyDisplay, screen);
            XSetWindowAttributes swa = {};
            swa.colormap = XCreateColormap(mDummyDisplay, rootWin, vi->visual, AllocNone);
            mDummyWindow = XCreateWindow(mDummyDisplay, rootWin,
                0, 0, 1, 1, 0, vi->depth, InputOutput, vi->visual,
                CWColormap, &swa);

            mDummyGLXContext = glXCreateContext(mDummyDisplay, vi, nullptr, GL_TRUE);
            XFree(vi);

            if (mDummyGLXContext == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "glXCreateContext failed !");
                ret = T3D_ERR_NULLGL4_CREATE_CONTEXT;
                break;
            }

            if (!glXMakeCurrent(mDummyDisplay, mDummyWindow, mDummyGLXContext))
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "glXMakeCurrent failed !");
                ret = T3D_ERR_NULLGL4_CREATE_CONTEXT;
                break;
            }

            if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddress))
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "gladLoadGLLoader failed !");
                ret = T3D_ERR_NULLGL4_GLAD_LOAD;
                break;
            }
#endif

            mGLADLoaded = true;

            T3D_LOG_INFO(LOG_TAG_NULLGL4RENDERER, "OpenGL Vendor: %s", glGetString(GL_VENDOR));
            T3D_LOG_INFO(LOG_TAG_NULLGL4RENDERER, "OpenGL Renderer: %s", glGetString(GL_RENDERER));
            T3D_LOG_INFO(LOG_TAG_NULLGL4RENDERER, "OpenGL Version: %s", glGetString(GL_VERSION));
            T3D_LOG_INFO(LOG_TAG_NULLGL4RENDERER, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            T3D_LOG_INFO(LOG_TAG_NULLGL4RENDERER, "NullGL4 dummy context initialized.");

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void NullGL4Context::destroyDummyContext()
    {
#if defined(T3D_OS_WINDOWS)
        if (mDummyHGLRC != nullptr)
        {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(mDummyHGLRC);
            mDummyHGLRC = nullptr;
        }

        if (mDummyHDC != nullptr && mDummyHWND != nullptr)
        {
            ReleaseDC(mDummyHWND, mDummyHDC);
            mDummyHDC = nullptr;
        }

        if (mDummyHWND != nullptr)
        {
            DestroyWindow(mDummyHWND);
            mDummyHWND = nullptr;
            UnregisterClassW(L"Tiny3DNullGL4DummyWindow", GetModuleHandleW(nullptr));
        }
#elif defined(T3D_OS_LINUX)
        if (mDummyGLXContext != nullptr)
        {
            glXMakeCurrent(mDummyDisplay, None, nullptr);
            glXDestroyContext(mDummyDisplay, mDummyGLXContext);
            mDummyGLXContext = nullptr;
        }

        if (mDummyWindow != 0)
        {
            XDestroyWindow(mDummyDisplay, mDummyWindow);
            mDummyWindow = 0;
        }

        if (mDummyDisplay != nullptr)
        {
            XCloseDisplay(mDummyDisplay);
            mDummyDisplay = nullptr;
        }
#endif
    }

    //--------------------------------------------------------------------------
    // Core: compileShader (glslang CPU-side compilation + reflection)
    //--------------------------------------------------------------------------

    TResult NullGL4Context::compileShader(ShaderVariant *shader)
    {
        size_t bytesLength = 0;
        const char *bytes = shader->getBytesCode(bytesLength);
        shader->setBytesCode(bytes, bytesLength);

        return glslangCompileAndReflect(shader);
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Context::glslangCompileAndReflect(ShaderVariant *shader)
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
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "glslangCompileAndReflect: unsupported shader stage !");
                ret = T3D_ERR_NULLGL4_SHADER_REFLECTION;
                break;
            }

            if (T3D_FAILED(ret))
                break;

            size_t bytesLength = 0;
            const char *source = shader->getBytesCode(bytesLength);

            glslang::TShader glslangShader(glslangStage);
            int sourceLen = static_cast<int>(bytesLength);
            glslangShader.setStringsWithLengths(&source, &sourceLen, 1);

            const TBuiltInResource *resources = GetDefaultResources();
            if (!glslangShader.parse(resources, 400, false, EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "glslang parse error:\n%s", glslangShader.getInfoLog());
                ret = T3D_ERR_NULLGL4_SHADER_REFLECTION;
                break;
            }

            glslang::TProgram program;
            program.addShader(&glslangShader);

            if (!program.link(EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "glslang link error:\n%s", program.getInfoLog());
                ret = T3D_ERR_NULLGL4_SHADER_REFLECTION;
                break;
            }

            if (!program.buildReflection(EShReflectionAllBlockVariables))
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "glslang buildReflection failed !");
                ret = T3D_ERR_NULLGL4_SHADER_REFLECTION;
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
    // Core: createXXXShader (real GL compilation)
    //--------------------------------------------------------------------------

    RHIShaderPtr NullGL4Context::createVertexShader(ShaderVariant *shader)
    {
        NullGL4VertexShaderPtr glShader = NullGL4VertexShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        glShader->GLShaderHandle = glCreateShader(GL_VERTEX_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
        glCompileShader(glShader->GLShaderHandle);

        GLint compiled = 0;
        glGetShaderiv(glShader->GLShaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint logLen = 0;
            glGetShaderiv(glShader->GLShaderHandle, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0)
            {
                TArray<char> log(logLen + 1, 0);
                glGetShaderInfoLog(glShader->GLShaderHandle, logLen, nullptr, log.data());
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "Vertex shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr NullGL4Context::createPixelShader(ShaderVariant *shader)
    {
        NullGL4PixelShaderPtr glShader = NullGL4PixelShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        glShader->GLShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
        glCompileShader(glShader->GLShaderHandle);

        GLint compiled = 0;
        glGetShaderiv(glShader->GLShaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint logLen = 0;
            glGetShaderiv(glShader->GLShaderHandle, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0)
            {
                TArray<char> log(logLen + 1, 0);
                glGetShaderInfoLog(glShader->GLShaderHandle, logLen, nullptr, log.data());
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "Pixel shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr NullGL4Context::createGeometryShader(ShaderVariant *shader)
    {
        NullGL4GeometryShaderPtr glShader = NullGL4GeometryShader::create();

        size_t bytecodeLength = 0;
        const char *bytecode = shader->getBytesCode(bytecodeLength);

        glShader->GLShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
        GLint len = static_cast<GLint>(bytecodeLength);
        glShaderSource(glShader->GLShaderHandle, 1, &bytecode, &len);
        glCompileShader(glShader->GLShaderHandle);

        GLint compiled = 0;
        glGetShaderiv(glShader->GLShaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint logLen = 0;
            glGetShaderiv(glShader->GLShaderHandle, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 0)
            {
                TArray<char> log(logLen + 1, 0);
                glGetShaderInfoLog(glShader->GLShaderHandle, logLen, nullptr, log.data());
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "Geometry shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr NullGL4Context::createHullShader(ShaderVariant *shader)
    {
        return NullGL4HullShader::create();
    }

    RHIShaderPtr NullGL4Context::createDomainShader(ShaderVariant *shader)
    {
        return NullGL4DomainShader::create();
    }

    RHIShaderPtr NullGL4Context::createComputeShader(ShaderVariant *shader)
    {
        return NullGL4ComputeShader::create();
    }

    //--------------------------------------------------------------------------
    // Core: reflectShaderAllBindings (read from glslang cache)
    //--------------------------------------------------------------------------

    TResult NullGL4Context::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mReflectionCache.find(shader);
            if (itr == mReflectionCache.end())
            {
                T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "reflectShaderAllBindings: no cached reflection data (compileShader not called?)");
                ret = T3D_ERR_NULLGL4_SHADER_REFLECTION;
                break;
            }

            const GlslangReflectionData &data = itr->second;

            // Reflect Uniform Blocks (cbuffers)
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

                    T3D_LOG_DEBUG(LOG_TAG_NULLGL4RENDERER, "Reflect - UBO: %s -> %s, uniform: %s -> %s, type: %u, size: %u, offset: %d",
                        data.blocks[blockIdx].name.c_str(), cbufferName.c_str(), uniform.name.c_str(), cname.c_str(), dataType, dataSize, uniform.offset);
                }
            }

            // Reflect standalone Uniforms (texture samplers)
            uint32_t samplerIndex = 0;
            for (const auto &uniform : data.uniforms)
            {
                if (uniform.blockIndex >= 0)
                    continue;

                bool isSampler = false;
                TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;

                switch (uniform.glDefineType)
                {
                case GL_SAMPLER_1D:         isSampler = true; texType = TEXTURE_TYPE::TT_1D; break;
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

                    T3D_LOG_DEBUG(LOG_TAG_NULLGL4RENDERER, "Reflect - sampler: %s -> %s, binding: %d, type: %d",
                        uniform.name.c_str(), name.c_str(), samplerIndex, texType);
                    samplerIndex++;
                }
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Context::reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    // Empty implementations
    //--------------------------------------------------------------------------

    TResult NullGL4Context::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) { return T3D_OK; }
    RHIRenderTargetPtr NullGL4Context::createRenderWindow(RenderWindow *renderWindow) { return NullGL4Window::create(); }
    RHIPixelBuffer2DPtr NullGL4Context::createRenderTexture(PixelBuffer2D *buffer) { return nullptr; }
    TResult NullGL4Context::setRenderTarget(RenderTarget *renderTarget) { return T3D_OK; }
    TResult NullGL4Context::resetRenderTarget() { return T3D_OK; }
    TResult NullGL4Context::setViewport(const Viewport &viewport) { return T3D_OK; }
    TResult NullGL4Context::clearColor(const ColorRGB &color) { return T3D_OK; }
    TResult NullGL4Context::clearDepth(Real depth) { return T3D_OK; }
    TResult NullGL4Context::clearDepthStencil(Real depth, uint32_t stencil) { return T3D_OK; }
    RHIBlendStatePtr NullGL4Context::createBlendState(BlendState *state) { return NullGL4BlendState::create(); }
    RHIDepthStencilStatePtr NullGL4Context::createDepthStencilState(DepthStencilState *state) { return NullGL4DepthStencilState::create(); }
    RHIRasterizerStatePtr NullGL4Context::createRasterizerState(RasterizerState *state) { return NullGL4RasterizerState::create(); }
    RHISamplerStatePtr NullGL4Context::createSamplerState(SamplerState *state) { return NullGL4SamplerState::create(); }
    TResult NullGL4Context::setBlendState(BlendState *state) { return T3D_OK; }
    TResult NullGL4Context::setDepthStencilState(DepthStencilState *state) { return T3D_OK; }
    TResult NullGL4Context::setRasterizerState(RasterizerState *state) { return T3D_OK; }
    RHIVertexDeclarationPtr NullGL4Context::createVertexDeclaration(VertexDeclaration *decl) { return NullGL4VertexDeclaration::create(); }
    TResult NullGL4Context::setVertexDeclaration(VertexDeclaration *decl) { return T3D_OK; }
    RHIVertexBufferPtr NullGL4Context::createVertexBuffer(VertexBuffer *buffer) { return NullGL4VertexBuffer::create(); }
    TResult NullGL4Context::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) { return T3D_OK; }
    RHIIndexBufferPtr NullGL4Context::createIndexBuffer(IndexBuffer *buffer) { return NullGL4IndexBuffer::create(); }
    TResult NullGL4Context::setIndexBuffer(IndexBuffer *buffer) { return T3D_OK; }
    RHIConstantBufferPtr NullGL4Context::createConstantBuffer(ConstantBuffer *buffer) { return NullGL4ConstantBuffer::create(); }
    RHIPixelBuffer1DPtr NullGL4Context::createPixelBuffer1D(PixelBuffer1D *buffer) { return NullGL4PixelBuffer1D::create(); }
    RHIPixelBuffer2DPtr NullGL4Context::createPixelBuffer2D(PixelBuffer2D *buffer) { return NullGL4PixelBuffer2D::create(); }
    RHIPixelBuffer3DPtr NullGL4Context::createPixelBuffer3D(PixelBuffer3D *buffer) { return nullptr; }
    TResult NullGL4Context::setVertexShader(ShaderVariant *shader) { return T3D_OK; }
    TResult NullGL4Context::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setVSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult NullGL4Context::setPixelShader(ShaderVariant *shader) { return T3D_OK; }
    TResult NullGL4Context::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setPSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult NullGL4Context::setHullShader(ShaderVariant *shader) { return T3D_OK; }
    TResult NullGL4Context::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setHSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult NullGL4Context::setDomainShader(ShaderVariant *shader) { return T3D_OK; }
    TResult NullGL4Context::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setDSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult NullGL4Context::setGeometryShader(ShaderVariant *shader) { return T3D_OK; }
    TResult NullGL4Context::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setGSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult NullGL4Context::setComputeShader(ShaderVariant *shader) { return T3D_OK; }
    TResult NullGL4Context::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult NullGL4Context::setCSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult NullGL4Context::setPrimitiveType(PrimitiveType primitive) { return T3D_OK; }
    TResult NullGL4Context::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) { return T3D_OK; }
    TResult NullGL4Context::render(uint32_t vertexCount, uint32_t startVertex) { return T3D_OK; }
    TResult NullGL4Context::reset() { return T3D_OK; }
    TResult NullGL4Context::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult NullGL4Context::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult NullGL4Context::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult NullGL4Context::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult NullGL4Context::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset) { return T3D_OK; }
    TResult NullGL4Context::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer) { return T3D_OK; }
}
