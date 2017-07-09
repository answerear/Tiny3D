
#include "mconv_ogreserializer.h"


using namespace Tiny3D;


namespace mconv
{
    #define OGRE_MESH_VERSION_130       "[MeshSerializer_v1.30]"
    #define OGRE_MESH_VERSION_140       "[MeshSerializer_v1.40]"
    #define OGRE_MESH_VERSION_141       "[MeshSerializer_v1.41]"

    #define OGRE_SERIALIZER_VERSION_110 "[Serializer_v1.10]"

    #define KF_ROTATION     (1<<0)
    #define KF_TRANSLATE    (1<<1)
    #define KF_SCALE        (1<<2)
    #define KF_ALL          (KF_ROTATION | KF_TRANSLATE | KF_SCALE)

    enum OgreMeshChundID
    {
        OGRE_HEADER = 0x1000,
        OGRE_OTHER_HEADER = 0x0010,

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
        OGRE_SKELETON_ANIMATION_TRACK_KEYFRAME_LIST = 0x4120,

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
        OgreMesh *mesh = new OgreMesh();

        bool ret = false;

        FileDataStream fs;

        if (fs.open(path.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            mSrcPath = path;

            size_t bytesOfRead = 0;
            uint16_t id;

            // 读取是否大小端交换标识
            bytesOfRead = fs.read(&id, sizeof(id));

            if (id == OGRE_HEADER)
            {
                mSwapEndian = false;
                ret = true;
            }
            else if (id == OGRE_OTHER_HEADER)
            {
                mSwapEndian = true;
                ret = true;
            }
            else
            {
                ret = false;
            }

            if (ret)
            {
                // 读取版本号
                OgreChunkData data;
                String version = readString(fs, data);

                if (version != OGRE_MESH_VERSION_130 
                    && version != OGRE_MESH_VERSION_140
                    && version != OGRE_MESH_VERSION_141)
                {
                    ret = false;
                }

                ret = ret && readChunk(fs, *mesh);
            }

            fs.close();
        }

        if (ret)
        {
            pData = mesh;
        }
        else
        {
            delete mesh;
            pData = nullptr;
        }

        return ret;
    }

    bool OgreSerializer::save(const String &path, void *pData)
    {
        return false;
    }

    bool OgreSerializer::readChunk(Tiny3D::DataStream &stream, OgreMesh &mesh)
    {
        bool ret = true;

        while (!stream.eof())
        {
            OgreChunkData data;
            readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_MESH:
                {
                    ret = ret && readMesh(stream, data, mesh);
                }
                break;
            default:
                {
                    T3D_ASSERT(data.header.id != 0);
                    size_t offset = data.header.length - data.read;
                    stream.seek(offset, true);
                    data.read += offset;
                }
                break;
            }
        }

        return ret;
    }

    bool OgreSerializer::readChunkData(Tiny3D::DataStream &stream, OgreChunkData &data)
    {
        bool ret = true;

        size_t bytesOfRead = readShorts(stream, data, &data.header.id);
        ret = ret && (bytesOfRead == sizeof(uint16_t));
        bytesOfRead = readInts(stream, data, &data.header.length);
        ret = ret && (bytesOfRead == sizeof(uint32_t));

        return ret;
    }

