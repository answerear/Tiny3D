
#include "mconv_ogreserializer.h"


namespace mconv
{
    enum OgreMeshChundID
    {
        OGRE_HEADER = 0x1000,

        //////////////////////////////////////////////////////////////////////////
        // Mesh
        //////////////////////////////////////////////////////////////////////////

        OGRE_MESH = 0x3000,

        OGRE_SUBMESH = 0x4000,
        OGRE_SUBMESH_OPERATION = 0x4010,
        OGRE_SUBMESH_BONE_ASSIGNMENT = 0x4100,
        OGRE_SUBMESH_TEXTURE_ALIAS = 0x4200,

        OGRE_GEOMETRY = 0x5000,
        OGRE_GEOMETRY_VERTEX_DECLARATION = 0x5100,
        OGRE_GEOMETRY_VERTEX_ELEMENT = 0x5110,
        OGRE_GEOMETRY_VERTEX_BUFFER = 0x5200,
        OGRE_GEOMETRY_VERTEX_BUFFER_DATA = 0x5210,

        OGRE_MESH_SKELETON_LINK = 0x6000,

        OGRE_MESH_BONE_ASSIGNMENT = 0x7000,

        OGRE_MESH_LOD = 0x8000,
        OGRE_MESH_LOD_USAGE = 0x8100,
        OGRE_MESH_LOD_MANUAL = 0x8110,
        OGRE_MESH_LOD_GENERATED = 0x8120,

        OGRE_MESH_BOUNDS = 0x9000,

        OGRE_SUBMESH_NAME_TABLE = 0xA000,
        OGRE_SUBMESH_NAME_TABLE_ELEMENT = 0xA100,

        OGRE_EDGE_LISTS = 0xB000,
        OGRE_EDGE_LIST_LOD = 0xB100,
        OGRE_EDGE_GROUP = 0xB110,

        OGRE_POSES = 0xC000,
        OGRE_POSE = 0xC100,
        OGRE_POSE_VERTEX = 0xC111,

        OGRE_ANIMATIONS = 0xD000,
        OGRE_ANIMATION = 0xD100,
        OGRE_ANIMATION_TRACK = 0xD110,
        OGRE_ANIMATION_MORPH_KEYFRAME = 0xD111,
        OGRE_ANIMATION_POSE_KEYFRAME = 0xD112,
        OGRE_ANIMATION_POSE_REF = 0xD113,

        OGRE_TABLE_EXTREMES = 0xE000,

        OGRE_GEOMETRY_NORMALS = 0x5100,
        OGRE_GEOMETRY_COLORS = 0x5200,
        OGRE_GEOMETRY_TEXCOORDS = 0x5300,

        //////////////////////////////////////////////////////////////////////////
        // Skeleton
        //////////////////////////////////////////////////////////////////////////

        OGRE_SKELETON_BONE = 0x2000,

        OGRE_SKELETON_BONE_PARENT = 0x3000,

        OGRE_SKELETON_ANIMATION = 0x4000,
        OGRE_SKELETON_ANIMATION_TRACK = 0x4100,
        OGRE_SKELETON_ANIMATION_TRACK_KEYFRAME = 0x4110,

        OGRE_SKELETON_ANIMATION_LINK = 0x5000,
    };

    OgreSerializer::OgreSerializer()
    {

    }

    OgreSerializer::~OgreSerializer()
    {

    }

    bool OgreSerializer::load(const String &path, void *&pData)
    {

        return false;
    }

    bool OgreSerializer::save(const String &path, void *pData)
    {
        return false;
    }
}
