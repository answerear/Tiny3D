
#include "mconv_ogreserializer.h"


using namespace Tiny3D;


namespace mconv
{
    #define OGRE_MESH_VERSION_130       "[MeshSerializer_v1.30]"
    #define OGRE_MESH_VERSION_140       "[MeshSerializer_v1.40]"
    #define OGRE_MESH_VERSION_141       "[MeshSerializer_v1.41]"


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
            size_t bytesOfRead = 0;
            uint16_t id;

            bool ret = true;

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

        if (ret)
        {
            data.read += sizeof(OgreChunkHeader);
        }

        return ret;
    }

    bool OgreSerializer::readMesh(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreMesh &mesh)
    {
        bool ret = true;
        size_t bytesOfRead = readBools(stream, parent, &mesh.hasSkeleton);

        ret = (bytesOfRead == sizeof(mesh.hasSkeleton));

        while (ret && parent.read < parent.header.length && !stream.eof())
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

                }
                break;
            case OGRE_MESH_BOUNDS:
                break;
            case OGRE_MESH_BONE_ASSIGNMENT:
                break;
            case OGRE_MESH_LOD:
                break;
            case OGRE_SUBMESH_NAME_TABLE:
                break;
            case OGRE_EDGE_LISTS:
                break;
            case OGRE_POSES:
                break;
            case OGRE_ANIMATIONS:
                break;
            case OGRE_TABLE_EXTREMES:
                break;
            default:
                {
                    // 跳过不需要解析的chunk
                    stream.seek(data.header.length - data.read);
                    data.read += (data.header.length - data.read);
                }
                break;
            }
        }

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
                    stream.seek(data.header.length - data.read);
                    data.read += (data.header.length - data.read);
                }
                break;
            }
        }

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
                    stream.seek(data.header.length - data.read);
                    data.read += (data.header.length - data.read);
                }
                break;
            }
        }

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

        OgreChunkData data;
        ret = ret && readChunkData(stream, data);

        if (data.header.id == OGRE_GEOMETRY_VERTEX_BUFFER_DATA)
        {
            vertexBuffer.vertices.resize(vertexBuffer.vertexSize * geometry.vertexCount);
            ret = ret && readFloats(stream, data, &(*(vertexBuffer.vertices.begin())), geometry.vertexCount * vertexBuffer.vertexSize);
        }

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
                break;
            default:
                {
                    // 跳过不需要解析的chunk
                    stream.seek(data.header.length - data.read);
                    data.read += (data.header.length - data.read);
                }
                break;
            }
        }

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

    bool OgreSerializer::readSubMeshBoneAssignment(Tiny3D::DataStream &stream, OgreChunkData &parent, OgreSubMesh &submesh)
    {
        bool ret = true;
        uint32_t vertexID;
        size_t bytesOfRead = readInts(stream, parent, &vertexID);
        ret = ret && (bytesOfRead == sizeof(vertexID));
        uint16_t boneID;
        bytesOfRead = readShorts(stream, parent, &boneID);
        ret = ret && (bytesOfRead == sizeof(boneID));
        float weight;
        bytesOfRead = readFloats(stream, parent, &weight);
        ret = ret && (bytesOfRead == sizeof(weight));

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
        data.read += ret;
        return ret;
    }

    size_t OgreSerializer::readObject(Tiny3D::DataStream &stream, OgreChunkData &data, Tiny3D::Quaternion &value)
    {
        float tmp[4];
        size_t ret = readFloats(stream, data, tmp, 4);
        value[0] = tmp[0];
        value[1] = tmp[1];
        value[2] = tmp[2];
        value[3] = tmp[3];
        data.read += ret;
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
}
