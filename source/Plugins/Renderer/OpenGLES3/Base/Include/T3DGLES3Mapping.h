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

        static GLenum get(PixelFormat format);
        static GLenum getInternalFormat(PixelFormat format);
        static GLenum getPixelType(PixelFormat format);

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

        static GLenum getBufferUsage(Usage usage);

        static GLint getVertexAttribSize(VertexAttribute::Type type);
        static GLenum getVertexAttribType(VertexAttribute::Type type);
        static GLboolean getVertexAttribNormalized(VertexAttribute::Type type);
        static bool isIntegerAttrib(VertexAttribute::Type type);

    private:
        static bool sBGRAExtSupported;
    };
}


#endif  /*__T3D_GLES3_MAPPING_H__*/
