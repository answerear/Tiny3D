

#include "mconv_ogreconverter.h"
#include "mconv_command.h"
#include "mconv_settings.h"
#include "mconv_ogreserializer.h"
#include "mconv_t3dSerializer.h"
#include "mconv_node.h"
#include "mconv_scene.h"
#include "mconv_mesh.h"
#include "mconv_model.h"
#include "mconv_material.h"
#include "mconv_animation.h"
#include "mconv_skeleton.h"
#include "mconv_bone.h"
#include "mconv_skin.h"
#include "mconv_texture.h"
#include "mconv_bound.h"
#include "mconv_vertexbuffer.h"


namespace mconv
{
    OgreConverter::OgreConverter(const Settings &settings)
        : ConverterImpl(settings)
    {

    }

    OgreConverter::~OgreConverter()
    {

    }

    bool OgreConverter::importScene()
    {
        bool result = true;

        delete mImporter;
        mImporter = nullptr;

        if (mSettings.mSrcType & E_FILETYPE_OGRE)
        {
            mImporter = new OgreSerializer();
            result = (mImporter != nullptr);
        }
        else
        {
            T3D_LOG_ERROR("Create importer failed ! Because of invalid source file format !");
            result = false;
        }

        result = result && mImporter->load(mSettings.mSrcPath, mSrcData);

        return result;
    }

    bool OgreConverter::exportScene()
    {
        bool result = false;

        delete mExporter;
        mExporter = nullptr;

        if ((mSettings.mDstType & E_FILETYPE_T3D) == E_FILETYPE_T3D)
        {
            mExporter = new T3DSerializer();
            result = (mExporter != nullptr);
        }
        else if (mSettings.mDstType & E_FILETYPE_TMB)
        {
            mExporter = new T3DBinSerializer();
            result = (mExporter != nullptr);
        }
        else if (mSettings.mDstType & E_FILETYPE_TMT)
        {
            mExporter = new T3DXMLSerializer();
            result = (mExporter != nullptr);
        }
        else
        {
            T3D_LOG_ERROR("Create exporter failed ! Because of invalid destination file format !");
            T3D_ASSERT(0);
            result = false;
        }

        result = result && mExporter->save(mSettings.mDstPath, mDstData);

        return result;
    }

    bool OgreConverter::convertToT3D()
    {
        if (mSrcData == nullptr)
        {
            T3D_LOG_ERROR("Convert to T3D failed ! Because of invalid source data !");
            return false;
        }

        bool result = true;

        OgreMesh *pOgreMesh = (OgreMesh *)mSrcData;

        String name = "Scene";

        Scene *pScene = new Scene(name);
        
        result = processOgreMesh(*pOgreMesh, pScene);

        mDstData = pScene;

        return result;
    }

    void OgreConverter::cleanup()
    {
        OgreMesh *pOgreMesh = (OgreMesh *)mSrcData;

        delete pOgreMesh;
        mSrcData = nullptr;
    }

    bool OgreConverter::processOgreMesh(const OgreMesh &mesh, Node *pRoot)
    {
        bool result = false;

        String name = "Model";

        Model *pModel = new Model(name);

        pRoot->addChild(pModel);

        result = processOgreSubMeshes(mesh, pModel);

        if (mesh.hasSkeleton)
        {
            result = result && processOgreSkeleton(mesh.skeleton, pModel);
        }

        return result;
    }

    bool OgreConverter::processOgreGeometry(const OgreGeometry &geometry, Mesh *pMesh)
    {
        bool result = false;

        VertexBuffers *pVBS = new VertexBuffers(pMesh->getID());
        pMesh->addChild(pVBS);

        result = (geometry.buffers.size() > 0);
        size_t i = 0;
        auto itr = geometry.buffers.begin();

        while (itr != geometry.buffers.end())
        {
            std::stringstream ss;
            ss<<itr->bindIndex;
            VertexBuffer *pVB = new VertexBuffer(ss.str());
            pVBS->addChild(pVB);

            result = result && processOgreVertexAttributes(geometry, pVB, itr->bindIndex);
            result = result && processOgreVertexBuffer(*itr, pVB, itr->bindIndex);
            ++itr;
        }

        return result;
    }

