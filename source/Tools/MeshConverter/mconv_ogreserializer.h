

#ifndef __MCONV_OGRE_SERIALIZER_H__
#define __MCONV_OGRE_SERIALIZER_H__


#include "mconv_serializer.h"

#include <T3DVector3.h>
#include <T3DQuaternion.h>


namespace mconv
{
    struct OgreChunkHeader
    {
        uint16_t    id;
        uint32_t    length;
    };

    struct OgreChunkData
    {
        OgreChunkHeader     header;
        uint32_t            read;

        OgreChunkData()
        {
            header.id = 0;
            header.length = 0;
            read = 0;
        }
    };

    struct OgreVertexElement
    {
        uint16_t    source;
        uint16_t    type;
        uint16_t    semantic;
        uint16_t    offset;
        uint16_t    index;
    };

    struct OgreVertexBuffer
    {
        uint16_t            bindIndex;
        uint16_t            vertexSize;
        std::vector<float>  vertices;
    };

    struct OgreGeometry
    {
        uint32_t                        vertexCount;
        std::vector<OgreVertexElement>  elements;
        std::vector<OgreVertexBuffer>   buffers;
    };

    struct OgreSubMesh
    {
        std::string             materialName;
        bool                    hasSharedVertices;
        std::vector<uint32_t>   indices;
        bool                    indices32Bit;
        OgreGeometry            geometry;
        uint16_t                operation;
    };

    struct OgreMesh
    {
        bool                        hasSkeleton;
        OgreGeometry                geometry;
        std::vector<OgreSubMesh>    submeshes;
    };

    class OgreSerializer : public Serializer
    {
    public:
        OgreSerializer();
        virtual ~OgreSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;

    protected:
        bool readChunk(Tiny3D::DataStream &stream, OgreMesh &mesh);
        bool readChunkData(Tiny3D::DataStream &stream, OgreChunkData &data);
        bool readMesh(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh);
        bool readSubMesh(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh);
        bool readGeometry(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh);
        bool readGeometryVertexDeclaration(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry);
        bool readGeometryVertexBuffer(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry);

        size_t readBools(Tiny3D::DataStream &stream, OgreChunkData &data, bool *value, size_t count = 1);
        size_t readBytes(Tiny3D::DataStream &stream, OgreChunkData &data, uint8_t *value, size_t count = 1);
        size_t readShorts(Tiny3D::DataStream &stream, OgreChunkData &data, uint16_t *value, size_t count = 1);
        size_t readInts(Tiny3D::DataStream &stream, OgreChunkData &data, uint32_t *value, size_t count = 1);
        size_t readFloats(Tiny3D::DataStream &stream, OgreChunkData &data, float *value, size_t count = 1);
        size_t readFloats(Tiny3D::DataStream &stream, OgreChunkData &data, double *value, size_t count = 1);
        size_t readObject(Tiny3D::DataStream &stream, OgreChunkData &data, Tiny3D::Vector3 &value);
        size_t readObject(Tiny3D::DataStream &stream, OgreChunkData &data, Tiny3D::Quaternion &value);

        String readString(Tiny3D::DataStream &stream, OgreChunkData &data);
        String readString(Tiny3D::DataStream &stream, OgreChunkData &data, size_t numChars);

        void swapEndian(void *data, size_t size, size_t count);
        void swapEndian(void *data, size_t size);

        bool    mSwapEndian;
    };
}


#endif  /*__MCONV_OGRE_SERIALIZER_H__*/
