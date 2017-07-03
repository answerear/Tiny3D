

#include "mconv_t3dserializer.h"

#include "mconv_scene.h"
#include "mconv_model.h"
#include "mconv_mesh.h"
#include "mconv_animation.h"
#include "mconv_submesh.h"
#include "mconv_skeleton.h"
#include "mconv_skin.h"
#include "mconv_bone.h"
#include "mconv_material.h"
#include "mconv_camera.h"
#include "mconv_light.h"
#include "mconv_texture.h"
#include "mconv_bound.h"
#include "mconv_transform.h"
#include "mconv_log.h"
#include <iomanip>


namespace mconv
{
    T3DBinSerializer::T3DBinSerializer()
    {

    }

    T3DBinSerializer::~T3DBinSerializer()
    {

    }

    bool T3DBinSerializer::load(const String &path, void *&pData)
    {
        return true;
    }

    bool T3DBinSerializer::save(const String &path, void *pData)
    {
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    const char * const T3DXMLSerializer::TAG_TINY3D = "TINY3D";
    const char * const T3DXMLSerializer::TAG_SCENE = "scene";
    const char * const T3DXMLSerializer::TAG_MODEL = "model";
    const char * const T3DXMLSerializer::TAG_MESH = "mesh";
    const char * const T3DXMLSerializer::TAG_VERTICES = "vertices";
    const char * const T3DXMLSerializer::TAG_BUFFER = "buffer";
    const char * const T3DXMLSerializer::TAG_ATTRIBUTES = "attributes";
    const char * const T3DXMLSerializer::TAG_ATTRIBUTE = "attribute";
    const char * const T3DXMLSerializer::TAG_DATA = "data";
    const char * const T3DXMLSerializer::TAG_SUBMESHES = "submeshes";
    const char * const T3DXMLSerializer::TAG_SUBMESH = "submesh";
    const char * const T3DXMLSerializer::TAG_INDICES = "indices";
    const char * const T3DXMLSerializer::TAG_MATERIALS = "materials";
    const char * const T3DXMLSerializer::TAG_MATERIAL = "material";
    const char * const T3DXMLSerializer::TAG_MODE = "mode";
    const char * const T3DXMLSerializer::TAG_AMBIENT = "ambient";
    const char * const T3DXMLSerializer::TAG_DIFFUSE = "diffuse";
    const char * const T3DXMLSerializer::TAG_SPECULAR = "specular";
    const char * const T3DXMLSerializer::TAG_EMISSIVE = "emissive";
    const char * const T3DXMLSerializer::TAG_SHININESS = "shininess";
    const char * const T3DXMLSerializer::TAG_TRANSPARENCY = "transparency";
    const char * const T3DXMLSerializer::TAG_REFLECTION = "reflection";
    const char * const T3DXMLSerializer::TAG_TEXTURES = "textures";
    const char * const T3DXMLSerializer::TAG_TEXTURE = "texture";
    const char * const T3DXMLSerializer::TAG_EFFECTS = "effects";
    const char * const T3DXMLSerializer::TAG_EFFECT = "effect";
    const char * const T3DXMLSerializer::TAG_SKIN = "skin";
    const char * const T3DXMLSerializer::TAG_SKIN_INFO = "skininfo";
    const char * const T3DXMLSerializer::TAG_SKELETON = "skeleton";
    const char * const T3DXMLSerializer::TAG_BONE = "bone";
    const char * const T3DXMLSerializer::TAG_TRANSFORM = "transform";
    const char * const T3DXMLSerializer::TAG_ANIMATION = "animation";
    const char * const T3DXMLSerializer::TAG_ACTION = "action";
    const char * const T3DXMLSerializer::TAG_KEYFRAME = "keyframe";
    const char * const T3DXMLSerializer::TAG_FRAME = "frame";
    const char * const T3DXMLSerializer::TAG_BOUND = "bound";
    const char * const T3DXMLSerializer::TAG_LIGHT = "light";
    const char * const T3DXMLSerializer::TAG_CAMERA = "camera";
    const char * const T3DXMLSerializer::TAG_HIERARCHY = "hierarchy";
    const char * const T3DXMLSerializer::TAG_LINK = "link";
    const char * const T3DXMLSerializer::TAG_TRANSLATION = "translation";
    const char * const T3DXMLSerializer::TAG_ORIENTATION = "orientation";
    const char * const T3DXMLSerializer::TAG_SCALE = "scale";
    const char * const T3DXMLSerializer::TAG_NODE = "node";

    const char * const T3DXMLSerializer::ATTRIB_VERSION = "version";
    const char * const T3DXMLSerializer::ATTRIB_MAGIC = "magic";
    const char * const T3DXMLSerializer::ATTRIB_ID = "id";
    const char * const T3DXMLSerializer::ATTRIB_COUNT = "count";
    const char * const T3DXMLSerializer::ATTRIB_SIZE = "size";
    const char * const T3DXMLSerializer::ATTRIB_TYPE = "type";
    const char * const T3DXMLSerializer::ATTRIB_PRIMITIVE = "primitive";
    const char * const T3DXMLSerializer::ATTRIB_MATERIAL = "material";
    const char * const T3DXMLSerializer::ATTRIB_16BITS = "is16bits";
    const char * const T3DXMLSerializer::ATTRIB_SPAN = "span";
    const char * const T3DXMLSerializer::ATTRIB_BONE = "bone";
    const char * const T3DXMLSerializer::ATTRIB_TIME = "time";
    const char * const T3DXMLSerializer::ATTRIB_WRAP_U = "wrap_u";
    const char * const T3DXMLSerializer::ATTRIB_WRAP_V = "wrap_v";
    const char * const T3DXMLSerializer::ATTRIB_SHARED = "shared_vertex";
    const char * const T3DXMLSerializer::ATTRIB_DURATION = "duration";
    const char * const T3DXMLSerializer::ATTRIB_HAS_GEOMETRY = "has_geometry";
    const char * const T3DXMLSerializer::ATTRIB_HAS_WORLD = "has_world";
    const char * const T3DXMLSerializer::ATTRIB_MESH = "mesh";
    const char * const T3DXMLSerializer::ATTRIB_SUBMESH = "submesh";
    const char * const T3DXMLSerializer::ATTRIB_INDEX = "index";

    T3DXMLSerializer::T3DXMLSerializer()
    {

    }

    T3DXMLSerializer::~T3DXMLSerializer()
    {

    }

    bool T3DXMLSerializer::load(const String &path, void *&pData)
    {
        return true;
    }

    bool T3DXMLSerializer::save(const String &path, void *pData)
    {
        MCONV_LOG_INFO("Start writing file : %s", path.c_str());
        using namespace tinyxml2;

        Scene *pScene = (Scene *)pData;
        if (pScene != nullptr)
        {
            mSavePath = path;

            XMLDocument *pDoc = new XMLDocument();
            XMLDeclaration *pDecl = pDoc->NewDeclaration();
            pDoc->LinkEndChild(pDecl);

            XMLElement *pTiny3DElement = pDoc->NewElement(TAG_TINY3D);
            pDoc->LinkEndChild(pTiny3DElement);
            pTiny3DElement->SetAttribute(ATTRIB_MAGIC, T3D_MODEL_FILE_MAGIC);
            pTiny3DElement->SetAttribute(ATTRIB_VERSION, T3D_MODEL_FILE_VER_CUR_STR);

//             XMLElement *pMagicElement = pDoc->NewElement(TAG_MAGIC);
//             pDoc->LinkEndChild(pMagicElement);
//             XMLText *pText = pDoc->NewText(T3D_MODEL_FILE_MAGIC);
//             pMagicElement->LinkEndChild(pText);
// 
//             XMLElement *pVersionElement = pDoc->NewElement(TAG_VERSION);
//             pDoc->LinkEndChild(pVersionElement);
//             pText = pDoc->NewText(T3D_MODEL_FILE_VER_CUR_STR);
//             pVersionElement->LinkEndChild(pText);

            XMLElement *pElement = pDoc->NewElement(TAG_SCENE);
            pElement->SetAttribute(ATTRIB_ID, pScene->getID().c_str());
            pTiny3DElement->LinkEndChild(pElement);

            mTabCount = 0;
            populateXMLNode(pDoc, pElement, pScene);

            pDoc->SaveFile(path.c_str());
            delete pDoc;
        }

        MCONV_LOG_INFO("Completed writing file !");

        return true;
    }

    XMLElement *T3DXMLSerializer::populateXMLNode(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pElement = nullptr;

        switch (pNode->getNodeType())
        {
        case Node::E_TYPE_ANIMATION:
            {
                pElement = buildXMLAnimation(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_ACTION:
            {
                pElement = buildXMLAction(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_BONE:
            {
                pElement = buildXMLBone(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_BOUND_AABB:
            {
                pElement = buildXMLAlignAxisBound(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_BOUND_SPHERE:
            {
                pElement = buildXMLSphereBound(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_CAMERA:
            {
                pElement = buildXMLCamera(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_HIERARCHY:
            {
                pElement = buildXMLHierarchy(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_LIGHT:
            {
                pElement = buildXMLLight(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_MATERIAL:
            {
                //                 pElement = buildXMLMaterial(pDoc, pParentElem, pNode);
                saveMaterial(pNode);
                pElement = pParentElem;
            }
            break;
        case Node::E_TYPE_MATERIALS:
            {
                //                 pElement = buildXMLMaterials(pDoc, pParentElem, pNode);
                pElement = pParentElem;
            }
            break;
        case Node::E_TYPE_MESH:
            {
                pElement = buildXMLMesh(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_MODEL:
            {
                pElement = buildXMLModel(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_SKIN:
            {
                pElement = buildXMLSkin(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_SKIN_INFO:
            {
                pElement = buildXMLSkinInfo(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_SKELETON:
            {
                pElement = buildXMLSkeleton(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_SUBMESH:
            {
                pElement = buildXMLSubMesh(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_SUBMESHES:
            {
                pElement = buildXMLSubMeshes(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_TEXTURE:
            {
                pElement = buildXMLTexture(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_TEXTURES:
            {
                pElement = buildXMLTextures(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_TRANSFORM:
            {
                pElement = buildXMLTransform(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_VERTEX_BUFFERS:
            {
                pElement = buildXMLVertexBuffers(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_VERTEX_BUFFER:
            {
                pElement = buildXMLVertexBuffer(pDoc, pParentElem, pNode);
            }
            break;
        default:
            {
                pElement = pParentElem;
            }
            break;
        }

        if (pNode->getNodeType() != Node::E_TYPE_MATERIAL)
        {
            size_t i = 0;
            for (i = 0; i < pNode->getChildrenCount(); ++i)
            {
                Node *pChild = pNode->getChild(i);
                populateXMLNode(pDoc, pElement, pChild);
            }
        }

        return pElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLModel(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pElement = pDoc->NewElement(TAG_MODEL);
        pElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        Model *pModel = (Model *)pNode;
        pElement->SetAttribute(ATTRIB_SHARED, pModel->mSharedVertex);
        pElement->SetAttribute(ATTRIB_COUNT, pModel->mMeshCount);
        pParentElem->LinkEndChild(pElement);
        return pElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLMesh(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        Mesh *pMesh = (Mesh *)pNode;
        XMLElement *pMeshElement = pDoc->NewElement(TAG_MESH);
        pParentElem->LinkEndChild(pMeshElement);

        pMeshElement->SetAttribute(ATTRIB_ID, pMesh->getID().c_str());

        //         bool bHasWorld = false;
        // 
        //         if (pMesh->mWorldMatrix == Matrix4::IDENTITY)
        //         {
        //             pMeshElement->SetAttribute(ATTRIB_HAS_WORLD, false);
        //         }
        //         else
        //         {
        //             pMeshElement->SetAttribute(ATTRIB_HAS_WORLD, true);
        //             bHasWorld = true;
        //         }
        // 
        //         bool bHasGeometry = false;
        // 
        //         if (pMesh->mGeometryMatrix == Matrix4::IDENTITY)
        //         {
        //             pMeshElement->SetAttribute(ATTRIB_HAS_GEOMETRY, false);
        //         }
        //         else
        //         {
        //             pMeshElement->SetAttribute(ATTRIB_HAS_GEOMETRY, true);
        //             bHasGeometry = true;
        //         }
        // 
        //         if (bHasWorld)
        //         {
        //             XMLElement *pTransformElement = pDoc->NewElement(TAG_TRANSFORM);
        //             pMeshElement->LinkEndChild(pTransformElement);
        //             pTransformElement->SetAttribute(ATTRIB_ID, "WORLD");
        // 
        //             std::stringstream ss;
        //             char szText[512] = { 0 };
        //             snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f ",
        //                 pMesh->mWorldMatrix[0][0], pMesh->mWorldMatrix[0][1], pMesh->mWorldMatrix[0][2], pMesh->mWorldMatrix[0][3],
        //                 pMesh->mWorldMatrix[1][0], pMesh->mWorldMatrix[1][1], pMesh->mWorldMatrix[1][2], pMesh->mWorldMatrix[1][3],
        //                 pMesh->mWorldMatrix[2][0], pMesh->mWorldMatrix[2][1], pMesh->mWorldMatrix[2][2], pMesh->mWorldMatrix[2][3],
        //                 pMesh->mWorldMatrix[3][0], pMesh->mWorldMatrix[3][1], pMesh->mWorldMatrix[3][2], pMesh->mWorldMatrix[3][3]);
        // 
        //             ss << szText;
        //             XMLText *pText = pDoc->NewText(ss.str().c_str());
        //             pTransformElement->LinkEndChild(pText);
        //         }
        // 
        //         if (bHasGeometry)
        //         {
        //             XMLElement *pTransformElement = pDoc->NewElement(TAG_TRANSFORM);
        //             pMeshElement->LinkEndChild(pTransformElement);
        //             pTransformElement->SetAttribute(ATTRIB_ID, "GEOMETRY");
        // 
        //             std::stringstream ss;
        //             char szText[512] = { 0 };
        //             snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f ",
        //                 pMesh->mGeometryMatrix[0][0], pMesh->mGeometryMatrix[0][1], pMesh->mGeometryMatrix[0][2], pMesh->mGeometryMatrix[0][3],
        //                 pMesh->mGeometryMatrix[1][0], pMesh->mGeometryMatrix[1][1], pMesh->mGeometryMatrix[1][2], pMesh->mGeometryMatrix[1][3],
        //                 pMesh->mGeometryMatrix[2][0], pMesh->mGeometryMatrix[2][1], pMesh->mGeometryMatrix[2][2], pMesh->mGeometryMatrix[2][3],
        //                 pMesh->mGeometryMatrix[3][0], pMesh->mGeometryMatrix[3][1], pMesh->mGeometryMatrix[3][2], pMesh->mGeometryMatrix[3][3]);
        // 
        //             ss << szText;
        //             XMLText *pText = pDoc->NewText(ss.str().c_str());
        //             pTransformElement->LinkEndChild(pText);
        //         }

        return pMeshElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLVertexBuffers(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        VertexBuffers *pVertices = (VertexBuffers *)pNode;
        XMLElement *pVerticesElement = pDoc->NewElement(TAG_VERTICES);
        pParentElem->LinkEndChild(pVerticesElement);

        pVerticesElement->SetAttribute(ATTRIB_ID, pVertices->getID().c_str());
        pVerticesElement->SetAttribute(ATTRIB_COUNT, pVertices->getChildrenCount());

        return pVerticesElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLVertexBuffer(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        VertexBuffer *pVB = (VertexBuffer *)pNode;
        XMLElement *pBufferElement = pDoc->NewElement(TAG_BUFFER);
        pBufferElement->SetAttribute(ATTRIB_ID, pVB->getID().c_str());
        pParentElem->LinkEndChild(pBufferElement);

        // 属性
        XMLElement *pAttribRootElement = pDoc->NewElement(TAG_ATTRIBUTES);
        pAttribRootElement->SetAttribute(ATTRIB_COUNT, pVB->mAttributes.size());
        pBufferElement->LinkEndChild(pAttribRootElement);

        int nVertexSize = 0;
        auto itrAttrib = pVB->mAttributes.begin();

        while (itrAttrib != pVB->mAttributes.end())
        {
            const VertexAttribute &attribute = *itrAttrib;
            XMLElement *pAttribElement = pDoc->NewElement(TAG_ATTRIBUTE);
            pAttribElement->SetAttribute(ATTRIB_ID, attribute.vertexTypeStr().c_str());
            pAttribElement->SetAttribute(ATTRIB_SIZE, attribute.mSize);
            pAttribElement->SetAttribute(ATTRIB_TYPE, attribute.dataTypeStr().c_str());
            pAttribRootElement->LinkEndChild(pAttribElement);
            nVertexSize += attribute.mSize;
            ++itrAttrib;
        }

        // 顶点数据
        XMLElement *pVertexElement = pDoc->NewElement(TAG_DATA);
        pBufferElement->LinkEndChild(pVertexElement);
        pVertexElement->SetAttribute(ATTRIB_COUNT, pVB->mVertices.size());

        auto itrVertex = pVB->mVertices.begin();

        int count = 0;
        bool first = true;

        std::stringstream ss;
        ss << "\n";

        const char *TAB_INDENT = "\t\t\t\t\t\t\t";
        const int MAX_BLEND_COUNT = 4;

        while (itrVertex != pVB->mVertices.end())
        {
            bool bFirstAttribute = true;

            const Vertex &vertex = *itrVertex;

            auto itrAttrib = pVB->mAttributes.begin();

            while (itrAttrib != pVB->mAttributes.end())
            {
                const VertexAttribute &attribute = *itrAttrib;

                if (attribute.mVertexType == VertexAttribute::E_VT_POSITION)
                {
                    // POSITION
                    char szText[64] = { 0 };
                    snprintf(szText, sizeof(szText) - 1, "% 8f % 8f % 8f", vertex.mPosition[0], vertex.mPosition[1], vertex.mPosition[2]);

                    if (bFirstAttribute)
                        ss << TAB_INDENT;
                    
                    ss << szText;
                    bFirstAttribute = false;
                }
                else if (attribute.mVertexType == VertexAttribute::E_VT_TEXCOORD)
                {
                    // TEXCOORD
                    auto itr2 = vertex.mTexElements.begin();

                    while (itr2 != vertex.mTexElements.end())
                    {
                        const Vector2 &uv = *itr2;
                        char szText[64] = { 0 };
                        snprintf(szText, sizeof(szText) - 1, " % 8f % 8f", uv[0], uv[1]);

                        if (bFirstAttribute)
                            ss << TAB_INDENT;

                        ss << szText;
                        ++itr2;
                        bFirstAttribute = false;
                    }
                }
                else if (attribute.mVertexType == VertexAttribute::E_VT_NORMAL)
                {
                    // NORMAL
                    auto itr3 = vertex.mNormalElements.begin();

                    while (itr3 != vertex.mNormalElements.end())
                    {
                        const Vector3 &normal = *itr3;
                        char szText[64] = { 0 };
                        snprintf(szText, sizeof(szText) - 1, " % 8f % 8f % 8f", normal[0], normal[1], normal[2]);

                        if (bFirstAttribute)
                            ss << TAB_INDENT;

                        ss << szText;
                        ++itr3;
                        bFirstAttribute = false;
                    }
                }
                else if (attribute.mVertexType == VertexAttribute::E_VT_BINORMAL)
                {
                    // BINORMAL
                    auto itr3 = vertex.mBinormalElements.begin();

                    while (itr3 != vertex.mBinormalElements.end())
                    {
                        const Vector3 &binormal = *itr3;
                        char szText[64] = { 0 };
                        snprintf(szText, sizeof(szText) - 1, " % 8f % 8f % 8f", binormal[0], binormal[1], binormal[2]);

                        if (bFirstAttribute)
                            ss << TAB_INDENT;

                        ss << szText;
                        ++itr3;
                        bFirstAttribute = false;
                    }
                }
                else if (attribute.mVertexType == VertexAttribute::E_VT_TANGENT)
                {
                    // TANGENT
                    auto itr3 = vertex.mTangentElements.begin();

                    while (itr3 != vertex.mTangentElements.end())
                    {
                        const Vector3 &tangent = *itr3;
                        char szText[64] = { 0 };
                        snprintf(szText, sizeof(szText) - 1, " % 8f % 8f % 8f", tangent[0], tangent[1], tangent[2]);

                        if (bFirstAttribute)
                            ss << TAB_INDENT;

                        ss << szText;
                        ++itr3;
                        bFirstAttribute = false;
                    }
                }
                else if (attribute.mVertexType == VertexAttribute::E_VT_COLOR)
                {
                    // COLOR
                    auto itr4 = vertex.mColorElements.begin();

                    while (itr4 != vertex.mColorElements.end())
                    {
                        const Vector4 &color = *itr4;
                        char szText[64] = { 0 };
                        snprintf(szText, sizeof(szText) - 1, " % 8f %8f % 8f % 8f", color[0], color[1], color[2], color[3]);

                        if (bFirstAttribute)
                            ss << TAB_INDENT;

                        ss << szText;
                        ++itr4;
                        bFirstAttribute = false;
                    }
                }
                else if (attribute.mVertexType == VertexAttribute::E_VT_BLEND_WEIGHT)
                {
                    // BLEND_WEIGHT 只写权重最大的4个顶点
                    if (vertex.mBlendInfo.size() > 0)
                    {
                        ss << " ";
                        int i = 0;
                        auto itrBlend = vertex.mBlendInfo.rbegin();
                        while (itrBlend != vertex.mBlendInfo.rend())
                        {
                            char szText[16] = { 0 };
//                             snprintf(szText, sizeof(szText) - 1, "% 8f", itrBlend->second.mBlendWeight);
                            snprintf(szText, sizeof(szText) - 1, "% 8f", itrBlend->mBlendWeight);
                            ss << szText;
                            ++i;
                            ++itrBlend;

                            if (i >= MAX_BLEND_COUNT)
                            {
                                break;
                            }

                            ss << " ";
                        }

                        while (i < MAX_BLEND_COUNT)
                        {
                            char szText[16] = { 0 };
                            snprintf(szText, sizeof(szText) - 1, "% 8f", 0.0f);
                            ss << szText;
                            ++i;
                            ss << " ";
                        }
                    }
                }
                else if (attribute.mVertexType == VertexAttribute::E_VT_BLEND_INDEX)
                {
                    // BLEND_INDEX 只写权重最大的4个顶点
                    int i = 0;
                    auto itrBlend = vertex.mBlendInfo.rbegin();
                    while (itrBlend != vertex.mBlendInfo.rend())
                    {
                        char szText[16] = { 0 };
//                         snprintf(szText, sizeof(szText) - 1, "% 8d", itrBlend->second.mBlendIndex);
                        snprintf(szText, sizeof(szText) - 1, "% 8d", itrBlend->mBlendIndex);
                        ss << szText;
                        ++i;
                        ++itrBlend;

                        if (i >= MAX_BLEND_COUNT)
                        {
                            break;
                        }

                        ss << " ";
                    }

                    while (i < MAX_BLEND_COUNT)
                    {
                        char szText[16] = { 0 };
                        snprintf(szText, sizeof(szText) - 1, "% 8d", 0);
                        ss << szText;
                        ++i;
                        if (i < MAX_BLEND_COUNT)
                            ss << " ";
                    }
                }

                ++itrAttrib;
            }

//             // POSITION
//             if (hasVertexAttribute(pVB, VertexAttribute::E_VT_POSITION))
//             {
//                 if (first)
//                 {
//                     char szText[64] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, "% 8f % 8f % 8f", vertex.mPosition[0], vertex.mPosition[1], vertex.mPosition[2]);
//                     ss << "\n\t\t\t\t\t\t\t" << szText;
//                 }
//                 else
//                 {
//                     char szText[64] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, "% 8f % 8f % 8f", vertex.mPosition[0], vertex.mPosition[1], vertex.mPosition[2]);
//                     ss << "\t\t\t\t\t\t\t" << szText;
//                 }
// 
//                 first = false;
//                 bFirstAttribute = false;
//             }
// 
//             // TEXCOORD
//             if (hasVertexAttribute(pVB, VertexAttribute::E_VT_TEXCOORD))
//             {
//                 auto itr2 = vertex.mTexElements.begin();
//                 while (itr2 != vertex.mTexElements.end())
//                 {
// 					if (first)
//                     {
//                         const Vector2 &uv = *itr2;
//                         char szText[64] = { 0 };
//                         snprintf(szText, sizeof(szText) - 1, " % 8f % 8f", uv[0], uv[1]);
// 						ss<<"\n\t\t\t\t\t\t\t"<<szText;
//                     }
// 					else
// 					{
// 						const Vector2 &uv = *itr2;
// 						char szText[64] = { 0 };
// 						snprintf(szText, sizeof(szText) - 1, " % 8f % 8f", uv[0], uv[1]);
//                         if (bFirstAttribute)
// 						    ss<<"\t\t\t\t\t\t\t"<<szText;
//                         else
//                             ss << szText;
// 					}
// 					
// 					first = false;
//                     ++itr2;
//                 }
// 
//                 bFirstAttribute = false;
//             }
// 
//             // NORMAL
//             if (hasVertexAttribute(pVB, VertexAttribute::E_VT_NORMAL))
//             {
//                 auto itr3 = vertex.mNormalElements.begin();
//                 while (itr3 != vertex.mNormalElements.end())
//                 {
//                     //                 const FbxVector3 &normal = *itr3;
//                     const Vector3 &normal = *itr3;
//                     char szText[64] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, " % 8f % 8f % 8f", normal[0], normal[1], normal[2]);
//                     ss << szText;
//                     ++itr3;
//                 }
//             }
// 
//             // BINORMAL
//             if (hasVertexAttribute(pVB, VertexAttribute::E_VT_BINORMAL))
//             {
//                 auto itr3 = vertex.mBinormalElements.begin();
//                 while (itr3 != vertex.mBinormalElements.end())
//                 {
//                     //                 const FbxVector3 &binormal = *itr3;
//                     const Vector3 &binormal = *itr3;
//                     char szText[64] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, " % 8f % 8f % 8f", binormal[0], binormal[1], binormal[2]);
//                     ss << szText;
//                     ++itr3;
//                 }
//             }
// 
//             // TANGENT
//             if (hasVertexAttribute(pVB, VertexAttribute::E_VT_TANGENT))
//             {
//                 auto itr3 = vertex.mTangentElements.begin();
//                 while (itr3 != vertex.mTangentElements.end())
//                 {
//                     //                 const FbxVector3 &tangent = *itr3;
//                     const Vector3 &tangent = *itr3;
//                     char szText[64] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, " % 8f % 8f % 8f", tangent[0], tangent[1], tangent[2]);
//                     ss << szText;
//                     //                 ss<<" "<<tangent[0]<<" "<<tangent[1]<<" "<<tangent[2];
//                     ++itr3;
//                 }
//             }
// 
//             // COLOR
//             if (hasVertexAttribute(pVB, VertexAttribute::E_VT_COLOR))
//             {
//                 auto itr4 = vertex.mColorElements.begin();
//                 while (itr4 != vertex.mColorElements.end())
//                 {
//                     //                 const FbxVector4 &color = *itr4;
//                     const Vector4 &color = *itr4;
//                     char szText[64] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, " % 8f %8f % 8f % 8f", color[0], color[1], color[2], color[3]);
//                     ss << szText;
//                     //                 ss<<" "<<color[0]<<" "<<color[1]<<" "<<color[2]<<" "<<color[3];
//                     ++itr4;
//                 }
//             }
// 
//             // BLEND_WEIGHT 只写权重最大的4个顶点
//             if (vertex.mBlendInfo.size() > 0)
//             {
//                 ss << " ";
//                 const int MAX_BLEND_COUNT = 4;
//                 int i = 0;
//                 auto itrBlend = vertex.mBlendInfo.rbegin();
//                 while (itrBlend != vertex.mBlendInfo.rend())
//                 {
//                     char szText[16] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, "% 8f", itrBlend->second.mBlendWeight);
//                     ss << szText;
//                     //                     ss<<itrBlend->second.mBlendWeight;
//                     ++i;
//                     ++itrBlend;
// 
//                     if (i >= MAX_BLEND_COUNT)
//                     {
//                         break;
//                     }
// 
//                     ss << " ";
//                 }
// 
//                 while (i < MAX_BLEND_COUNT)
//                 {
//                     //                     ss<<"0";
//                     char szText[16] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, "% 8f", 0.0f);
//                     ss << szText;
//                     ++i;
//                     ss << " ";
//                 }
// 
//                 // BLEND_INDEX 只写权重最大的4个顶点
//                 i = 0;
//                 itrBlend = vertex.mBlendInfo.rbegin();
//                 while (itrBlend != vertex.mBlendInfo.rend())
//                 {
//                     char szText[16] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, "% 8d", itrBlend->second.mBlendIndex);
//                     ss << szText;
//                     //                     ss<<itrBlend->second.mBlendIndex;
//                     ++i;
//                     ++itrBlend;
// 
//                     if (i >= MAX_BLEND_COUNT)
//                     {
//                         break;
//                     }
// 
//                     ss << " ";
//                 }
// 
//                 while (i < MAX_BLEND_COUNT)
//                 {
//                     char szText[16] = { 0 };
//                     snprintf(szText, sizeof(szText) - 1, "% 8d", 0);
//                     ss << szText;
//                     //                     ss<<"-1";
//                     ++i;
//                     if (i < MAX_BLEND_COUNT)
//                         ss << " ";
//                 }
//             }

            ss << " \n";
            XMLText *pText = pDoc->NewText(ss.str().c_str());
            pVertexElement->LinkEndChild(pText);

            ss.str("");
            ++itrVertex;
            ++count;
        }

        XMLText *pText = pDoc->NewText("\t\t\t\t\t\t");
        pVertexElement->LinkEndChild(pText);

        return pBufferElement;
    }

    bool T3DXMLSerializer::hasVertexAttribute(Node *pNode, int16_t attribType)
    {
        bool found = false;

        VertexBuffer *pVB = (VertexBuffer *)pNode;
        T3D_ASSERT(pVB->getNodeType() == Node::E_TYPE_VERTEX_BUFFER);

        auto itr = pVB->mAttributes.begin();

        while (itr != pVB->mAttributes.end())
        {
            const VertexAttribute &attribute = *itr;

            if (attribType == attribute.mVertexType)
            {
                found = true;
                break;
            }

            ++itr;
        }

        return found;
    }

    XMLElement *T3DXMLSerializer::buildXMLSubMeshes(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pSubmeshesElem = pDoc->NewElement(TAG_SUBMESHES);
        pParentElem->LinkEndChild(pSubmeshesElem);

        size_t nChildrenCount = pNode->getChildrenCount();
        pSubmeshesElem->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        pSubmeshesElem->SetAttribute(ATTRIB_COUNT, nChildrenCount);

        return pSubmeshesElem;
    }

    XMLElement *T3DXMLSerializer::buildXMLSubMesh(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pSubmeshElement = pDoc->NewElement(TAG_SUBMESH);
        pParentElem->LinkEndChild(pSubmeshElement);

        // sub-mesh
        SubMesh *pSubMesh = (SubMesh *)pNode;
        pSubmeshElement->SetAttribute(ATTRIB_ID, pSubMesh->getID().c_str());
        pSubmeshElement->SetAttribute(ATTRIB_PRIMITIVE, "triangles");
        size_t nIndexCount = pSubMesh->mIndices.size();
        size_t nTriangleCount = nIndexCount / 3;
        pSubmeshElement->SetAttribute(ATTRIB_COUNT, nTriangleCount);
        pSubmeshElement->SetAttribute(ATTRIB_MATERIAL, pSubMesh->mMaterialName.c_str());

        // 顶点索引
        XMLElement *pIndicesElement = pDoc->NewElement(TAG_INDICES);
        pSubmeshElement->LinkEndChild(pIndicesElement);

        pIndicesElement->SetAttribute(ATTRIB_COUNT, nIndexCount);
        bool b16Bits = (nIndexCount > 0xFFFF ? false : true);
        pIndicesElement->SetAttribute(ATTRIB_16BITS, b16Bits);

        std::stringstream ss;
        ss << "\n\t\t\t\t\t\t\t";
        size_t i = 0;
        auto itr = pSubMesh->mIndices.begin();
        while (itr != pSubMesh->mIndices.end())
        {
            int nIndex = *itr;
            ss << nIndex << " ";

            if (i == 32)
            {
                ss << "\n\t\t\t\t\t\t\t";
                XMLText *pText = pDoc->NewText(ss.str().c_str());
                pIndicesElement->LinkEndChild(pText);
                ss.clear();
                ss.str("");
                i = 0;
            }

            ++i;
            ++itr;
        }

        //         if ((nIndexCount & 31) != 0)
        //         {
        //             /// 有多余的，需要重新写回去
        //             XMLText *pText = pDoc->NewText(ss.str().c_str());
        //             pIndicesElement->LinkEndChild(pText);
        //         }

        ss << "\n\t\t\t\t\t\t";
        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pIndicesElement->LinkEndChild(pText);

        return pSubmeshElement;
    }

    //     XMLElement *T3DXMLSerializer::buildXMLMaterials(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    //     {
    //         XMLElement *pMatsElement = pDoc->NewElement(TAG_MATERIALS);
    //         pParentElem->LinkEndChild(pMatsElement);
    // 
    //         pMatsElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
    //         pMatsElement->SetAttribute(ATTRIB_COUNT, pNode->getChildrenCount());
    // 
    //         return pMatsElement;
    //     }

    //     XMLElement *T3DXMLSerializer::buildXMLMaterial(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    bool T3DXMLSerializer::saveMaterial(Node *pNode)
    {
        XMLDocument *pDoc = new XMLDocument();

        XMLDeclaration *pDecl = pDoc->NewDeclaration();
        pDoc->LinkEndChild(pDecl);

//         XMLElement *pMagicElement = pDoc->NewElement(TAG_MAGIC);
//         pDoc->LinkEndChild(pMagicElement);
//         XMLText *pText = pDoc->NewText(T3D_MATERIAL_FILE_MAGIC);
//         pMagicElement->LinkEndChild(pText);
// 
//         XMLElement *pVersionElement = pDoc->NewElement(TAG_VERSION);
//         pDoc->LinkEndChild(pVersionElement);
//         pText = pDoc->NewText(T3D_MATERIAL_FILE_VER_CUR_STR);
//         pVersionElement->LinkEndChild(pText);
        XMLElement *pTiny3DElement = pDoc->NewElement(TAG_TINY3D);
        pDoc->LinkEndChild(pTiny3DElement);

        pTiny3DElement->SetAttribute(ATTRIB_MAGIC, T3D_MATERIAL_FILE_MAGIC);
        pTiny3DElement->SetAttribute(ATTRIB_VERSION, T3D_MATERIAL_FILE_VER_CUR_STR);

        XMLElement *pMatElement = pDoc->NewElement(TAG_MATERIAL);
        pTiny3DElement->LinkEndChild(pMatElement);

        Material *pMaterial = (Material *)pNode;
        pMatElement->SetAttribute(ATTRIB_ID, pMaterial->getID().c_str());

        // shader算法
        XMLElement *pModeElement = pDoc->NewElement(TAG_MODE);
        pMatElement->LinkEndChild(pModeElement);
        XMLText *pText = pDoc->NewText(pMaterial->mMode.c_str());
        pModeElement->LinkEndChild(pText);

        // ambient
        XMLElement *pAmbientElement = pDoc->NewElement(TAG_AMBIENT);
        pMatElement->LinkEndChild(pAmbientElement);
        std::stringstream ss;
        ss << pMaterial->mAmbientColor[0] << " ";
        ss << pMaterial->mAmbientColor[1] << " ";
        ss << pMaterial->mAmbientColor[2] << " ";
        ss << pMaterial->mAmbientColor[3];
        pText = pDoc->NewText(ss.str().c_str());
        pAmbientElement->LinkEndChild(pText);

        // diffuse
        XMLElement *pDiffuseElement = pDoc->NewElement(TAG_DIFFUSE);
        pMatElement->LinkEndChild(pDiffuseElement);
        ss.clear();
        ss.str("");
        ss << pMaterial->mDiffuseColor[0] << " ";
        ss << pMaterial->mDiffuseColor[1] << " ";
        ss << pMaterial->mDiffuseColor[2] << " ";
        ss << pMaterial->mDiffuseColor[3];
        pText = pDoc->NewText(ss.str().c_str());
        pDiffuseElement->LinkEndChild(pText);

        // specular
        XMLElement *pSpecularElement = pDoc->NewElement(TAG_SPECULAR);
        pMatElement->LinkEndChild(pSpecularElement);
        ss.clear();
        ss.str("");
        ss << pMaterial->mSpecularColor[0] << " ";
        ss << pMaterial->mSpecularColor[1] << " ";
        ss << pMaterial->mSpecularColor[2] << " ";
        ss << pMaterial->mSpecularColor[3];
        pText = pDoc->NewText(ss.str().c_str());
        pSpecularElement->LinkEndChild(pText);

        // emissive
        XMLElement *pEmissiveElement = pDoc->NewElement(TAG_EMISSIVE);
        pMatElement->LinkEndChild(pEmissiveElement);
        ss.clear();
        ss.str("");
        ss << pMaterial->mEmissiveColor[0] << " ";
        ss << pMaterial->mEmissiveColor[1] << " ";
        ss << pMaterial->mEmissiveColor[2] << " ";
        ss << pMaterial->mEmissiveColor[3];
        pText = pDoc->NewText(ss.str().c_str());
        pEmissiveElement->LinkEndChild(pText);

        // shininess
        XMLElement *pShinElement = pDoc->NewElement(TAG_SHININESS);
        pMatElement->LinkEndChild(pShinElement);
        ss.clear();
        ss.str("");
        ss << pMaterial->mShininess;
        pText = pDoc->NewText(ss.str().c_str());
        pShinElement->LinkEndChild(pText);

        // transparency
        XMLElement *pTransElement = pDoc->NewElement(TAG_TRANSPARENCY);
        pMatElement->LinkEndChild(pTransElement);
        ss.clear();
        ss.str("");
        ss << pMaterial->mTransparency;
        pText = pDoc->NewText(ss.str().c_str());
        pTransElement->LinkEndChild(pText);

        // reflection
        XMLElement *pReflectElement = pDoc->NewElement(TAG_REFLECTION);
        pMatElement->LinkEndChild(pReflectElement);
        ss.clear();
        ss.str("");
        ss << pMaterial->mReflection;
        pText = pDoc->NewText(ss.str().c_str());
        pReflectElement->LinkEndChild(pText);

        // texture
        size_t i = 0;
        for (i = 0; i < pNode->getChildrenCount(); ++i)
        {
            Node *pChild = pNode->getChild(i);
            populateXMLNode(pDoc, pMatElement, pChild);
        }

        size_t pos = mSavePath.rfind('\\');
        if (pos == String::npos)
        {
            pos = mSavePath.rfind('/');
        }

        String strPath;
        if (pos != String::npos)
        {
            strPath = mSavePath.substr(0, pos + 1);
        }

        String strFullPath = strPath + pMaterial->getID() + "." + T3D_TXT_MATERIAL_FILE_EXT;
        pDoc->SaveFile(strFullPath.c_str());

        delete pDoc;

        return true;
    }

    XMLElement *T3DXMLSerializer::buildXMLTextures(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pTexElement = pDoc->NewElement(TAG_TEXTURES);
        pParentElem->LinkEndChild(pTexElement);

        pTexElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        pTexElement->SetAttribute(ATTRIB_COUNT, pNode->getChildrenCount());

        return pTexElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLTexture(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        Texture *pTexture = (Texture *)pNode;
        XMLElement *pTexElement = pDoc->NewElement(TAG_TEXTURE);
        pParentElem->LinkEndChild(pTexElement);

        pTexElement->SetAttribute(ATTRIB_ID, pTexture->getID().c_str());
        pTexElement->SetAttribute(ATTRIB_TYPE, pTexture->mType.c_str());
        pTexElement->SetAttribute(ATTRIB_WRAP_U, pTexture->mWrapModeU.c_str());
        pTexElement->SetAttribute(ATTRIB_WRAP_V, pTexture->mWrapModeV.c_str());

        XMLText *pText = pDoc->NewText(pTexture->mFilename.c_str());
        pTexElement->LinkEndChild(pText);

        return pTexElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLAnimation(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        Animation *pAnim = (Animation *)pNode;
        XMLElement *pAnimElement = pDoc->NewElement(TAG_ANIMATION);
        pParentElem->LinkEndChild(pAnimElement);

        pAnimElement->SetAttribute(ATTRIB_ID, pAnim->getID().c_str());
        pAnimElement->SetAttribute(ATTRIB_COUNT, pAnim->getChildrenCount());

        return pAnimElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLAction(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        Action *pAction = (Action *)pNode;
        XMLElement *pActionElement = pDoc->NewElement(TAG_ACTION);
        pParentElem->LinkEndChild(pActionElement);

        pActionElement->SetAttribute(ATTRIB_ID, pAction->getID().c_str());
        pActionElement->SetAttribute(ATTRIB_DURATION, pAction->mDuration);

        auto itr = pAction->mTKeyframes.begin();
        while (itr != pAction->mTKeyframes.end())
        {
            XMLElement *pKeyElement = pDoc->NewElement(TAG_KEYFRAME);
            pActionElement->LinkEndChild(pKeyElement);

            pKeyElement->SetAttribute(ATTRIB_TYPE, "translation");
            pKeyElement->SetAttribute(ATTRIB_BONE, itr->first.c_str());
            pKeyElement->SetAttribute(ATTRIB_COUNT, itr->second.size());

            Keyframes &keyframes = itr->second;

            auto i = keyframes.begin();
            while (i != keyframes.end())
            {
                XMLElement *pFrameElement = pDoc->NewElement(TAG_FRAME);
                pKeyElement->LinkEndChild(pFrameElement);

                KeyframeT *pFrame = (KeyframeT *)*i;
                pFrameElement->SetAttribute(ATTRIB_ID, pFrame->mID);
                pFrameElement->SetAttribute(ATTRIB_TIME, pFrame->mTimestamp);

                char szText[512] = { 0 };
                snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f", pFrame->x, pFrame->y, pFrame->z);
                XMLText *pText = pDoc->NewText(szText);
                pFrameElement->LinkEndChild(pText);
                ++i;
            }

            ++itr;
        }

        itr = pAction->mRKeyframes.begin();
        while (itr != pAction->mRKeyframes.end())
        {
            XMLElement *pKeyElement = pDoc->NewElement(TAG_KEYFRAME);
            pActionElement->LinkEndChild(pKeyElement);

            pKeyElement->SetAttribute(ATTRIB_TYPE, "rotation");
            pKeyElement->SetAttribute(ATTRIB_BONE, itr->first.c_str());
            pKeyElement->SetAttribute(ATTRIB_COUNT, itr->second.size());

            Keyframes &keyframes = itr->second;

            auto i = keyframes.begin();
            while (i != keyframes.end())
            {
                XMLElement *pFrameElement = pDoc->NewElement(TAG_FRAME);
                pKeyElement->LinkEndChild(pFrameElement);

                KeyframeR *pFrame = (KeyframeR *)*i;
                pFrameElement->SetAttribute(ATTRIB_ID, pFrame->mID);
                pFrameElement->SetAttribute(ATTRIB_TIME, pFrame->mTimestamp);

                //                 std::stringstream ss;
                //                 ss<<pFrame->x<<" "<<pFrame->y<<" "<<pFrame->z<<" "<<pFrame->w;
                char szText[512] = { 0 };
                snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f", pFrame->x, pFrame->y, pFrame->z, pFrame->w);
                XMLText *pText = pDoc->NewText(szText);
                pFrameElement->LinkEndChild(pText);
                ++i;
            }

            ++itr;
        }

        itr = pAction->mSKeyframes.begin();
        while (itr != pAction->mSKeyframes.end())
        {
            XMLElement *pKeyElement = pDoc->NewElement(TAG_KEYFRAME);
            pActionElement->LinkEndChild(pKeyElement);

            pKeyElement->SetAttribute(ATTRIB_TYPE, "scaling");
            pKeyElement->SetAttribute(ATTRIB_BONE, itr->first.c_str());
            pKeyElement->SetAttribute(ATTRIB_COUNT, itr->second.size());

            Keyframes &keyframes = itr->second;

            auto i = keyframes.begin();
            while (i != keyframes.end())
            {
                XMLElement *pFrameElement = pDoc->NewElement(TAG_FRAME);
                pKeyElement->LinkEndChild(pFrameElement);

                KeyframeS *pFrame = (KeyframeS *)*i;
                pFrameElement->SetAttribute(ATTRIB_ID, pFrame->mID);
                pFrameElement->SetAttribute(ATTRIB_TIME, pFrame->mTimestamp);

                //                 std::stringstream ss;
                //                 ss<<pFrame->x<<" "<<pFrame->y<<" "<<pFrame->z;
                char szText[512] = { 0 };
                snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f", pFrame->x, pFrame->y, pFrame->z);
                XMLText *pText = pDoc->NewText(szText);
                pFrameElement->LinkEndChild(pText);
                ++i;
            }

            ++itr;
        }

        return pActionElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLSkeleton(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pSkelElement = pDoc->NewElement(TAG_SKELETON);
        pParentElem->LinkEndChild(pSkelElement);

        pSkelElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        Skeleton *pSkel = (Skeleton *)pNode;
        pSkelElement->SetAttribute(ATTRIB_COUNT, pSkel->mBoneCount);

        return pSkelElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLSkin(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pSkinElement = pDoc->NewElement(TAG_SKIN);
        pParentElem->LinkEndChild(pSkinElement);

        pSkinElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        pSkinElement->SetAttribute(ATTRIB_COUNT, pNode->getChildrenCount());

//         XMLElement *pTransformElement = pDoc->NewElement(TAG_TRANSFORM);
//         pSkinElement->LinkEndChild(pTransformElement);
// 
//         Skin *pSkin = (Skin *)pNode;
//         std::stringstream ss;
//         char szText[512] = { 0 };
//         snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f ",
//             pSkin->mVertexMatrix[0][0], pSkin->mVertexMatrix[0][1], pSkin->mVertexMatrix[0][2], pSkin->mVertexMatrix[0][3],
//             pSkin->mVertexMatrix[1][0], pSkin->mVertexMatrix[1][1], pSkin->mVertexMatrix[1][2], pSkin->mVertexMatrix[1][3],
//             pSkin->mVertexMatrix[2][0], pSkin->mVertexMatrix[2][1], pSkin->mVertexMatrix[2][2], pSkin->mVertexMatrix[2][3],
//             pSkin->mVertexMatrix[3][0], pSkin->mVertexMatrix[3][1], pSkin->mVertexMatrix[3][2], pSkin->mVertexMatrix[3][3]);
// 
//         ss << szText;
// 
//         XMLText *pText = pDoc->NewText(ss.str().c_str());
//         pTransformElement->LinkEndChild(pText);

        return pSkinElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLSkinInfo(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pSkinElement = pDoc->NewElement(TAG_SKIN_INFO);
        pParentElem->LinkEndChild(pSkinElement);

        pSkinElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());

        XMLElement *pTransformElement = pDoc->NewElement(TAG_TRANSFORM);
        pSkinElement->LinkEndChild(pTransformElement);

        SkinInfo *pSkinInfo = (SkinInfo *)pNode;
        std::stringstream ss;
        char szText[512] = { 0 };
        snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f ",
            pSkinInfo->mOffsetMatrix[0][0], pSkinInfo->mOffsetMatrix[0][1], pSkinInfo->mOffsetMatrix[0][2], pSkinInfo->mOffsetMatrix[0][3],
            pSkinInfo->mOffsetMatrix[1][0], pSkinInfo->mOffsetMatrix[1][1], pSkinInfo->mOffsetMatrix[1][2], pSkinInfo->mOffsetMatrix[1][3],
            pSkinInfo->mOffsetMatrix[2][0], pSkinInfo->mOffsetMatrix[2][1], pSkinInfo->mOffsetMatrix[2][2], pSkinInfo->mOffsetMatrix[2][3],
            pSkinInfo->mOffsetMatrix[3][0], pSkinInfo->mOffsetMatrix[3][1], pSkinInfo->mOffsetMatrix[3][2], pSkinInfo->mOffsetMatrix[3][3]);

        ss << szText;

        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pTransformElement->LinkEndChild(pText);

        return pSkinElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLBone(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pBoneElement = pDoc->NewElement(TAG_BONE);
        pParentElem->LinkEndChild(pBoneElement);

        Bone *pBone = (Bone *)pNode;
        pBoneElement->SetAttribute(ATTRIB_ID, pBone->getID().c_str());
        pBoneElement->SetAttribute(ATTRIB_INDEX, pBone->mBoneIndex);

        XMLElement *pTransformElement = pDoc->NewElement(TAG_TRANSFORM);
        pBoneElement->LinkEndChild(pTransformElement);
        pTransformElement->SetAttribute(ATTRIB_ID, "LOCAL");

        std::stringstream ss;
        char szText[512] = { 0 };
        snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f ",
            pBone->mLocalTransform[0][0], pBone->mLocalTransform[0][1], pBone->mLocalTransform[0][2], pBone->mLocalTransform[0][3],
            pBone->mLocalTransform[1][0], pBone->mLocalTransform[1][1], pBone->mLocalTransform[1][2], pBone->mLocalTransform[1][3],
            pBone->mLocalTransform[2][0], pBone->mLocalTransform[2][1], pBone->mLocalTransform[2][2], pBone->mLocalTransform[2][3],
            pBone->mLocalTransform[3][0], pBone->mLocalTransform[3][1], pBone->mLocalTransform[3][2], pBone->mLocalTransform[3][3]);

        ss << szText;
        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pTransformElement->LinkEndChild(pText);

        pTransformElement = pDoc->NewElement(TAG_TRANSFORM);
        pBoneElement->LinkEndChild(pTransformElement);
        pTransformElement->SetAttribute(ATTRIB_ID, "OFFSET");

        ss.str("");
        snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f ",
            pBone->mOffsetMatrix[0][0], pBone->mOffsetMatrix[0][1], pBone->mOffsetMatrix[0][2], pBone->mOffsetMatrix[0][3],
            pBone->mOffsetMatrix[1][0], pBone->mOffsetMatrix[1][1], pBone->mOffsetMatrix[1][2], pBone->mOffsetMatrix[1][3],
            pBone->mOffsetMatrix[2][0], pBone->mOffsetMatrix[2][1], pBone->mOffsetMatrix[2][2], pBone->mOffsetMatrix[2][3],
            pBone->mOffsetMatrix[3][0], pBone->mOffsetMatrix[3][1], pBone->mOffsetMatrix[3][2], pBone->mOffsetMatrix[3][3]);

        ss << szText;
        pText = pDoc->NewText(ss.str().c_str());
        pTransformElement->LinkEndChild(pText);

        return pBoneElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLHierarchy(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pHiarachyElement = pDoc->NewElement(TAG_HIERARCHY);
        pParentElem->LinkEndChild(pHiarachyElement);

        pHiarachyElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());

        return pHiarachyElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLTransform(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pNodeElement = pDoc->NewElement(TAG_NODE);
        pParentElem->LinkEndChild(pNodeElement);

        pNodeElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());

#if 0
        // 解出RTS
        Transform *pTransform = (Transform *)pNode;
        Vector3 pos, scale;
        Quaternion orientation;
        pTransform->mMatrix.decomposition(pos, scale, orientation);

        // Translation
        XMLElement *pTranslationElement = pDoc->NewElement(TAG_TRANSLATION);
        pNodeElement->LinkEndChild(pTranslationElement);

        std::stringstream ss;
        char szText[128] = { 0 };
        snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f", pos[0], pos[1], pos[2]);
        ss << szText;
        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pTranslationElement->LinkEndChild(pText);

        // Orientation
        XMLElement *pOrientationElement = pDoc->NewElement(TAG_ORIENTATION);
        pNodeElement->LinkEndChild(pOrientationElement);

        ss.str("");
        snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f", orientation[0], orientation[1], orientation[2], orientation[3]);
        ss << szText;
        pText = pDoc->NewText(ss.str().c_str());
        pOrientationElement->LinkEndChild(pText);

        // Scale
        XMLElement *pScaleElement = pDoc->NewElement(TAG_SCALE);
        pNodeElement->LinkEndChild(pScaleElement);

        ss.str("");
        snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f", orientation[0], orientation[1], orientation[2], orientation[3]);
        ss << szText;
        pText = pDoc->NewText(ss.str().c_str());
        pScaleElement->LinkEndChild(pText);
#else
        XMLElement *pTransformElement = pDoc->NewElement(TAG_TRANSFORM);
        pNodeElement->LinkEndChild(pTransformElement);
        Transform *pTransform = (Transform *)pNode;
        std::stringstream ss;
        char szText[512] = { 0 };
        snprintf(szText, sizeof(szText) - 1, " %8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f % 8f ",
            pTransform->mMatrix[0][0], pTransform->mMatrix[0][1], pTransform->mMatrix[0][2], pTransform->mMatrix[0][3],
            pTransform->mMatrix[1][0], pTransform->mMatrix[1][1], pTransform->mMatrix[1][2], pTransform->mMatrix[1][3],
            pTransform->mMatrix[2][0], pTransform->mMatrix[2][1], pTransform->mMatrix[2][2], pTransform->mMatrix[2][3],
            pTransform->mMatrix[3][0], pTransform->mMatrix[3][1], pTransform->mMatrix[3][2], pTransform->mMatrix[3][3]);

        ss << szText;
        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pTransformElement->LinkEndChild(pText);
#endif

        // Link
        auto itr = pTransform->mEntities.begin();

        while (itr != pTransform->mEntities.end())
        {
            auto entity = *itr;
            XMLElement *pLinkElement = pDoc->NewElement(TAG_LINK);
            pNodeElement->LinkEndChild(pLinkElement);
            pLinkElement->SetAttribute(ATTRIB_MESH, entity.first->getID().c_str());
            pLinkElement->SetAttribute(ATTRIB_SUBMESH, entity.second->getID().c_str());
            ++itr;
        }

        return pNodeElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLAlignAxisBound(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pBoundElement = pDoc->NewElement(TAG_BOUND);
        pParentElem->LinkEndChild(pBoundElement);

        AabbBound *pBound = (AabbBound *)pNode;

        pBoundElement->SetAttribute(ATTRIB_ID, pBound->getID().c_str());
        pBoundElement->SetAttribute(ATTRIB_TYPE, "aabb");

        std::stringstream ss;
        ss<<pBound->mMinX<<" "<<pBound->mMaxX<<" "<<pBound->mMinY<<" "<<pBound->mMaxY<<" "<<pBound->mMinZ<<" "<<pBound->mMaxZ;
        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pBoundElement->LinkEndChild(pText);

        return pBoundElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLSphereBound(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pBoundElement = pDoc->NewElement(TAG_BOUND);
        pParentElem->LinkEndChild(pBoundElement);

        SphereBound *pBound = (SphereBound *)pNode;

        pBoundElement->SetAttribute(ATTRIB_ID, pBound->getID().c_str());
        pBoundElement->SetAttribute(ATTRIB_TYPE, "sphere");

        std::stringstream ss;
        ss<<pBound->mCenterX<<" "<<pBound->mCenterY<<" "<<pBound->mCenterZ<<" "<<pBound->mRadius;
        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pBoundElement->LinkEndChild(pText);

        return pBoundElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLLight(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pLightElement = pDoc->NewElement(TAG_LIGHT);
        pParentElem->LinkEndChild(pLightElement);
        return pLightElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLCamera(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pCameraElement = pDoc->NewElement(TAG_CAMERA);
        pParentElem->LinkEndChild(pCameraElement);
        return pCameraElement;
    }

    //////////////////////////////////////////////////////////////////////////

    T3DSerializer::T3DSerializer()
        : mBinSerializer(new T3DBinSerializer())
        , mXMLSerializer(new T3DXMLSerializer())
    {

    }

    T3DSerializer::~T3DSerializer()
    {
        delete mBinSerializer;
        mBinSerializer = nullptr;

        delete mXMLSerializer;
        mXMLSerializer = nullptr;
    }

    bool T3DSerializer::load(const String &path, void *&pData)
    {
        T3D_LOG_WARNING("Unable to load two different format files at the same time !");
        T3D_ASSERT(0);
        return true;
    }

    bool T3DSerializer::save(const String &path, void *pData)
    {
        bool result = false;

        if (mBinSerializer != nullptr)
        {
            String dstPath = path + "." + T3D_BIN_MODEL_FILE_EXT;
            result = mBinSerializer->save(dstPath, pData);
        }

        if (mXMLSerializer != nullptr)
        {
            String dstPath = path + "." + T3D_TXT_MODEL_FILE_EXT;
            result = result && mXMLSerializer->save(dstPath, pData);
        }

        return result;
    }
}
