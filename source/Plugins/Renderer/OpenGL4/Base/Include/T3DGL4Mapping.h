/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4_MAPPING_H__
#define __T3D_GL4_MAPPING_H__


#include "T3DGL4PrerequisitesBase.h"


namespace Tiny3D
{
    class GL4Mapping
    {
    public:
        static GLenum get(PixelFormat format);
        /// 不支持的格式返回 GL_NONE
        static GLenum getInternalFormat(PixelFormat format);
        static GLenum getPixelType(PixelFormat format);
        static uint32_t getBytesPerPixel(PixelFormat format);
        static bool isDepthFormat(PixelFormat format);
        static bool hasStencil(PixelFormat format);

        static GLenum get(BlendFactor factor);
        static GLenum get(BlendOperation op);

        static GLenum get(CompareFunction func);
        static GLenum get(StencilOp op);

        static GLenum get(PolygonMode mode);
        static GLenum get(CullingMode mode);

        static GLenum get(TextureAddressMode mode);
        static GLenum getMinFilter(FilterOptions minFilter, FilterOptions mipFilter);
        static GLenum getMagFilter(FilterOptions magFilter);

        static GLenum get(PrimitiveType primitive);
        static GLenum get(IndexType idxType);

        /// Usage 与 CPUAccessMode 的组合规则与 D3D11Mapping::get 一致，非法组合返回 T3D_ERR_INVALID_PARAM
        static TResult checkUsage(Usage usage, uint32_t accessMode);
        /// 组合是否对应 D3D11_USAGE_DEFAULT，只有这类资源可以带 kGPUUnorderedAccess
        static bool isDefaultUsage(Usage usage, uint32_t accessMode);
        static GLenum getBufferUsage(Usage usage, uint32_t accessMode);

        static GLint getVertexAttribSize(VertexAttribute::Type type);
        static GLenum getVertexAttribType(VertexAttribute::Type type);
        static GLboolean getVertexAttribNormalized(VertexAttribute::Type type);
        static bool isIntegerAttrib(VertexAttribute::Type type);
        /// 与 D3D11 输入布局一致的 HLSL 语义名（不含序号）
        static const char *getVertexSemanticName(VertexAttribute::Semantic semantic);
    };
}


#endif  /*__T3D_GL4_MAPPING_H__*/
