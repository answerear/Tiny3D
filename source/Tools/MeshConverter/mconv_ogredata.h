

#ifndef __MCONV_OGRE_DATA_H__
#define __MCONV_OGRE_DATA_H__


#include "mconv_prerequisites.h"


namespace mconv
{
    enum OgreVertexElementSemantic
    {
        VES_POSITION = 1,
        VES_BLEND_WEIGHTS = 2,
        VES_BLEND_INDICES = 3,
        VES_NORMAL = 4,
        VES_DIFFUSE = 5,
        VES_SPECULAR = 6,
        VES_TEXTURE_COORDINATES = 7,
        VES_BINORMAL = 8,
        VES_TANGENT = 9,
    };

    enum OgreVertexElementType
    {
        VET_FLOAT1 = 0,
        VET_FLOAT2 = 1,
        VET_FLOAT3 = 2,
        VET_FLOAT4 = 3,
        VET_COLOR = 4,
        VET_SHORT1 = 5,
        VET_SHORT2 = 6,
        VET_SHORT3 = 7,
        VET_SHORT4 = 8,
        VET_UBYTE4 = 9,
        VET_COLOR_ARGB = 10,
        VET_COLOR_ABGR = 11,
    };

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
        String                          materialName;
        bool                            hasSharedVertices;
        std::vector<uint32_t>           indices;
        bool                            indices32Bit;
        OgreGeometry                    geometry;
        uint16_t                        operation;
        std::vector<OgreTextureAlias>   textureAliases;
        std::vector<OgreBoneAssignment> boneAssignments;
    };

    struct OgreBone
    {
        String                          name;
        Vector3                         position;
        Quaternion                      orientation;
        Vector3                         scale;
        uint16_t                        handle;
        uint16_t                        parent;
    };

    struct OgreKeyframe
    {
        uint16_t                        boneID;
        float                           time;
        Vector3                         position;
        Quaternion                      orientation;
        Vector3                         scale;
    };

    struct OgreAnimation
    {
        String                          name;
        float                           length;
        std::vector<OgreKeyframe>       keyframes;
    };

    struct OgreSkeleton
    {
        std::vector<OgreBone>           bones;
        std::vector<OgreAnimation>      animations;
    };

    struct OgreMesh
    {
        bool                            hasSkeleton;
        OgreGeometry                    geometry;
        std::vector<OgreSubMesh>        submeshes;
		std::vector<OgreBoneAssignment> boneAssignments;
        Vector3                         minEdge;
        Vector3                         maxEdge;
        float                           radius;

        OgreSkeleton                    skeleton;
    };

    struct OgreTexture
    {
        
    };

    struct OgrePass
    {
    };

    struct OgreTechnique
    {
    };

    struct OgreMaterial
    {
    };
}


#endif  /*__MCONV_OGRE_DATA_H__*/
