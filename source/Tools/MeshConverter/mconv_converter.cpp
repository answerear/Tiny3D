

#include "mconv_converter.h"
#include "mconv_command.h"
#include "mconv_settings.h"
#include "mconv_fbxserializer.h"
#include "mconv_t3dSerializer.h"
#include "mconv_node.h"
#include "mconv_scene.h"
#include "mconv_mesh.h"


namespace mconv
{
    Converter::Converter()
        : mImporter(nullptr)
        , mExporter(nullptr)
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

        Scene *pScene = new Scene("Root");
        mDstData = pScene;

        processFbxScene(pFbxScene);

        return true;
    }

    bool Converter::processFbxScene(FbxScene *pFbxScene)
    {
        FbxNode *pFbxRoot = pFbxScene->GetRootNode();

        return processFbxNode(pFbxRoot);
    }

    bool Converter::processFbxNode(FbxNode *pFbxNode)
    {
        bool result = false;
        if (pFbxNode->GetNodeAttribute() != nullptr)
        {
            FbxNodeAttribute::EType attribType = pFbxNode->GetNodeAttribute()->GetAttributeType();
            switch (attribType)
            {
            case FbxNodeAttribute::eMesh:
                {
                    result = processFbxMesh(pFbxNode);
                }
                break;
            case FbxNodeAttribute::eSkeleton:
                {
                    result = processFbxSkeleton(pFbxNode);
                }
                break;
            case FbxNodeAttribute::eCamera:
                {
                    result = processFbxCamera(pFbxNode);
                }
                break;
            case FbxNodeAttribute::eLight:
                {
                    result = processFbxLight(pFbxNode);
                }
                break;
            }
        }

        int i = 0;
        for (i = 0; i < pFbxNode->GetChildCount(); ++i)
        {
            processFbxNode(pFbxNode->GetChild(i));
        }

        return result;
    }

    bool Converter::processFbxMesh(FbxNode *pFbxNode)
    {
        FbxMesh *pFbxMesh = pFbxNode->GetMesh();

        if (pFbxMesh == nullptr)
        {
            T3D_LOG_ERROR("FBX mesh is invalid !");
            return false;
        }

        int nTriangleCount = pFbxMesh->GetPolygonCount();
        int nVertexCount = 0;
        int i = 0, j = 0;

        Vertex vertex;

        for (i = 0; i < nTriangleCount; ++i)
        {
            for (j = 0; j < 3; ++j)
            {
                int nControlPointIdx = pFbxMesh->GetPolygonVertex(i, j);

                // 读取顶点位置信息
                readPosition(pFbxMesh, nControlPointIdx, vertex.mPosition);

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

                ++nVertexCount;
            }
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

    bool Converter::processFbxSkeleton(FbxNode *pFbxNode)
    {
        return true;
    }

    bool Converter::processFbxCamera(FbxNode *pFbxNode)
    {
        return true;
    }

    bool Converter::processFbxLight(FbxNode *pFbxNode)
    {
        return true;
    }
}
