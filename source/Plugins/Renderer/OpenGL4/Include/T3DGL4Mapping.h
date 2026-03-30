/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4_MAPPING_H__
#define __T3D_GL4_MAPPING_H__


#include "T3DGL4Prerequisites.h"


namespace Tiny3D
{
    class GL4Mapping
    {
    public:
        static GLenum get(PixelFormat format);
        static GLenum getInternalFormat(PixelFormat format);
        static GLenum getPixelType(PixelFormat format);

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

        static GLenum getBufferUsage(Usage usage);

        static GLint getVertexAttribSize(VertexAttribute::Type type);
        static GLenum getVertexAttribType(VertexAttribute::Type type);
        static GLboolean getVertexAttribNormalized(VertexAttribute::Type type);
    };
}


#endif  /*__T3D_GL4_MAPPING_H__*/
