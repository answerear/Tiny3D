

#include "mconv_converter.h"
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


namespace mconv
{
    Converter::Converter()
        : mImporter(nullptr)
        , mExporter(nullptr)
        , mHasSkeleton(false)
#ifdef _DEBUG
        , mTabCount(0)
#endif
    {

    }

    Converter::~Converter()
    {
        delete mImporter;
        mImporter = nullptr;

        delete mExporter;
        mExporter = nullptr;
    }

    bool Converter::execute(int argc, char *argv[])
    {
        bool result = false;

        Command command;
        result = command.parse(argc, argv, mSettings);

        result = result && importScene();
        result = result && convert();
        result = result && exportScene();

        cleanup();

        return result;
    }

    bool Converter::importScene()
    {
        delete mImporter;
        mImporter = nullptr;

        if ((mSettings.mSrcType & E_FILETYPE_FBX)
            || (mSettings.mSrcType & E_FILETYPE_DAE))
        {
            mImporter = new FbxSerializer();
        }
        else if ((mSettings.mSrcType & E_FILETYPE_T3D) == E_FILETYPE_T3D)
        {
            mImporter = new T3DSerializer();
        }
        else if (mSettings.mSrcType & E_FILETYPE_T3DB)
        {
            mImporter = new T3DBinSerializer();
        }
        else if (mSettings.mSrcType & E_FILETYPE_T3DT)
        {
            mImporter = new T3DXMLSerializer();
        }
        else
        {
            T3D_LOG_ERROR("Create importer failed ! Because of invalid source file format !");
        }

        bool result = false;

        if (mImporter != nullptr)
        {
            result = mImporter->load(mSettings.mSrcPath, mSrcData);
        }

        return result;
    }

    bool Converter::exportScene()
    {
        delete mExporter;
        mExporter = nullptr;

        if ((mSettings.mDstType & E_FILETYPE_FBX)
            || (mSettings.mDstType & E_FILETYPE_DAE))
        {
            mExporter = new FbxSerializer();
        }
        else if ((mSettings.mDstType & E_FILETYPE_T3D) == E_FILETYPE_T3D)
        {
            mExporter = new T3DSerializer();
        }
        else if (mSettings.mDstType & E_FILETYPE_T3DB)
        {
            mExporter = new T3DBinSerializer();
        }
        else if (mSettings.mDstType & E_FILETYPE_T3DT)
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
            result = mExporter->save(mSettings.mDstPath, mDstData);
        }

        return result;
    }

    bool Converter::convert()
    {
        bool result = false;
        if ((mSettings.mDstType & E_FILETYPE_FBX) || (mSettings.mDstType & E_FILETYPE_DAE))
        {
            result = convertToFBX();
        }
        else if (mSettings.mDstType & E_FILETYPE_T3D)
        {
            result = convertToT3D();
        }
        else
        {
            T3D_LOG_ERROR("Convert failed ! Because of invalid destination file format !");
            T3D_ASSERT(0);
        }

        return result;
    }

    bool Converter::convertToFBX()
    {
        

        return true;
    }

    bool Converter::convertToT3D()
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
        mDstData = pScene;

#ifdef _DEBUG
        mTabCount = 0;