    bool OgreSerializer::readMesh(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh)
    {
        bool ret = true;
        size_t bytesOfRead = readBools(stream, parent, &mesh.hasSkeleton);

        ret = (bytesOfRead == sizeof(mesh.hasSkeleton));

        while (ret && !stream.eof())
        {
            OgreChunkData data;
            ret = readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_SUBMESH:
                {
                    mesh.submeshes.push_back(OgreSubMesh());
                    ret = ret && readSubMesh(stream, data, mesh.submeshes.back());
                }
                break;
            case OGRE_GEOMETRY:
                {
                    ret = ret && readGeometry(stream, data, mesh.geometry);
                }
                break;
            case OGRE_MESH_SKELETON_LINK:
                {
                    String skeletonName = readString(stream, data);
                    ret = ret && readSkeleton(skeletonName, mesh.skeleton);
                }
                break;
            case OGRE_MESH_BOUNDS:
                {
                    ret = ret && readMeshBound(stream, data, mesh);
                }
                break;
            case OGRE_MESH_BONE_ASSIGNMENT:
                {
                    mesh.boneAssignments.push_back(OgreBoneAssignment());
                    ret = ret && readBoneAssignment(stream, data, mesh.boneAssignments.back());
                }
                break;
            case OGRE_MESH_LOD:
                {
                    T3D_ASSERT(0);
                }
                break;
            case OGRE_SUBMESH_NAME_TABLE:
                {
                    ret = ret && readSubMeshNameTable(stream, data, mesh);
                }
                break;
            case OGRE_EDGE_LISTS:
                {
                    size_t offset = data.header.length - data.read;
                    stream.seek(offset, true);
                    data.read += offset;
                }
                break;
            case OGRE_POSES:
                {
                    T3D_ASSERT(0);
                }
                break;
            case OGRE_ANIMATIONS:
                {
                    T3D_ASSERT(0);
                }
                break;
            case OGRE_TABLE_EXTREMES:
                {
                    T3D_ASSERT(0);
                }
                break;
            default:
                {
                    // 跳过不需要解析的chunk
                    size_t offset = data.header.length - data.read;
                    stream.seek(offset, true);
                    data.read += offset;
                }
                break;
            }

            size_t s = stream.size();
            size_t pos = stream.tell();

            parent.read += data.read;
        }

//         T3D_ASSERT(parent.read == parent.header.length);

