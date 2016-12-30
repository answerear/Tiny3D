

#ifndef __MCONV_OGRE_SERIALIZER_H__
#define __MCONV_OGRE_SERIALIZER_H__


#include "mconv_serializer.h"
#include "mconv_ogredata.h"

#include <T3DVector3.h>
#include <T3DQuaternion.h>


namespace mconv
{
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
        bool readGeometry(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry);
        bool readGeometryVertexDeclaration(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry);
        bool readGeometryVertexBuffer(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry);

        bool readSubMesh(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh);
        bool readSubMeshOperation(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh);
        bool readSubMeshTextureAlias(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh);
        bool readSubMeshBoneAssignment(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh);

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