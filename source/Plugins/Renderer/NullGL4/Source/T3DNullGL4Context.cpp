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
        destroyDummyContext();
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Context::init()
    {
        return initDummyContext();
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
    // Core: compileShader (GLSL pass-through)
    //--------------------------------------------------------------------------

    TResult NullGL4Context::compileShader(ShaderVariant *shader)
    {
        size_t bytesLength = 0;
        const char *bytes = shader->getBytesCode(bytesLength);
        shader->setBytesCode(bytes, bytesLength);
        return T3D_OK;
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
    // Core: reflectShaderAllBindings (GL reflect API)
    //--------------------------------------------------------------------------

    TResult NullGL4Context::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            // If RHI shader is not yet created (reflect() is called without createXXXShader),
            // we need to compile the GLSL source into a temporary GL shader for reflection.
            GLuint tempShaderHandle = 0;
            bool ownsTempShader = false;

            NullGL4Shader *glShader = static_cast<NullGL4Shader*>(shader->getRHIShader());
            if (glShader == nullptr || glShader->GLShaderHandle == 0)
            {
                // Determine GL shader type from ShaderVariant stage
                GLenum glShaderType = 0;
                switch (shader->getShaderStage())
                {
                case SHADER_STAGE::kVertex:   glShaderType = GL_VERTEX_SHADER; break;
                case SHADER_STAGE::kPixel:    glShaderType = GL_FRAGMENT_SHADER; break;
                case SHADER_STAGE::kGeometry: glShaderType = GL_GEOMETRY_SHADER; break;
                case SHADER_STAGE::kCompute:  glShaderType = GL_COMPUTE_SHADER; break;
                default:
                    T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "Cannot reflect shader: unsupported stage !");
                    ret = T3D_ERR_NULLGL4_SHADER_REFLECTION;
                    break;
                }

                if (T3D_FAILED(ret))
                    break;

                size_t bytecodeLength = 0;
                const char *bytecode = shader->getBytesCode(bytecodeLength);

                tempShaderHandle = glCreateShader(glShaderType);
                GLint len = static_cast<GLint>(bytecodeLength);
                glShaderSource(tempShaderHandle, 1, &bytecode, &len);
                glCompileShader(tempShaderHandle);

                GLint compiled = 0;
                glGetShaderiv(tempShaderHandle, GL_COMPILE_STATUS, &compiled);
                if (!compiled)
                {
                    GLint logLen = 0;
                    glGetShaderiv(tempShaderHandle, GL_INFO_LOG_LENGTH, &logLen);
                    if (logLen > 0)
                    {
                        TArray<char> log(logLen + 1, 0);
                        glGetShaderInfoLog(tempShaderHandle, logLen, nullptr, log.data());
                        T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "Shader compile error during reflection: %s", log.data());
                    }
                    glDeleteShader(tempShaderHandle);
                    ret = T3D_ERR_NULLGL4_COMPILE_SHADER;
                    break;
                }

                ownsTempShader = true;
            }
            else
            {
                tempShaderHandle = glShader->GLShaderHandle;
            }

            GLuint tempProgram = glCreateProgram();
            glAttachShader(tempProgram, tempShaderHandle);
            glProgramParameteri(tempProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
            glLinkProgram(tempProgram);

            GLint linked = 0;
            glGetProgramiv(tempProgram, GL_LINK_STATUS, &linked);
            if (!linked)
            {
                GLint logLen = 0;
                glGetProgramiv(tempProgram, GL_INFO_LOG_LENGTH, &logLen);
                if (logLen > 0)
                {
                    TArray<char> log(logLen + 1, 0);
                    glGetProgramInfoLog(tempProgram, logLen, nullptr, log.data());
                    T3D_LOG_ERROR(LOG_TAG_NULLGL4RENDERER, "Shader link error during reflection: %s", log.data());
                }
                glDeleteProgram(tempProgram);
                ret = T3D_ERR_NULLGL4_LINK_PROGRAM;
                break;
            }

            // Reflect Uniform Blocks (cbuffers)
            GLint numBlocks = 0;
            glGetProgramiv(tempProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

            for (GLint i = 0; i < numBlocks; ++i)
            {
                char blockName[256] = {};
                GLsizei nameLen = 0;
                glGetActiveUniformBlockName(tempProgram, i, sizeof(blockName), &nameLen, blockName);

                GLint blockSize = 0;
                glGetActiveUniformBlockiv(tempProgram, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

                GLint numUniforms = 0;
                glGetActiveUniformBlockiv(tempProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms);

                TArray<GLint> uniformIndices(numUniforms);
                glGetActiveUniformBlockiv(tempProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());

                for (GLint j = 0; j < numUniforms; ++j)
                {
                    GLuint idx = static_cast<GLuint>(uniformIndices[j]);
                    char uniformName[256] = {};
                    GLsizei uniformNameLen = 0;
                    GLint uniformSize = 0;
                    GLenum uniformType = 0;
                    glGetActiveUniform(tempProgram, idx, sizeof(uniformName), &uniformNameLen, &uniformSize, &uniformType, uniformName);

                    GLint offset = 0;
                    glGetActiveUniformsiv(tempProgram, 1, &idx, GL_UNIFORM_OFFSET, &offset);

                    GLint arrayStride = 0;
                    glGetActiveUniformsiv(tempProgram, 1, &idx, GL_UNIFORM_ARRAY_STRIDE, &arrayStride);

                    uint32_t dataSize = 0;
                    ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_FLOAT;

                    switch (uniformType)
                    {
                    case GL_FLOAT:       dataSize = sizeof(float) * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY : ShaderConstantParam::DATA_TYPE::DT_FLOAT; break;
                    case GL_FLOAT_VEC4:  dataSize = sizeof(float) * 4 * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_VECTOR4; break;
                    case GL_FLOAT_MAT4:  dataSize = sizeof(float) * 16 * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY : ShaderConstantParam::DATA_TYPE::DT_MATRIX4; break;
                    case GL_INT:         dataSize = sizeof(int) * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY : ShaderConstantParam::DATA_TYPE::DT_INTEGER; break;
                    case GL_BOOL:        dataSize = sizeof(int) * uniformSize; dataType = (uniformSize > 1) ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY : ShaderConstantParam::DATA_TYPE::DT_BOOL; break;
                    default:             dataSize = arrayStride > 0 ? arrayStride * uniformSize : 4 * uniformSize; dataType = ShaderConstantParam::DATA_TYPE::DT_STRUCT; break;
                    }

                    String cbufferName(blockName);
                    String cname(uniformName);

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

                    ShaderConstantParamPtr param = ShaderConstantParam::create(cbufferName, cname, i, dataSize, offset, dataType);
                    constantParams.emplace(param->getName(), param);

                    T3D_LOG_DEBUG(LOG_TAG_NULLGL4RENDERER, "Reflect - UBO: %s -> %s, uniform: %s -> %s, type: %u, size: %u, offset: %d", blockName, cbufferName.c_str(), uniformName, cname.c_str(), dataType, dataSize, offset);
                }
            }

            // Reflect standalone Uniforms (texture samplers)
            GLint numUniforms = 0;
            glGetProgramiv(tempProgram, GL_ACTIVE_UNIFORMS, &numUniforms);

            uint32_t samplerIndex = 0;
            for (GLint i = 0; i < numUniforms; ++i)
            {
                char uniformName[256] = {};
                GLsizei nameLen = 0;
                GLint uniformSize = 0;
                GLenum uniformType = 0;
                glGetActiveUniform(tempProgram, i, sizeof(uniformName), &nameLen, &uniformSize, &uniformType, uniformName);

                bool isSampler = false;
                TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;

                switch (uniformType)
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
                    GLint loc = glGetUniformLocation(tempProgram, uniformName);
                    if (loc < 0)
                        continue;

                    String name(uniformName);

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
                    const auto itr = samplerParams.find(name);
                    if (itr == samplerParams.end())
                    {
                        param = ShaderSamplerParam::create(name);
                        samplerParams.emplace(name, param);
                    }
                    else
                    {
                        param = itr->second;
                    }

                    param->setTexBinding(samplerIndex);
                    param->setSamplerBinding(samplerIndex);
                    param->setTextureType(texType);

                    T3D_LOG_DEBUG(LOG_TAG_NULLGL4RENDERER, "Reflect - sampler: %s -> %s, binding: %d, type: %d", uniformName, name.c_str(), samplerIndex, texType);
                    samplerIndex++;
                }
            }

            glDetachShader(tempProgram, tempShaderHandle);
            glDeleteProgram(tempProgram);

            // Clean up temporary shader if we created it internally
            if (ownsTempShader)
            {
                glDeleteShader(tempShaderHandle);
            }

        } while (false);

        return ret;
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
