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
#include <iomanip>


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
            // 文件头
            FileModel *data = model->getModelData();
            Script::FileFormat::FileHeader *header = data->mutable_header();
            header->set_magic(T3D_FILE_MAGIC);
            header->set_version(T3D_FILE_MDL_VERSION);
            header->set_type(Script::FileFormat::FileHeader_FileType_Model);

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
            ret = processFbxScene(pFbxScene, data);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // Bind Pose
            ret = processFbxBindPose(pFbxScene);
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

    TResult FBXReader::processFbxScene(FbxScene *pFbxScene, FileModel *model)
    {
        TResult ret = T3D_OK;

        do 
        {
            String name = pFbxScene->GetName();

            FbxNode *pFbxRoot = pFbxScene->GetRootNode();

            mTabCount = 0;
            Script::SceneSystem::Node *pNode = nullptr;
            processFbxNode(pFbxRoot, model, nullptr, pNode);
        } while (0);

        return ret;
    }

    float precision(float value)
    {
        return (float)((std::ceil((double)value * 1000000)) / 1000000);
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxNode(FbxNode *pFbxNode, FileModel *model, Script::SceneSystem::Node *parent, Script::SceneSystem::Node *&pNode)
    {
        TResult ret = T3D_OK;

        do 
        {
            // 新建一个 node
            String uuid = UUID::generate();
            auto nodes = model->mutable_nodes();
            auto rval = nodes->insert({ uuid, Script::SceneSystem::Node() });
            pNode = &rval.first->second;
            pNode->set_uuid(uuid);
            pNode->set_name(pFbxNode->GetName());

            // set parent of this node
            if (parent != nullptr)
            {
                pNode->set_parent(parent->uuid());
            }
            else
            {
                pNode->set_parent("");
            }

            std::stringstream ss;
            for (size_t i = 0; i < mTabCount; ++i)
            {
                ss << "\t";
            }

            MCONV_LOG_INFO("%s%s(Node)", ss.str().c_str(), pFbxNode->GetName());
            MCONV_LOG_INFO("%s{", ss.str().c_str());

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

            // Transform
            auto components = pNode->mutable_components();
            auto component = components->Add();
            component->set_type(Script::SceneSystem::Component_Type_Transform);
            auto transform = component->mutable_transform();
            // position
            auto pos = transform->mutable_position();
            pos->set_x(t.x());
            pos->set_y(t.y());
            pos->set_z(t.z());
            // rotation
            auto euler = transform->mutable_rotation();
            euler->set_x(pitch.valueDegrees());
            euler->set_y(yaw.valueDegrees());
            euler->set_z(roll.valueDegrees());
            // scaling
            auto scaling = transform->mutable_scaling();
            scaling->set_x(s.x());
            scaling->set_y(s.y());
            scaling->set_z(s.z());

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
                        ret = processFbxMesh(pFbxNode, pFbxMesh, model);
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
                Script::SceneSystem::Node *pChild = nullptr;
                processFbxNode(pFbxChild, model, pNode, pChild);
                pNode->add_children(pChild->uuid());
            }

            mTabCount--;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxMesh(FbxNode *pFbxNode, FbxMesh *pFbxMesh, FileModel *model)
    {
        TResult ret = T3D_OK;

        do 
        {
            // generate a mesh
            auto meshes = model->mutable_meshes();
            String uuid = UUID::generate();
            auto rval = meshes->insert({ uuid, Script::ModelSystem::MeshData() });
            Script::ModelSystem::MeshData *pMesh = &rval.first->second;
            pMesh->set_name(pFbxNode->GetName());

            // attributes of vertex in this mesh
            ret = processFbxMeshAttributes(pFbxMesh, pMesh);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // vertices and indices data
            ret = processFbxMeshData(pFbxMesh, pMesh);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = processFbxSkin(pFbxMesh);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    static Script::ModelSystem::VertexAttribute *getAttribute(
        google::protobuf::RepeatedPtrField<Script::ModelSystem::VertexBuffer> *vbs, 
        google::protobuf::RepeatedPtrField<Script::ModelSystem::VertexAttribute> *attributes,
        int idx)
    {
        Script::ModelSystem::VertexAttribute *attr = nullptr;

        if (vbs->size() > idx)
        {
            attr = attributes->Add();
        }
        else
        {
            auto vbo = &vbs->at(idx);
            attributes = vbo->mutable_attributes();
            attr = attributes->Add();
        }

        return attr;
    }


    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxMeshAttributes(FbxMesh *pFbxMesh, Script::ModelSystem::MeshData *pMesh)
    {
        TResult ret = T3D_OK;

        int32_t layers = 0;
        int32_t i = 0;

        auto vbs = pMesh->mutable_vertex_buffers();
        auto vbo = vbs->Add();
        auto attributes = vbo->mutable_attributes();

        // POSITION
        auto attr = attributes->Add();
        attr->set_semantic(Script::ModelSystem::VertexAttribute_Semantic_VAS_POSITION);
        attr->set_type(Script::ModelSystem::VertexAttribute_Type_VAT_FLOAT);
        attr->set_size(3);

        // COLOR
        layers = pFbxMesh->GetElementVertexColorCount();
        for (i = 0; i < layers; ++i)
        {
            attr = getAttribute(vbs, attributes, i);
            attr->set_semantic(Script::ModelSystem::VertexAttribute_Semantic_VAS_DIFFUSE);
            attr->set_type(Script::ModelSystem::VertexAttribute_Type_VAT_FLOAT);
            attr->set_size(4);
        }

        // TEXCOORD
        layers = pFbxMesh->GetElementUVCount();
        for (i = 0; i < layers; ++i)
        {
            attr = getAttribute(vbs, attributes, i);
            attr->set_semantic(Script::ModelSystem::VertexAttribute_Semantic_VAS_TEXCOORD);
            attr->set_type(Script::ModelSystem::VertexAttribute_Type_VAT_FLOAT);
            attr->set_size(2);
        }

        // NORMAL
        layers = pFbxMesh->GetElementNormalCount();
        for (i = 0; i < layers; ++i)
        {
            attr = getAttribute(vbs, attributes, i);
            attr->set_semantic(Script::ModelSystem::VertexAttribute_Semantic_VAS_NORMAL);
            attr->set_type(Script::ModelSystem::VertexAttribute_Type_VAT_FLOAT);
            attr->set_size(3);
        }

        // BINORMAL
        layers = pFbxMesh->GetElementBinormalCount();
        for (i = 0; i < layers; ++i)
        {
            attr = getAttribute(vbs, attributes, i);
            attr->set_semantic(Script::ModelSystem::VertexAttribute_Semantic_VAS_BINORMAL);
            attr->set_type(Script::ModelSystem::VertexAttribute_Type_VAT_FLOAT);
            attr->set_size(3);
        }

        // TANGENT
        layers = pFbxMesh->GetElementTangentCount();
        for (i = 0; i < layers; ++i)
        {
            attr = getAttribute(vbs, attributes, i);
            attr->set_semantic(Script::ModelSystem::VertexAttribute_Semantic_VAS_TANGENT);
            attr->set_type(Script::ModelSystem::VertexAttribute_Type_VAT_FLOAT);
            attr->set_size(3);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxMeshData(FbxMesh *pFbxMesh, Script::ModelSystem::MeshData *pMesh)
    {
        TResult ret = T3D_OK;

        do 
        {
            auto vbs = pMesh->mutable_vertex_buffers();
            auto vbo = &vbs->at(0);

            FbxVector4 *pPoints = pFbxMesh->GetControlPoints();
            int32_t nPointsCount = pFbxMesh->GetControlPointsCount();

            for (int32_t i = 0; i < nPointsCount; ++i)
            {
                FbxVector4 pos = pPoints[i];
                vbo->add_vertices(float(pos[0]));
                vbo->add_vertices(float(pos[1]));
                vbo->add_vertices(float(pos[2]));
                //auto v = vbo->add_vertices();
                
                //auto v = vbo->add_vertices();
                //v->add_values(pos[0]);
                //v->add_values(pos[1]);
                //v->add_values(pos[2]);
            }

            FbxGeometryElementUV* pVertexUV = pFbxMesh->GetElementUV(0);
            int rt = pVertexUV->RemapIndexTo(FbxLayerElement::eByControlPoint);

            auto pUV = pFbxMesh->GetElementUV();
            auto mappingMode = pUV->GetMappingMode();
            auto refMode = pUV->GetReferenceMode();
            int32_t cnt = pFbxMesh->GetTextureUVCount();

            FbxLayerElementArrayTemplate<FbxVector2>* pUVArray = nullptr;
            pFbxMesh->GetTextureUV(&pUVArray);
            cnt = pUVArray->GetCount();
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

    TResult FBXReader::processFbxBindPose(FbxScene *pFbxScene)
    {
        TResult ret = T3D_OK;

        int nPoseCount = pFbxScene->GetPoseCount();
        int i = 0;

        MCONV_LOG_INFO("\n");
        MCONV_LOG_INFO("Bind Pose (%d)", nPoseCount);

//         FbxMatrix a(FbxVector4(2, 0, 0, 0), FbxVector4(0, 0, 0, 0), FbxVector4(1, 1, 1));
//         double a00 = a[0][0];
//         double a01 = a[0][1];
//         double a02 = a[0][2];
//         double a03 = a[0][3];
//         double a30 = a[3][0];
// 
//         FbxAMatrix b(FbxVector4(4, 0, 0, 0), FbxVector4(0, 0, 0, 0), FbxVector4(1, 1, 1));
//         double b00 = b[0][0];
//         double b01 = b[0][1];
//         double b02 = b[0][2];
//         double b03 = b[0][3];
//         double b30 = b[3][0];

        for (i = 0; i < nPoseCount; ++i)
        {
            FbxPose *pFbxPose = pFbxScene->GetPose(i);
            String name = pFbxPose->GetName();
            int nItemCount = pFbxPose->GetCount();

            MCONV_LOG_INFO("Pose Name : %s", name.c_str());
            MCONV_LOG_INFO("Is a bind pose : %d", pFbxPose->IsBindPose());
            MCONV_LOG_INFO("Number of item in pose : %d", nItemCount);

            int j = 0;
            std::stringstream ss;
            for (j = 0; j < nItemCount; ++j)
            {
                name = pFbxPose->GetNodeName(j).GetCurrentName();

                FbxMatrix lMatrix = pFbxPose->GetMatrix(j);
                Matrix4 m;
                convertMatrix(lMatrix, m);

                ss << "Item name : " << name;

                if (!pFbxPose->IsBindPose())
                {
                    // Rest pose can have local matrix
                    ss << "    Is local space matrix: " << pFbxPose->IsLocalMatrix(j);
                }
                ss << "\nMatrix value: \n";

                for (int k = 0; k < 4; ++k)
                {
                    char        lRowValue[1024];
                    FBXSDK_sprintf(lRowValue, 1024, "%9.4f %9.4f %9.4f %9.4f\n", m[k][0], m[k][1], m[k][2], m[k][3]);
                    ss << lRowValue;
                }

                MCONV_LOG_INFO("%s", ss.str().c_str());
                ss.str("");
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FBXReader::processFbxSkin(FbxGeometry* pFbxGeometry)
    {
        TResult ret = T3D_OK;

        int nSkinCount = pFbxGeometry->GetDeformerCount(FbxDeformer::eSkin);
        int i = 0;

        MCONV_LOG_INFO("Skin (%d) : ", nSkinCount);

        for (i = 0; i < nSkinCount; ++i)
        {
            FbxSkin* pFbxSkin = (FbxSkin*)pFbxGeometry->GetDeformer(i, FbxDeformer::eSkin);
            int nClusterCount = pFbxSkin->GetClusterCount();

            int j = 0;
            for (j = 0; j < nClusterCount; ++j)
            {
                FbxCluster* pFbxCluster = pFbxSkin->GetCluster(j);
                const char* modes[] = { "Normalize", "Additive", "Total1" };

                if (pFbxCluster->GetLink() != nullptr)
                {
                    MCONV_LOG_INFO("\tName : %s", pFbxCluster->GetLink()->GetName());
                }

                MCONV_LOG_INFO("\tMode : %s", modes[pFbxCluster->GetLinkMode()]);

                int nIndexCount = pFbxCluster->GetControlPointIndicesCount();
                int* pIndices = pFbxCluster->GetControlPointIndices();
                double* pWeights = pFbxCluster->GetControlPointWeights();

                int k = 0;

                std::stringstream ss0, ss1;
                ss0 << "Indices :";
                ss1 << "Weights :";

                for (k = 0; k < nIndexCount; ++k)
                {
                    ss0 << pIndices[k];
                    ss1 << pWeights[k];

                    if (k < nIndexCount - 1)
                    {
                        ss0 << ",";
                        ss1 << ",";
                    }
                }

                MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                MCONV_LOG_INFO("\t%s", ss1.str().c_str());

                ss0.str("");
                ss1.str("");

                // Transformation
                FbxAMatrix lMatrix;
                Matrix4 m;

                lMatrix = pFbxCluster->GetTransformMatrix(lMatrix);
                convertMatrix(lMatrix, m);

                Vector3 T, S;
                Quaternion Q;
                m.decomposition(T, S, Q);

                ss0 << "Transform Translation: " << std::fixed << std::setprecision(6) << T[0] << ", " << T[1] << ", " << T[2];
                MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                ss0.str("");

                Matrix3 R;
                Q.toRotationMatrix(R);
                Radian roll, yaw, pitch;
                R.toEulerAnglesZYX(roll, yaw, pitch);

                ss0 << "Transform Rotation: " << std::fixed << std::setprecision(6) << roll.valueDegrees() << ", " << yaw.valueDegrees() << ", " << pitch.valueDegrees();
                MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                ss0.str("");

                ss0 << "Transform Scaling: " << std::fixed << std::setprecision(6) << S[0] << ", " << S[1] << ", " << S[2];
                MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                ss0.str("");

                // Link transformation
                lMatrix = pFbxCluster->GetTransformLinkMatrix(lMatrix);
                convertMatrix(lMatrix, m);

                //ss0 << "\nMatrix value: \n";

                //for (int k = 0; k < 4; ++k)
                //{
                //    char        lRowValue[1024];
                //    FBXSDK_sprintf(lRowValue, 1024, "%9.4f %9.4f %9.4f %9.4f\n", m[k][0], m[k][1], m[k][2], m[k][3]);
                //    ss0 << lRowValue;
                //}

                MCONV_LOG_INFO("%s", ss0.str().c_str());
                ss0.str("");

                m.decomposition(T, S, Q);

                //FbxAMatrix lGlobalMatrix = pFbxCluster->GetLink()->EvaluateGlobalTransform();
                //convertMatrix(lGlobalMatrix, m);

                //Vector3 T0, S0;
                //Quaternion Q0;
                //m.decomposition(T0, S0, Q0);

                //ss0 << "Transform Link Global Translation: " << std::setprecision(6) << T0[0] << ", " << T0[1] << ", " << T0[2];
                //MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                //ss0.str("");

                ss0 << "Transform Link Translation: " << std::setprecision(6) << T[0] << ", " << T[1] << ", " << T[2];
                MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                ss0.str("");

                //ss0 << "FBX Transform Link Translation: " << lMatrix.GetT()[0] << ", " << lMatrix.GetT()[1] << ", " << lMatrix.GetT()[2];
                //MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                //ss0.str("");

                Q.toRotationMatrix(R);
                R.toEulerAnglesZYX(roll, yaw, pitch);

                ss0 << "Transform Link Rotation: " << roll.valueDegrees() << ", " << yaw.valueDegrees() << ", " << pitch.valueDegrees();
                MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                ss0.str("");

                ss0 << "Transform Link Scaling: " << S[0] << ", " << S[1] << ", " << S[2];
                MCONV_LOG_INFO("\t%s", ss0.str().c_str());
                ss0.str("");

                MCONV_LOG_INFO("");

                // M(G) : Matrix of Geometry 
                //  Retrieve from :
                //  FbxAMatrix GetGeometry(FbxNode* pNode)
                //  {
                //      const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
                //      const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
                //      const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
                //      return FbxAMatrix(lT, lR, lS);
                //  }
                // 
                // M(v) : Matrix of Mesh, 从模型空间变换到世界空间的矩阵
                //  Retrieve from :
                //  pFbxCluster->GetTransformMatrix(lMatrix);
                //
                // M(b) : Matrix of Bone in binding poss, binding pose 时，从骨骼空间变换到世界空间的矩阵
                //  Retrieve from :
                //  pFbxCluster->GetTransformLinkMatrix(lMatrix);
                //  
                // 1. 把在 Binding 时刻的顶点，由 Mesh 空间变换到世界空间 M(v) * M(G)
                // 2. 把在 Binding 时刻的顶点，由世界空间变换到骨骼空间 Inverse(M(b)) * M(v) * M(G)
            }
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

    //--------------------------------------------------------------------------

    void FBXReader::convertMatrix(const FbxMatrix &src, Matrix4 &dst)
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

