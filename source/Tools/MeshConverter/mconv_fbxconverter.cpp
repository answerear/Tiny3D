

#include "mconv_fbxconverter.h"
#include "mconv_command.h"
#include "mconv_settings.h"
#include "mconv_fbxserializer.h"
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
    FBXConverter::FBXConverter(const Settings &settings)
        : ConverterImpl(settings)
        , mImporter(nullptr)
        , mExporter(nullptr)
        , mSrcData(nullptr)
        , mCurScene(nullptr)
        , mCurModel(nullptr)
        , mCurMesh(nullptr)
        , mCurSkin(nullptr)
        , mCurSkeleton(nullptr)
        , mCurAnimation(nullptr)
        , mCurMaterials(nullptr)
        , mCurBound(nullptr)
        , mHasSkeleton(false)
        , mHasVertexBlending(false)
        , mHasAnimation(false)
#ifdef _DEBUG
        , mTabCount(0)
#endif
    {

    }

    FBXConverter::~FBXConverter()
    {
        delete mImporter;
        mImporter = nullptr;

        delete mExporter;
        mExporter = nullptr;
    }

    bool FBXConverter::importScene()
    {
        bool result = false;

        delete mImporter;
        mImporter = nullptr;

        if ((mSettings.mSrcType & E_FILETYPE_FBX)
            || (mSettings.mSrcType & E_FILETYPE_DAE))
        {
            mImporter = new FbxSerializer();
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

    bool FBXConverter::exportScene()
    {
        delete mExporter;
        mExporter = nullptr;

        if ((mSettings.mDstType & E_FILETYPE_T3D) == E_FILETYPE_T3D)
        {
            mExporter = new T3DSerializer();
        }
        else if (mSettings.mDstType & E_FILETYPE_TMB)
        {
            mExporter = new T3DBinSerializer();
        }
        else if (mSettings.mDstType & E_FILETYPE_TMT)
        {
            mExporter = new T3DXMLSerializer();
        }
        else
        {
            T3D_LOG_ERROR("Create exporter failed ! Because of invalid destination file format !");
            T3D_ASSERT(0);
        }

        bool result = false;
        
        if (mExporter != nullptr)
        {
//             result = mExporter->save(mSettings.mDstPath, mDstData);
            if (E_FM_SPLIT_MESH == mSettings.mFileMode)
            {
                auto itr = mSceneList.begin();
                while (itr != mSceneList.end())
                {
                    const SceneInfo &sceneInfo = *itr;
                    String dstPath = mSettings.mDstPath + sceneInfo.mName;
                    result = mExporter->save(dstPath, sceneInfo.mRoot);
                    ++itr;
                }
            }
            else
            {
                const SceneInfo &sceneInfo = mSceneList.front();
                result = mExporter->save(mSettings.mDstPath, sceneInfo.mRoot);
            }
        }

        return result;
    }

    bool FBXConverter::convertToT3D()
    {
        if (mSrcData == nullptr)
        {
            T3D_LOG_ERROR("Convert to T3D failed ! Because of invalid source data !");
            return false;
        }

        FbxScene *pFbxScene = static_cast<FbxScene *>(mSrcData);

        String name = pFbxScene->GetName();

        if (name.empty())
        {
            name = "Scene";
        }

        Scene *pScene = new Scene(name);
        mCurScene = pScene;
        processFbxScene(pFbxScene, pScene);

        return true;
    }

    void FBXConverter::cleanup()
    {
        //         Node *pNode = (Node *)mDstData;
        auto itr = mSceneList.begin();
        while (itr != mSceneList.end())
        {
            const SceneInfo &info = *itr;
            Node *pNode = (Node *)info.mRoot;
            pNode->removeAllChildren();
            ++itr;
        }
    }

    bool FBXConverter::processFbxScene(FbxScene *pFbxScene, Node *pRoot)
    {
        FbxNode *pFbxRoot = pFbxScene->GetRootNode();
        bool result = processFbxNode(pFbxRoot, pRoot);

        if (E_FM_SHARE_VERTEX == mSettings.mFileMode)
        {
            result = result && optimizeMesh(pRoot);
        }

        if (E_FM_SPLIT_MESH != mSettings.mFileMode)
        {
            // 如果不是分割模型文件模式，则调整骨骼、动画和材质数据到最后
            if (mCurSkeleton != nullptr)
            {
                mCurSkeleton->removeFromParent(false);
                mCurModel->addChild(mCurSkeleton);
            }

            if (mCurAnimation != nullptr)
            {
                mCurAnimation->removeFromParent(false);
                mCurModel->addChild(mCurAnimation);
            }

            if (mCurMaterials != nullptr)
            {
                mCurMaterials->removeFromParent(false);
                mCurModel->addChild(mCurMaterials);
            }

            if (mCurBound != nullptr)
            {
                mCurBound->removeFromParent(false);
                mCurModel->addChild(mCurBound);
            }

            if (E_FM_SHARE_VERTEX == mSettings.mFileMode)
            {
                Model *pModel = (Model *)mCurModel;
                pModel->mMeshCount = 1;
            }
            else
            {
                size_t count = 0;
                size_t i = 0;
                for (i = 0; i < mCurModel->getChildrenCount(); ++i)
                {
                    Node *pChild = mCurModel->getChild(i);
                    if (pChild->getNodeType() == Node::E_TYPE_MESH)
                    {
                        count++;
                    }
                }

                Model *pModel = (Model *)mCurModel;
                pModel->mMeshCount = count;
            }
        }
        else
        {
            Model *pModel = (Model *)mCurModel;
            pModel->mMeshCount = 1;
        }

        return result;
    }

    bool FBXConverter::processFbxNode(FbxNode *pFbxNode, Node *pParent)
    {
        bool result = false;
        Node *pNode = nullptr;

        int nAttribCount = pFbxNode->GetNodeAttributeCount();
        int i = 0;
        for (i = 0; i < nAttribCount; ++i)
        {
            if (pFbxNode->GetNodeAttribute() != nullptr)
            {
                FbxNodeAttribute::EType attribType = pFbxNode->GetNodeAttribute()->GetAttributeType();
                switch (attribType)
                {
                case FbxNodeAttribute::eMesh:
                    {
                        String name = pFbxNode->GetName();
                        if (name.empty())
                        {
                            name = "Model";
                        }

                        if (E_FM_SPLIT_MESH == mSettings.mFileMode)
                        {
                            if (!mSceneList.empty())
                            {
                                // 不是第一个场景节点，需要重新创建，第一个场景节点已经在外部创建了
                                Scene *pScene = new Scene(name);
                                mCurScene = pScene;
                            }

                            String sceneName = "_" + name;
                            SceneInfo info = {mCurScene, sceneName};
                            mSceneList.push_back(info);
                            pParent = mCurScene;

                            Model *pModel = new Model(name);
                            pModel->mSharedVertex = true;
                            pParent->addChild(pModel);
                            mCurModel = pModel;
                        }
                        else
                        {
                            if (mCurModel == nullptr)
                            {
                                SceneInfo info = {mCurScene, mCurScene->getID()};
                                mSceneList.push_back(info);

                                Model *pModel = new Model(name);

                                if (E_FM_SHARE_VERTEX == mSettings.mFileMode)
                                {
                                    pModel->mSharedVertex = true;
                                }
                                else
                                {
                                    pModel->mSharedVertex = false;
                                }

                                pParent->addChild(pModel);
                                mCurModel = pModel;
                            }
                        }

                        mCurSkin = nullptr;

                        result = processFbxMesh(pFbxNode, mCurModel, pNode);
                        result = result && processFbxSkin(pFbxNode, pParent, (Mesh *)pNode);
                        result = result && processFbxMaterial(pFbxNode, mCurModel);

                        if (E_FM_SHARE_VERTEX != mSettings.mFileMode)
                        {
                            result = result && optimizeMesh(pNode);
                        }

                        pNode = mCurModel;
                    }
                    break;
                case FbxNodeAttribute::eSkeleton:
                    {
                    }
                    break;
                case FbxNodeAttribute::eCamera:
                    {
                        result = processFbxCamera(pFbxNode, pParent, pNode);
                    }
                    break;
                case FbxNodeAttribute::eLight:
                    {
                        result = processFbxLight(pFbxNode, pParent, pNode);
                    }
                    break;
                default:
                    {
                    }
                    break;
                }
            }
        }

        if (pNode == nullptr)
        {
            pNode = pParent;
            result = true;
        }

#ifdef _DEBUG
        mTabCount++;
#endif

        if (result)
        {
            for (i = 0; i < pFbxNode->GetChildCount(); ++i)
            {
                result = result && processFbxNode(pFbxNode->GetChild(i), pNode);
            }
        }

#ifdef _DEBUG
        mTabCount--;
#endif

        return result;
    }

    bool FBXConverter::processFbxMesh(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode)
    {
        FbxMesh *pFbxMesh = pFbxNode->GetMesh();

        if (pFbxMesh == nullptr)
        {
            T3D_LOG_ERROR("FBX mesh is invalid !");
            return false;
        }

#ifdef _DEBUG
        std::stringstream ssTab;
        for (int t = 0; t < mTabCount; ++t)
        {
            ssTab<<"\t";
        }
        T3D_LOG_INFO("%sMesh : %s, 0x%p", ssTab.str().c_str(), pFbxNode->GetName(), pFbxMesh);
#endif

        String name = pFbxNode->GetName();
        if (name.empty())
        {
            name = "Mesh";
        }

        Mesh *pMesh = nullptr;
        VertexBuffers *pVBS = nullptr;
        VertexBuffer *pVB = nullptr;

        if (E_FM_SHARE_VERTEX == mSettings.mFileMode)
        {
            if (mCurMesh == nullptr)
            {
                name = pParent->getID();

                pMesh = new Mesh(name);
                pParent->addChild(pMesh);
                pNewNode = pMesh;
                mCurMesh = pMesh;
                mHasVertexBlending = false;

                pVBS = new VertexBuffers(name);
                pMesh->addChild(pVBS);

                pVB = new VertexBuffer("0");
                pVBS->addChild(pVB);
            }
            else
            {
                pMesh = (Mesh *)mCurMesh;
                pNewNode = pMesh;

                searchVertexBuffer(pMesh, pVB);
            }
        }
        else
        {
            pMesh = new Mesh(name);
            pParent->addChild(pMesh);
            pNewNode = pMesh;
            mCurMesh = pMesh;
            mHasVertexBlending = false;

            pVBS = new VertexBuffers(name);
            pMesh->addChild(pVBS);

            pVB = new VertexBuffer("0");
            pVBS->addChild(pVB);
        }

        FbxVector4 T = pFbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        FbxVector4 R = pFbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
        FbxVector4 S = pFbxNode->GetGeometricScaling(FbxNode::eSourcePivot);
//         pMesh->mWorldMatrix.SetTRS(T, R, S);
        FbxAMatrix M;
        M.SetTRS(T, R, S);
        convertMatrix(M, pMesh->mWorldMatrix);

        int nTriangleCount = pFbxMesh->GetPolygonCount();
        int nVertexCount = 0;
        int i = 0, j = 0;

        // 生成顶点属性
        if (!processVertexAttribute(pFbxMesh, pVB))
        {
            return false;
        }

        // 构建顶点数据
        for (i = 0; i < nTriangleCount; ++i)
        {
            for (j = 0; j < 3; ++j)
            {
                int nControlPointIdx = pFbxMesh->GetPolygonVertex(i, j);

                Vertex vertex;
                vertex.mCtrlPointIdx = nControlPointIdx;

                // 读取顶点位置信息
                readPosition(pFbxMesh, nControlPointIdx, vertex.mPosition);
//                 FbxVector4 pos(vertex.mPosition);
                Vector3 pos = vertex.mPosition;
//                 pos = pMesh->mWorldMatrix.MultT(pos);
                pos = pMesh->mWorldMatrix * pos;
                vertex.mPosition[0] = pos[0];
                vertex.mPosition[1] = pos[1];
                vertex.mPosition[2] = pos[2];

                // 读取顶点颜色信息
//                 FbxVector4 color;
                Vector4 color;
                int k = 0;
                bool ret = false;
                do 
                {
                    ret = readColor(pFbxMesh, nControlPointIdx, nVertexCount, k++, color);
                    if (ret)
                    {
                        vertex.mColorElements.push_back(color);
                    }
                } while (ret);

                // 读取纹理UV坐标
//                 FbxVector2 uv;
                Vector2 uv;
                int nTexCount = pFbxMesh->GetElementUVCount();
                for (k = 0; k < nTexCount; ++k)
                {
                    if (readUV(pFbxMesh, nControlPointIdx, pFbxMesh->GetTextureUVIndex(i, j), k, uv))
                    {
                        vertex.mTexElements.push_back(uv);
                    }
                }

                // 读取顶点法线
//                 FbxVector3 normal;
                Vector3 normal;
                k = 0;
                do 
                {
                    ret = readNormal(pFbxMesh, nControlPointIdx, nVertexCount, k++, normal);
                    if (ret)
                    {
                        vertex.mNormalElements.push_back(normal);
                    }
                } while (ret);

                // 读取副法线
//                 FbxVector3 binormal;
                Vector3 binormal;
                k = 0;
                do
                {
                    ret = readBinormal(pFbxMesh, nControlPointIdx, nVertexCount, k++, binormal);
                    if (ret)
                    {
                        vertex.mBinormalElements.push_back(binormal);
                    }
                } while (ret);

                // 读取切线
//                 FbxVector3 tangent;
                Vector3 tangent;
                k = 0;
                do 
                {
                    ret = readTangent(pFbxMesh, nControlPointIdx, nVertexCount, k++, tangent);
                    if (ret)
                    {
                        vertex.mTangentElements.push_back(tangent);
                    }
                } while (ret);

                int nMaterialIdx = 0;
                if (ret = readMaterial(pFbxMesh, i, nMaterialIdx))
                {
                    vertex.mMaterialIdx = nMaterialIdx;
                }

                pVB->mVertices.push_back(vertex);

                ++nVertexCount;
            }
        }

        return true;
    }

    bool FBXConverter::searchVertexBuffer(Mesh *pMesh, VertexBuffer *&pVertexBuffer)
    {
        bool found = false;

        pVertexBuffer = nullptr;
        size_t count = pMesh->getChildrenCount();
        size_t i = 0;

        for (i = 0; i < count; ++i)
        {
            VertexBuffers *pBuffers = dynamic_cast<VertexBuffers *>(pMesh->getChild(i));

            if (pBuffers != nullptr)
            {
                size_t j = 0;

                for (j = 0; j < pBuffers->getChildrenCount(); ++j)
                {
                    VertexBuffer *pBuffer = dynamic_cast<VertexBuffer *>(pBuffers->getChild(i));
                    
                    if (pBuffer != nullptr)
                    {
                        pVertexBuffer = pBuffer;
                        found = true;
                    }
                }
            }
        }

        return found;
    }

    bool FBXConverter::putVertexAttribute(VertexBuffer *pVertexBuffer, bool bHasAttributes, const VertexAttribute &rkSource, const VertexAttribute &rkOther)
    {
        if (E_FM_SHARE_VERTEX == mSettings.mFileMode)
        {
            if (bHasAttributes)
            {
                if (rkSource != rkOther)
                {
                    T3D_LOG_ERROR("Different vertex format ! Shared vertex file mode could not be used !");
                    return false;
                }
            }
            else
            {
                pVertexBuffer->mAttributes.push_back(rkOther);
            }
        }
        else
        {
            pVertexBuffer->mAttributes.push_back(rkOther);
        }

        return true;
    }

    bool FBXConverter::processVertexAttribute(FbxMesh *pFbxMesh, VertexBuffer *pVertexBuffer)
    {
        int i = 0;

        bool bHasAttributes = (pVertexBuffer->mAttributes.size() > 0);
        auto itr = pVertexBuffer->mAttributes.begin();
        VertexAttribute source;

        // 位置
        VertexAttribute attribute;
        attribute.mVertexType = VertexAttribute::E_VT_POSITION;
        attribute.mSize = 3;
        attribute.mDataType = VertexAttribute::E_VT_FLOAT;
        if (!putVertexAttribute(pVertexBuffer, bHasAttributes, (bHasAttributes ? *itr : source), attribute))
        {
            return false;
        }

        int nControlPointsCount = pFbxMesh->GetControlPointsCount();
        int nUVCount = pFbxMesh->GetElementUVCount();
        int nNormalCount = pFbxMesh->GetElementNormalCount();
        int nBinormalCount = pFbxMesh->GetElementBinormalCount();
        int nTangentCount = pFbxMesh->GetElementTangentCount();

        // UV
        for (i = 0; i < pFbxMesh->GetElementUVCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_TEXCOORD;
            attribute.mSize = 2;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            if (!putVertexAttribute(pVertexBuffer, bHasAttributes, (bHasAttributes ? *(++itr) : source), attribute))
            {
                return false;
            }
        }

        // 法线
        for (i = 0; i < pFbxMesh->GetElementNormalCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_NORMAL;
            attribute.mSize = 3;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            if (!putVertexAttribute(pVertexBuffer, bHasAttributes, (bHasAttributes ? *(++itr) : source), attribute))
            {
                return false;
            }
        }

        // 副法线
        for (i = 0; i < pFbxMesh->GetElementBinormalCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_BINORMAL;
            attribute.mSize = 3;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            if (!putVertexAttribute(pVertexBuffer, bHasAttributes, (bHasAttributes ? *(++itr) : source), attribute))
            {
                return false;
            }
        }

        // 切线
        for (i = 0; i < pFbxMesh->GetElementTangentCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_TANGENT;
            attribute.mSize = 3;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            if (!putVertexAttribute(pVertexBuffer, bHasAttributes, (bHasAttributes ? *(++itr) : source), attribute))
            {
                return false;
            }
        }

        // 颜色
        for (i = 0; i < pFbxMesh->GetElementVertexColorCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_COLOR;
            attribute.mSize = 4;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            if (!putVertexAttribute(pVertexBuffer, bHasAttributes, (bHasAttributes ? *(++itr) : source), attribute))
            {
                return false;
            }
        }

        return true;
    }

//     bool FBXConverter::readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, FbxVector3 &pos)
    bool FBXConverter::readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, Vector3 &pos)
    {
        FbxVector4 *pControlPoint = pFbxMesh->GetControlPoints();
        pos[0] = pControlPoint[nControlPointIdx][0];
        pos[1] = pControlPoint[nControlPointIdx][1];
        pos[2] = pControlPoint[nControlPointIdx][2];
        return true;
    }

//     bool FBXConverter::readColor(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector4 &color)
    bool FBXConverter::readColor(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector4 &color)
    {
        if (pFbxMesh->GetElementVertexColorCount() < 1)
        {
            return false;
        }

        bool result = false;

        FbxGeometryElementVertexColor *pVertexColor = pFbxMesh->GetElementVertexColor(nLayer);
        if (pVertexColor != nullptr)
        {
            result = true;

            switch (pVertexColor->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pVertexColor->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            color[0] = pVertexColor->GetDirectArray().GetAt(nControlPointIdx).mBlue;
                            color[1] = pVertexColor->GetDirectArray().GetAt(nControlPointIdx).mGreen;
                            color[2] = pVertexColor->GetDirectArray().GetAt(nControlPointIdx).mRed;
                            color[3] = pVertexColor->GetDirectArray().GetAt(nControlPointIdx).mAlpha;
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexColor->GetIndexArray().GetAt(nControlPointIdx);
                            color[0] = pVertexColor->GetDirectArray().GetAt(idx).mBlue;
                            color[1] = pVertexColor->GetDirectArray().GetAt(idx).mGreen;
                            color[2] = pVertexColor->GetDirectArray().GetAt(idx).mRed;
                            color[3] = pVertexColor->GetDirectArray().GetAt(idx).mAlpha;
                        }
                        break;
                    }
                }
                break;
            case FbxGeometryElement::eByPolygonVertex:
                {
                    switch (pVertexColor->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            color[0] = pVertexColor->GetDirectArray().GetAt(nVertexIndex).mBlue;
                            color[1] = pVertexColor->GetDirectArray().GetAt(nVertexIndex).mGreen;
                            color[2] = pVertexColor->GetDirectArray().GetAt(nVertexIndex).mRed;
                            color[3] = pVertexColor->GetDirectArray().GetAt(nVertexIndex).mAlpha;
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexColor->GetIndexArray().GetAt(nVertexIndex);
                            color[0] = pVertexColor->GetDirectArray().GetAt(idx).mBlue;
                            color[1] = pVertexColor->GetDirectArray().GetAt(idx).mGreen;
                            color[2] = pVertexColor->GetDirectArray().GetAt(idx).mRed;
                            color[3] = pVertexColor->GetDirectArray().GetAt(idx).mAlpha;
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

//     bool FBXConverter::readUV(FbxMesh *pFbxMesh, int nControlPointIdx, int nUVIndex, int nLayer, FbxVector2 &uv)
    bool FBXConverter::readUV(FbxMesh *pFbxMesh, int nControlPointIdx, int nUVIndex, int nLayer, Vector2 &uv)
    {
        if (pFbxMesh->GetElementUVCount() < 1 || nLayer >= pFbxMesh->GetElementUVCount())
        {
            return false;
        }

        bool result = false;
        FbxGeometryElementUV *pVertexUV = pFbxMesh->GetElementUV(nLayer);

        if (pVertexUV != nullptr)
        {
            result = true;

            switch (pVertexUV->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pVertexUV->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            uv[0] = pVertexUV->GetDirectArray().GetAt(nControlPointIdx)[0];
                            uv[1] = pVertexUV->GetDirectArray().GetAt(nControlPointIdx)[1];
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexUV->GetIndexArray().GetAt(nControlPointIdx);
                            uv[0] = pVertexUV->GetDirectArray().GetAt(idx)[0];
                            uv[1] = pVertexUV->GetDirectArray().GetAt(idx)[1];
                        }
                        break;
                    }
                }
                break;
            case FbxGeometryElement::eByPolygonVertex:
                {
                    switch (pVertexUV->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            uv[0] = pVertexUV->GetDirectArray().GetAt(nUVIndex)[0];
                            uv[1] = pVertexUV->GetDirectArray().GetAt(nUVIndex)[1];
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

//     bool FBXConverter::readNormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &normal)
    bool FBXConverter::readNormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &normal)
    {
        if (pFbxMesh->GetElementNormalCount() < 1)
        {
            return false;
        }

        bool result = false;
        FbxGeometryElementNormal *pNormal = pFbxMesh->GetElementNormal(nLayer);

        if (pNormal != nullptr)
        {
            result = true;

            switch (pNormal->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pNormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            normal[0] = pNormal->GetDirectArray().GetAt(nControlPointIdx)[0];
                            normal[1] = pNormal->GetDirectArray().GetAt(nControlPointIdx)[1];
                            normal[2] = pNormal->GetDirectArray().GetAt(nControlPointIdx)[2];
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pNormal->GetIndexArray().GetAt(nControlPointIdx);
                            normal[0] = pNormal->GetDirectArray().GetAt(idx)[0];
                            normal[1] = pNormal->GetDirectArray().GetAt(idx)[1];
                            normal[2] = pNormal->GetDirectArray().GetAt(idx)[2];
                        }
                        break;
                    }
                }
                break;
            case FbxGeometryElement::eByPolygonVertex:
                {
                    switch (pNormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            normal[0] = pNormal->GetDirectArray().GetAt(nVertexIndex)[0];
                            normal[1] = pNormal->GetDirectArray().GetAt(nVertexIndex)[1];
                            normal[2] = pNormal->GetDirectArray().GetAt(nVertexIndex)[2];
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pNormal->GetIndexArray().GetAt(nVertexIndex);
                            normal[0] = pNormal->GetDirectArray().GetAt(idx)[0];
                            normal[1] = pNormal->GetDirectArray().GetAt(idx)[1];
                            normal[2] = pNormal->GetDirectArray().GetAt(idx)[2];
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

//     bool FBXConverter::readTangent(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &tangent)
    bool FBXConverter::readTangent(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &tangent)
    {
        if (pFbxMesh->GetElementTangentCount() < 1)
        {
            return false;
        }

        bool result = false;
        FbxGeometryElementTangent *pTangent = pFbxMesh->GetElementTangent(nLayer);

        if (pTangent != nullptr)
        {
            result = true;

            switch (pTangent->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pTangent->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            tangent[0] = pTangent->GetDirectArray().GetAt(nControlPointIdx)[0];
                            tangent[1] = pTangent->GetDirectArray().GetAt(nControlPointIdx)[1];
                            tangent[2] = pTangent->GetDirectArray().GetAt(nControlPointIdx)[2];
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pTangent->GetIndexArray().GetAt(nControlPointIdx);
                            tangent[0] = pTangent->GetDirectArray().GetAt(idx)[0];
                            tangent[1] = pTangent->GetDirectArray().GetAt(idx)[1];
                            tangent[2] = pTangent->GetDirectArray().GetAt(idx)[2];
                        }
                        break;
                    }
                }
                break;
            case FbxGeometryElement::eByPolygonVertex:
                {
                    switch (pTangent->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            tangent[0] = pTangent->GetDirectArray().GetAt(nVertexIndex)[0];
                            tangent[1] = pTangent->GetDirectArray().GetAt(nVertexIndex)[1];
                            tangent[2] = pTangent->GetDirectArray().GetAt(nVertexIndex)[2];
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pTangent->GetIndexArray().GetAt(nVertexIndex);
                            tangent[0] = pTangent->GetDirectArray().GetAt(idx)[0];
                            tangent[1] = pTangent->GetDirectArray().GetAt(idx)[1];
                            tangent[2] = pTangent->GetDirectArray().GetAt(idx)[2];
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

//     bool FBXConverter::readBinormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &binormal)
    bool FBXConverter::readBinormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, Vector3 &binormal)
    {
        if (pFbxMesh->GetElementBinormalCount() < 1)
        {
            return false;
        }

        bool result = false;
        FbxGeometryElementBinormal *pBinormal = pFbxMesh->GetElementBinormal(nLayer);

        if (pBinormal != nullptr)
        {
            result = true;

            switch (pBinormal->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pBinormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            binormal[0] = pBinormal->GetDirectArray().GetAt(nControlPointIdx)[0];
                            binormal[1] = pBinormal->GetDirectArray().GetAt(nControlPointIdx)[1];
                            binormal[2] = pBinormal->GetDirectArray().GetAt(nControlPointIdx)[2];
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pBinormal->GetIndexArray().GetAt(nControlPointIdx);
                            binormal[0] = pBinormal->GetDirectArray().GetAt(idx)[0];
                            binormal[1] = pBinormal->GetDirectArray().GetAt(idx)[1];
                            binormal[2] = pBinormal->GetDirectArray().GetAt(idx)[2];
                        }
                        break;
                    }
                }
                break;
            case FbxGeometryElement::eByPolygonVertex:
                {
                    switch (pBinormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            binormal[0] = pBinormal->GetDirectArray().GetAt(nVertexIndex)[0];
                            binormal[1] = pBinormal->GetDirectArray().GetAt(nVertexIndex)[1];
                            binormal[2] = pBinormal->GetDirectArray().GetAt(nVertexIndex)[2];
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pBinormal->GetIndexArray().GetAt(nVertexIndex);
                            binormal[0] = pBinormal->GetDirectArray().GetAt(idx)[0];
                            binormal[1] = pBinormal->GetDirectArray().GetAt(idx)[1];
                            binormal[2] = pBinormal->GetDirectArray().GetAt(idx)[2];
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

    bool FBXConverter::readMaterial(FbxMesh *pFbxMesh, int nTriangleIndex, int &nMaterialIndex)
    {
        bool result = false;

        if (pFbxMesh->GetElementMaterial() != nullptr)
        {
            FbxLayerElementArrayTemplate<int> &indices = pFbxMesh->GetElementMaterial()->GetIndexArray();
            FbxGeometryElement::EMappingMode eMappingMode = pFbxMesh->GetElementMaterial()->GetMappingMode();

            if (indices.GetCount() > 0)
            {
                result = true;

                switch (eMappingMode)
                {
                case FbxGeometryElement::eByPolygon:
                    {
                        nMaterialIndex = indices.GetAt(nTriangleIndex);
                    }
                    break;
                case FbxGeometryElement::eAllSame:
                    {
                        nMaterialIndex = indices.GetAt(0);
                    }
                    break;
                }
            }
        }
        
        return result;
    }

    bool FBXConverter::processFbxSkeleton(FbxNode *pFbxNode, Node *pParent, Model *pModel)
    {
#ifdef _DEBUG
        std::stringstream ssTab;
        for (int t = 0; t < mTabCount; ++t)
        {
            ssTab<<"\t";
        }
        FbxGeometry *pFbxGeometry = pFbxNode->GetGeometry();

        T3D_LOG_INFO("%sSkeleton : %s, 0x%p", ssTab.str().c_str(), pFbxNode->GetName(), pFbxNode);
#endif

        FbxSkeleton *pFbxSkel = pFbxNode->GetSkeleton();
        if (!mHasSkeleton)
        {
            String name = pFbxNode->GetName();
            if (name.empty())
            {
                name = "skeleton";
            }
            Skeleton *pSkel = new Skeleton(name);
            pParent->addChild(pSkel);
            pParent = pSkel;
            SkeletonsValue value(pFbxNode, pSkel);
            mSkeletons.insert(value);
            mHasSkeleton = true;

            mCurSkeleton = pSkel;
        }

        String name = pFbxNode->GetName();
        if (name.empty())
        {
            name = "bone";
        }
        Bone *pBone = new Bone(name);
        pParent->addChild(pBone);

        FbxAMatrix &M = pFbxNode->EvaluateLocalTransform();
//         pBone->mLocalTransform = M;
        convertMatrix(M, pBone->mLocalTransform);
        mTabCount++;

        processFbxAnimation(pFbxNode, pModel);

        int i = 0;
        int nChildrenCount = pFbxNode->GetChildCount();
        for (i = 0; i < nChildrenCount; ++i)
        {
            processFbxSkeleton(pFbxNode->GetChild(i), pBone, pModel);
        }

        mTabCount--;

        return true;
    }

    bool FBXConverter::processFbxCamera(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode)
    {
#ifdef _DEBUG
        std::stringstream ssTab;
        for (int t = 0; t < mTabCount; ++t)
        {
            ssTab<<"\t";
        }
        T3D_LOG_INFO("%sCamera : %s", ssTab.str().c_str(), pFbxNode->GetName());
#endif

        return true;
    }

    bool FBXConverter::processFbxLight(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode)
    {
#ifdef _DEBUG
        std::stringstream ssTab;
        for (int t = 0; t < mTabCount; ++t)
        {
            ssTab<<"\t";
        }
        T3D_LOG_INFO("%sLight : %s", ssTab.str().c_str(), pFbxNode->GetName());
#endif

        return true;
    }

    bool FBXConverter::processFbxMaterial(FbxNode *pFbxNode, Node *pParent)
    {
        int nMaterialCount = pFbxNode->GetMaterialCount();
        int i = 0;

#ifdef _DEBUG
        std::stringstream ssTab;
        for (i = 0; i < mTabCount; ++i)
        {
            ssTab<<"\t";
        }
#endif

        if (E_FM_SPLIT_MESH != mSettings.mFileMode)
        {
            if (mCurMaterials == nullptr)
            {
                Materials *pMaterials = new Materials("Materials");
                pParent->addChild(pMaterials);
                mCurMaterials = pMaterials;
            }
        }
        else
        {
            Materials *pMaterials = new Materials("Materials");
            pParent->addChild(pMaterials);
            mCurMaterials = pMaterials;
        }

        for (i = 0; i < nMaterialCount; ++i)
        {
            FbxSurfaceMaterial *pFbxMaterial = pFbxNode->GetMaterial(i);

            Material *pMaterial = nullptr;
            if (!searchMaterial(pFbxMaterial->GetName(), pMaterial))
            {
                pMaterial = new Material(pFbxMaterial->GetName());
                mCurMaterials->addChild(pMaterial);
#ifdef _DEBUG
                T3D_LOG_INFO("%sMaterial : %s", ssTab.str().c_str(), pFbxMaterial->GetName());
#endif

                if (pFbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
                {
                    FbxSurfacePhong *pFbxMatPhong = (FbxSurfacePhong *)pFbxMaterial;
                    pMaterial->mMode = "Phong";
                    FbxDouble3 value = pFbxMatPhong->Ambient.Get();
                    pMaterial->mAmbientColor[0] = value[2];
                    pMaterial->mAmbientColor[1] = value[1];
                    pMaterial->mAmbientColor[2] = value[0];
                    pMaterial->mAmbientColor[3] = pFbxMatPhong->AmbientFactor;
                    value = pFbxMatPhong->Diffuse.Get();
                    pMaterial->mDiffuseColor[0] = value[2];
                    pMaterial->mDiffuseColor[1] = value[1];
                    pMaterial->mDiffuseColor[2] = value[0];
                    pMaterial->mDiffuseColor[3] = pFbxMatPhong->DiffuseFactor;
                    value = pFbxMatPhong->Specular.Get();
                    pMaterial->mSpecularColor[0] = value[2];
                    pMaterial->mSpecularColor[1] = value[1];
                    pMaterial->mSpecularColor[2] = value[0];
                    pMaterial->mSpecularColor[3] = pFbxMatPhong->SpecularFactor;
                    value = pFbxMatPhong->Emissive.Get();
                    pMaterial->mEmissiveColor[0] = value[2];
                    pMaterial->mEmissiveColor[1] = value[1];
                    pMaterial->mEmissiveColor[2] = value[0];
                    pMaterial->mEmissiveColor[3] = pFbxMatPhong->EmissiveFactor;
                    pMaterial->mShininess = pFbxMatPhong->Shininess;
                    pMaterial->mTransparency = pFbxMatPhong->TransparencyFactor;
                    pMaterial->mReflection = pFbxMatPhong->ReflectionFactor;

                    Textures *pTextures = nullptr;

                    int nAmbientTexCount = pFbxMatPhong->Ambient.GetSrcObjectCount();
                    int nDiffuseTexCount = pFbxMatPhong->Diffuse.GetSrcObjectCount();
                    int nEmissiveTexCount = pFbxMatPhong->Emissive.GetSrcObjectCount();

                    if (nAmbientTexCount > 0 || nDiffuseTexCount > 0)
                    {
                        pTextures = new Textures("Textures");
                        pMaterial->addChild(pTextures);

                        int i = 0;

                        // Ambient texture
                        for (i = 0; i < nAmbientTexCount; ++i)
                        {
                            FbxFileTexture *pFbxTexture = (FbxFileTexture *)pFbxMatPhong->Ambient.GetSrcObject(i);
                            if (pFbxTexture != nullptr)
                            {
                                Texture *pTexture = new Texture(pFbxTexture->GetName());
                                pTextures->addChild(pTexture);

                                pTexture->mFilename = pFbxTexture->GetFileName();
                                pTexture->mType = "ambient";
                                pTexture->mWrapModeU = FbxWrapModeToString(pFbxTexture->WrapModeU.Get());
                                pTexture->mWrapModeV = FbxWrapModeToString(pFbxTexture->WrapModeV.Get());
                            }
                        }

                        // Diffuse texture
                        for (i = 0; i < nDiffuseTexCount; ++i)
                        {
                            FbxFileTexture *pFbxTexture = (FbxFileTexture *)pFbxMatPhong->Diffuse.GetSrcObject(i);
                            if (pFbxTexture != nullptr)
                            {
                                Texture *pTexture = new Texture(pFbxTexture->GetName());
                                pTextures->addChild(pTexture);

                                pTexture->mFilename = pFbxTexture->GetFileName();
                                pTexture->mType = "diffuse";
                                pTexture->mWrapModeU = FbxWrapModeToString(pFbxTexture->WrapModeU.Get());
                                pTexture->mWrapModeV = FbxWrapModeToString(pFbxTexture->WrapModeV.Get());
                            }
                        }
                    }
                }
                else if (pFbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
                {
                    FbxSurfaceLambert *pFbxMatLambert = (FbxSurfaceLambert *)pFbxMaterial;
                    pMaterial->mMode = "Lambert";
                    FbxDouble3 value = pFbxMatLambert->Ambient.Get();
                    pMaterial->mAmbientColor[0] = value[0];
                    pMaterial->mAmbientColor[1] = value[1];
                    pMaterial->mAmbientColor[2] = value[2];
                    pMaterial->mAmbientColor[3] = pFbxMatLambert->AmbientFactor;
                    value = pFbxMatLambert->Diffuse.Get();
                    pMaterial->mDiffuseColor[0] = value[2];
                    pMaterial->mDiffuseColor[1] = value[1];
                    pMaterial->mDiffuseColor[2] = value[0];
                    pMaterial->mDiffuseColor[3] = pFbxMatLambert->DiffuseFactor;
                    pMaterial->mSpecularColor[0] = pMaterial->mSpecularColor[1] = pMaterial->mSpecularColor[2] = 0.0f;
                    value = pFbxMatLambert->Emissive.Get();
                    pMaterial->mEmissiveColor[0] = value[2];
                    pMaterial->mEmissiveColor[1] = value[1];
                    pMaterial->mEmissiveColor[2] = value[0];
                    pMaterial->mEmissiveColor[3] = pFbxMatLambert->EmissiveFactor;
                    pMaterial->mShininess = 0.0f;
                    pMaterial->mTransparency = pFbxMatLambert->TransparencyFactor;
                    pMaterial->mReflection = 0.0f;

                    Textures *pTextures = nullptr;

                    int nAmbientTexCount = pFbxMatLambert->Ambient.GetSrcObjectCount();
                    int nDiffuseTexCount = pFbxMatLambert->Diffuse.GetSrcObjectCount();
                    int nEmissiveTexCount = pFbxMatLambert->Emissive.GetSrcObjectCount();

                    if (nAmbientTexCount > 0 || nDiffuseTexCount > 0 || nEmissiveTexCount > 0)
                    {
                        pTextures = new Textures("Textures");
                        pMaterial->addChild(pTextures);

                        int i = 0;

                        // Ambient texture
                        for (i = 0; i < nAmbientTexCount; ++i)
                        {
                            FbxFileTexture *pFbxTexture = (FbxFileTexture *)pFbxMatLambert->Ambient.GetSrcObject(i);
                            if (pFbxTexture != nullptr)
                            {
                                Texture *pTexture = new Texture(pFbxTexture->GetName());
                                pTextures->addChild(pTexture);

                                pTexture->mFilename = pFbxTexture->GetFileName();
                                pTexture->mType = "ambient";
                                pTexture->mWrapModeU = FbxWrapModeToString(pFbxTexture->WrapModeU.Get());
                                pTexture->mWrapModeV = FbxWrapModeToString(pFbxTexture->WrapModeV.Get());
                            }
                        }

                        // Diffuse texture
                        for (i = 0; i < nDiffuseTexCount; ++i)
                        {
                            FbxFileTexture *pFbxTexture = (FbxFileTexture *)pFbxMatLambert->Diffuse.GetSrcObject(i);
                            if (pFbxTexture != nullptr)
                            {
                                Texture *pTexture = new Texture(pFbxTexture->GetName());
                                pTextures->addChild(pTexture);

                                pTexture->mFilename = pFbxTexture->GetFileName();
                                pTexture->mType = "diffuse";
                                pTexture->mWrapModeU = FbxWrapModeToString(pFbxTexture->WrapModeU.Get());
                                pTexture->mWrapModeV = FbxWrapModeToString(pFbxTexture->WrapModeV.Get());
                            }
                        }

                        // Emissive texture
                        for (i = 0; i < nEmissiveTexCount; ++i)
                        {
                            FbxFileTexture *pFbxTexture = (FbxFileTexture *)pFbxMatLambert->Emissive.GetSrcObject(i);
                            if (pFbxTexture != nullptr)
                            {
                                Texture *pTexture = new Texture(pFbxTexture->GetName());
                                pTextures->addChild(pTexture);

                                pTexture->mFilename = pFbxTexture->GetFileName();
                                pTexture->mType = "emissive";
                                pTexture->mWrapModeU = FbxWrapModeToString(pFbxTexture->WrapModeU.Get());
                                pTexture->mWrapModeV = FbxWrapModeToString(pFbxTexture->WrapModeV.Get());
                            }
                        }
                    }
                }
            }
        }

        return true;
    }

    bool FBXConverter::searchMaterial(const String &name, Material *&pMaterial)
    {
        bool result = false;

        if (E_FM_SPLIT_MESH != mSettings.mFileMode)
        {
            result = mCurMaterials->getChild(name, (Node *&)pMaterial);
        }

        return result;
    }

    bool FBXConverter::processFbxAnimation(FbxNode *pFbxNode, Model *pModel)
    {
#ifdef _DEBUG
        std::stringstream ssTab;
        for (int t = 0; t < mTabCount; ++t)
        {
            ssTab<<"\t";
        }
        T3D_LOG_INFO("%sAnimation : %s 0x%p", ssTab.str().c_str(), pFbxNode->GetName(), pFbxNode);
#endif

        Animation *pAnimation = nullptr;
        if (!mHasAnimation)
        {
            pAnimation = new Animation(pModel->getID());
            pModel->addChild(pAnimation);
            mHasAnimation = true;

            mCurAnimation = pAnimation;
        }
        else
        {
            searchAnimation(pModel->getID(), pModel, pAnimation);
        }

        FbxSkeleton *pFbxSkel = pFbxNode->GetSkeleton();

        String strBoneName = pFbxNode->GetName();

        FbxScene *pFbxScene = (FbxScene *)mSrcData;
        int nAnimStackCount = pFbxScene->GetSrcObjectCount(FbxAnimStack::ClassId);
        int i = 0;
        if (nAnimStackCount > 0)
        {
            ssTab<<"\t";
        }
        for (i = 0; i < nAnimStackCount; ++i)
        {
            FbxAnimStack *pFbxAnimStack = (FbxAnimStack *)pFbxScene->GetSrcObject(FbxAnimStack::ClassId, i);

            String name = pFbxAnimStack->GetName();
            if (name.empty())
            {
                name = "Action";
            }
#ifdef _DEBUG
            T3D_LOG_INFO("%sAction : %s", ssTab.str().c_str(), name.c_str());
#endif

            int nAnimLayerCount = pFbxAnimStack->GetMemberCount();
#ifdef _DEBUG
            std::stringstream ss;
            if (nAnimLayerCount > 0)
            {
                ss<<ssTab.str()<<"\t";
            }
#endif
            
            int j = 0;
            for (j = 0; j < nAnimLayerCount; ++j)
            {
                FbxAnimLayer *pFbxAnimLayer = (FbxAnimLayer *)pFbxAnimStack->GetMember(j);

                Action *pAction = nullptr;
                if (searchAction(name, pAnimation, pAction))
                {

                }
                else
                {
                    pAction = new Action(name);
                    pAnimation->addChild(pAction);
                }

                FbxAnimCurve *pFbxTransCurve = pFbxNode->LclTranslation.GetCurve(pFbxAnimLayer);
                FbxAnimCurve *pFbxRotationCurve = pFbxNode->LclRotation.GetCurve(pFbxAnimLayer);
                FbxAnimCurve *pFbxScaleCurve = pFbxNode->LclScaling.GetCurve(pFbxAnimLayer);

                if (pFbxTransCurve != nullptr || pFbxRotationCurve != nullptr || pFbxScaleCurve != nullptr)
                {
#ifdef _DEBUG
                    T3D_LOG_INFO("%sLayer : %s", ss.str().c_str(), name.c_str());
#endif
                }
                else
                {
                }

                if (pFbxTransCurve != nullptr)
                {
                    int nKeyframeCount = pFbxTransCurve->KeyGetCount();

                    int k = 0;
                    for (k = 0; k < nKeyframeCount; ++k)
                    {
                        KeyframeT *pFrame = new KeyframeT(k);
                        FbxTime frameTime = pFbxTransCurve->KeyGetTime(k);
                        FbxVector4 translate = pFbxNode->EvaluateLocalTranslation(frameTime);
                        pFrame->x = translate[0];
                        pFrame->y = translate[1];
                        pFrame->z = translate[2];
                        pFrame->mTimestamp = frameTime.GetSecondDouble();
                        pAction->addKeyframe(pFrame, strBoneName, pAction->mTKeyframes);

                        if (pAction->mDuration < pFrame->mTimestamp)
                            pAction->mDuration = pFrame->mTimestamp;
                    }
                }
                else
                {

                }

                if (pFbxRotationCurve != nullptr)
                {
                    int nKeyframeCount = pFbxRotationCurve->KeyGetCount();

                    int k = 0;
                    for (k = 0; k < nKeyframeCount; ++k)
                    {
                        FbxTime frameTime = pFbxRotationCurve->KeyGetTime(k);
                        FbxVector4 rotation = pFbxNode->EvaluateLocalRotation(frameTime);
                        FbxQuaternion orientation;
                        orientation.ComposeSphericalXYZ(rotation);
                        KeyframeR *pFrame = new KeyframeR(k);
                        pFrame->x = orientation[0];
                        pFrame->y = orientation[1];
                        pFrame->z = orientation[2];
                        pFrame->w = orientation[3];
                        pFrame->mTimestamp = frameTime.GetSecondDouble();
                        pAction->addKeyframe(pFrame, strBoneName, pAction->mRKeyframes);

                        if (pAction->mDuration < pFrame->mTimestamp)
                            pAction->mDuration = pFrame->mTimestamp;
                    }
                }
                else
                {

                }

                if (pFbxScaleCurve != nullptr)
                {
                    int nKeyframeCount = pFbxScaleCurve->KeyGetCount();

                    int k = 0;
                    for (k = 0; k < nKeyframeCount; ++k)
                    {
                        FbxTime frameTime = pFbxScaleCurve->KeyGetTime(k);
                        FbxVector4 scale = pFbxNode->EvaluateLocalScaling(frameTime);
                        KeyframeS *pFrame = new KeyframeS(k);
                        pFrame->x = scale[0];
                        pFrame->y = scale[1];
                        pFrame->z = scale[2];
                        pFrame->mTimestamp = frameTime.GetSecondDouble();
                        pAction->addKeyframe(pFrame, strBoneName, pAction->mSKeyframes);

                        if (pAction->mDuration < pFrame->mTimestamp)
                            pAction->mDuration = pFrame->mTimestamp;
                    }
                }
                else
                {

                }
            }
        }

        return true;
    }

    bool FBXConverter::processFbxSkin(FbxNode *pFbxNode, Node *pParent, Mesh *pMesh)
    {
#ifdef _DEBUG
        std::stringstream ssTab;
        for (int t = 0; t < mTabCount; ++t)
        {
            ssTab<<"\t";
        }
        T3D_LOG_INFO("%sSkin : %s", ssTab.str().c_str(), pFbxNode->GetName());
#endif

        FbxMesh *pFbxMesh = pFbxNode->GetMesh();
        Model *pModel = (Model *)pMesh->getParent();
        int nDeformerCount = pFbxMesh->GetDeformerCount();

        int i = 0;
        for (i = 0; i < nDeformerCount; ++i)
        {
            FbxDeformer *pFbxDeformer = pFbxMesh->GetDeformer(i);
            if (pFbxDeformer == nullptr)
            {
                continue;
            }

            if (pFbxDeformer->GetDeformerType() != FbxDeformer::eSkin)
            {
                continue;
            }

            FbxSkin *pFbxSkin = (FbxSkin *)pFbxDeformer;
            if (pFbxSkin == nullptr)
            {
                continue;
            }

            int j = 0;
            int nBoneCount = pFbxSkin->GetClusterCount();

#ifdef _DEBUG
            std::stringstream ss;
            if (nBoneCount > 0)
            {
                ss<<ssTab.str()<<"\t";
            }
#endif
            Skin *pSkin = new Skin(pMesh->getID());
            mCurSkin = pSkin;
            pMesh->addChild(pSkin);

            for (j = 0; j < nBoneCount; ++j)
            {
                FbxCluster *pFbxCluster = pFbxSkin->GetCluster(j);
                FbxNode *pFbxLinkNode = pFbxCluster->GetLink();
#ifdef _DEBUG
                T3D_LOG_INFO("%sLink node : %s, 0x%p", ss.str().c_str(), pFbxLinkNode->GetName(), pFbxLinkNode);
#endif

                if (j == 0)
                {
                    FbxNode *pFbxSkelRoot = nullptr;
                    if (searchSkeletonRoot(pFbxLinkNode, pFbxSkelRoot) && !searchSkeleton(pFbxSkelRoot))
                    {
                        mHasSkeleton = false;
                        mHasAnimation = false;
                        processFbxSkeleton(pFbxSkelRoot, pModel, pModel);
                    }
                }
                
                FbxAMatrix matLink;
                pFbxCluster->GetTransformLinkMatrix(matLink);
                FbxAMatrix mat;
                pFbxCluster->GetTransformMatrix(mat);
                FbxVector4 T = pFbxLinkNode->GetGeometricTranslation(FbxNode::eSourcePivot);
                FbxVector4 R = pFbxLinkNode->GetGeometricRotation(FbxNode::eSourcePivot);
                FbxVector4 S = pFbxLinkNode->GetGeometricScaling(FbxNode::eSourcePivot);
                FbxAMatrix matGeometry(T, R, S);
                FbxAMatrix bindpose = (matLink.Inverse() * mat * matGeometry);
//                 FbxAMatrix mp = pFbxLinkNode->EvaluateLocalTransform();
// 
//                 FbxScene *pFbxScene = (FbxScene *)(mSrcData);
//                 const int poseCount = pFbxScene->GetPoseCount();
//                 for (int ii = 0; ii < poseCount; ++ii)
//                 {
//                     FbxPose *pFbxPose = pFbxScene->GetPose(ii);
//                     assert(pFbxPose);
//                     if (pFbxPose->IsBindPose())
//                     {
//                         for (int jj = 0; jj < pFbxPose->GetCount(); ++jj)
//                         {
//                             FbxNode* fbxNode = pFbxPose->GetNode(jj);
//                             if (std::string(fbxNode->GetName()) == "global")
//                             {
//                                 int a = 0;
//                             }
// //                             if (fbxNode->GetMesh() != NULL)
//                             {
//                                 FbxMatrix bindshape = pFbxPose->GetMatrix(jj);
//                                 int a = 0;
//                             }
//                         }
//                         
//                     }
//                 }

                Bone *pBone = new Bone(pFbxLinkNode->GetName());
//                 pBone->mLocalTransform = bindpose;
                convertMatrix(bindpose, pBone->mLocalTransform);
                pSkin->addChild(pBone);

                updateVertexBlendAttributes(pMesh);

                int nCtrlPointCount = pFbxCluster->GetControlPointIndicesCount();
                int *pCtrlPointIndices = pFbxCluster->GetControlPointIndices();
                double *pCtrlPointWeights = pFbxCluster->GetControlPointWeights();
                int k = 0;
                for (k = 0; k < nCtrlPointCount; ++k)
                {
                    int nCtrlPointIdx = pCtrlPointIndices[k];
                    double fCtrlPointWeight = pCtrlPointWeights[k];
                    updateVertexBlendIndexAndWeight(pMesh, nCtrlPointIdx, j, fCtrlPointWeight);
                }
            }
        }

        return true;
    }

    bool FBXConverter::optimizeMesh(Node *pNode)
    {
        if (pNode->getNodeType() == Node::E_TYPE_MESH)
        {
            Mesh *pMesh = (Mesh *)pNode;
            pMesh->split();

            if (mCurSkin != nullptr)
            {
                mCurSkin->removeFromParent(false);
                pMesh->addChild(mCurSkin);
            }

            computeBoundingBox(pMesh);
        }

        size_t i = 0;
        for (i = 0; i < pNode->getChildrenCount(); ++i)
        {
            Node *pChild = pNode->getChild(i);
            optimizeMesh(pChild);
        }

        return true;
    }

    bool FBXConverter::searchSkeletonRoot(FbxNode *pFbxNode, FbxNode *&pFbxRootNode)
    {
        bool result = false;
        FbxScene *pFbxScene = (FbxScene *)mSrcData;
        FbxNode *pFbxSceneRoot = pFbxScene->GetRootNode();
        FbxNode *pFbxParent = pFbxNode;
        pFbxRootNode = nullptr;

        while (pFbxParent != pFbxSceneRoot)
        {
            FbxNodeAttribute *pNodeAttrib = pFbxParent->GetNodeAttribute();
            FbxNodeAttribute::EType attribType = pFbxNode->GetNodeAttribute()->GetAttributeType();
            if (pNodeAttrib == nullptr || 
                (pNodeAttrib != nullptr && pNodeAttrib->GetAttributeType() != FbxNodeAttribute::eSkeleton))
            {
                break;
            }

            pFbxRootNode = pFbxParent;
            pFbxParent = pFbxParent->GetParent();
            result = true;
        }

        return result;
    }

    bool FBXConverter::searchSkeleton(FbxNode *pFbxNode)
    {
        bool result = false;
        SkeletonsConstItr itr = mSkeletons.find(pFbxNode);
        if (itr != mSkeletons.end())
        {
            result = true;
        }
        return result;
    }

    bool FBXConverter::searchAnimation(const String &name, Node *pNode, Animation *&pAnim)
    {
        bool result = false;

        if (pNode->getID() == name && pNode->getNodeType() == Node::E_TYPE_MODEL)
        {
            size_t i = 0;
            for (i = 0; i < pNode->getChildrenCount(); ++i)
            {
                Node *pChild = pNode->getChild(i);
                if (pChild->getNodeType() == Node::E_TYPE_ANIMATION)
                {
                    pAnim = (Animation *)pChild;
                    result = true;
                    break;
                }
            }
        }
        else
        {
            size_t i = 0;
            for (i = 0; i < pNode->getChildrenCount(); ++i)
            {
                Node *pChild = pNode->getChild(i);
                result = searchAnimation(name, pChild, pAnim);
                if (result)
                    break;
            }
        }

        return result;
    }

    bool FBXConverter::searchAction(const String &name, Animation *pAnim, Action *&pAction)
    {
        bool result = false;

        size_t i = 0;
        for (i = 0; i < pAnim->getChildrenCount(); ++i)
        {
            Node *pNode = pAnim->getChild(i);
            if (pNode->getNodeType() == Node::E_TYPE_ACTION
                && pNode->getID() == name)
            {
                pAction = (Action *)pNode;
                result = true;
                break;
            }
        }

        return result;
    }

    bool FBXConverter::updateVertexBlendIndexAndWeight(Mesh *pMesh, int nCtrlPointIdx, int nBlendIndex, double fBlendWeight)
    {
        VertexBuffer *pVB = nullptr;
        bool ret = searchVertexBuffer(pMesh, pVB);

        if (ret)
        {
            VerticesItr itr = pVB->mVertices.begin();

            while (itr != pVB->mVertices.end())
            {
                if (itr->mCtrlPointIdx == nCtrlPointIdx)
                {
                    Vertex &vertex = *itr;

                    // 更新骨骼索引
                    BlendInfo blend;
                    blend.mBlendIndex = nBlendIndex;
                    blend.mBlendWeight = fBlendWeight;
                    BlendInfoValue value(fBlendWeight, blend);
                    vertex.mBlendInfo.insert(value);

                    auto i = vertex.mBlendInfo.rbegin();
                    double len = 0.0;
                    while (i != vertex.mBlendInfo.rend())
                    {
                        len += i->second.mBlendWeight;
                        ++i;
                    }

                    BlendInfoDict blends;
                    int j = 0;
                    i = vertex.mBlendInfo.rbegin();
                    while (i != vertex.mBlendInfo.rend())
                    {
                        auto blend = i->second;
                        blend.mBlendWeight /= len;
                        BlendInfoValue value(blend.mBlendWeight, blend);
                        blends.insert(value);
                        ++j;
                        ++i;

                        if (j >= 4)
                            break;
                    }

                    vertex.mBlendInfo = blends;
                }

                ++itr;
            }

            ret = true;
        }

        return ret;
    }

    void FBXConverter::updateVertexBlendAttributes(Mesh *pMesh)
    {
        if (!mHasVertexBlending)
        {
            VertexBuffer *pVB = nullptr;
            if (searchVertexBuffer(pMesh, pVB))
            {
                VertexAttribute attribute;
                attribute.mVertexType = VertexAttribute::E_VT_BLEND_WEIGHT;
                attribute.mSize = 4;
                attribute.mDataType = VertexAttribute::E_VT_FLOAT;
                pVB->mAttributes.push_back(attribute);

                attribute.mVertexType = VertexAttribute::E_VT_BLEND_INDEX;
                attribute.mSize = 4;
                attribute.mDataType = VertexAttribute::E_VT_INT32;
                pVB->mAttributes.push_back(attribute);

                mHasVertexBlending = true;
            }
        }
    }

    String FBXConverter::FbxWrapModeToString(FbxTexture::EWrapMode eWrapMode) const
    {
        String s;
        switch (eWrapMode)
        {
        case fbxsdk_2015_1::FbxTexture::eRepeat:
            {
                s = "repeat";
            }
            break;
        case fbxsdk_2015_1::FbxTexture::eClamp:
            {
                s = "clamp";
            }
            break;
        default:
            {
                s = "repeat";
            }
            break;
        }

        return s;
    }

    bool FBXConverter::computeBoundingBox(Mesh *pMesh)
    {
        Model *pModel = (Model *)pMesh->getParent();

        bool result = false;

        switch (mSettings.mBoundType)
        {
        case E_BT_SPHERE:
            {
                result = computeBoundingSphere(pModel, pMesh);
            }
            break;
        case E_BT_AABB:
            {
                result = computeAlignAxisBoundingBox(pModel, pMesh);
            }
            break;
        }

        return result;
    }

    bool FBXConverter::computeBoundingSphere(Model *pModel, Mesh *pMesh)
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float radius = 0.0f;

        VertexBuffer *pVB = nullptr;

        if (searchVertexBuffer(pMesh, pVB))
        {
            auto itr = pVB->mVertices.begin();
            while (itr != pVB->mVertices.end())
            {
                Vertex &vertex = *itr;

                x += vertex.mPosition[0];
                y += vertex.mPosition[1];
                z += vertex.mPosition[2];

                ++itr;
            }

            size_t nVertexCount = pVB->mVertices.size();
            x /= nVertexCount;
            y /= nVertexCount;
            z /= nVertexCount;

            SphereBound *pBound = new SphereBound(pMesh->getID());
            pMesh->addChild(pBound);

            pBound->mCenterX = x;
            pBound->mCenterY = y;
            pBound->mCenterZ = z;

            itr = pVB->mVertices.begin();
            while (itr != pVB->mVertices.end())
            {
                Vertex &vertex = *itr;
                float dx = x - vertex.mPosition[0];
                float dy = y - vertex.mPosition[1];
                float dz = z - vertex.mPosition[2];
                float length = sqrt(x * x + y * y + z * z);
                if (length > radius)
                    radius = length;
                ++itr;
            }

            pBound->mRadius = radius;
        }

        return true;
    }

    bool FBXConverter::computeAlignAxisBoundingBox(Model *pModel, Mesh *pMesh)
    {
        VertexBuffer *pVB = nullptr;

        if (searchVertexBuffer(pMesh, pVB))
        {
            float fMinX = FLT_MAX;
            float fMaxX = FLT_MIN;
            float fMinY = FLT_MAX;
            float fMaxY = FLT_MIN;
            float fMinZ = FLT_MAX;
            float fMaxZ = FLT_MIN;

            if (mCurBound != nullptr)
            {
                AabbBound *pBound = (AabbBound *)mCurBound;
                fMinX = pBound->mMinX;
                fMaxX = pBound->mMaxX;
                fMinY = pBound->mMinY;
                fMaxY = pBound->mMaxY;
                fMinZ = pBound->mMinZ;
                fMaxZ = pBound->mMaxZ;
            }

            auto itr = pVB->mVertices.begin();
            while (itr != pVB->mVertices.end())
            {
                Vertex &vertex = *itr;

                if (vertex.mPosition[0] < fMinX)
                    fMinX = vertex.mPosition[0];
                else if (vertex.mPosition[0] > fMaxX)
                    fMaxX = vertex.mPosition[0];

                if (vertex.mPosition[1] < fMinY)
                    fMinY = vertex.mPosition[1];
                else if (vertex.mPosition[1] > fMaxY)
                    fMaxY = vertex.mPosition[1];

                if (vertex.mPosition[2] < fMinZ)
                    fMinZ = vertex.mPosition[2];
                else if (vertex.mPosition[2] > fMaxZ)
                    fMaxZ = vertex.mPosition[2];

                ++itr;
            }

            AabbBound *pBound = nullptr;

            if (mCurBound == nullptr)
            {
                pBound = new AabbBound(pMesh->getID());
                pModel->addChild(pBound);
                mCurBound = pBound;
            }
            else
            {
                pBound = (AabbBound *)mCurBound;
            }

            pBound->mMinX = fMinX;
            pBound->mMaxX = fMaxX;
            pBound->mMinY = fMinY;
            pBound->mMaxY = fMaxY;
            pBound->mMinZ = fMinZ;
            pBound->mMaxZ = fMaxZ;
        }

        return true;
    }

    void FBXConverter::convertMatrix(const FbxAMatrix &fbxMat, Matrix4 &m)
    {
        m[0][0] = fbxMat[0][0];
        m[0][1] = fbxMat[0][1];
        m[0][2] = fbxMat[0][2];
        m[0][3] = fbxMat[0][3];

        m[1][0] = fbxMat[1][0];
        m[1][1] = fbxMat[1][1];
        m[1][2] = fbxMat[1][2];
        m[1][3] = fbxMat[1][3];

        m[2][0] = fbxMat[2][0];
        m[2][1] = fbxMat[2][1];
        m[2][2] = fbxMat[2][2];
        m[2][3] = fbxMat[2][3];

        m[3][0] = fbxMat[3][0];
        m[3][1] = fbxMat[3][1];
        m[3][2] = fbxMat[3][2];
        m[3][3] = fbxMat[3][3];
    }
}
