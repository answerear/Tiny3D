/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

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
#include "mconv_transform.h"
#include "mconv_log.h"


namespace mconv
{
    FBXConverter::FBXConverter(const Settings &settings)
        : ConverterImpl(settings)
        , mImporter(nullptr)
        , mExporter(nullptr)
        , mSrcData(nullptr)
        , mDstData(nullptr)
        , mCurScene(nullptr)
        , mCurModel(nullptr)
        , mCurMesh(nullptr)
        , mCurSubMeshes(nullptr)
        , mCurSkeleton(nullptr)
        , mCurAnimation(nullptr)
        , mCurMaterials(nullptr)
        , mRootTransform(nullptr)
        , mCurSkin(nullptr)
        , mHasSkeleton(false)
        , mHasVertexBlending(false)
        , mHasAnimation(false)
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
            MCONV_LOG_ERROR("Create importer failed ! Because of invalid source file format !");
            result = false;
        }

        result = result && mImporter->load(mSettings.mSrcPath, mSrcData);

        return result;
    }

    bool FBXConverter::exportScene()
    {
        bool result = false;

        delete mExporter;
        mExporter = nullptr;

        if ((mSettings.mDstType & E_FILETYPE_T3D) == E_FILETYPE_T3D)
        {
            mExporter = new T3DSerializer();
            result = (mExporter != nullptr);
        }
        else if (mSettings.mDstType & E_FILETYPE_T3B)
        {
            mExporter = new T3DBinSerializer();
            result = (mExporter != nullptr);
        }
        else if (mSettings.mDstType & E_FILETYPE_T3T)
        {
            mExporter = new T3DXMLSerializer();
            result = (mExporter != nullptr);
        }
        else
        {
            MCONV_LOG_ERROR("Create exporter failed ! Because of invalid destination file format !");
            return false;
        }

        result = result && mExporter->save(mSettings.mDstPath, mDstData);

        return result;
    }

    bool FBXConverter::convertToT3D()
    {
        if (mSrcData == nullptr)
        {
            MCONV_LOG_ERROR("Convert to T3D failed ! Because of invalid source data !");
            return false;
        }

        FbxScene *pFbxScene = static_cast<FbxScene *>(mSrcData);

        String name = pFbxScene->GetName();

        if (name.empty())
        {
            name = "Scene";
        }

        MCONV_LOG_INFO("Start converting to T3D ......");
        Scene *pScene = new Scene(name);
        mCurScene = pScene;
        mDstData = pScene;
        bool ret = processFbxScene(pFbxScene, pScene);
        if (!ret)
        {
            MCONV_LOG_ERROR("Failed converting !");
        }
        else
        {
            MCONV_LOG_INFO("Completed converting successfully !");
        }

        return ret;
    }

    void FBXConverter::cleanup()
    {
        Node *pNode = (Node *)mDstData;
        pNode->removeAllChildren();
    }

    bool FBXConverter::processFbxScene(FbxScene *pFbxScene, Node *pRoot)
    {
        FbxNode *pFbxRoot = pFbxScene->GetRootNode();

        mMaxBoneIdx = 0;
        mBoneIdx = 0;

        mRootTransform = new Hierarchy("Hierarchy");

        bool result = processFbxNode(pFbxRoot, mRootTransform, pRoot);

        if (result && E_FM_SHARE_VERTEX == mSettings.mFileMode)
        {
            MCONV_LOG_INFO("Start optimizing mesh ......");
            result = result && optimizeMesh(pRoot);

            if (!result)
            {
                MCONV_LOG_ERROR("Failed optimizing mesh !");
            }
            else
            {
                MCONV_LOG_INFO("Completed optimizing mesh !");
            }

            MCONV_LOG_INFO("Start computing bounding box ......");
            result = processBoundingBox(pRoot);

            if (!result)
            {
                MCONV_LOG_ERROR("Failed computing bounding box !");
            }
            else
            {
                MCONV_LOG_INFO("Completed computing bounding box !");
            }
        }

        pRoot->addChild(mRootTransform);

        // 如果不是分割模型文件模式，则调整骨骼、动画和材质数据到最后
        if (mCurSkeleton != nullptr)
        {
            mCurSkeleton->removeFromParent(false);
            mCurModel->addChild(mCurSkeleton);

            // 修正骨骼pallette，避免有骨骼不在骨骼pallette里面
            fixBoneIndex((Bone *)mCurSkeleton->getChild(0));
        }

        if (mCurSkin != nullptr)
        {
            mCurSkin->removeFromParent(false);
            mCurModel->addChild(mCurSkin);
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

//         if (mCurBound != nullptr)
//         {
//             mCurBound->removeFromParent(false);
//             mCurModel->addChild(mCurBound);
//         }

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

        return result;
    }

    bool FBXConverter::processFbxNode(FbxNode *pFbxNode, Node *pTransformNode, Node *pParent)
    {
        bool result = false;
        Node *pNode = nullptr;
        Node *pParentTransform = nullptr;

        String name = pFbxNode->GetName();
        int nAttribCount = pFbxNode->GetNodeAttributeCount();
        int i = 0;
        for (i = 0; i < nAttribCount; ++i)
        {
            if (pFbxNode->GetNodeAttribute() != nullptr)
            {
                FbxNodeAttribute::EType attribType = pFbxNode->GetNodeAttribute()->GetAttributeType();
                switch (attribType)
                {
                case FbxNodeAttribute::eNull:
                    {
                        MCONV_LOG_INFO("Start processing transform ......");
                        Transform *pTransform = nullptr;

                        pTransform = new Transform(name);
                        pTransformNode->addChild(pTransform);
                        pParentTransform = pTransform;

                        FbxAMatrix M = pFbxNode->EvaluateLocalTransform();
                        convertMatrix(M, pTransform->mMatrix);
                        MCONV_LOG_INFO("Completed processing transform !");
                    }
                    break;
                case FbxNodeAttribute::eMesh:
                    {
                        String name = pFbxNode->GetName();
                        if (name.empty())
                        {
                            name = "Model";
                        }

                        if (mCurModel == nullptr)
                        {
                            // 没有model节点，先创建一个，用于后续挂接所有mesh节点
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

                        Transform *pTransform = nullptr;

                        pTransform = new Transform(name);
                        pTransformNode->addChild(pTransform);
                        pParentTransform = pTransform;

                        FbxAMatrix M = pFbxNode->EvaluateLocalTransform();
                        convertMatrix(M, pTransform->mMatrix);

#ifdef T3D_DEBUG
                        pTransform->mMatrix.printLog(name + "NodeMatrix : ");
#endif

                        result = processFbxMesh(pFbxNode, mCurModel, pTransform, pNode);
                        result = result && processFbxSkin(pFbxNode, pParent, (Mesh *)pNode);
                        result = result && processFbxMaterial(pFbxNode, mCurModel);

                        if (E_FM_SHARE_VERTEX != mSettings.mFileMode)
                        {
                            result = result && optimizeMesh(pNode);
                            processBoundingBox(pNode);
                        }

                        pNode = mCurModel;
                    }
                    break;
                case FbxNodeAttribute::eSkeleton:
                    {
                        if (pFbxNode->GetParent() &&
                            pFbxNode->GetParent()->GetNodeAttribute() == NULL)
                        {
                            MCONV_LOG_INFO("Start processing skeleton ......");
                        }

                        result = processFbxSkeleton(pFbxNode, pParent, (Model *)mCurModel, pNode);

                        if (pFbxNode->GetParent() &&
                            pFbxNode->GetParent()->GetNodeAttribute() == NULL)
                        {
                            if (!result)
                            {
                                MCONV_LOG_ERROR("Failed processing skeleton !");
                            }
                            else
                            {
                                MCONV_LOG_INFO("Completed processing skeleton successfully !");
                            }
                        }
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

        if (pParentTransform == nullptr)
        {
            pParentTransform = pTransformNode;
        }

        if (result)
        {
            for (i = 0; i < pFbxNode->GetChildCount(); ++i)
            {
                result = result && processFbxNode(pFbxNode->GetChild(i), pParentTransform, pNode);
            }
        }

        return result;
    }

    bool FBXConverter::processFbxMesh(FbxNode *pFbxNode, Node *pParent, Transform *pTransform, Node *&pNewNode)
    {
        MCONV_LOG_INFO("Start processing mesh ......");

        FbxMesh *pFbxMesh = pFbxNode->GetMesh();

        if (pFbxMesh == nullptr)
        {
            MCONV_LOG_ERROR("FBX mesh is invalid !");
            return false;
        }

        String name = pFbxNode->GetName();
        if (name.empty())
        {
            name = "Mesh";
        }

        Mesh *pMesh = nullptr;
        SubMeshes *pSubMeshes = nullptr;
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

                pSubMeshes = new SubMeshes(name);
                pMesh->addChild(pSubMeshes);

                mCurSubMeshes = pSubMeshes;
            }
            else
            {
                pMesh = (Mesh *)mCurMesh;
                pNewNode = pMesh;
                pSubMeshes = (SubMeshes *)mCurSubMeshes;

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

            pSubMeshes = new SubMeshes(name);
            pMesh->addChild(pSubMeshes);

            mCurSubMeshes = pSubMeshes;
        }

        SubMesh *pCurSubMesh = nullptr;

        FbxVector4 T = pFbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        FbxVector4 R = pFbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
        FbxVector4 S = pFbxNode->GetGeometricScaling(FbxNode::eSourcePivot);
        FbxAMatrix M;
        M.SetTRS(T, R, S);
        Matrix4 m;
        convertMatrix(M, m);

        int nTriangleCount = pFbxMesh->GetPolygonCount();
        int nVertexCount = 0;
        int i = 0, j = 0;

        // 生成顶点属性
        if (!processVertexAttribute(pFbxMesh, pVB))
        {
            MCONV_LOG_ERROR("Failed processing vertex attribute !");
            return false;
        }

        typedef std::map<size_t, SubMesh*>  SubMeshMap;
        typedef SubMeshMap::iterator        SubMeshMapItr;
        typedef std::pair<size_t, SubMesh*> SubMeshValue;

        SubMeshMap  submeshes;

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
                Vector3 pos = m * vertex.mPosition;
                vertex.mPosition[0] = pos[0];
                vertex.mPosition[1] = pos[1];
                vertex.mPosition[2] = pos[2];

                // 读取顶点颜色信息
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
                else
                {
                    nMaterialIdx = -1;
                }

                if (pCurSubMesh == nullptr)
                {
                    // 没有子网格，生成一个
                    pCurSubMesh = new SubMesh(name);
                    pSubMeshes->addChild(pCurSubMesh);
                    submeshes.insert(SubMeshValue(nMaterialIdx, pCurSubMesh));
                    processVertexAttribute(pFbxMesh, pCurSubMesh->mVB);
                    pCurSubMesh->mMaterialIdx = nMaterialIdx;
                    pTransform->mEntities.push_back(Transform::Entity(pMesh, pCurSubMesh));
                }
                else
                {
                    // 有子网格，看看是否跟当前的材质相同，不相同则查找一个现成 ，如果现成的都没有，则生成一个新的
                    if (pCurSubMesh->mMaterialIdx != nMaterialIdx)
                    {
                        auto itr = submeshes.find(nMaterialIdx);

                        if (itr != submeshes.end())
                        {
                            pCurSubMesh = itr->second;
                        }
                        else
                        {
                            pCurSubMesh = new SubMesh(name);
                            pSubMeshes->addChild(pCurSubMesh);
                            submeshes.insert(SubMeshValue(nMaterialIdx, pCurSubMesh));
                            processVertexAttribute(pFbxMesh, pCurSubMesh->mVB);
                            pCurSubMesh->mMaterialIdx = nMaterialIdx;
                            pTransform->mEntities.push_back(Transform::Entity(pMesh, pCurSubMesh));
                        }
                    }
                }

                pCurSubMesh->mVB->mVertices.push_back(vertex);
                pVB->mVertices.push_back(vertex);

                ++nVertexCount;
            }
        }

        MCONV_LOG_INFO("Completed processing mesh successfully !");

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
                    VertexBuffer *pBuffer = dynamic_cast<VertexBuffer *>(pBuffers->getChild(j));
                    
                    if (pBuffer != nullptr)
                    {
                        pVertexBuffer = pBuffer;
                        found = true;
                        break;
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
                    MCONV_LOG_ERROR("Different vertex format, shared vertex file mode could not be used ! Please using \"-m original\" insteading of \"-m shared\" !");
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

    bool FBXConverter::readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, Vector3 &pos)
    {
        FbxVector4 *pControlPoint = pFbxMesh->GetControlPoints();
        pos[0] = Real(pControlPoint[nControlPointIdx][0]);
        pos[1] = Real(pControlPoint[nControlPointIdx][1]);
        pos[2] = Real(pControlPoint[nControlPointIdx][2]);
        return true;
    }

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
                            color[0] = Real(pVertexColor->GetDirectArray().GetAt(nControlPointIdx).mBlue);
                            color[1] = Real(pVertexColor->GetDirectArray().GetAt(nControlPointIdx).mGreen);
                            color[2] = Real(pVertexColor->GetDirectArray().GetAt(nControlPointIdx).mRed);
                            color[3] = Real(pVertexColor->GetDirectArray().GetAt(nControlPointIdx).mAlpha);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexColor->GetIndexArray().GetAt(nControlPointIdx);
                            color[0] = Real(pVertexColor->GetDirectArray().GetAt(idx).mBlue);
                            color[1] = Real(pVertexColor->GetDirectArray().GetAt(idx).mGreen);
                            color[2] = Real(pVertexColor->GetDirectArray().GetAt(idx).mRed);
                            color[3] = Real(pVertexColor->GetDirectArray().GetAt(idx).mAlpha);
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
                            color[0] = Real(pVertexColor->GetDirectArray().GetAt(nVertexIndex).mBlue);
                            color[1] = Real(pVertexColor->GetDirectArray().GetAt(nVertexIndex).mGreen);
                            color[2] = Real(pVertexColor->GetDirectArray().GetAt(nVertexIndex).mRed);
                            color[3] = Real(pVertexColor->GetDirectArray().GetAt(nVertexIndex).mAlpha);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexColor->GetIndexArray().GetAt(nVertexIndex);
                            color[0] = Real(pVertexColor->GetDirectArray().GetAt(idx).mBlue);
                            color[1] = Real(pVertexColor->GetDirectArray().GetAt(idx).mGreen);
                            color[2] = Real(pVertexColor->GetDirectArray().GetAt(idx).mRed);
                            color[3] = Real(pVertexColor->GetDirectArray().GetAt(idx).mAlpha);
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

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
                            uv[0] = Real(pVertexUV->GetDirectArray().GetAt(nControlPointIdx)[0]);
                            uv[1] = Real(pVertexUV->GetDirectArray().GetAt(nControlPointIdx)[1]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexUV->GetIndexArray().GetAt(nControlPointIdx);
                            uv[0] = Real(pVertexUV->GetDirectArray().GetAt(idx)[0]);
                            uv[1] = Real(pVertexUV->GetDirectArray().GetAt(idx)[1]);
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
                            uv[0] = Real(pVertexUV->GetDirectArray().GetAt(nUVIndex)[0]);
                            uv[1] = Real(pVertexUV->GetDirectArray().GetAt(nUVIndex)[1]);
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

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
                            normal[0] = Real(pNormal->GetDirectArray().GetAt(nControlPointIdx)[0]);
                            normal[1] = Real(pNormal->GetDirectArray().GetAt(nControlPointIdx)[1]);
                            normal[2] = Real(pNormal->GetDirectArray().GetAt(nControlPointIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pNormal->GetIndexArray().GetAt(nControlPointIdx);
                            normal[0] = Real(pNormal->GetDirectArray().GetAt(idx)[0]);
                            normal[1] = Real(pNormal->GetDirectArray().GetAt(idx)[1]);
                            normal[2] = Real(pNormal->GetDirectArray().GetAt(idx)[2]);
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
                            normal[0] = Real(pNormal->GetDirectArray().GetAt(nVertexIndex)[0]);
                            normal[1] = Real(pNormal->GetDirectArray().GetAt(nVertexIndex)[1]);
                            normal[2] = Real(pNormal->GetDirectArray().GetAt(nVertexIndex)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pNormal->GetIndexArray().GetAt(nVertexIndex);
                            normal[0] = Real(pNormal->GetDirectArray().GetAt(idx)[0]);
                            normal[1] = Real(pNormal->GetDirectArray().GetAt(idx)[1]);
                            normal[2] = Real(pNormal->GetDirectArray().GetAt(idx)[2]);
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

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
                            tangent[0] = Real(pTangent->GetDirectArray().GetAt(nControlPointIdx)[0]);
                            tangent[1] = Real(pTangent->GetDirectArray().GetAt(nControlPointIdx)[1]);
                            tangent[2] = Real(pTangent->GetDirectArray().GetAt(nControlPointIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pTangent->GetIndexArray().GetAt(nControlPointIdx);
                            tangent[0] = Real(pTangent->GetDirectArray().GetAt(idx)[0]);
                            tangent[1] = Real(pTangent->GetDirectArray().GetAt(idx)[1]);
                            tangent[2] = Real(pTangent->GetDirectArray().GetAt(idx)[2]);
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
                            tangent[0] = Real(pTangent->GetDirectArray().GetAt(nVertexIndex)[0]);
                            tangent[1] = Real(pTangent->GetDirectArray().GetAt(nVertexIndex)[1]);
                            tangent[2] = Real(pTangent->GetDirectArray().GetAt(nVertexIndex)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pTangent->GetIndexArray().GetAt(nVertexIndex);
                            tangent[0] = Real(pTangent->GetDirectArray().GetAt(idx)[0]);
                            tangent[1] = Real(pTangent->GetDirectArray().GetAt(idx)[1]);
                            tangent[2] = Real(pTangent->GetDirectArray().GetAt(idx)[2]);
                        }
                        break;
                    }
                }
                break;
            }
        }

        return result;
    }

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
                            binormal[0] = Real(pBinormal->GetDirectArray().GetAt(nControlPointIdx)[0]);
                            binormal[1] = Real(pBinormal->GetDirectArray().GetAt(nControlPointIdx)[1]);
                            binormal[2] = Real(pBinormal->GetDirectArray().GetAt(nControlPointIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pBinormal->GetIndexArray().GetAt(nControlPointIdx);
                            binormal[0] = Real(pBinormal->GetDirectArray().GetAt(idx)[0]);
                            binormal[1] = Real(pBinormal->GetDirectArray().GetAt(idx)[1]);
                            binormal[2] = Real(pBinormal->GetDirectArray().GetAt(idx)[2]);
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
                            binormal[0] = Real(pBinormal->GetDirectArray().GetAt(nVertexIndex)[0]);
                            binormal[1] = Real(pBinormal->GetDirectArray().GetAt(nVertexIndex)[1]);
                            binormal[2] = Real(pBinormal->GetDirectArray().GetAt(nVertexIndex)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pBinormal->GetIndexArray().GetAt(nVertexIndex);
                            binormal[0] = Real(pBinormal->GetDirectArray().GetAt(idx)[0]);
                            binormal[1] = Real(pBinormal->GetDirectArray().GetAt(idx)[1]);
                            binormal[2] = Real(pBinormal->GetDirectArray().GetAt(idx)[2]);
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

    bool FBXConverter::processFbxSkeleton(FbxNode *pFbxNode, Node *pParent, Model *pModel, Node *&pNode)
    {
        FbxGeometry *pFbxGeometry = pFbxNode->GetGeometry();
        FbxSkeleton *pFbxSkel = pFbxNode->GetSkeleton();

        Matrix4 m;
        const FbxAMatrix &M = pFbxNode->EvaluateLocalTransform();
        convertMatrix(M, m);

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
//             SkeletonsValue value(pFbxNode, pSkel);
//             mSkeletons.insert(value);
            pSkel->mBoneCount = 0;
            mHasSkeleton = true;
            mCurSkeleton = pSkel;

            updateBoneMatrix(pFbxNode, m, pSkel, pNode);
            pSkel->mBoneCount++;
        }
        else
        {
            updateBoneMatrix(pFbxNode, m, pParent, pNode);
            Skeleton *pSkel = (Skeleton *)mCurSkeleton;
            pSkel->mBoneCount++;
        }

//         String name = pFbxNode->GetName();
//         if (name.empty())
//         {
//             name = "bone";
//         }
//         Bone *pBone = new Bone(name);
//         pParent->addChild(pBone);
//         mBoneCount++;
// 
//         FbxAMatrix &M = pFbxNode->EvaluateLocalTransform();
//         convertMatrix(M, pBone->mLocalTransform);

        processFbxAnimation(pFbxNode, pModel);

//         int i = 0;
//         int nChildrenCount = pFbxNode->GetChildCount();
//         for (i = 0; i < nChildrenCount; ++i)
//         {
//             processFbxSkeleton(pFbxNode->GetChild(i), pBone, pModel);
//         }

        return true;
    }

    bool FBXConverter::processFbxCamera(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode)
    {
        return true;
    }

    bool FBXConverter::processFbxLight(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode)
    {
        return true;
    }

    bool FBXConverter::processFbxMaterial(FbxNode *pFbxNode, Node *pParent)
    {
        MCONV_LOG_INFO("Start processing material ......");

        int nMaterialCount = pFbxNode->GetMaterialCount();
        int i = 0;
        
        Materials *pMaterials = new Materials("Materials");
        pParent->addChild(pMaterials);
        mCurMaterials = pMaterials;

        for (i = 0; i < nMaterialCount; ++i)
        {
            FbxSurfaceMaterial *pFbxMaterial = pFbxNode->GetMaterial(i);

            Material *pMaterial = nullptr;
            if (!searchMaterial(pFbxMaterial->GetName(), pMaterial))
            {
                pMaterial = new Material(pFbxMaterial->GetName());
                mCurMaterials->addChild(pMaterial);

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
                                
                                pTexture->mFilename = getFileName(pFbxTexture->GetFileName());
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

                                pTexture->mFilename = getFileName(pFbxTexture->GetFileName());
                                pTexture->mType = "emissive";
                                pTexture->mWrapModeU = FbxWrapModeToString(pFbxTexture->WrapModeU.Get());
                                pTexture->mWrapModeV = FbxWrapModeToString(pFbxTexture->WrapModeV.Get());
                            }
                        }
                    }
                }
            }
        }

        MCONV_LOG_INFO("Completed processing material !");

        return true;
    }

    String FBXConverter::getFileName(const String &strPath)
    {
        String strName;
        size_t pos = strPath.rfind("\\");
        if (pos != String::npos)
        {
            strName = strPath.substr(pos + 1);
        }
        else
        {
            strName = strPath;
        }
        return strName;
    }

    bool FBXConverter::searchMaterial(const String &name, Material *&pMaterial)
    {
        bool result = false;

        result = mCurMaterials->getChild(name, (Node *&)pMaterial);

        return result;
    }

    bool FBXConverter::processFbxAnimation(FbxNode *pFbxNode, Model *pModel)
    {
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

        for (i = 0; i < nAnimStackCount; ++i)
        {
            FbxAnimStack *pFbxAnimStack = (FbxAnimStack *)pFbxScene->GetSrcObject(FbxAnimStack::ClassId, i);

            String name = pFbxAnimStack->GetName();
            if (name.empty())
            {
                name = "Action";
            }

            int nAnimLayerCount = pFbxAnimStack->GetMemberCount();
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
                
                if (pFbxTransCurve != nullptr)
                {
                    int nKeyframeCount = pFbxTransCurve->KeyGetCount();

                    int k = 0;
                    for (k = 0; k < nKeyframeCount; ++k)
                    {
                        KeyframeT *pFrame = new KeyframeT(k);
                        FbxTime frameTime = pFbxTransCurve->KeyGetTime(k);
                        FbxVector4 translate = pFbxNode->EvaluateLocalTranslation(frameTime);
//                         const FbxAMatrix &M = pFbxNode->EvaluateLocalTransform(frameTime);
//                         FbxVector4 translate = M.GetT();
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
                        FbxAMatrix M;
                        M.SetIdentity();
                        M.SetR(rotation);
                        FbxQuaternion Q = M.GetQ();
//                         const FbxAMatrix &M = pFbxNode->EvaluateLocalTransform(frameTime);
//                         FbxQuaternion Q = M.GetQ();
                        KeyframeR *pFrame = new KeyframeR(k);
                        pFrame->x = Q[0];//orientation[0];
                        pFrame->y = Q[1];//orientation[1];
                        pFrame->z = Q[2];//orientation[2];
                        pFrame->w = Q[3];//orientation[3];
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
//                         const FbxAMatrix &M = pFbxNode->EvaluateLocalTransform(frameTime);
//                         FbxVector4 scale = M.GetS();
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
        MCONV_LOG_INFO("Start processing skin ......");

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

//             Skin *pSkin = nullptr;
// 
//             if (mCurSkin == nullptr)
//             {
//                 pSkin = new Skin(pModel->getID());
//                 pModel->addChild(pSkin);
//                 mCurSkin = pSkin;
//             }
//             else
//             {
//                 pSkin = (Skin *)mCurSkin;
//             }
            
            for (j = 0; j < nBoneCount; ++j)
            {
                FbxCluster *pFbxCluster = pFbxSkin->GetCluster(j);
                FbxNode *pFbxLinkNode = pFbxCluster->GetLink();

//                 if (j == 0)
//                 {
//                     FbxAMatrix Mv;
//                     pFbxCluster->GetTransformMatrix(Mv);
//                     convertMatrix(Mv, pSkin->mVertexMatrix);
// 
//                     mBoneCount = 0;
//                     FbxNode *pFbxSkelRoot = nullptr;
//                     if (searchSkeletonRoot(pFbxLinkNode, pFbxSkelRoot) && !searchSkeleton(pFbxSkelRoot))
//                     {
//                         mHasSkeleton = false;
//                         mHasAnimation = false;
//                         processFbxSkeleton(pFbxSkelRoot, pModel, pModel);
//                     }
//                 }
                
                FbxAMatrix Mb;
                pFbxCluster->GetTransformLinkMatrix(Mb);
                FbxAMatrix Mv;
                pFbxCluster->GetTransformMatrix(Mv);
                FbxAMatrix m = pFbxLinkNode->EvaluateLocalTransform();
                FbxVector4 T = pFbxLinkNode->GetGeometricTranslation(FbxNode::eSourcePivot);
                FbxVector4 R = pFbxLinkNode->GetGeometricRotation(FbxNode::eSourcePivot);
                FbxVector4 S = pFbxLinkNode->GetGeometricScaling(FbxNode::eSourcePivot);
                FbxAMatrix matGeometry(T, R, S);
                FbxAMatrix bindpose = Mb.Inverse() * Mv;
                Matrix4 mb;
                Matrix4 mv;
                Matrix4 mp;
                convertMatrix(Mb, mb);
                convertMatrix(Mv, mv);
                convertMatrix(bindpose, mp);

                mb.printLog(std::string(pFbxLinkNode->GetName()) + " BoneMatrix : ");
                mv.printLog(std::string(pFbxLinkNode->GetName()) + " VertexMatrix : ");
                mp.printLog(std::string(pFbxLinkNode->GetName()) + " BineposeMatrix : ");

//                 SkinInfo *pSkinInfo = new SkinInfo(pFbxLinkNode->GetName());
//                 convertMatrix(bindpose, pSkinInfo->mOffsetMatrix);
//                 pSkin->addChild(pSkinInfo);

                if (updateSkinInfo(pFbxLinkNode, mBoneIdx, mp))
                {
                    mBoneIdx++;
                }

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

            // 不知道为何骨骼层次中的骨骼数和蒙皮计算的骨骼偏移矩阵的数差一，这里在蒙皮计算的偏移矩阵末尾插入一个骨骼根结点
//             if (nBoneCount == mBoneCount - 1)
//             {
//                 size_t i = 0;
//                 for (i = 0; i < pModel->getChildrenCount(); ++i)
//                 {
//                     Node *pNode = pModel->getChild(i);
//                     if (pNode->getNodeType() == Node::E_TYPE_SKELETON)
//                     {
//                         Skeleton *pSkel = (Skeleton *)pNode;
//                         Bone *pBone = (Bone *)pSkel->getChild(0);
//                         T3D_ASSERT(pBone->getNodeType() == Node::E_TYPE_BONE);
//                         Bone *pNewBone = new Bone(pBone->getID());
// //                         convertMatrix(Mv, pNewBone->mLocalTransform);
//                         pNewBone->mLocalTransform = pBone->mLocalTransform;
//                         pSkin->addChild(pNewBone);
//                     }
//                 }
//             }
//             else if (nBoneCount == mBoneCount)
//             {
// 
//             }
//             else
//             {
//                 T3D_ASSERT(0);
//             }
        }

        MCONV_LOG_INFO("Completed processing skin successfully !");

        return true;
    }

    bool FBXConverter::optimizeMesh(Node *pNode)
    {
        if (pNode->getNodeType() == Node::E_TYPE_MESH)
        {
            Mesh *pMesh = (Mesh *)pNode;
            pMesh->optimize();

//             computeBoundingBox(pMesh);
        }

        size_t i = 0;
        for (i = 0; i < pNode->getChildrenCount(); ++i)
        {
            Node *pChild = pNode->getChild(i);
            optimizeMesh(pChild);
        }

        return true;
    }

    bool FBXConverter::updateSkinInfo(FbxNode *pFbxNode, size_t boneIdx, const Matrix4 &m)
    {
        bool ret = false;

        auto itr = mBones.find(pFbxNode);

        if (itr != mBones.end())
        {
            auto pBone = itr->second;
            pBone->mBoneIndex = boneIdx;
            pBone->mOffsetMatrix = m;
            ret = true;
        }
        else
        {
            Bone *pBone = new Bone(pFbxNode->GetName());
            std::pair<BonesItr, bool> r = mBones.insert(BonesValue(pFbxNode, pBone));
            if (r.second)
            {
                pBone->mBoneIndex = boneIdx;
                pBone->mOffsetMatrix = m;
                ret = true;
            }
            else
            {
                T3D_ASSERT(0);
            }
        }

        if (boneIdx > mMaxBoneIdx)
        {
            mMaxBoneIdx = boneIdx;
        }

        return true;
    }

    bool FBXConverter::updateBoneMatrix(FbxNode *pFbxNode, const Matrix4 &m, Node *pParent, Node *&pNode)
    {
        auto itr = mBones.find(pFbxNode);

        if (itr != mBones.end())
        {
            auto pBone = itr->second;
            pBone->mLocalTransform = m;
            pParent->addChild(pBone);
            pNode = pBone;
        }
        else
        {
            Bone *pBone = new Bone(pFbxNode->GetName());
            mBones.insert(BonesValue(pFbxNode, pBone));
            pBone->mLocalTransform = m;
            pParent->addChild(pBone);
            pNode = pBone;
        }

        return true;
    }

    bool FBXConverter::fixBoneIndex(Bone *pBone)
    {
        uint16_t count = mMaxBoneIdx + 1;
        auto itr = mBones.begin();

        while (itr != mBones.end())
        {
            if (pBone->mBoneIndex == 0xFFFF)
            {
                pBone->mBoneIndex = count;
                ++count;
            }

            ++itr;
        }

        return true;
    }
//     bool FBXConverter::searchSkeletonRoot(FbxNode *pFbxNode, FbxNode *&pFbxRootNode)
//     {
//         bool result = false;
//         FbxScene *pFbxScene = (FbxScene *)mSrcData;
//         FbxNode *pFbxSceneRoot = pFbxScene->GetRootNode();
//         FbxNode *pFbxParent = pFbxNode;
//         pFbxRootNode = nullptr;
// 
//         while (pFbxParent != pFbxSceneRoot)
//         {
//             FbxNodeAttribute *pNodeAttrib = pFbxParent->GetNodeAttribute();
//             FbxNodeAttribute::EType attribType = pFbxNode->GetNodeAttribute()->GetAttributeType();
//             if (pNodeAttrib == nullptr || 
//                 (pNodeAttrib != nullptr && pNodeAttrib->GetAttributeType() != FbxNodeAttribute::eSkeleton))
//             {
//                 break;
//             }
// 
//             pFbxRootNode = pFbxParent;
//             pFbxParent = pFbxParent->GetParent();
//             result = true;
//         }
// 
//         return result;
//     }
// 
//     bool FBXConverter::searchSkeleton(FbxNode *pFbxNode)
//     {
//         bool result = false;
//         SkeletonsConstItr itr = mSkeletons.find(pFbxNode);
//         if (itr != mSkeletons.end())
//         {
//             result = true;
//         }
//         return result;
//     }

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

    bool FBXConverter::updateVertexBlendIndexAndWeight(VertexBuffer *pVB, int nCtrlPointIdx, int nBlendIndex, double fBlendWeight)
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
//                 BlendInfoValue value(fBlendWeight, blend);
//                 vertex.mBlendInfo.insert(value);
                vertex.mBlendInfo.push_back(blend);
                vertex.mBlendInfo.sort();

                auto i = vertex.mBlendInfo.rbegin();
                double len = 0.0;
                while (i != vertex.mBlendInfo.rend())
                {
                    len += i->mBlendWeight;//i->second.mBlendWeight;
                    ++i;
                }

//                 BlendInfoDict blends;
                BlendInfoList blends;
                int j = 0;
                i = vertex.mBlendInfo.rbegin();
                while (i != vertex.mBlendInfo.rend())
                {
                    auto blend = *i;// i->second;
                    blend.mBlendWeight /= len;
//                     BlendInfoValue value(blend.mBlendWeight, blend);
//                     blends.insert(value);
                    blends.push_back(blend);
                    ++j;
                    ++i;

                    if (j >= 4)
                        break;
                }

                vertex.mBlendInfo = blends;
            }

            ++itr;
        }

        return true;
    }

    bool FBXConverter::updateVertexBlendIndexAndWeight(Mesh *pMesh, int nCtrlPointIdx, int nBlendIndex, double fBlendWeight)
    {
        VertexBuffer *pVB = nullptr;
        bool ret = searchVertexBuffer(pMesh, pVB);
        ret = ret && updateVertexBlendIndexAndWeight(pVB, nCtrlPointIdx, nBlendIndex, fBlendWeight);

        size_t i = 0;
        for (i = 0; i < pMesh->getChildrenCount(); ++i)
        {
            Node *pNode = pMesh->getChild(i);
            if (pNode->getNodeType() == Node::E_TYPE_SUBMESHES)
            {
                SubMeshes *pSubMeshes = (SubMeshes *)pNode;
                size_t j = 0;

                for (j = 0; j < pSubMeshes->getChildrenCount(); ++j)
                {
                    SubMesh *pSubMesh = (SubMesh *)pSubMeshes->getChild(j);
                    T3D_ASSERT(pSubMesh->getNodeType() == Node::E_TYPE_SUBMESH);
                    ret = ret && updateVertexBlendIndexAndWeight(pSubMesh->mVB, nCtrlPointIdx, nBlendIndex, fBlendWeight);
                }

                break;
            }
        }

        return ret;
    }

    void FBXConverter::putVertexBlendAndWeightAttributes(VertexBuffer *pVB)
    {
        VertexAttribute attribute;
        attribute.mVertexType = VertexAttribute::E_VT_BLEND_WEIGHT;
        attribute.mSize = 4;
        attribute.mDataType = VertexAttribute::E_VT_FLOAT;
        pVB->mAttributes.push_back(attribute);

        attribute.mVertexType = VertexAttribute::E_VT_BLEND_INDEX;
        attribute.mSize = 4;
        attribute.mDataType = VertexAttribute::E_VT_INT16;
        pVB->mAttributes.push_back(attribute);
    }

    void FBXConverter::updateVertexBlendAttributes(Mesh *pMesh)
    {
        if (!mHasVertexBlending)
        {
            VertexBuffer *pVB = nullptr;
            if (searchVertexBuffer(pMesh, pVB))
            {
                putVertexBlendAndWeightAttributes(pVB);
                mHasVertexBlending = true;
            }

            size_t i = 0;
            for (i = 0; i < pMesh->getChildrenCount(); ++i)
            {
                Node *pNode = pMesh->getChild(i);
                if (pNode->getNodeType() == Node::E_TYPE_SUBMESHES)
                {
                    SubMeshes *pSubMeshes = (SubMeshes *)pNode;
                    size_t j = 0;

                    for (j = 0; j < pSubMeshes->getChildrenCount(); ++j)
                    {
                        SubMesh *pSubMesh = (SubMesh *)pSubMeshes->getChild(j);
                        T3D_ASSERT(pSubMesh->getNodeType() == Node::E_TYPE_SUBMESH);
                        putVertexBlendAndWeightAttributes(pSubMesh->mVB);
                    }

                    break;
                }
            }
        }
    }

    String FBXConverter::FbxWrapModeToString(FbxTexture::EWrapMode eWrapMode) const
    {
        String s;
        switch (eWrapMode)
        {
        case FbxTexture::eRepeat:
            {
                s = "repeat";
            }
            break;
        case FbxTexture::eClamp:
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

    bool FBXConverter::processBoundingBox(Node *pNode)
    {
        bool result = true;

        if (pNode->getNodeType() == Node::E_TYPE_SUBMESH)
        {
            SubMesh *pSubMesh = (SubMesh *)pNode;
            result = computeBoundingBox(pSubMesh);
        }

        size_t i = 0;
        for (i = 0; i < pNode->getChildrenCount(); ++i)
        {
            Node *pChild = pNode->getChild(i);
            result = processBoundingBox(pChild);
        }

        return result;
    }

    bool FBXConverter::computeBoundingBox(SubMesh *pSubMesh)
    {
        bool result = false;

        switch (mSettings.mBoundType)
        {
        case E_BT_SPHERE:
            {
                result = computeBoundingSphere(pSubMesh);
            }
            break;
        case E_BT_AABB:
            {
                result = computeAlignAxisBoundingBox(pSubMesh);
            }
            break;
        }

        return result;
    }

    bool FBXConverter::computeBoundingSphere(SubMesh *pSubMesh)
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float radius = 0.0f;

        VertexBuffer *pVB = pSubMesh->mVB;

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

        SphereBound *pBound = new SphereBound(pSubMesh->getID());
        pSubMesh->addChild(pBound);

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

        return true;
    }

    bool FBXConverter::computeAlignAxisBoundingBox(SubMesh *pSubMesh)
    {
        VertexBuffer *pVB = pSubMesh->mVB;

        float fMinX = FLT_MAX;
        float fMaxX = FLT_MIN;
        float fMinY = FLT_MAX;
        float fMaxY = FLT_MIN;
        float fMinZ = FLT_MAX;
        float fMaxZ = FLT_MIN;

//         if (mCurBound != nullptr)
//         {
//             AabbBound *pBound = new AabbBound();// (AabbBound *)mCurBound;
//             fMinX = pBound->mMinX;
//             fMaxX = pBound->mMaxX;
//             fMinY = pBound->mMinY;
//             fMaxY = pBound->mMaxY;
//             fMinZ = pBound->mMinZ;
//             fMaxZ = pBound->mMaxZ;
//         }

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

        pBound = new AabbBound(pSubMesh->getID());
        pSubMesh->addChild(pBound);

        pBound->mMinX = fMinX;
        pBound->mMaxX = fMaxX;
        pBound->mMinY = fMinY;
        pBound->mMaxY = fMaxY;
        pBound->mMinZ = fMinZ;
        pBound->mMaxZ = fMaxZ;

        return true;
    }

    void FBXConverter::convertMatrix(const FbxAMatrix &fbxMat, Matrix4 &m)
    {
        m[0][0] = fbxMat[0][0];
        m[1][0] = fbxMat[0][1];
        m[2][0] = fbxMat[0][2];
        m[3][0] = fbxMat[0][3];

        m[0][1] = fbxMat[1][0];
        m[1][1] = fbxMat[1][1];
        m[2][1] = fbxMat[1][2];
        m[3][1] = fbxMat[1][3];

        m[0][2] = fbxMat[2][0];
        m[1][2] = fbxMat[2][1];
        m[2][2] = fbxMat[2][2];
        m[3][2] = fbxMat[2][3];

        m[0][3] = fbxMat[3][0];
        m[1][3] = fbxMat[3][1];
        m[2][3] = fbxMat[3][2];
        m[3][3] = fbxMat[3][3];
    }
}
