

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
        
        result = processOgreMesh(pOgreMesh, pScene);

        return result;
    }

    void OgreConverter::cleanup()
    {
        OgreMesh *pOgreMesh = (OgreMesh *)mSrcData;

        delete pOgreMesh;
        mSrcData = nullptr;
    }

    bool OgreConverter::processOgreMesh(OgreMesh *pOgreMesh, Node *pRoot)
    {
        bool result = false;

        String name = "Model";

        Model *pModel = new Model(name);

        pRoot->addChild(pModel);

        result = processOgreSubMeshes(*pOgreMesh, pModel);

        return result;
    }

    bool OgreConverter::processOgreGeometry(const OgreGeometry &geometry, Mesh *pMesh)
    {
        bool result = processOgreVertexAttributes(geometry, pMesh);

        if (result)
        {
            auto itr = geometry.buffers.begin();

            while (itr != geometry.buffers.end())
            {
                result = result && processOgreVertexBuffer(*itr, pMesh);
                ++itr;
            }
        }

        return result;
    }

    bool OgreConverter::processOgreVertexAttributes(const OgreGeometry &geometry, Mesh *pMesh)
    {
        bool result = (geometry.elements.size() > 0);

        auto itr = geometry.elements.begin();

        while (itr != geometry.elements.end())
        {
            const OgreVertexElement &element = *itr;
            result = result && putVertexAttribute(element, pMesh);
            ++itr;
        }

        return result;
    }

    bool OgreConverter::putVertexAttribute(const OgreVertexElement &element, Mesh *pMesh)
    {
        bool result;

        VertexAttribute attribute;
        result = processVertexSemantic(element.semantic, attribute);
        result = result && processVertexType(element.type, attribute);

        if (result)
        {
            pMesh->mAttributes.push_back(attribute);
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

    bool OgreConverter::processOgreVertexBuffer(const OgreVertexBuffer &buffer, Mesh *pMesh)
    {
        bool result = (buffer.vertices.size() > 0);

        size_t i = 0;

        while (i < buffer.vertices.size())
        {
            pMesh->mVertices.push_back(Vertex());
            Vertex &vertex = pMesh->mVertices.back();
            result = result && putVertexData(buffer.vertices, i, pMesh->mAttributes, vertex);
        }

        return result;
    }

    bool OgreConverter::putVertexData(const std::vector<float> &vertices, size_t &index, const VertexAttributes &attributes, Vertex &vertex)
    {
        bool result = (attributes.size() > 0);

        auto itr = attributes.begin();

        while (itr != attributes.end() && index < vertices.size())
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

        return result;
    }

    bool OgreConverter::processOgreSubMeshes(const OgreMesh &mesh, Model *pModel)
    {
        bool result = false;

        Mesh *pGlobalMesh = nullptr;

        size_t i = 0;
        auto itr = mesh.submeshes.begin();

        while (itr != mesh.submeshes.end())
        {
            auto submesh = *itr;

            if (0 == i)
            {
                // 第一个submesh，不管什么情况，先生成一个mesh，用于存放共享的顶点数据
                char szName[64];
                snprintf(szName, sizeof(szName)-1, "Mesh#% 3d", i);
                String strName = szName;
                Mesh *pMesh = new Mesh(strName);
                pModel->addChild(pMesh);
                result = processOgreGeometry(mesh.geometry, pMesh);
                pGlobalMesh = pMesh;
            }

            if (submesh.hasSharedVertices)
            {
                SubMesh *pSubMesh = new SubMesh("SubMesh");
                pGlobalMesh->addChild(pSubMesh);
            }
            else
            {
                // submesh自己独享顶点数据，需要创建一个mesh出来

            }

            ++i;
            ++itr;
        }

        return result;
    }

    bool OgreConverter::processOgreSubMesh(const OgreSubMesh &submesh, Model *pModel)
    {
        return true;
    }
}
