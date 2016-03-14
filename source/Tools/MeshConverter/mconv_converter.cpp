

#include "mconv_converter.h"
#include "mconv_command.h"
#include "mconv_settings.h"
#include "mconv_fbxserializer.h"
#include "mconv_t3dSerializer.h"
#include "mconv_node.h"
#include "mconv_scene.h"


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

        if (!pFbxMesh->IsTriangleMesh())
        {
            // 不是三角形为面的mesh，统一转换成三角形为面的mesh
            FbxManager *pFbxManager = pFbxNode->GetFbxManager();
            FbxGeometryConverter converter(pFbxManager);
            converter.Triangulate(pFbxMesh, true);
        }

        return true;
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
