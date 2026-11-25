/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GLES3_MAPPING_H__
#define __T3D_GLES3_MAPPING_H__


#include "T3DGLES3PrerequisitesBase.h"


namespace Tiny3D
{
    class GLES3Mapping
    {
    public:
        static void detectExtensions();
        static bool isBGRAFormat(PixelFormat format);
        static bool isBGRAExtSupported() { return sBGRAExtSupported; }
        static bool isAnisotropicSupported() { return sAnisotropicSupported; }
        static bool isBorderClampSupported() { return sBorderClampSupported; }
        static bool isDrawBuffersIndexedSupported() { return sDrawBuffersIndexedSupported; }
        static bool isTessellationSupported() { return sTessellationSupported; }
        static bool isBaseVertexExtSupported() { return sBaseVertexExtSupported; }
        static bool isColorBufferFloatSupported() { return sColorBufferFloatSupported; }
        static GLfloat getMaxAnisotropy() { return sMaxAnisotropy; }

        static uint32_t getBytesPerPixel(PixelFormat format);

        static GLenum get(PixelFormat format);
        static GLenum getInternalFormat(PixelFormat format);
        static GLenum getPixelType(PixelFormat format);

        /**
         * \brief 按 ES 3.x 规范的格式表与已检测到的扩展给出 PixelFormatSupportFlags
         * \param [in] has31 : 是否 GLES 3.1+（image load/store 需要）
         * \remarks 不含 kFormatSupportMultisample，采样数要在有上下文时查询。
         *          GLES3 后端没有实现 kTyped 结构化缓冲，不给 kFormatSupportTypedBuffer
         */
        static uint32_t getFormatSupport(PixelFormat format, bool has31);

        static GLenum get(BlendFactor factor);
        static GLenum get(BlendOperation op);

        static GLenum get(CompareFunction func);
        static GLenum get(StencilOp op);

        static GLenum get(CullingMode mode);

        static GLenum get(TextureAddressMode mode);
        static GLenum getMinFilter(FilterOptions minFilter, FilterOptions mipFilter);
        static GLenum getMagFilter(FilterOptions magFilter);

        static GLenum get(PrimitiveType primitive);
        static GLenum get(IndexType idxType);

        /// GL 的 usage hint；带 kGPUUnorderedAccess 或 kGPUIndirectArgs 的缓冲由 GPU 写、GPU 读，返回 GL_DYNAMIC_COPY
        static GLenum getBufferUsage(Usage usage, uint32_t gpuAccess);

        static GLint getVertexAttribSize(VertexAttribute::Type type);
        static GLenum getVertexAttribType(VertexAttribute::Type type);
        static GLboolean getVertexAttribNormalized(VertexAttribute::Type type);
        static bool isIntegerAttrib(VertexAttribute::Type type);

    private:
        static bool sBGRAExtSupported;
        static bool sAnisotropicSupported;
        static bool sBorderClampSupported;
        static bool sDrawBuffersIndexedSupported;
        static bool sTessellationSupported;
        static bool sBaseVertexExtSupported;
        static bool sColorBufferFloatSupported;
        static bool sColorBufferHalfFloatSupported;
        static bool sTextureFloatLinearSupported;
        static bool sFloatBlendSupported;
        static bool sTextureNorm16Supported;
        static GLfloat sMaxAnisotropy;
    };
}


#endif  /*__T3D_GLES3_MAPPING_H__*/
