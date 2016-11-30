

#ifndef __MCONV_OGRE_SERIALIZER_H__
#define __MCONV_OGRE_SERIALIZER_H__


#include "mconv_serializer.h"


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
    };

    struct OgreNode
    {
        enum Type
        {
            E_TYPE_NONE = 0,
            E_TYPE_SCENE,
            E_TYPE_MESH,
            E_TYPE_SUBMESH,
        };

        virtual Type getType() const = 0;
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
        std::vector<OgreVertexElement>  elements;
        std::vector<OgreVertexBuffer>   buffers;
    };

    struct OgreSubMesh
    {
        std::string             materialName;
        bool                    hasSharedVertices;
        std::vector<int32_t>    indices;
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
        bool readChunk(Tiny3D::DataStream &stream, );
        bool readChunkData(Tiny3D::DataStream &stream, OgreChunkHeader &header);
        bool readMesh(Tiny3D::DataStream &stream, OgreMesh &mesh);
    };
}


#endif  /*__MCONV_OGRE_SERIALIZER_H__*/