        return ret;
    }

    bool OgreSerializer::readGeometry(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry)
    {
        bool ret = true;

        size_t bytesOfRead = readInts(stream, parent, &geometry.vertexCount);
        ret = (bytesOfRead == sizeof(geometry.vertexCount));

        while (ret && parent.read < parent.header.length && !stream.eof())
        {
            OgreChunkData data;
            ret = readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_GEOMETRY_VERTEX_DECLARATION:
                {
                    ret = ret && readGeometryVertexDeclaration(stream, data, geometry);
                }
                break;
            case OGRE_GEOMETRY_VERTEX_BUFFER:
                {
                    ret = ret && readGeometryVertexBuffer(stream, data, geometry);
                }
                break;
            default:
                {
                    // 跳过不需要解析的chunk
                    size_t offset = data.header.length - data.read;
                    stream.seek(offset, true);
                    data.read += offset;
                }
                break;
            }

            T3D_ASSERT(data.read == data.header.length);

            parent.read += data.read;
        }

        T3D_ASSERT(parent.read == parent.header.length);

        return ret;
    }

    bool OgreSerializer::readGeometryVertexDeclaration(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry)
    {
        bool ret = true;
        size_t bytesOfRead = 0;

        while (ret && parent.read < parent.header.length && !stream.eof())
        {
            OgreChunkData data;
            ret = readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_GEOMETRY_VERTEX_ELEMENT:
                {
                    geometry.elements.push_back(OgreVertexElement());
                    OgreVertexElement &element = geometry.elements.back();
                    bytesOfRead = readShorts(stream, data, &element.source);
                    ret = (ret && (bytesOfRead == sizeof(element.source)));
                    bytesOfRead = readShorts(stream, data, &element.type);
                    ret = (ret && (bytesOfRead == sizeof(element.type)));
                    bytesOfRead = readShorts(stream, data, &element.semantic);
                    ret = (ret && (bytesOfRead == sizeof(element.semantic)));
                    bytesOfRead = readShorts(stream, data, &element.offset);
                    ret = (ret && (bytesOfRead == sizeof(element.offset)));
                    bytesOfRead = readShorts(stream, data, &element.index);
                    ret = (ret && (bytesOfRead == sizeof(element.index)));
                }
                break;
            default:
                {
                    // 跳过不需要解析的chunk
                    size_t offset = data.header.length - data.read;
                    stream.seek(offset, true);
                    data.read += offset;
                }
                break;
            }

            T3D_ASSERT(data.read == data.header.length);

            parent.read += data.read;
        }

        T3D_ASSERT(parent.read == parent.header.length);

        return ret;
    }

    bool OgreSerializer::readGeometryVertexBuffer(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreGeometry &geometry)
    {
        bool ret = true;

        geometry.buffers.push_back(OgreVertexBuffer());
        OgreVertexBuffer &vertexBuffer = geometry.buffers.back();

        size_t bytesOfRead = readShorts(stream, parent, &vertexBuffer.bindIndex);
        ret = ret && (bytesOfRead == sizeof(vertexBuffer.bindIndex));

        bytesOfRead = readShorts(stream, parent, &vertexBuffer.vertexSize);
        ret = ret && (bytesOfRead == sizeof(vertexBuffer.vertexSize));
        vertexBuffer.vertexSize /= sizeof(float);

        OgreChunkData data;
        ret = ret && readChunkData(stream, data);

        if (data.header.id == OGRE_GEOMETRY_VERTEX_BUFFER_DATA)
        {
            vertexBuffer.vertices.resize(vertexBuffer.vertexSize * geometry.vertexCount);
            ret = ret && readFloats(stream, data, &(*(vertexBuffer.vertices.begin())), geometry.vertexCount * vertexBuffer.vertexSize);
            parent.read += data.read;
        }

        T3D_ASSERT(data.read == data.header.length);

        return ret;
    }

    bool OgreSerializer::readSubMesh(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh)
    {
        bool ret = true;

        submesh.materialName = readString(stream, parent);
        size_t bytesOfRead = readBools(stream, parent, &submesh.hasSharedVertices);
        ret = ret && (bytesOfRead == sizeof(submesh.hasSharedVertices));

        uint32_t indexCount = 0;
        ret = ret && readInts(stream, parent, &indexCount);
        submesh.indices.resize(indexCount);
        ret = ret && readBools(stream, parent, &submesh.indices32Bit);

        if (submesh.indices32Bit)
        {
            ret = ret && readInts(stream, parent, &(*submesh.indices.begin()), indexCount);
        }
        else
        {
            size_t i = 0;

            for (i = 0; i < indexCount; ++i)
            {
                uint16_t value;
                bytesOfRead = ret && readShorts(stream, parent, &value);
                submesh.indices[i] = value;
            }
        }

        while (ret && parent.read < parent.header.length && !stream.eof())
        {
            OgreChunkData data;
            ret = readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_GEOMETRY:
                {
                    ret = ret && readGeometry(stream, data, submesh.geometry);
                }
                break;
            case OGRE_SUBMESH_OPERATION:
                {
                    ret = ret && readSubMeshOperation(stream, data, submesh);
                }
                break;
            case OGRE_SUBMESH_TEXTURE_ALIAS:
                {
                    ret = ret && readSubMeshTextureAlias(stream, data, submesh);
                }
                break;
            case OGRE_SUBMESH_BONE_ASSIGNMENT:
                {
                    submesh.boneAssignments.push_back(OgreBoneAssignment());
                    ret = ret && readBoneAssignment(stream, data, submesh.boneAssignments.back());
                }
                break;
            default:
                {
                    // 不是自己负责解析的chunk，返回跳出，交给外面来解析处理
                    long_t read = data.read;
                    stream.seek(-read, true);
                    return true;
                }
                break;
            }

            T3D_ASSERT(data.read == data.header.length);

            parent.read += data.read;
        }

        T3D_ASSERT(parent.read = parent.header.length);

        return ret;
    }

    bool OgreSerializer::readSubMeshOperation(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh)
    {
        size_t bytesOfRead = readShorts(stream, parent, &submesh.operation);
        return (bytesOfRead == sizeof(submesh.operation));
    }

    bool OgreSerializer::readSubMeshTextureAlias(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh)
    {
        String aliasName = readString(stream, parent);
        String textureName = readString(stream, parent);
        submesh.textureAliases.push_back(OgreTextureAlias(textureName, aliasName));
        return true;
    }

    bool OgreSerializer::readBoneAssignment(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreBoneAssignment &assignment)
    {
        bool ret = true;
        size_t bytesOfRead = readInts(stream, parent, &assignment.vertexID);
        ret = ret && (bytesOfRead == sizeof(assignment.vertexID));
        bytesOfRead = readShorts(stream, parent, &assignment.boneID);
        ret = ret && (bytesOfRead == sizeof(assignment.boneID));
        bytesOfRead = readFloats(stream, parent, &assignment.weight);
        ret = ret && (bytesOfRead == sizeof(assignment.weight));
        return ret;
    }

    bool OgreSerializer::readMeshBound(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh)
    {
        bool ret = true;
        size_t bytesOfRead = readObject(stream, parent, mesh.minEdge);
        ret = ret && (bytesOfRead == sizeof(mesh.minEdge));
        bytesOfRead = readObject(stream, parent, mesh.maxEdge);
        ret = ret && (bytesOfRead == sizeof(mesh.maxEdge));
        bytesOfRead = readFloats(stream, parent, &mesh.radius);
        ret = ret && (bytesOfRead == sizeof(mesh.radius));
        return ret;
    }

    bool OgreSerializer::readSubMeshNameTable(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh)
    {
        std::map<uint16_t, String> submeshNames;

        bool ret = true;

        while (ret && parent.read < parent.header.length && !stream.eof())
        {
            OgreChunkData data;
            ret = readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_SUBMESH_NAME_TABLE_ELEMENT:
                {
                    uint16_t submeshIndex;
                    size_t bytesOfRead = readShorts(stream, data, &submeshIndex);
                    ret = ret && (bytesOfRead == sizeof(submeshIndex));
                    String submeshName = readString(stream, data);
                    submeshNames[submeshIndex] = submeshName;
                }
                break;
            default:
                {
                    // 不是自己负责解析的chunk，返回跳出，交给外面来解析处理
                    long_t read = data.read;
                    stream.seek(-read, true);
                    return true;
                }
                break;
            }

            T3D_ASSERT(data.read == data.header.length);

            parent.read += data.read;
        }

        return ret;
    }

    bool OgreSerializer::readSkeleton(const String &name, OgreSkeleton &skeleton)
    {
        bool ret = false;

        Tiny3D::FileDataStream fs;

        String path;
        size_t pos = mSrcPath.rfind("\\");
        if (pos == String::npos)
        {
            pos = mSrcPath.rfind("/");
        }

        if (pos != String::npos)
        {
            path = mSrcPath.substr(0, pos);
            path += "\\" + name;
        }

        if (fs.open(path.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            size_t bytesOfRead = 0;
            uint16_t id;

            // 读取是否大小端交换标识
            bytesOfRead = fs.read(&id, sizeof(id));

            if (id == OGRE_HEADER)
            {
                mSwapEndian = false;
                ret = true;
            }
            else if (id == OGRE_OTHER_HEADER)
            {
                mSwapEndian = true;
                ret = true;
            }
            else
            {
                ret = false;
            }

            if (ret)
            {
                // 读取版本号
                OgreChunkData data;
                String version = readString(fs, data);

                if (version != OGRE_SERIALIZER_VERSION_110)
                {
                    ret = false;
                }

                ret = ret && readSkeletonChunk(fs, skeleton);
            }

            fs.close();
        }

        return ret;
    }

    bool OgreSerializer::readSkeletonChunk(Tiny3D::DataStream &stream, OgreSkeleton &skeleton)
    {
        bool ret = true;

        while (!stream.eof())
        {
            OgreChunkData data;
            readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_SKELETON_BONE:
                {
                    ret = ret && readSkeletonBone(stream, data, skeleton);
                }
                break;
            case OGRE_SKELETON_BONE_PARENT:
                {
                    ret = ret && readSkeletonBoneParent(stream, data, skeleton);
                }
                break;
            case OGRE_SKELETON_ANIMATION:
                {
                    ret = ret && readSkeletonAnimation(stream, data, skeleton);
                }
                break;
            case OGRE_SKELETON_ANIMATION_LINK:
                {
                    ret = ret && readSkeletonAnimationLink(stream, data, skeleton);
                }
                break;
            default:
                {
//                     T3D_ASSERT(data.header.id != 0);
//                     size_t offset = data.header.length - data.read;
//                     stream.seek(offset, true);
//                     data.read += offset;
                }
                break;
            }

//             T3D_ASSERT(data.read == data.header.length);
        }

        return ret;
    }

    bool OgreSerializer::readSkeletonBone(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSkeleton &skeleton)
    {
        bool ret = true;

        skeleton.bones.push_back(OgreBone());
        OgreBone &bone = skeleton.bones.back();
        bone.name = readString(stream, parent);
        size_t bytesOfRead = readShorts(stream, parent, &bone.handle);
        ret = ret && (bytesOfRead == sizeof(bone.handle));
        bytesOfRead = readObject(stream, parent, bone.position);
        ret = ret && (bytesOfRead == sizeof(bone.position));
        bytesOfRead = readObject(stream, parent, bone.orientation);

        if (parent.read < (parent.header.length - bone.name.size()))
        {
            bytesOfRead = readObject(stream, parent, bone.scale);
            ret = ret && (bytesOfRead == sizeof(bone.scale));
        }
        else
        {
            bone.scale = Vector3(1, 1, 1);
        }

        bone.parent = 0xffff;

        return ret;
    }

    bool OgreSerializer::readSkeletonBoneParent(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSkeleton &skeleton)
    {
        bool ret = true;

        uint16_t childHandle;
        uint16_t parentHandle;

        size_t bytesOfRead = readShorts(stream, parent, &childHandle);
        ret = ret && (bytesOfRead == sizeof(childHandle));
        bytesOfRead = readShorts(stream, parent, &parentHandle);

        if (childHandle < skeleton.bones.size() && parentHandle < skeleton.bones.size())
        {
            skeleton.bones[childHandle].parent = parentHandle;
        }

        T3D_ASSERT(parent.read == parent.header.length);

        return ret;
    }

    bool OgreSerializer::readSkeletonAnimation(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSkeleton &skeleton)
    {
        bool ret = true;

        skeleton.animations.push_back(OgreAnimation());
        OgreAnimation &animation = skeleton.animations.back();
        animation.name = readString(stream, parent);
        size_t bytesOfRead = readFloats(stream, parent, &animation.length);
        ret = ret && (bytesOfRead == sizeof(animation.length));

        while (ret && parent.read < parent.header.length && !stream.eof())
        {
            OgreChunkData data;
            ret = readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_SKELETON_ANIMATION_TRACK:
                {
                    ret = ret && readAnimationTrack(stream, data, animation);
                }
                break;
            default:
                {
                    T3D_ASSERT(data.header.id != 0);
                    size_t offset = data.header.length - data.read;
                    stream.seek(offset, true);
                    data.read += offset;
                }
                break;
            }

//             T3D_ASSERT(data.read == data.header.length);

            parent.read += data.read;
        }

