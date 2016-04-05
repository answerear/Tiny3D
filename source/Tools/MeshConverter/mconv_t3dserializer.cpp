

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

    const char * const T3DXMLSerializer::TAG_SCENE = "scene";
    const char * const T3DXMLSerializer::TAG_MODEL = "model";
    const char * const T3DXMLSerializer::TAG_MESH = "mesh";
    const char * const T3DXMLSerializer::TAG_ATTRIBUTES = "attributes";
    const char * const T3DXMLSerializer::TAG_ATTRIBUTE = "attribute";
    const char * const T3DXMLSerializer::TAG_VERTICES = "vertices";
    const char * const T3DXMLSerializer::TAG_PARTS = "parts";
    const char * const T3DXMLSerializer::TAG_PART = "part";
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
    const char * const T3DXMLSerializer::TAG_SKELETON = "skeleton";
    const char * const T3DXMLSerializer::TAG_BONE = "bone";
    const char * const T3DXMLSerializer::TAG_TRANSFORM = "transform";
    const char * const T3DXMLSerializer::TAG_ANIMATION = "animation";
    const char * const T3DXMLSerializer::TAG_ACTION = "action";
    const char * const T3DXMLSerializer::TAG_KEYFRAME = "keyframe";
    const char * const T3DXMLSerializer::TAG_FRAME = "frame";
    const char * const T3DXMLSerializer::TAG_LIGHT = "light";
    const char * const T3DXMLSerializer::TAG_CAMERA = "camera";

    const char * const T3DXMLSerializer::ATTRIB_ID = "id";
    const char * const T3DXMLSerializer::ATTRIB_COUNT = "count";
    const char * const T3DXMLSerializer::ATTRIB_SIZE = "size";
    const char * const T3DXMLSerializer::ATTRIB_TYPE = "type";
    const char * const T3DXMLSerializer::ATTRIB_PRIMITIVE = "primitive";
    const char * const T3DXMLSerializer::ATTRIB_MATERIAL = "material";
    const char * const T3DXMLSerializer::ATTRIB_16BITS = "16bits";
    const char * const T3DXMLSerializer::ATTRIB_SPAN = "span";
    const char * const T3DXMLSerializer::ATTRIB_BONE = "bone";
    const char * const T3DXMLSerializer::ATTRIB_TIME = "time";

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
        using namespace tinyxml2;

        Scene *pScene = (Scene *)pData;
        if (pScene != nullptr)
        {
            XMLDocument *pDoc = new XMLDocument(true, COLLAPSE_WHITESPACE);
            XMLDeclaration *pDecl = pDoc->NewDeclaration();
            pDoc->LinkEndChild(pDecl);

            XMLElement *pElement = pDoc->NewElement(TAG_SCENE);
            pElement->SetAttribute(ATTRIB_ID, pScene->getID().c_str());
            pDoc->LinkEndChild(pElement);

            mTabCount = 0;
            populateXMLNode(pDoc, pElement, pScene);

            pDoc->SaveFile(path.c_str());
            delete pDoc;
        }

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
        case Node::E_TYPE_CAMERA:
            {
                pElement = buildXMLCamera(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_LIGHT:
            {
                pElement = buildXMLLight(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_MATERIAL:
            {
                pElement = buildXMLMaterial(pDoc, pParentElem, pNode);
            }
            break;
        case Node::E_TYPE_MATERIALS:
            {
                pElement = buildXMLMaterials(pDoc, pParentElem, pNode);
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
        default:
            {
                pElement = pParentElem;
            }
            break;
        }

        size_t i = 0;
        for (i = 0; i < pNode->getChildrenCount(); ++i)
        {
            Node *pChild = pNode->getChild(i);
            populateXMLNode(pDoc, pElement, pChild);
        }

        return pElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLModel(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pElement = pDoc->NewElement(TAG_MODEL);
        pElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        pParentElem->LinkEndChild(pElement);
        return pElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLMesh(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        Mesh *pMesh = (Mesh *)pNode;
        XMLElement *pMeshElement = pDoc->NewElement(TAG_MESH);
        pParentElem->LinkEndChild(pMeshElement);

        pMeshElement->SetAttribute(ATTRIB_ID, pMesh->getID().c_str());

        // 属性
        XMLElement *pAttribRootElement = pDoc->NewElement(TAG_ATTRIBUTES);
        pAttribRootElement->SetAttribute(ATTRIB_COUNT, pMesh->mAttributes.size());
        pMeshElement->LinkEndChild(pAttribRootElement);

        int nVertexSize = 0;
        auto itrAttrib = pMesh->mAttributes.begin();

        while (itrAttrib != pMesh->mAttributes.end())
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
        XMLElement *pVertexElement = pDoc->NewElement(TAG_VERTICES);
        pMeshElement->LinkEndChild(pVertexElement);
        pVertexElement->SetAttribute(ATTRIB_COUNT, pMesh->mVertices.size());

        auto itrVertex = pMesh->mVertices.begin();

        int count = 0;

        while (itrVertex != pMesh->mVertices.end())
        {
            std::stringstream ss;
            const Vertex &vertex = *itrVertex;

            // POSITION
            if (count == 0)
                ss<<"\n"<<"\t\t\t\t"<<vertex.mPosition[0]<<" "<<vertex.mPosition[1]<<" "<<vertex.mPosition[2];
            else
                ss<<"\t\t\t\t"<<vertex.mPosition[0]<<" "<<vertex.mPosition[1]<<" "<<vertex.mPosition[2];

            // TEXCOORD
            auto itr2 = vertex.mTexElements.begin();
            while (itr2 != vertex.mTexElements.end())
            {
                const FbxVector2 &uv = *itr2;
                ss<<" "<<uv[0]<<" "<<uv[1];
                ++itr2;
            }

            // NORMAL
            auto itr3 = vertex.mNormalElements.begin();
            while (itr3 != vertex.mNormalElements.end())
            {
                const FbxVector3 &normal = *itr3;
                ss<<" "<<normal[0]<<" "<<normal[1]<<" "<<normal[2];
                ++itr3;
            }

            // BINORMAL
            itr3 = vertex.mBinormalElements.begin();
            while (itr3 != vertex.mBinormalElements.end())
            {
                const FbxVector3 &binormal = *itr3;
                ss<<" "<<binormal[0]<<" "<<binormal[1]<<" "<<binormal[2];
                ++itr3;
            }

            // TANGENT
            itr3 = vertex.mTangentElements.begin();
            while (itr3 != vertex.mTangentElements.end())
            {
                const FbxVector3 &tangent = *itr3;
                ss<<" "<<tangent[0]<<" "<<tangent[1]<<" "<<tangent[2];
                ++itr3;
            }

            // COLOR
            auto itr4 = vertex.mColorElements.begin();
            while (itr4 != vertex.mColorElements.end())
            {
                const FbxVector4 &color = *itr4;
                ss<<" "<<color[0]<<" "<<color[1]<<" "<<color[2]<<" "<<color[3];
                ++itr4;
            }

            // BLEND_WEIGHT 只写权重最大的4个顶点
            ss<<" ";
            const int MAX_BLEND_COUNT = 4;
            int i = 0;
            auto itrBlend = vertex.mBlendInfo.rbegin();
            while (itrBlend != vertex.mBlendInfo.rend())
            {
                ss<<itrBlend->second.mBlendWeight;
                ++i;
                ++itrBlend;

                if (i >= MAX_BLEND_COUNT)
                {
                    break;
                }

                ss<<" ";
            }

            while (i < MAX_BLEND_COUNT)
            {
                ss<<"0";
                ++i;
                ss<<" ";
            }

            // BLEND_INDEX 只写权重最大的4个顶点
            i = 0;
            itrBlend = vertex.mBlendInfo.rbegin();
            while (itrBlend != vertex.mBlendInfo.rend())
            {
                ss<<itrBlend->second.mBlendIndex;
                ++i;
                ++itrBlend;

                if (i >= MAX_BLEND_COUNT)
                {
                    break;
                }

                ss<<" ";
            }

            while (i < MAX_BLEND_COUNT)
            {
                ss<<"-1";
                ++i;
                if (i < MAX_BLEND_COUNT)
                    ss<<" ";
            }

            ss<<"\n";
            XMLText *pText = pDoc->NewText(ss.str().c_str());

            pVertexElement->LinkEndChild(pText);

            ++itrVertex;

            ++count;
        }

        XMLText *pText = pDoc->NewText("\t\t\t");
        pVertexElement->LinkEndChild(pText);

        return pMeshElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLSubMeshes(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pSubmeshesElem = pDoc->NewElement(TAG_PARTS);
        pParentElem->LinkEndChild(pSubmeshesElem);

        size_t nChildrenCount = pNode->getChildrenCount();
        pSubmeshesElem->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        pSubmeshesElem->SetAttribute(ATTRIB_COUNT, nChildrenCount);

        return pSubmeshesElem;
    }

    XMLElement *T3DXMLSerializer::buildXMLSubMesh(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pSubmeshElement = pDoc->NewElement(TAG_PART);
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
        ss<<"\n\t\t\t\t\t\t";
        size_t i = 0;
        auto itr = pSubMesh->mIndices.begin();
        while (itr != pSubMesh->mIndices.end())
        {
            int nIndex = *itr;
            ss<<nIndex<<" ";
            
            if (i == 32)
            {
                ss<<"\n\t\t\t\t\t\t";
                XMLText *pText = pDoc->NewText(ss.str().c_str());
                pIndicesElement->LinkEndChild(pText);
                ss.clear();
                ss.str("");
                i = 0;
            }

            ++i;
            ++itr;
        }

        if ((nIndexCount & 31) != 0)
        {
            /// 有多余的，需要重新写回去
            XMLText *pText = pDoc->NewText(ss.str().c_str());
            pIndicesElement->LinkEndChild(pText);
        }

        ss<<"\n\t\t\t\t\t";
        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pIndicesElement->LinkEndChild(pText);

        return pSubmeshElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLMaterials(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pMatsElement = pDoc->NewElement(TAG_MATERIALS);
        pParentElem->LinkEndChild(pMatsElement);

        pMatsElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        pMatsElement->SetAttribute(ATTRIB_COUNT, pNode->getChildrenCount());

        return pMatsElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLMaterial(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pMatElement = pDoc->NewElement(TAG_MATERIAL);
        pParentElem->LinkEndChild(pMatElement);

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
        ss<<pMaterial->mAmbientColor[0]<<" ";
        ss<<pMaterial->mAmbientColor[1]<<" ";
        ss<<pMaterial->mAmbientColor[2]<<" ";
        ss<<pMaterial->mAmbientColor[3];
        pText = pDoc->NewText(ss.str().c_str());
        pAmbientElement->LinkEndChild(pText);

        // diffuse
        XMLElement *pDiffuseElement = pDoc->NewElement(TAG_DIFFUSE);
        pMatElement->LinkEndChild(pDiffuseElement);
        ss.clear();
        ss.str("");
        ss<<pMaterial->mDiffuseColor[0]<<" ";
        ss<<pMaterial->mDiffuseColor[1]<<" ";
        ss<<pMaterial->mDiffuseColor[2]<<" ";
        ss<<pMaterial->mDiffuseColor[3];
        pText = pDoc->NewText(ss.str().c_str());
        pDiffuseElement->LinkEndChild(pText);

        // specular
        XMLElement *pSpecularElement = pDoc->NewElement(TAG_SPECULAR);
        pMatElement->LinkEndChild(pSpecularElement);
        ss.clear();
        ss.str("");
        ss<<pMaterial->mSpecularColor[0]<<" ";
        ss<<pMaterial->mSpecularColor[1]<<" ";
        ss<<pMaterial->mSpecularColor[2]<<" ";
        ss<<pMaterial->mSpecularColor[3];
        pText = pDoc->NewText(ss.str().c_str());
        pSpecularElement->LinkEndChild(pText);

        // emissive
        XMLElement *pEmissiveElement = pDoc->NewElement(TAG_EMISSIVE);
        pMatElement->LinkEndChild(pEmissiveElement);
        ss.clear();
        ss.str("");
        ss<<pMaterial->mEmissiveColor[0]<<" ";
        ss<<pMaterial->mEmissiveColor[1]<<" ";
        ss<<pMaterial->mEmissiveColor[2]<<" ";
        ss<<pMaterial->mEmissiveColor[3];
        pText = pDoc->NewText(ss.str().c_str());
        pEmissiveElement->LinkEndChild(pText);

        // shininess
        XMLElement *pShinElement = pDoc->NewElement(TAG_SHININESS);
        pMatElement->LinkEndChild(pShinElement);
        ss.clear();
        ss.str("");
        ss<<pMaterial->mShininess;
        pText = pDoc->NewText(ss.str().c_str());
        pShinElement->LinkEndChild(pText);

        // transparency
        XMLElement *pTransElement = pDoc->NewElement(TAG_TRANSPARENCY);
        pMatElement->LinkEndChild(pTransElement);
        ss.clear();
        ss.str("");
        ss<<pMaterial->mTransparency;
        pText = pDoc->NewText(ss.str().c_str());
        pTransElement->LinkEndChild(pText);

        // reflection
        XMLElement *pReflectElement = pDoc->NewElement(TAG_REFLECTION);
        pMatElement->LinkEndChild(pReflectElement);
        ss.clear();
        ss.str("");
        ss<<pMaterial->mReflection;
        pText = pDoc->NewText(ss.str().c_str());
        pReflectElement->LinkEndChild(pText);

        return pMatElement;
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

                std::stringstream ss;
                ss<<pFrame->x<<" "<<pFrame->y<<" "<<pFrame->z;
                XMLText *pText = pDoc->NewText(ss.str().c_str());
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

                std::stringstream ss;
                ss<<pFrame->x<<" "<<pFrame->y<<" "<<pFrame->z<<" "<<pFrame->w;
                XMLText *pText = pDoc->NewText(ss.str().c_str());
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

                std::stringstream ss;
                ss<<pFrame->x<<" "<<pFrame->y<<" "<<pFrame->z;
                XMLText *pText = pDoc->NewText(ss.str().c_str());
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

        return pSkelElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLSkin(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pSkinElement = pDoc->NewElement(TAG_SKIN);
        pParentElem->LinkEndChild(pSkinElement);

        pSkinElement->SetAttribute(ATTRIB_ID, pNode->getID().c_str());
        pSkinElement->SetAttribute(ATTRIB_COUNT, pNode->getChildrenCount());

        return pSkinElement;
    }

    XMLElement *T3DXMLSerializer::buildXMLBone(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode)
    {
        XMLElement *pBoneElement = pDoc->NewElement(TAG_BONE);
        pParentElem->LinkEndChild(pBoneElement);

        Bone *pBone = (Bone *)pNode;
        pBoneElement->SetAttribute(ATTRIB_ID, pBone->getID().c_str());

        XMLElement *pTransformElement = pDoc->NewElement(TAG_TRANSFORM);
        pBoneElement->LinkEndChild(pTransformElement);

        std::stringstream ss;
        ss<<pBone->mLocalTransform[0][0]<<" "<<pBone->mLocalTransform[1][0]<<" "<<pBone->mLocalTransform[2][0]<<" "<<pBone->mLocalTransform[3][0]<<" ";
        ss<<pBone->mLocalTransform[0][1]<<" "<<pBone->mLocalTransform[1][1]<<" "<<pBone->mLocalTransform[2][1]<<" "<<pBone->mLocalTransform[3][1]<<" ";
        ss<<pBone->mLocalTransform[0][2]<<" "<<pBone->mLocalTransform[1][2]<<" "<<pBone->mLocalTransform[2][2]<<" "<<pBone->mLocalTransform[3][2]<<" ";
        ss<<pBone->mLocalTransform[0][3]<<" "<<pBone->mLocalTransform[1][3]<<" "<<pBone->mLocalTransform[2][3]<<" "<<pBone->mLocalTransform[3][3];
        XMLText *pText = pDoc->NewText(ss.str().c_str());
        pTransformElement->LinkEndChild(pText);

        return pBoneElement;
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
            String dstPath = path + ".t3db";
            result = mBinSerializer->save(dstPath, pData);
        }

        if (mXMLSerializer != nullptr)
        {
            String dstPath = path + ".t3dt";
            result = result && mXMLSerializer->save(dstPath, pData);
        }

        return result;
    }
}