    bool OgreConverter::processOgreVertexAttributes(const OgreGeometry &geometry, VertexBuffer *pVertexBuffer, size_t source)
    {
        bool result = (geometry.elements.size() > 0);

        auto itr = geometry.elements.begin();

        while (itr != geometry.elements.end())
        {
            const OgreVertexElement &element = *itr;
            if (element.source == source)
            {
                result = result && putVertexAttribute(element, pVertexBuffer);
            }

            ++itr;
        }

        return result;
    }

    bool OgreConverter::putVertexAttribute(const OgreVertexElement &element, VertexBuffer *pVertexBuffer)
    {
        bool result;

        VertexAttribute attribute;
        result = processVertexSemantic(element.semantic, attribute);
        result = result && processVertexType(element.type, attribute);

        if (result)
        {
            pVertexBuffer->mAttributes.push_back(attribute);
        }

        return result;
    }

    bool OgreConverter::processVertexSemantic(uint16_t semantic, VertexAttribute &attribute)
    {
        bool result = true;

        switch (semantic)
        {
        case VES_POSITION:
            {
                attribute.mVertexType = VertexAttribute::E_VT_POSITION;
            }
            break;
        case VES_BLEND_WEIGHTS:
            {
                attribute.mVertexType = VertexAttribute::E_VT_BLEND_WEIGHT;
            }
            break;
        case VES_BLEND_INDICES:
            {
                attribute.mVertexType = VertexAttribute::E_VT_BLEND_INDEX;
            }
            break;
        case VES_NORMAL:
            {
                attribute.mVertexType = VertexAttribute::E_VT_NORMAL;
            }
            break;
        case VES_DIFFUSE:
            {
                attribute.mVertexType = VertexAttribute::E_VT_COLOR;
            }
            break;
        case VES_SPECULAR:
            {
                attribute.mVertexType = VertexAttribute::E_VT_COLOR;
            }
            break;
        case VES_TEXTURE_COORDINATES:
            {
                attribute.mVertexType = VertexAttribute::E_VT_TEXCOORD;
            }
            break;
        case VES_BINORMAL:
            {
                attribute.mVertexType = VertexAttribute::E_VT_BINORMAL;
            }
            break;
        case VES_TANGENT:
            {
                attribute.mVertexType = VertexAttribute::E_VT_TANGENT;
            }
            break;
        default:
            {
                result = false;
            }
            break;
        }

        return result;
    }