//         T3D_ASSERT(parent.read == parent.header.length);

        return ret;
    }

    bool OgreSerializer::readAnimationTrack(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreAnimation &animation)
    {
        bool ret = true;

        uint16_t boneHandle;
        size_t bytesOfRead = readShorts(stream, parent, &boneHandle);
        ret = ret && (bytesOfRead == sizeof(boneHandle));

        while (ret && parent.read < parent.header.length && !stream.eof())
        {
            OgreChunkData data;
            ret = readChunkData(stream, data);

            switch (data.header.id)
            {
            case OGRE_SKELETON_ANIMATION_TRACK_KEYFRAME:
                {
                    ret = ret && readAnimationKeyframe(stream, data, animation, boneHandle);
                }
                break;
            case OGRE_SKELETON_ANIMATION_TRACK_KEYFRAME_LIST:
                {
                    ret = ret && readAnimationKeyframeList(stream, data, animation, boneHandle);
                }
                break;
            default:
                {
                    T3D_ASSERT(data.header.id != 0);
                    size_t offset = data.header.length - data.read;
                    stream.seek(offset, true);
                    data.read += offset;
                }
                break;
            }

//             T3D_ASSERT(data.read == data.header.length);
            parent.read += data.read;
        }

//         T3D_ASSERT(parent.read == parent.header.length);

        return ret;
    }

    bool OgreSerializer::readAnimationKeyframe(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreAnimation &animation, uint16_t bone)
    {
        bool ret = true;

        animation.keyframes.push_back(OgreKeyframe());
        OgreKeyframe &keyframe = animation.keyframes.back();
        keyframe.boneID = bone;
        size_t bytesOfRead = readFloats(stream, parent, &keyframe.time);
        ret = ret && (bytesOfRead == sizeof(keyframe.time));
        bytesOfRead = readObject(stream, parent, keyframe.orientation);
        ret = ret && (bytesOfRead == sizeof(keyframe.orientation));
        bytesOfRead = readObject(stream, parent, keyframe.position);
        ret = ret && (bytesOfRead == sizeof(keyframe.position));

        if (parent.read < parent.header.length)
        {
            bytesOfRead = readObject(stream, parent, keyframe.scale);
            ret = ret && (bytesOfRead == sizeof(keyframe.scale));
        }
        else
        {
            keyframe.scale = Vector3(1, 1, 1);
        }

        keyframe.boneID = bone;

        return ret;
    }

    bool OgreSerializer::readAnimationKeyframeList(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreAnimation &animation, uint16_t bone)
    {
        bool ret = true;

        uint16_t length;
        size_t bytesOfRead = readShorts(stream, parent, &length);
        ret = ret && (bytesOfRead == sizeof(length));

        uint16_t flag;
        bytesOfRead = readShorts(stream, parent, &flag);
        ret = ret && (bytesOfRead == sizeof(flag));

        float time;

        size_t i = 0;
        for (i = 0; i < length; ++i)
        {
            animation.keyframes.push_back(OgreKeyframe());
            OgreKeyframe &keyframe = animation.keyframes.back();
            keyframe.boneID = bone;

            bytesOfRead = readFloats(stream, parent, &keyframe.time);
            ret = ret && (bytesOfRead == sizeof(keyframe.time));

            if (flag & KF_ROTATION)
            {
                bytesOfRead = readObject(stream, parent, keyframe.orientation);
                ret = ret && (bytesOfRead == sizeof(keyframe.orientation));
            }
            else
            {
                keyframe.orientation = Quaternion::IDENTITY;
            }

            if (flag & KF_TRANSLATE)
            {
                bytesOfRead = readObject(stream, parent, keyframe.position);
                ret = ret && (bytesOfRead == sizeof(keyframe.position));
            }
            else
            {
                keyframe.position = Vector3::ZERO;
            }

            if (flag & KF_SCALE)
            {
                bytesOfRead = readObject(stream, parent, keyframe.scale);
                ret = ret && (bytesOfRead == sizeof(keyframe.scale));
            }
            else
            {
                keyframe.scale = Vector3::UNIT_SCALE;
            }
        }

        return ret;
    }

    bool OgreSerializer::readSkeletonAnimationLink(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSkeleton &skeleton)
    {
        bool ret = true;

        String skelName = readString(stream, parent);
        float scale;
        size_t bytesOfRead = readFloats(stream, parent, &scale);
        ret = ret && (bytesOfRead == sizeof(scale));

        T3D_ASSERT(parent.read == parent.header.length);

        return ret;
    }

    size_t OgreSerializer::readBools(Tiny3D::DataStream &stream, OgreChunkData &data, bool *value, size_t count /* = 1 */)
    {
        size_t ret = stream.read(value, sizeof(bool)*count);
        data.read += ret;
        return ret;
    }

    size_t OgreSerializer::readBytes(Tiny3D::DataStream &stream, OgreChunkData &data, uint8_t *value, size_t count /* = 1 */)
    {
        size_t ret = stream.read(value, sizeof(uint8_t)*count);
        data.read += ret;
        return ret;
    }

    size_t OgreSerializer::readShorts(Tiny3D::DataStream &stream, OgreChunkData &data, uint16_t *value, size_t count /* = 1 */)
    {
        size_t ret = stream.read(value, sizeof(uint16_t)*count);
        if (mSwapEndian)
        {
            swapEndian(value, sizeof(uint16_t), count);
        }
        data.read += ret;
        return ret;
    }

    size_t OgreSerializer::readInts(Tiny3D::DataStream &stream, OgreChunkData &data, uint32_t *value, size_t count /* = 1 */)
    {
        size_t ret = stream.read(value, sizeof(uint32_t)*count);
        if (mSwapEndian)
        {
            swapEndian(value, sizeof(uint32_t), count);
        }
        data.read += ret;
        return ret;
    }

    size_t OgreSerializer::readFloats(Tiny3D::DataStream &stream, OgreChunkData &data, float *value, size_t count /* = 1 */)
    {
        size_t ret = stream.read(value, sizeof(float)*count);
        if (mSwapEndian)
        {
            swapEndian(value, sizeof(float), count);
        }
        data.read += ret;
        return ret;
    }

    size_t OgreSerializer::readFloats(Tiny3D::DataStream &stream, OgreChunkData &data, double *value, size_t count /* = 1 */)
    {
        float *tmp = new float[count];
        size_t ret = stream.read(tmp, sizeof(float)*count);
        float *ptmp = tmp;

        if (mSwapEndian)
        {
            swapEndian(tmp, sizeof(float), count);
        }

        while (count--)
        {
            *value++ = *ptmp++;
        }

        data.read += ret;
        delete []tmp;
        return ret;
    }

    size_t OgreSerializer::readObject(Tiny3D::DataStream &stream, OgreChunkData &data, Tiny3D::Vector3 &value)
    {
        float tmp[3];
        size_t ret = readFloats(stream, data, tmp, 3);
        value[0] = tmp[0];
        value[1] = tmp[1];
        value[2] = tmp[2];
        return ret;
    }

    size_t OgreSerializer::readObject(Tiny3D::DataStream &stream, OgreChunkData &data, Tiny3D::Quaternion &value)
    {
        float tmp[4];
        size_t ret = readFloats(stream, data, tmp, 4);
        value.x() = tmp[0];
        value.y() = tmp[1];
        value.z() = tmp[2];
        value.w() = tmp[3];
        return ret;
    }

    String OgreSerializer::readString(Tiny3D::DataStream &stream, OgreChunkData &data)
    {
        String s;
        char c;
        size_t numberOfRead = 0;

        do
        {
            size_t bytes = stream.read(&c, sizeof(c));
            if (c != '\n')
                s.append(1, c);
            numberOfRead += bytes;
        }
        while (!stream.eof() && c != '\n');

        data.read += numberOfRead;
        return s;
    }

    String OgreSerializer::readString(Tiny3D::DataStream &stream, OgreChunkData &data, size_t numChars)
    {
        String s;
        char c;
        size_t numberOfRead = 0;
        size_t i = 0;
        s.reserve(numChars+1);

        do
        {
            size_t bytes = stream.read(&c, sizeof(c));
            if (c != '\n')
                s.append(1, c);
            numberOfRead += bytes;
            ++i;
        }
        while (!stream.eof() && c != '\n' && i < numChars);

        data.read += numberOfRead;
        return s;
    }

    void OgreSerializer::swapEndian(void *data, size_t size, size_t count)
    {
        size_t i = 0;
        for (i = 0; i < count; ++i)
        {
            swapEndian((void *)((long)data + (i * size)), size);
        }
    }

    void OgreSerializer::swapEndian(void *data, size_t size)
    {
        char swapByte;
        size_t i = 0;
        for (i = 0; i < size / 2; ++i)
        {
            swapByte = *(char *)((long)data + i);
            *(char *)((long)data + i) = *(char *)((long)data + size - i - 1);
            *(char *)((long)data + size - i - 1) = swapByte;
        }
    }


    ////////////////////////////////////////////////////////////////////////////

    OgreMaterialSerializer::OgreMaterialSerializer()
    {

    }

    OgreMaterialSerializer::~OgreMaterialSerializer()
    {

    }

    bool OgreMaterialSerializer::load(const String &path, void *&pData)
    {
        return true;
    }

    bool OgreMaterialSerializer::save(const String &path, void *pData)
    {
        return true;
    }
}
