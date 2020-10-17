/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


#include "T3DFBXReader.h"
#include "T3DFBXDataStream.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    FBXReaderPtr FBXReader::create()
    {
        FBXReaderPtr reader = new FBXReader();
        reader->release();
        return reader;
    }

    //--------------------------------------------------------------------------

    FBXReader::FBXReader()
        : mFbxManager(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    FBXReader::~FBXReader()
    {

    }

    //--------------------------------------------------------------------------

    TResult FBXReader::initFbxObjects()
    {
        TResult ret = T3D_OK;

        do 
        {
            ret = destroyFbxObjects();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 创建FBX SDK Manager
            mFbxManager = FbxManager::Create();
            if (mFbxManager == nullptr)
            {
                MCONV_LOG_ERROR("Unable to create FBX Manager !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            // 创建IOSettings object
            FbxIOSettings *pIOSettings = FbxIOSettings::Create(mFbxManager, IOSROOT);
            mFbxManager->SetIOSettings(pIOSettings);

            // Load plugins from the executable directory (optional)
            FbxString path = FbxGetApplicationDirectory();
            mFbxManager->LoadPluginsDirectory(path.Buffer());

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::destroyFbxObjects()
    {
        if (mFbxManager != nullptr)
        {
            mFbxManager->Destroy();
            mFbxManager = nullptr;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::importFbxScene(DataStream &stream, FbxScene *pFbxScene)
    {
        TResult ret = T3D_OK;

        do 
        {
            // Create an fbx importer object.
            FbxImporter *importer = FbxImporter::Create(mFbxManager, "");
            if (importer == nullptr)
            {
                MCONV_LOG_ERROR("Unable to create FBX importer object !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            // Initialize the importer by providing a filename.
            FbxDataStream fbxStream(stream, mFbxManager, true);
            bool bImportStatus = importer->Initialize(&fbxStream, nullptr, -1, mFbxManager->GetIOSettings());

            if (!bImportStatus)
            {
                MCONV_LOG_ERROR("Unable to initialize FBX importer !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            MCONV_LOG_INFO("Initialize FBX importer successfully !");

            // Import the scene
            bool bStatus = importer->Import(pFbxScene);

            if (!bStatus)
            {
                MCONV_LOG_ERROR("Import FBX file failed !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            MCONV_LOG_INFO("Import FBX file successfully !");

            // Destroy the importer
            importer->Destroy();
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::setupMetricSystem(FbxScene *pFbxScene)
    {
        TResult ret = T3D_OK;

        do 
        {
            // 统一切换成OpenGL的右手坐标系和以米为单位的坐标系
            FbxAxisSystem SceneAxisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();
            FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
            if (SceneAxisSystem != OurAxisSystem)
            {
                MCONV_LOG_INFO("Start converting axis system to RIGHT HAND ......");
                OurAxisSystem.ConvertScene(pFbxScene);
                MCONV_LOG_INFO("Completed converting axis system !");
            }

            // Convert Unit System to what is used in this example, if needed
            FbxSystemUnit SceneSystemUnit = pFbxScene->GetGlobalSettings().GetSystemUnit();
            if (SceneSystemUnit.GetScaleFactor() != 1.0)
            {
                MCONV_LOG_INFO("Start converting unit to centimeter ......");
                //The unit in this example is centimeter.
                FbxSystemUnit::cm.ConvertScene(pFbxScene);
                MCONV_LOG_INFO("Completed converting unit !");
            }

            // 不是三角形为面的mesh，统一转换成三角形为面的mesh
            MCONV_LOG_INFO("Start converting face to triangles ......");
            FbxGeometryConverter converter(mFbxManager);
            converter.Triangulate(pFbxScene, true);
            MCONV_LOG_INFO("Completed converting face to triangles !");
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::parse(DataStream &stream, Model *model)
    {
        TResult ret = T3D_OK;

        FbxScene *pFbxScene = nullptr;

        do 
        {
            // Initialize fbx objects.
            ret = initFbxObjects();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // Create an FBX scene. 
            // This object holds most objects imported/exported from/to files.
            pFbxScene = FbxScene::Create(mFbxManager, "My Scene");
            if (pFbxScene == nullptr)
            {
                MCONV_LOG_ERROR("Unable to create FBX scene !");
                ret = T3D_ERR_SYS_NOT_INIT;
                break;
            }

            // Import data from fbx file.
            ret = importFbxScene(stream, pFbxScene);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // Setup metric system
            ret = setupMetricSystem(pFbxScene);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // Scene
            ret = processFbxScene(pFbxScene, model);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // Animation
            ret = processFbxAnimation(pFbxScene);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (0);

        destroyFbxObjects();

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxScene(FbxScene *pFbxScene, Model *model)
    {
        TResult ret = T3D_OK;

        do 
        {
            String name = pFbxScene->GetName();

            FbxNode *pFbxRoot = pFbxScene->GetRootNode();

            mTabCount = 0;
            processFbxNode(pFbxRoot);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxNode(FbxNode *pFbxNode)
    {
        TResult ret = T3D_OK;

        do 
        {
            std::stringstream ss;
            for (size_t i = 0; i < mTabCount; ++i)
            {
                ss << "\t";
            }

            MCONV_LOG_INFO("%s%s(Node)", ss.str().c_str(), pFbxNode->GetName());
            MCONV_LOG_INFO("%s{", ss.str().c_str());
            FbxDouble3 pos = pFbxNode->LclTranslation.Get();
            MCONV_LOG_INFO("%s\toriginal position (%f, %f, %f)", ss.str().c_str(), pos[0], pos[1], pos[2]);
            FbxDouble3 rotation = pFbxNode->LclRotation.Get();
            MCONV_LOG_INFO("%s\toriginal rotation (%f, %f, %f)", ss.str().c_str(), rotation[0], rotation[1], rotation[2]);
            FbxDouble3 scale = pFbxNode->LclScaling.Get();
            MCONV_LOG_INFO("%s\toriginal scaling (%f, %f, %f)", ss.str().c_str(), scale[0], scale[1], scale[2]);

            pos = pFbxNode->GeometricTranslation.Get();
            MCONV_LOG_INFO("%s\tgeometry position (%f, %f, %f)", ss.str().c_str(), pos[0], pos[1], pos[2]);
            rotation = pFbxNode->GeometricRotation.Get();
            MCONV_LOG_INFO("%s\tgeometry rotation (%f, %f, %f)", ss.str().c_str(), rotation[0], rotation[1], rotation[2]);
            scale = pFbxNode->GeometricScaling.Get();
            MCONV_LOG_INFO("%s\tgeometry scaling (%f, %f, %f)", ss.str().c_str(), scale[0], scale[1], scale[2]);
//             pos = pFbxNode->RotationOffset.Get();
//             MCONV_LOG_INFO("%s\trotation offset (%f, %f, %f)", ss.str().c_str(), pos[0], pos[1], pos[2]);
//             pos = pFbxNode->RotationPivot.Get();
//             MCONV_LOG_INFO("%s\trotation pivot (%f, %f, %f)", ss.str().c_str(), pos[0], pos[1], pos[2]);
//             rotation = pFbxNode->PreRotation.Get();
//             MCONV_LOG_INFO("%s\tpre-rotation (%f, %f, %f)", ss.str().c_str(), rotation[0], rotation[1], rotation[2]);
//             rotation = pFbxNode->PostRotation.Get();
//             MCONV_LOG_INFO("%s\tpost-rotation (%f, %f, %f)", ss.str().c_str(), rotation[0], rotation[1], rotation[2]);

            FbxAMatrix &lFbxLocalMatrix = pFbxNode->EvaluateLocalTransform();
            Matrix4 mat;
            convertMatrix(lFbxLocalMatrix, mat);

            Vector3 t;
            Vector3 s;
            Quaternion q;
            mat.decomposition(t, s, q);
            Matrix3 R;
            q.toRotationMatrix(R);
            
            Radian pitch, yaw, roll;

            EFbxRotationOrder order = pFbxNode->RotationOrder;
            switch (order)
            {
            case eEulerXYZ:
                R.toEulerAnglesZYX(roll, yaw, pitch);
                break;
            case eEulerXZY:
                R.toEulerAnglesYZX(yaw, roll, pitch);
                break;
            case eEulerYZX:
                R.toEulerAnglesXZY(pitch, roll, yaw);
                break;
            case eEulerYXZ:
                R.toEulerAnglesZXY(roll, pitch, yaw);
                break;
            case eEulerZXY:
                R.toEulerAnglesYXZ(yaw, pitch, roll);
                break;
            case eEulerZYX:
                R.toEulerAnglesXYZ(pitch, yaw, roll);
                break;
            case eSphericXYZ:
                break;
            }

            MCONV_LOG_INFO("%s\tposition (%f, %f, %f)", ss.str().c_str(), t.x(), t.y(), t.z());
            MCONV_LOG_INFO("%s\trotation (%f, %f, %f)", ss.str().c_str(), pitch.valueDegrees(), yaw.valueDegrees(), roll.valueDegrees());
            MCONV_LOG_INFO("%s\tscaling (%f, %f, %f)", ss.str().c_str(), s.x(), s.y(), s.z());

            for (int32_t i = 0; i < pFbxNode->GetNodeAttributeCount(); ++i)
            {
                FbxNodeAttribute *pFbxAttrib = pFbxNode->GetNodeAttributeByIndex(i);

                FbxNodeAttribute::EType type = pFbxAttrib->GetAttributeType();

                switch (type)
                {
                case FbxNodeAttribute::EType::eNull:
                    MCONV_LOG_INFO("%s\tAttribute : eNull", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eMarker:
                    MCONV_LOG_INFO("%s\tAttribute : eMarker", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eSkeleton:
                    MCONV_LOG_INFO("%s\tAttribute : eSkeleton", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eMesh:
                    {
                        MCONV_LOG_INFO("%s\tAttribute : eMesh", ss.str().c_str());
                        FbxMesh *pFbxMesh = (FbxMesh *)pFbxNode->GetNodeAttributeByIndex(i);
                        ret = processFbxMesh(pFbxNode, pFbxMesh);
                    }
                    break;
                case FbxNodeAttribute::EType::eNurbs:
                    MCONV_LOG_INFO("%s\tAttribute : eNurbs", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::ePatch:
                    MCONV_LOG_INFO("%s\tAttribute : ePatch", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eCamera:
                    MCONV_LOG_INFO("%s\tAttribute : eCamera", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eCameraStereo:
                    MCONV_LOG_INFO("%s\tAttribute : eCameraStereo", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eCameraSwitcher:
                    MCONV_LOG_INFO("%s\tAttribute : eCameraSwitcher", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eLight:
                    MCONV_LOG_INFO("%s\tAttribute : eLight", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eOpticalReference:
                    MCONV_LOG_INFO("%s\tAttribute : eOpticalReference", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eOpticalMarker:
                    MCONV_LOG_INFO("%s\tAttribute : eOpticalMarker", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eNurbsCurve:
                    MCONV_LOG_INFO("%s\tAttribute : eNurbsCurve", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eTrimNurbsSurface:
                    MCONV_LOG_INFO("%s\tAttribute : eTrimNurbsSurface", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eBoundary:
                    MCONV_LOG_INFO("%s\tAttribute : eBoundary", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eNurbsSurface:
                    MCONV_LOG_INFO("%s\tAttribute : eNurbsSurface", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eShape:
                    MCONV_LOG_INFO("%s\tAttribute : eShape", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eLODGroup:
                    MCONV_LOG_INFO("%s\tAttribute : eLODGroup", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eSubDiv:
                    MCONV_LOG_INFO("%s\tAttribute : eSubDiv", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eCachedEffect:
                    MCONV_LOG_INFO("%s\tAttribute : eCachedEffect", ss.str().c_str());
                    break;
                case FbxNodeAttribute::EType::eLine:
                    MCONV_LOG_INFO("%s\tAttribute : eLine", ss.str().c_str());
                    break;
                }
            }

            MCONV_LOG_INFO("%s}", ss.str().c_str());

            mTabCount++;

            for (int32_t i = 0; i < pFbxNode->GetChildCount(); ++i)
            {
                FbxNode *pFbxChild = pFbxNode->GetChild(i);
                processFbxNode(pFbxChild);
            }

            mTabCount--;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxMesh(FbxNode *pFbxNode, FbxMesh *pFbxMesh)
    {
        TResult ret = T3D_OK;

        do 
        {
            ret = processFbxMeshAttributes(pFbxMesh);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = processFbxMeshData(pFbxMesh);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxMeshAttributes(FbxMesh *pFbxMesh)
    {
        TResult ret = T3D_OK;

        int32_t layers = 0;
        int32_t i = 0;

        // POSITION

        // COLOR
        layers = pFbxMesh->GetElementVertexColorCount();
        for (i = 0; i < layers; ++i)
        {

        }

        // TEXCOORD
        layers = pFbxMesh->GetElementUVCount();
        for (i = 0; i < layers; ++i)
        {

        }

        // NORMAL
        layers = pFbxMesh->GetElementNormalCount();
        for (i = 0; i < layers; ++i)
        {

        }

        // BINORMAL
        layers = pFbxMesh->GetElementBinormalCount();
        for (i = 0; i < layers; ++i)
        {

        }

        // TANGENT
        layers = pFbxMesh->GetElementTangentCount();
        for (i = 0; i < layers; ++i)
        {

        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxMeshData(FbxMesh *pFbxMesh)
    {
        TResult ret = T3D_OK;

        do 
        {
            int32_t triangleCount = pFbxMesh->GetPolygonCount();

            int32_t vertexCount = 0;
            int32_t layers = 0;

            for (int32_t i = 0; i < triangleCount; ++i)
            {
                for (int32_t j = 0; j < 3; ++j)
                {
                    int32_t ctrlPointIdx = pFbxMesh->GetPolygonVertex(i, j);

                    // POSITION
                    Vector3 pos;
                    ret = readPosition(pFbxMesh, ctrlPointIdx, pos);
                    if (T3D_FAILED(ret))
                    {
                        break;
                    }

                    // COLOR
                    ColorRGBA color;
                    int32_t k;
                    layers = pFbxMesh->GetElementVertexColorCount();
                    for (k = 0; k < layers; k++)
                    {
                        ret = readColor(pFbxMesh, ctrlPointIdx, vertexCount, k, 
                            color);
                    };

                    // TEXCOORD
                    Vector2 uv;
                    layers = pFbxMesh->GetElementUVCount();
                    for (k = 0; k < layers; k++)
                    {
                        int32_t uvIdx = pFbxMesh->GetTextureUVIndex(i, j);
                        ret = readUV(pFbxMesh, ctrlPointIdx, uvIdx, k, uv);
                    }

                    // NORMAL
                    Vector3 normal;
                    layers = pFbxMesh->GetElementNormalCount();
                    for (k = 0; k < layers; k++)
                    {
                        ret = readNormal(pFbxMesh, ctrlPointIdx, vertexCount, k,
                            normal);
                    }

                    // BINORMAL
                    Vector3 binormal;
                    layers = pFbxMesh->GetElementBinormalCount();
                    for (k = 0; k < layers; k++)
                    {
                        ret = readBinormal(pFbxMesh, ctrlPointIdx, vertexCount,
                            k, binormal);
                    }

                    // TANGENT
                    Vector3 tangent;
                    layers = pFbxMesh->GetElementTangentCount();
                    for (k = 0; k < layers; k++)
                    {
                        ret = readTangent(pFbxMesh, ctrlPointIdx, vertexCount,
                            k, tangent);
                    }
                }

                if (T3D_FAILED(ret))
                {
                    MCONV_LOG_ERROR("Read mesh info failed !");
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::readPosition(FbxMesh *pFbxMesh, int32_t ctrlPointIdx, 
        Vector3 &pos)
    {
        FbxVector4 *pControlPoint = pFbxMesh->GetControlPoints();
        pos[0] = float32_t(pControlPoint[ctrlPointIdx][0]);
        pos[1] = float32_t(pControlPoint[ctrlPointIdx][1]);
        pos[2] = float32_t(pControlPoint[ctrlPointIdx][2]);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::readColor(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
        int32_t vertexIdx, int32_t layer, ColorRGBA &color)
    {
        TResult ret = T3D_OK;

        FbxGeometryElementVertexColor *pVertexColor 
            = pFbxMesh->GetElementVertexColor(layer);

        if (pVertexColor != nullptr)
        {
            switch (pVertexColor->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pVertexColor->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            color.red() = Real(pVertexColor->GetDirectArray().GetAt(ctrlPointIdx).mBlue);
                            color.green() = Real(pVertexColor->GetDirectArray().GetAt(ctrlPointIdx).mGreen);
                            color.blue() = Real(pVertexColor->GetDirectArray().GetAt(ctrlPointIdx).mRed);
                            color.alpha() = Real(pVertexColor->GetDirectArray().GetAt(ctrlPointIdx).mAlpha);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexColor->GetIndexArray().GetAt(ctrlPointIdx);
                            color.red() = Real(pVertexColor->GetDirectArray().GetAt(idx).mBlue);
                            color.green() = Real(pVertexColor->GetDirectArray().GetAt(idx).mGreen);
                            color.blue() = Real(pVertexColor->GetDirectArray().GetAt(idx).mRed);
                            color.alpha() = Real(pVertexColor->GetDirectArray().GetAt(idx).mAlpha);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
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
                            color.red() = Real(pVertexColor->GetDirectArray().GetAt(ctrlPointIdx).mBlue);
                            color.green() = Real(pVertexColor->GetDirectArray().GetAt(ctrlPointIdx).mGreen);
                            color.blue() = Real(pVertexColor->GetDirectArray().GetAt(ctrlPointIdx).mRed);
                            color.alpha()= Real(pVertexColor->GetDirectArray().GetAt(ctrlPointIdx).mAlpha);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexColor->GetIndexArray().GetAt(ctrlPointIdx);
                            color.red() = Real(pVertexColor->GetDirectArray().GetAt(idx).mBlue);
                            color.green() = Real(pVertexColor->GetDirectArray().GetAt(idx).mGreen);
                            color.blue() = Real(pVertexColor->GetDirectArray().GetAt(idx).mRed);
                            color.alpha() = Real(pVertexColor->GetDirectArray().GetAt(idx).mAlpha);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
                        break;
                    }
                }
                break;
            default:
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::readUV(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
        int32_t uvIdx, int32_t layer, Vector2 &uv)
    {
        TResult ret = T3D_OK;

        FbxGeometryElementUV *pVertexUV = pFbxMesh->GetElementUV(layer);

        if (pVertexUV != nullptr)
        {
            switch (pVertexUV->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pVertexUV->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            uv[0] = Real(pVertexUV->GetDirectArray().GetAt(ctrlPointIdx)[0]);
                            uv[1] = Real(pVertexUV->GetDirectArray().GetAt(ctrlPointIdx)[1]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pVertexUV->GetIndexArray().GetAt(ctrlPointIdx);
                            uv[0] = Real(pVertexUV->GetDirectArray().GetAt(idx)[0]);
                            uv[1] = Real(pVertexUV->GetDirectArray().GetAt(idx)[1]);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
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
                            uv[0] = Real(pVertexUV->GetDirectArray().GetAt(uvIdx)[0]);
                            uv[1] = Real(pVertexUV->GetDirectArray().GetAt(uvIdx)[1]);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
                        break;
                    }
                }
                break;
            default:
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::readNormal(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
        int32_t vertexIdx, int32_t layer, Vector3 &normal)
    {
        TResult ret = T3D_OK;

        FbxGeometryElementNormal *pNormal = pFbxMesh->GetElementNormal(layer);

        if (pNormal != nullptr)
        {
            switch (pNormal->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pNormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            normal[0] = Real(pNormal->GetDirectArray().GetAt(ctrlPointIdx)[0]);
                            normal[1] = Real(pNormal->GetDirectArray().GetAt(ctrlPointIdx)[1]);
                            normal[2] = Real(pNormal->GetDirectArray().GetAt(ctrlPointIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pNormal->GetIndexArray().GetAt(ctrlPointIdx);
                            normal[0] = Real(pNormal->GetDirectArray().GetAt(idx)[0]);
                            normal[1] = Real(pNormal->GetDirectArray().GetAt(idx)[1]);
                            normal[2] = Real(pNormal->GetDirectArray().GetAt(idx)[2]);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
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
                            normal[0] = Real(pNormal->GetDirectArray().GetAt(vertexIdx)[0]);
                            normal[1] = Real(pNormal->GetDirectArray().GetAt(vertexIdx)[1]);
                            normal[2] = Real(pNormal->GetDirectArray().GetAt(vertexIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pNormal->GetIndexArray().GetAt(vertexIdx);
                            normal[0] = Real(pNormal->GetDirectArray().GetAt(idx)[0]);
                            normal[1] = Real(pNormal->GetDirectArray().GetAt(idx)[1]);
                            normal[2] = Real(pNormal->GetDirectArray().GetAt(idx)[2]);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
                        break;
                    }
                }
                break;
            default:
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::readBinormal(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
        int32_t vertexIdx, int32_t layer, Vector3 &binormal)
    {
        TResult ret = T3D_OK;

        FbxGeometryElementBinormal *pBinormal = pFbxMesh->GetElementBinormal(layer);

        if (pBinormal != nullptr)
        {
            switch (pBinormal->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pBinormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            binormal[0] = Real(pBinormal->GetDirectArray().GetAt(ctrlPointIdx)[0]);
                            binormal[1] = Real(pBinormal->GetDirectArray().GetAt(ctrlPointIdx)[1]);
                            binormal[2] = Real(pBinormal->GetDirectArray().GetAt(ctrlPointIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pBinormal->GetIndexArray().GetAt(ctrlPointIdx);
                            binormal[0] = Real(pBinormal->GetDirectArray().GetAt(idx)[0]);
                            binormal[1] = Real(pBinormal->GetDirectArray().GetAt(idx)[1]);
                            binormal[2] = Real(pBinormal->GetDirectArray().GetAt(idx)[2]);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
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
                            binormal[0] = Real(pBinormal->GetDirectArray().GetAt(vertexIdx)[0]);
                            binormal[1] = Real(pBinormal->GetDirectArray().GetAt(vertexIdx)[1]);
                            binormal[2] = Real(pBinormal->GetDirectArray().GetAt(vertexIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pBinormal->GetIndexArray().GetAt(vertexIdx);
                            binormal[0] = Real(pBinormal->GetDirectArray().GetAt(idx)[0]);
                            binormal[1] = Real(pBinormal->GetDirectArray().GetAt(idx)[1]);
                            binormal[2] = Real(pBinormal->GetDirectArray().GetAt(idx)[2]);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
                        break;
                    }
                }
                break;
            default:
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::readTangent(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
        int32_t vertexIdx, int32_t layer, Vector3 &tangent)
    {
        TResult ret = T3D_OK;

        FbxGeometryElementTangent *pTangent = pFbxMesh->GetElementTangent(layer);

        if (pTangent != nullptr)
        {
            switch (pTangent->GetMappingMode())
            {
            case FbxGeometryElement::eByControlPoint:
                {
                    switch (pTangent->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            tangent[0] = Real(pTangent->GetDirectArray().GetAt(ctrlPointIdx)[0]);
                            tangent[1] = Real(pTangent->GetDirectArray().GetAt(ctrlPointIdx)[1]);
                            tangent[2] = Real(pTangent->GetDirectArray().GetAt(ctrlPointIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pTangent->GetIndexArray().GetAt(ctrlPointIdx);
                            tangent[0] = Real(pTangent->GetDirectArray().GetAt(idx)[0]);
                            tangent[1] = Real(pTangent->GetDirectArray().GetAt(idx)[1]);
                            tangent[2] = Real(pTangent->GetDirectArray().GetAt(idx)[2]);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
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
                            tangent[0] = Real(pTangent->GetDirectArray().GetAt(vertexIdx)[0]);
                            tangent[1] = Real(pTangent->GetDirectArray().GetAt(vertexIdx)[1]);
                            tangent[2] = Real(pTangent->GetDirectArray().GetAt(vertexIdx)[2]);
                        }
                        break;
                    case FbxGeometryElement::eIndex:
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int idx = pTangent->GetIndexArray().GetAt(vertexIdx);
                            tangent[0] = Real(pTangent->GetDirectArray().GetAt(idx)[0]);
                            tangent[1] = Real(pTangent->GetDirectArray().GetAt(idx)[1]);
                            tangent[2] = Real(pTangent->GetDirectArray().GetAt(idx)[2]);
                        }
                        break;
                    default:
                        ret = T3D_ERR_INVALID_PARAM;
                        break;
                    }
                }
                break;
            default:
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxMaterial(FbxNode *pFbxNode, FbxMesh *pFbxMesh)
    {
        TResult ret = T3D_OK;

        int32_t materialCount = pFbxNode->GetMaterialCount();

        int32_t i = 0;
        for (i = 0; i < materialCount; ++i)
        {
            FbxSurfaceMaterial *pFbxMaterial = pFbxNode->GetMaterial(i);

            // Name
            String name = pFbxMaterial->GetName();


        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxCamera(FbxNode *pFbxNode)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxLight(FbxNode *pFbxNode)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxAnimation(FbxScene *pFbxScene)
    {
        TResult ret = T3D_OK;

        int nAnimStackCount = pFbxScene->GetSrcObjectCount<FbxAnimStack>();
        int i = 0;

        MCONV_LOG_INFO("\n");
        MCONV_LOG_INFO("Animations (%d)", nAnimStackCount);

        for (i = 0; i < nAnimStackCount; ++i)
        {
            FbxAnimStack *pFbxAnimStack = pFbxScene->GetSrcObject<FbxAnimStack>(i);
            ret = processFbxAnimation(pFbxAnimStack, pFbxScene->GetRootNode(), i);
            if (T3D_FAILED(ret))
            {
                break;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxAnimation(FbxAnimStack *pFbxAnimStack, FbxNode *pFbxNode, int32_t idx)
    {
        TResult ret = T3D_OK;

        String name = pFbxAnimStack->GetName();
        if (name.empty())
        {
            name = "Animation #" + idx;
        }

        int nAnimLayerCount = pFbxAnimStack->GetMemberCount<FbxAnimLayer>();
        int i = 0;

        MCONV_LOG_INFO("\tAnimation : %s (%d)", name.c_str(), nAnimLayerCount);

        for (i = 0; i < nAnimLayerCount; ++i)
        {
            FbxAnimLayer *pFbxAnimLayer = pFbxAnimStack->GetMember<FbxAnimLayer>(i);
            ret = processFbxAnimation(pFbxAnimLayer, pFbxNode, i);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxAnimation(FbxAnimLayer *pFbxAnimLayer, FbxNode *pFbxNode, int32_t idx)
    {
        TResult ret = T3D_OK;

        String name = pFbxAnimLayer->GetName();
        if (name.empty())
        {
            name = "Animation Clip #" + idx;
        }

        int nChildrenCount = pFbxNode->GetChildCount();
        int i = 0;

        MCONV_LOG_INFO("\t\tAnimation Clip (%s) (%s)", pFbxNode->GetName(), name.c_str());

        ret = processFbxAnimationChannels(pFbxAnimLayer, pFbxNode);
        if (!T3D_FAILED(ret))
        {
            for (i = 0; i < nChildrenCount; ++i)
            {
                ret = processFbxAnimation(pFbxAnimLayer, pFbxNode->GetChild(i), idx);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxAnimationChannels(FbxAnimLayer *pFbxAnimLayer, FbxNode *pFbxNode)
    {
        TResult ret = T3D_OK;

        FbxAnimCurve *pFbxAnimCurve = nullptr;

        do 
        {
            // Translation X
            pFbxAnimCurve = pFbxNode->LclTranslation.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tTranslation X : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            // Translation Y
            pFbxAnimCurve = pFbxNode->LclTranslation.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tTranslation Y : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            // Translation Z
            pFbxAnimCurve = pFbxNode->LclTranslation.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tTranslation Z : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            // Rotation X
            pFbxAnimCurve = pFbxNode->LclRotation.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tRotation X : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            // Rotation Y
            pFbxAnimCurve = pFbxNode->LclRotation.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tRotation Y : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            // Rotation Z
            pFbxAnimCurve = pFbxNode->LclRotation.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tRotation Z : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            // Scaling X
            pFbxAnimCurve = pFbxNode->LclScaling.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tScaling X : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            // Scaling Y
            pFbxAnimCurve = pFbxNode->LclScaling.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tScaling Y : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            // Scaling Z
            pFbxAnimCurve = pFbxNode->LclScaling.GetCurve(pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
            if (pFbxAnimCurve != nullptr)
            {
                MCONV_LOG_INFO("\t\t\tScaling Z : ");
                ret = processFbxAnimationCurve(pFbxAnimCurve);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (0);
        

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxAnimationCurve(FbxAnimCurve *pFbxAnimCurve)
    {
        TResult ret = T3D_OK;

        static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
        static const char* constantMode[] = { "?", "Standard", "Next" };
        static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
        static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

        int nKeyCount = pFbxAnimCurve->KeyGetCount();
        int i = 0;

        std::stringstream ss;
        char szTime[256];

        for (i = 0; i < nKeyCount; ++i)
        {
            float value = static_cast<float>(pFbxAnimCurve->KeyGetValue(i));
            FbxTime fbxTime = pFbxAnimCurve->KeyGetTime(i);

            fbxTime.GetTimeString(szTime, FbxUShort(256));

            ss << "Time : " << szTime << "\t" << "Value : " << value << "\t";
            ss << "[" << interpolation[InterpolationFlagToIndex(pFbxAnimCurve->KeyGetInterpolation(i))];
            if ((pFbxAnimCurve->KeyGetInterpolation(i)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
            {
                ss << " | " << constantMode[ConstantmodeFlagToIndex(pFbxAnimCurve->KeyGetConstantMode(i))];
            }
            else if ((pFbxAnimCurve->KeyGetInterpolation(i) & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
            {
                ss << " | " << cubicMode[TangentmodeFlagToIndex(pFbxAnimCurve->KeyGetTangentMode(i))];
                ss << " | " << tangentWVMode[TangentweightFlagToIndex(pFbxAnimCurve->KeyGet(i).GetTangentWeightMode())];
                ss << " | " << tangentWVMode[TangentVelocityFlagToIndex(pFbxAnimCurve->KeyGet(i).GetTangentVelocityMode())];
            }
            ss << "]";
            MCONV_LOG_INFO("\t\t\t\t%s", ss.str().c_str());
            ss.str("");
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    int FBXReader::InterpolationFlagToIndex(int flags)
    {
        if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
        if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
        if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
        return 0;
    }

    //--------------------------------------------------------------------------

    int FBXReader::ConstantmodeFlagToIndex(int flags)
    {
        if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
        if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
        return 0;
    }

    //--------------------------------------------------------------------------

    int FBXReader::TangentmodeFlagToIndex(int flags)
    {
        if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
        if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
        if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
        if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
        if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
        if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
        return 0;
    }

    //--------------------------------------------------------------------------

    int FBXReader::TangentweightFlagToIndex(int flags)
    {
        if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
        if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
        if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
        return 0;
    }

    //--------------------------------------------------------------------------

    int FBXReader::TangentVelocityFlagToIndex(int flags)
    {
        if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
        if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
        if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
        return 0;
    }

    //--------------------------------------------------------------------------

    void FBXReader::convertMatrix(const FbxAMatrix &src, Matrix4 &dst)
    {
        dst[0][0] = src[0][0];
        dst[1][0] = src[0][1];
        dst[2][0] = src[0][2];
        dst[3][0] = src[0][3];

        dst[0][1] = src[1][0];
        dst[1][1] = src[1][1];
        dst[2][1] = src[1][2];
        dst[3][1] = src[1][3];

        dst[0][2] = src[2][0];
        dst[1][2] = src[2][1];
        dst[2][2] = src[2][2];
        dst[3][2] = src[2][3];

        dst[0][3] = src[3][0];
        dst[1][3] = src[3][1];
        dst[2][3] = src[3][2];
        dst[3][3] = src[3][3];
    }
}