    bool OgreConverter::processVertexType(uint16_t type, VertexAttribute &attribute)
    {
        bool result = true;

        switch (type)
        {
        case VET_FLOAT1:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 1;
            }
            break;
        case VET_FLOAT2:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 2;
            }
            break;
        case VET_FLOAT3:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 3;
            }
            break;
        case VET_FLOAT4:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 4;
            }
            break;
        case VET_COLOR:
            {
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                attribute.mSize = 4;
            }
            break;
        case VET_SHORT1:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 1;
            }
            break;
        case VET_SHORT2:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 2;
            }
            break;
        case VET_SHORT3:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 3;
            }
            break;
        case VET_SHORT4:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 4;
            }
            break;
        case VET_UBYTE4:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT8;
                attribute.mSize = 4;
            }
            break;
        case VET_COLOR_ARGB:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT16;
                attribute.mSize = 4;
            }
            break;
        case VET_COLOR_ABGR:
            {
                attribute.mDataType = VertexAttribute::E_VT_INT32;
                attribute.mSize = 4;
            }
            break;
        default:
            {
                result = false;
            }
            break;
        }

        return result;
    }

    bool OgreConverter::processOgreVertexBuffer(const OgreVertexBuffer &buffer, VertexBuffer *pVertexBuffer, size_t source)
    {
        bool result = (buffer.vertices.size() > 0);

        size_t i = 0;

        while (i < buffer.vertices.size())
        {
            pVertexBuffer->mVertices.push_back(Vertex());
            Vertex &vertex = pVertexBuffer->mVertices.back();
            result = result && putVertexData(buffer.vertices, i, pVertexBuffer->mAttributes, vertex);
        }

        return result;
    }

    bool OgreConverter::putVertexData(const std::vector<float> &vertices, size_t &index, const VertexAttributes &attributes, Vertex &vertex)
    {
        bool result = (attributes.size() > 0);

//         while (index < vertices.size())
        {
            auto itr = attributes.begin();

            while (itr != attributes.end())
            {
                const VertexAttribute &attr = *itr;

                switch (attr.mVertexType)
                {
                case VertexAttribute::E_VT_POSITION:
                    {
                        vertex.mPosition[0] = vertices[index++];
                        vertex.mPosition[1] = vertices[index++];
                        vertex.mPosition[2] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_TEXCOORD:
                    {
                        vertex.mTexElements.push_back(Vector2());
                        Vector2 &texcoord = vertex.mTexElements.back();
                        texcoord[0] = vertices[index++];
                        texcoord[1] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_NORMAL:
                    {
                        vertex.mNormalElements.push_back(Vector3());
                        Vector3 &normal = vertex.mNormalElements.back();
                        normal[0] = vertices[index++];
                        normal[1] = vertices[index++];
                        normal[2] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_TANGENT:
                    {
                        vertex.mTangentElements.push_back(Vector3());
                        Vector3 &tangent = vertex.mTangentElements.back();
                        tangent[0] = vertices[index++];
                        tangent[1] = vertices[index++];
                        tangent[2] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_BINORMAL:
                    {
                        vertex.mBinormalElements.push_back(Vector3());
                        Vector3 &binormal = vertex.mBinormalElements.back();
                        binormal[0] = vertices[index++];
                        binormal[1] = vertices[index++];
                        binormal[2] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_COLOR:
                    {
                        vertex.mColorElements.push_back(Vector4());
                        Vector4 &color = vertex.mColorElements.back();
                        color[0] = vertices[index++];
                        color[1] = vertices[index++];
                        color[2] = vertices[index++];
                        color[4] = vertices[index++];
                    }
                    break;
                case VertexAttribute::E_VT_BLEND_WEIGHT:
                    {
                        T3D_ASSERT(0);
                    }
                    break;
                case VertexAttribute::E_VT_BLEND_INDEX:
                    {
                        T3D_ASSERT(0);
                    }
                    break;
                default:
                    {
                        result = false;
                    }
                    break;
                }

                ++itr;
            }
        }

        return result;
    }

    bool OgreConverter::createMesh(Model *pModel, Mesh *&pMesh, SubMeshes *&pSubMeshes, size_t index)
    {
        char szName[64];
        snprintf(szName, sizeof(szName)-1, "Mesh#% 3d", index);
        String strName = szName;
        pMesh = new Mesh(strName);
        bool result = (pMesh != nullptr);
        if (result)
        {
            pModel->addChild(pMesh);
        }

        pSubMeshes = new SubMeshes("SubMeshes");
        result = (pSubMeshes != nullptr);
        if (result)
        {
            pMesh->addChild(pSubMeshes);
        }

        return result;
    }

    bool OgreConverter::processOgreSubMeshes(const OgreMesh &mesh, Model *pModel)
    {
        bool result = (mesh.submeshes.size() > 0);

        Mesh *pGlobalMesh = nullptr;
        SubMeshes *pGlobalSubMeshes = nullptr;

        bool bHasSharedVertices = false;

        size_t i = 0;
        auto itr = mesh.submeshes.begin();

        while (itr != mesh.submeshes.end() && result)
        {
            auto submesh = *itr;

            if (submesh.hasSharedVertices)
            {
                if (0 == i)
                {
                    bHasSharedVertices = submesh.hasSharedVertices;

                    // 第一个submesh，用于存放共享的顶点数据
                    result = createMesh(pModel, pGlobalMesh, pGlobalSubMeshes, i);
                    result = result && processOgreGeometry(mesh.geometry, pGlobalMesh);
                }
                else
                {
                    T3D_ASSERT(bHasSharedVertices == submesh.hasSharedVertices);
                }

                result = result && processOgreSubMesh(submesh, pGlobalMesh, pGlobalSubMeshes);
            }
            else
            {
                // submesh自己独享顶点数据，需要创建一个mesh出来
                if (i == 0)
                {
                    bHasSharedVertices = submesh.hasSharedVertices;
                }
                else
                {
                    T3D_ASSERT(bHasSharedVertices == submesh.hasSharedVertices);
                }
                
                Mesh *pMesh = nullptr;
                SubMeshes *pSubMeshes = nullptr;

                result = createMesh(pModel, pMesh, pSubMeshes, i);
                result = result && processOgreGeometry(submesh.geometry, pMesh);
                result = result && processOgreSubMesh(submesh, pMesh, pSubMeshes);
            }

            ++i;
            ++itr;
        }

        return result;
    }

    bool OgreConverter::processOgreSubMesh(const OgreSubMesh &submesh, Mesh *pMesh, SubMeshes *pSubMeshes)
    {
        bool result = (submesh.indices.size() > 0);

        SubMesh *pSubMesh = new SubMesh("SubMesh");
        pSubMeshes->addChild(pSubMesh);

        pSubMesh->mMaterialName = submesh.materialName;

        pSubMesh->mIndices.resize(submesh.indices.size());

        auto itr = pSubMesh->mIndices.begin();
        size_t i = 0;

        for (i = 0; i < submesh.indices.size() && itr != pSubMesh->mIndices.end(); ++i)
        {
            *itr = submesh.indices[i];
            ++itr;
        }

        return result;
    }

    bool OgreConverter::processOgreSkeleton(const OgreSkeleton &skeleton, Model *pModel)
    {
        bool result = processOgreSkin(skeleton, pModel);
        result = result && processOgreBones(skeleton, pModel);
        result = result && processOgreAnimations(skeleton, pModel);
        return result;
    }

    bool OgreConverter::processOgreSkin(const OgreSkeleton &skeleton, Model *pModel)
    {
        bool result = (skeleton.bones.size() > 0);



        return result;
    }

    bool OgreConverter::processOgreBones(const OgreSkeleton &skeleton, Model *pModel)
    {
        bool result = (skeleton.bones.size() > 0);

        if (result)
        {
            Skeleton *pSkel = new Skeleton("skeleton");
            pModel->addChild(pSkel);

            // 先分配所有骨骼结点对象
            std::vector<Bone*> bones(skeleton.bones.size(), nullptr);
            size_t i = 0;
            for (i = 0; i < bones.size(); ++i)
            {
                bones[i] = new Bone("unknown");
            }

            // 构建树形骨骼关系和计算变换矩阵
            auto itr = skeleton.bones.begin();

            while (itr != skeleton.bones.end())
            {
                const OgreBone &bone = *itr;
                Bone *pBone = bones[bone.handle];

                if (bone.parent == 0xFFFF)
                {
                    pBone->setID(bone.name);
                    pSkel->addChild(pBone);
                }
                else
                {
                    Bone *pParent = bones[bone.parent];
                    pBone->setID(bone.name);
                    T3D_ASSERT(pBone->getParent() == nullptr);
                    pParent->addChild(pBone);
                }

                // 计算变换矩阵
                Matrix4 R(bone.orientation);
                Matrix4 T(bone.position);
                Matrix4 S;
                S.setScale(bone.scale);
                pBone->mLocalTransform = T * R * S;
                ++itr;
            }
        }

        return result;
    }

    bool OgreConverter::processOgreAnimations(const OgreSkeleton &skeleton, Model *pModel)
    {
        bool result = true;

        return result;
    }
}
