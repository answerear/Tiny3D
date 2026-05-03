/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsoleContext.h"
#include "T3DGL4ConsoleError.h"
#include "T3DGL4ConsoleWindow.h"
#include "T3DGL4ConsoleRenderBuffer.h"
#include "T3DGL4ConsoleRenderState.h"
#include "T3DGL4ConsoleShader.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4ConsoleContextPtr GL4ConsoleContext::create()
    {
        return T3D_NEW GL4ConsoleContext();
    }

    //--------------------------------------------------------------------------

    GL4ConsoleContext::GL4ConsoleContext()
    {
    }

    //--------------------------------------------------------------------------

    GL4ConsoleContext::~GL4ConsoleContext()
    {
        if (mGlslangInitialized)
        {
            glslang::FinalizeProcess();
            mGlslangInitialized = false;
        }
        destroyDummyContext();
    }

    //--------------------------------------------------------------------------

    TResult GL4ConsoleContext::init()
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

    TResult GL4ConsoleContext::initDummyContext()
    {
        TResult ret = T3D_OK;

        do
        {
#if defined(T3D_OS_WINDOWS)
            const wchar_t *className = L"Tiny3DGL4ConsoleDummyWindow";
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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "Failed to create dummy window !");
                ret = T3D_ERR_GL4CONSOLE_CREATE_CONTEXT;
                break;
            }

            mDummyHDC = GetDC(mDummyHWND);
            if (mDummyHDC == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "Failed to get dummy HDC !");
                ret = T3D_ERR_GL4CONSOLE_CREATE_CONTEXT;
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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "ChoosePixelFormat failed !");
                ret = T3D_ERR_GL4CONSOLE_SET_PIXEL_FORMAT;
                break;
            }

            if (!SetPixelFormat(mDummyHDC, pixelFormat, &pfd))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "SetPixelFormat failed !");
                ret = T3D_ERR_GL4CONSOLE_SET_PIXEL_FORMAT;
                break;
            }

            mDummyHGLRC = wglCreateContext(mDummyHDC);
            if (mDummyHGLRC == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "wglCreateContext failed !");
                ret = T3D_ERR_GL4CONSOLE_CREATE_CONTEXT;
                break;
            }

            if (!wglMakeCurrent(mDummyHDC, mDummyHGLRC))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "wglMakeCurrent failed !");
                ret = T3D_ERR_GL4CONSOLE_CREATE_CONTEXT;
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
                    T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "gladLoadGLLoader failed !");
                    ret = T3D_ERR_GL4CONSOLE_GLAD_LOAD;
                    break;
                }
            }

#elif defined(T3D_OS_LINUX)
            mDummyDisplay = XOpenDisplay(nullptr);
            if (mDummyDisplay == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "XOpenDisplay failed !");
                ret = T3D_ERR_GL4CONSOLE_CREATE_CONTEXT;
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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "glXChooseVisual failed !");
                ret = T3D_ERR_GL4CONSOLE_CREATE_CONTEXT;
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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "glXCreateContext failed !");
                ret = T3D_ERR_GL4CONSOLE_CREATE_CONTEXT;
                break;
            }

            if (!glXMakeCurrent(mDummyDisplay, mDummyWindow, mDummyGLXContext))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "glXMakeCurrent failed !");
                ret = T3D_ERR_GL4CONSOLE_CREATE_CONTEXT;
                break;
            }

            if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddress))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "gladLoadGLLoader failed !");
                ret = T3D_ERR_GL4CONSOLE_GLAD_LOAD;
                break;
            }