#endif

        processFbxScene(pFbxScene, pScene);

        return true;
    }

    bool Converter::processFbxScene(FbxScene *pFbxScene, Node *pRoot)
    {
        FbxNode *pFbxRoot = pFbxScene->GetRootNode();
        bool result = processFbxNode(pFbxRoot, pRoot);
        result = result && optimizeMesh(pRoot);
        return result;
    }

    bool Converter::processFbxNode(FbxNode *pFbxNode, Node *pParent)
    {
        bool result = false;
        Node *pNode = nullptr;

#ifdef _DEBUG
        std::stringstream ssTab;
        for (int t = 0; t < mTabCount; ++t)
        {
            ssTab<<"\t";
        }
        T3D_LOG_INFO("%sNode : %s", ssTab.str().c_str(), pFbxNode->GetName());
#endif

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
                        Model *pModel = new Model(name);
                        pParent->addChild(pModel);

                        result = processFbxMesh(pFbxNode, pModel, pNode);
                        result = result && processFbxSkin(pFbxNode, pParent, (Mesh *)pNode);
                        result = result && processFbxMaterial(pFbxNode, pModel);

                        pNode = pModel;
                    }
                    break;
                case FbxNodeAttribute::eSkeleton:
                    {
//                         result = processFbxSkeleton(pFbxNode, pParent, pNode);
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

        for (i = 0; i < pFbxNode->GetChildCount(); ++i)
        {
            result = result && processFbxNode(pFbxNode->GetChild(i), pNode);
        }

#ifdef _DEBUG
        mTabCount--;
#endif

        return result;
    }

    bool Converter::processFbxMesh(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode)
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
        Mesh *pMesh = new Mesh(name);
        pParent->addChild(pMesh);
        pNewNode = pMesh;
        mHasVertexBlending = false;

        FbxVector4 T = pFbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        FbxVector4 R = pFbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
        FbxVector4 S = pFbxNode->GetGeometricScaling(FbxNode::eSourcePivot);
        pMesh->mWorldMatrix.SetTRS(T, R, S);

        int nTriangleCount = pFbxMesh->GetPolygonCount();
        int nVertexCount = 0;
        int i = 0, j = 0;

        // 生成顶点属性
        processVertexAttribute(pFbxMesh, pMesh);

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
                FbxVector4 pos(vertex.mPosition);
                pos = pMesh->mWorldMatrix.MultT(pos);
                vertex.mPosition[0] = pos[0];
                vertex.mPosition[1] = pos[1];
                vertex.mPosition[2] = pos[2];

                // 读取顶点颜色信息
                FbxVector4 color;
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
                FbxVector2 uv;
                int nTexCount = pFbxMesh->GetTextureUVCount();
                for (k = 0; k < nTexCount; ++k)
                {
                    if (readUV(pFbxMesh, nControlPointIdx, pFbxMesh->GetTextureUVIndex(i, j), k, uv))
                    {
                        vertex.mTexElements.push_back(uv);
                    }
                }

                // 读取顶点法线
                FbxVector3 normal;
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
                FbxVector3 binormal;
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
                FbxVector3 tangent;
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

                pMesh->mVertices.push_back(vertex);

                ++nVertexCount;
            }
        }

        return true;
    }

    bool Converter::processVertexAttribute(FbxMesh *pFbxMesh, Mesh *pMesh)
    {
        int i = 0;

        // 位置
        VertexAttribute attribute;
        attribute.mVertexType = VertexAttribute::E_VT_POSITION;
        attribute.mSize = 3;
        attribute.mDataType = VertexAttribute::E_VT_FLOAT;
        pMesh->mAttributes.push_back(attribute);

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
            pMesh->mAttributes.push_back(attribute);
        }

        // 法线
        for (i = 0; i < pFbxMesh->GetElementNormalCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_NORMAL;
            attribute.mSize = 3;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            pMesh->mAttributes.push_back(attribute);
        }

        // 副法线
        for (i = 0; i < pFbxMesh->GetElementBinormalCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_BINORMAL;
            attribute.mSize = 3;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            pMesh->mAttributes.push_back(attribute);
        }

        // 切线
        for (i = 0; i < pFbxMesh->GetElementTangentCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_TANGENT;
            attribute.mSize = 3;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            pMesh->mAttributes.push_back(attribute);
        }

        // 颜色
        for (i = 0; i < pFbxMesh->GetElementVertexColorCount(); ++i)
        {
            attribute.mVertexType = VertexAttribute::E_VT_COLOR;
            attribute.mSize = 4;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            pMesh->mAttributes.push_back(attribute);
        }

        return true;
    }

    bool Converter::readPosition(FbxMesh *pFbxMesh, int nControlPointIdx, FbxVector3 &pos)
    {
        FbxVector4 *pControlPoint = pFbxMesh->GetControlPoints();
        pos[0] = pControlPoint[nControlPointIdx][0];
        pos[1] = pControlPoint[nControlPointIdx][1];
        pos[2] = pControlPoint[nControlPointIdx][2];
        return true;
    }

    bool Converter::readColor(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector4 &color)
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

    bool Converter::readUV(FbxMesh *pFbxMesh, int nControlPointIdx, int nUVIndex, int nLayer, FbxVector2 &uv)
    {
        if (pFbxMesh->GetElementUVCount() < 1)
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

    bool Converter::readNormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &normal)
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

    bool Converter::readTangent(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &tangent)
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

    bool Converter::readBinormal(FbxMesh *pFbxMesh, int nControlPointIdx, int nVertexIndex, int nLayer, FbxVector3 &binormal)
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

    bool Converter::readMaterial(FbxMesh *pFbxMesh, int nTriangleIndex, int &nMaterialIndex)
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

    bool Converter::processFbxSkeleton(FbxNode *pFbxNode, Node *pParent, Model *pModel)
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
        }

        String name = pFbxNode->GetName();
        if (name.empty())
        {
            name = "bone";
        }
        Bone *pBone = new Bone(name);
        pParent->addChild(pBone);

        FbxAMatrix &M = pFbxNode->EvaluateLocalTransform();
        pBone->mLocalTransform = M;

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

    bool Converter::processFbxCamera(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode)
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

    bool Converter::processFbxLight(FbxNode *pFbxNode, Node *pParent, Node *&pNewNode)
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

    bool Converter::processFbxMaterial(FbxNode *pFbxNode, Node *pParent)
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

        Materials *pMaterials = new Materials("Materials");
        pParent->addChild(pMaterials);

        for (i = 0; i < nMaterialCount; ++i)
        {
            FbxSurfaceMaterial *pFbxMaterial = pFbxNode->GetMaterial(i);
            Material *pMaterial = new Material(pFbxMaterial->GetName());
            pMaterials->addChild(pMaterial);
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

                FbxTexture *pFbxTexture = (FbxTexture *)pFbxMatPhong->Diffuse.GetSrcObject();
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

                int nTextureCount = pFbxMatLambert->Diffuse.GetSrcObjectCount();
                if (nTextureCount > 0)
                {
                    Textures *pTextures = new Textures("Textures");
                    pMaterial->addChild(pTextures);

                    int i = 0;
                    for (i = 0; i < nTextureCount; ++i)
                    {
                        FbxFileTexture *pFbxTexture = (FbxFileTexture *)pFbxMatLambert->Diffuse.GetSrcObject(i);
                        if (pFbxTexture != nullptr)
                        {
                            Texture *pTexture = new Texture(pFbxTexture->GetName());
                            pTextures->addChild(pTexture);

                            pTexture->mFilename = pFbxTexture->GetFileName();
                        }
                    }
                }
            }
        }

        return true;
    }

    bool Converter::processFbxAnimation(FbxNode *pFbxNode, Model *pModel)
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
                    }
                }
                else
                {

                }
            }
        }

        return true;
    }

    bool Converter::processFbxSkin(FbxNode *pFbxNode, Node *pParent, Mesh *pMesh)
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
            Skin *pSkin = new Skin(pFbxSkin->GetName());
            pModel->addChild(pSkin);

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
                FbxAMatrix matOffset = matLink.Inverse() * mat * matGeometry;

                Bone *pBone = new Bone(pFbxLinkNode->GetName());
                pBone->mLocalTransform = matOffset;
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

    bool Converter::optimizeMesh(Node *pNode)
    {
        if (pNode->getNodeType() == Node::E_TYPE_MESH)
        {
            Mesh *pMesh = (Mesh *)pNode;
            pMesh->split();
        }

        size_t i = 0;
        for (i = 0; i < pNode->getChildrenCount(); ++i)
        {
            Node *pChild = pNode->getChild(i);
            optimizeMesh(pChild);
        }

        return true;
    }

    bool Converter::searchSkeletonRoot(FbxNode *pFbxNode, FbxNode *&pFbxRootNode)
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

    bool Converter::searchSkeleton(FbxNode *pFbxNode)
    {
        bool result = false;
        SkeletonsConstItr itr = mSkeletons.find(pFbxNode);
        if (itr != mSkeletons.end())
        {
            result = true;
        }
        return result;
    }

    bool Converter::searchAnimation(const String &name, Node *pNode, Animation *&pAnim)
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

    bool Converter::searchAction(const String &name, Animation *pAnim, Action *&pAction)
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

    bool Converter::updateVertexBlendIndexAndWeight(Mesh *pMesh, int nCtrlPointIdx, int nBlendIndex, double fBlendWeight)
    {
        VerticesItr itr = pMesh->mVertices.begin();

        while (itr != pMesh->mVertices.end())
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
            }

            ++itr;
        }

        return true;
    }

    void Converter::updateVertexBlendAttributes(Mesh *pMesh)
    {
        if (!mHasVertexBlending)
        {
            VertexAttribute attribute;
            attribute.mVertexType = VertexAttribute::E_VT_BLEND_WEIGHT;
            attribute.mSize = 4;
            attribute.mDataType = VertexAttribute::E_VT_FLOAT;
            pMesh->mAttributes.push_back(attribute);

            attribute.mVertexType = VertexAttribute::E_VT_BLEND_INDEX;
            attribute.mSize = 4;
            attribute.mDataType = VertexAttribute::E_VT_INT32;
            pMesh->mAttributes.push_back(attribute);

            mHasVertexBlending = true;
        }
    }

    void Converter::cleanup()
    {
        Node *pNode = (Node *)mDstData;
        pNode->removeAllChildren();
    }
}
