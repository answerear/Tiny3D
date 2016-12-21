

#ifndef __MCONV_OGRE_DATA_H__
#define __MCONV_OGRE_DATA_H__


#include "mconv_prerequisites.h"


namespace mconv
{
#pragma pack(push, 1)
    struct OgreChunkHeader
    {
        uint16_t    id;
        uint32_t    length;
    };
#pragma pack(pop)

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

    struct OgreTextureAlias
    {
        OgreTextureAlias()  {}
        OgreTextureAlias(const String &texture, const String &alias) : textureName(texture), aliasName(alias)   {}

        String  textureName;
        String  aliasName;
    };

    struct OgreBoneAssignment
    {
        uint32_t    vertexID;
        uint16_t    boneID;
        float       weight;
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
        std::string                     materialName;
        bool                            hasSharedVertices;
        std::vector<uint32_t>           indices;
        bool                            indices32Bit;
        OgreGeometry                    geometry;
        uint16_t                        operation;
        std::vector<OgreTextureAlias>   textureAliases;
        std::vector<OgreBoneAssignment> boneAssignments;
    };

    struct OgreMesh
    {
        bool                        hasSkeleton;
        OgreGeometry                geometry;
        std::vector<OgreSubMesh>    submeshes;
    };
}


#endif  /*__MCONV_OGRE_DATA_H__*/