#endif

            mGLADLoaded = true;

            T3D_LOG_INFO(LOG_TAG_GL4CONSOLERENDERER, "OpenGL Vendor: %s", glGetString(GL_VENDOR));
            T3D_LOG_INFO(LOG_TAG_GL4CONSOLERENDERER, "OpenGL Renderer: %s", glGetString(GL_RENDERER));
            T3D_LOG_INFO(LOG_TAG_GL4CONSOLERENDERER, "OpenGL Version: %s", glGetString(GL_VERSION));
            T3D_LOG_INFO(LOG_TAG_GL4CONSOLERENDERER, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            T3D_LOG_INFO(LOG_TAG_GL4CONSOLERENDERER, "GL4Console dummy context initialized.");

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void GL4ConsoleContext::destroyDummyContext()
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
            UnregisterClassW(L"Tiny3DGL4ConsoleDummyWindow", GetModuleHandleW(nullptr));
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

    TResult GL4ConsoleContext::compileShader(ShaderVariant *shader)
    {
        size_t bytesLength = 0;
        const char *bytes = shader->getBytesCode(bytesLength);
        shader->setBytesCode(bytes, bytesLength);

        return glslangCompileAndReflect(shader);
    }

    //--------------------------------------------------------------------------

    TResult GL4ConsoleContext::glslangCompileAndReflect(ShaderVariant *shader)
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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "glslangCompileAndReflect: unsupported shader stage !");
                ret = T3D_ERR_GL4CONSOLE_SHADER_REFLECTION;
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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "glslang parse error:\n%s", glslangShader.getInfoLog());
                ret = T3D_ERR_GL4CONSOLE_SHADER_REFLECTION;
                break;
            }

            glslang::TProgram program;
            program.addShader(&glslangShader);

            if (!program.link(EShMsgDefault))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "glslang link error:\n%s", program.getInfoLog());
                ret = T3D_ERR_GL4CONSOLE_SHADER_REFLECTION;
                break;
            }

            if (!program.buildReflection(EShReflectionAllBlockVariables))
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "glslang buildReflection failed !");
                ret = T3D_ERR_GL4CONSOLE_SHADER_REFLECTION;
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

    RHIShaderPtr GL4ConsoleContext::createVertexShader(ShaderVariant *shader)
    {
        GL4ConsoleVertexShaderPtr glShader = GL4ConsoleVertexShader::create();

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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "Vertex shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr GL4ConsoleContext::createPixelShader(ShaderVariant *shader)
    {
        GL4ConsolePixelShaderPtr glShader = GL4ConsolePixelShader::create();

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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "Pixel shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr GL4ConsoleContext::createGeometryShader(ShaderVariant *shader)
    {
        GL4ConsoleGeometryShaderPtr glShader = GL4ConsoleGeometryShader::create();

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
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "Geometry shader compile error: %s", log.data());
            }
            GL_SAFE_DELETE_SHADER(glShader->GLShaderHandle);
            return nullptr;
        }

        return glShader;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr GL4ConsoleContext::createHullShader(ShaderVariant *shader)
    {
        return GL4ConsoleHullShader::create();
    }

    RHIShaderPtr GL4ConsoleContext::createDomainShader(ShaderVariant *shader)
    {
        return GL4ConsoleDomainShader::create();
    }

    RHIShaderPtr GL4ConsoleContext::createComputeShader(ShaderVariant *shader)
    {
        return GL4ConsoleComputeShader::create();
    }

    //--------------------------------------------------------------------------
    // Core: reflectShaderAllBindings (read from glslang cache)
    //--------------------------------------------------------------------------

    TResult GL4ConsoleContext::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mReflectionCache.find(shader);
            if (itr == mReflectionCache.end())
            {
                T3D_LOG_ERROR(LOG_TAG_GL4CONSOLERENDERER, "reflectShaderAllBindings: no cached reflection data (compileShader not called?)");
                ret = T3D_ERR_GL4CONSOLE_SHADER_REFLECTION;
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

                    T3D_LOG_DEBUG(LOG_TAG_GL4CONSOLERENDERER, "Reflect - UBO: %s -> %s, uniform: %s -> %s, type: %u, size: %u, offset: %d",
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

                    T3D_LOG_DEBUG(LOG_TAG_GL4CONSOLERENDERER, "Reflect - sampler: %s -> %s, binding: %d, type: %d",
                        uniform.name.c_str(), name.c_str(), samplerIndex, texType);
                    samplerIndex++;
                }
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4ConsoleContext::reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    // Empty implementations
    //--------------------------------------------------------------------------

    TResult GL4ConsoleContext::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) { return T3D_OK; }
    RHIRenderTargetPtr GL4ConsoleContext::createRenderWindow(RenderWindow *renderWindow) { return GL4ConsoleWindow::create(); }
    RHIPixelBuffer2DPtr GL4ConsoleContext::createRenderTexture(PixelBuffer2D *buffer) { return nullptr; }
    TResult GL4ConsoleContext::setRenderTarget(RenderTarget *renderTarget) { return T3D_OK; }
    TResult GL4ConsoleContext::resetRenderTarget() { return T3D_OK; }
    TResult GL4ConsoleContext::setViewport(const Viewport &viewport) { return T3D_OK; }
    TResult GL4ConsoleContext::clearColor(const ColorRGB &color) { return T3D_OK; }
    TResult GL4ConsoleContext::clearDepth(Real depth) { return T3D_OK; }
    TResult GL4ConsoleContext::clearDepthStencil(Real depth, uint32_t stencil) { return T3D_OK; }
    RHIBlendStatePtr GL4ConsoleContext::createBlendState(BlendState *state) { return GL4ConsoleBlendState::create(); }
    RHIDepthStencilStatePtr GL4ConsoleContext::createDepthStencilState(DepthStencilState *state) { return GL4ConsoleDepthStencilState::create(); }
    RHIRasterizerStatePtr GL4ConsoleContext::createRasterizerState(RasterizerState *state) { return GL4ConsoleRasterizerState::create(); }
    RHISamplerStatePtr GL4ConsoleContext::createSamplerState(SamplerState *state) { return GL4ConsoleSamplerState::create(); }
    TResult GL4ConsoleContext::setBlendState(BlendState *state) { return T3D_OK; }
    TResult GL4ConsoleContext::setDepthStencilState(DepthStencilState *state) { return T3D_OK; }
    TResult GL4ConsoleContext::setRasterizerState(RasterizerState *state) { return T3D_OK; }
    RHIVertexDeclarationPtr GL4ConsoleContext::createVertexDeclaration(VertexDeclaration *decl) { return GL4ConsoleVertexDeclaration::create(); }
    TResult GL4ConsoleContext::setVertexDeclaration(VertexDeclaration *decl) { return T3D_OK; }
    RHIVertexBufferPtr GL4ConsoleContext::createVertexBuffer(VertexBuffer *buffer) { return GL4ConsoleVertexBuffer::create(); }
    TResult GL4ConsoleContext::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) { return T3D_OK; }
    RHIIndexBufferPtr GL4ConsoleContext::createIndexBuffer(IndexBuffer *buffer) { return GL4ConsoleIndexBuffer::create(); }
    TResult GL4ConsoleContext::setIndexBuffer(IndexBuffer *buffer) { return T3D_OK; }
    RHIConstantBufferPtr GL4ConsoleContext::createConstantBuffer(ConstantBuffer *buffer) { return GL4ConsoleConstantBuffer::create(); }
    RHIPixelBuffer1DPtr GL4ConsoleContext::createPixelBuffer1D(PixelBuffer1D *buffer) { return GL4ConsolePixelBuffer1D::create(); }
    RHIPixelBuffer2DPtr GL4ConsoleContext::createPixelBuffer2D(PixelBuffer2D *buffer) { return GL4ConsolePixelBuffer2D::create(); }
    RHIPixelBuffer3DPtr GL4ConsoleContext::createPixelBuffer3D(PixelBuffer3D *buffer) { return nullptr; }
    TResult GL4ConsoleContext::setVertexShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setPixelShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setHullShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setHSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setDomainShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setDSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setGeometryShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setGSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setComputeShader(ShaderVariant *shader) { return T3D_OK; }
    TResult GL4ConsoleContext::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) { return T3D_OK; }
    TResult GL4ConsoleContext::setCSSamplers(uint32_t startSlot, const Samplers &samplers) { return T3D_OK; }
    TResult GL4ConsoleContext::setPrimitiveType(PrimitiveType primitive) { return T3D_OK; }
    TResult GL4ConsoleContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) { return T3D_OK; }
    TResult GL4ConsoleContext::render(uint32_t vertexCount, uint32_t startVertex) { return T3D_OK; }
    TResult GL4ConsoleContext::reset() { return T3D_OK; }
    TResult GL4ConsoleContext::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset) { return T3D_OK; }
    TResult GL4ConsoleContext::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer) { return T3D_OK; }
}
