


#include "T3DGeometryData.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    GeometryDataPtr GeometryData::create(Renderer::PrimitiveType primitiveType, const VertexBuffers &buffers, const Indices &indices, bool is16bits, const String &materialName)
    {
        GeometryDataPtr geometry = new GeometryData();

        if (geometry != nullptr && geometry->init(primitiveType, buffers, indices, is16bits, materialName))
        {
            geometry->release();
        }
        else
        {
            T3D_SAFE_RELEASE(geometry);
        }

        return geometry;
    }

    GeometryData::GeometryData()
        : mPrimitiveType(Renderer::E_PT_TRIANGLE_LIST)
        , mVertexData(nullptr)
        , mIndexData(nullptr)
        , mMaterialName()
    {
    }

    GeometryData::~GeometryData()
    {
    }

    bool GeometryData::init(Renderer::PrimitiveType primitiveType, const VertexBuffers &buffers, const Indices &indices, bool is16bits, const String &materialName)
    {
        bool ret = false;

        mPrimitiveType = primitiveType;
        mMaterialName = materialName;

        auto itrBuffer = buffers.begin();

        while (itrBuffer != buffers.end())
        {
            const VertexBuffer &buffer = *itrBuffer;

            VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
            auto itr = buffer.attributes.begin();
            while (itr != buffer.attributes.end())
            {
                auto vertexElement = *itr;
                vertexDecl->addElement(vertexElement);
                ++itr;
            }

            size_t vertexCount = buffer.vertices.size() / buffer.vertexSize;
            HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(buffer.vertexSize, vertexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

            if (vertexDecl != nullptr && vertexBuffer != nullptr)
            {
                ret = vertexBuffer->writeData(0, buffer.vertices.size(), &buffer.vertices[0]);

                if (ret)
                {
                    if (mVertexData != nullptr)
                    {
                        mVertexData = VertexData::create();
                    }

                    VertexStreamPtr stream = VertexStream::create(vertexDecl, vertexBuffer);
                    mVertexData->addVertexStream(stream);
                }
            }

            ++itrBuffer;
        }

        HardwareIndexBuffer::Type indexType = HardwareIndexBuffer::E_IT_32BITS;
        size_t indexCount = indices.size() / sizeof(uint32_t);

        if (is16bits)
        {
            indexType = HardwareIndexBuffer::E_IT_16BITS;
            indexCount = indices.size() / sizeof(uint16_t);
        }

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(indexType, indexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        if (indexBuffer != nullptr)
        {
            size_t indexSize = indices.size();
            ret = indexBuffer->writeData(0, indexSize, &indices[0]);

            if (ret)
            {
                mIndexData = IndexData::create(indexBuffer);
            }
        }

        return ret;
    }
}
