

#include "mconv_fbxserializer.h"


namespace mconv
{
    FbxSerializer::FbxSerializer()
        : mFbxManager(nullptr)
        , mFbxScene(nullptr)
    {
        initFBXObjects();
    }

    FbxSerializer::~FbxSerializer()
    {
        destroyFBXObjects();
    }

    bool FbxSerializer::load(const String &path, void *&pData)
    {
        if (mFbxManager == nullptr)
        {
            T3D_LOG_ERROR("FBX Manager haven't been initialized !");
            return false;
        }

        // Create an importer
        FbxImporter *pFbxImporter = FbxImporter::Create(mFbxManager, "");

        // Initialize the importer by providing a filename.
        bool bImportStatus = pFbxImporter->Initialize(path.c_str(), -1, mFbxManager->GetIOSettings());

        if (!bImportStatus)
        {
            T3D_LOG_ERROR("Unable to initialize FBX importer !");
            return false;
        }

        // Import the scene
        bool bStatus = pFbxImporter->Import(mFbxScene);

        // Destroy the importer
        pFbxImporter->Destroy();

        // 统一切换成OpenGL的右手坐标系和以米为单位的坐标系
        FbxAxisSystem SceneAxisSystem = mFbxScene->GetGlobalSettings().GetAxisSystem();
        FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
        if (SceneAxisSystem != OurAxisSystem)
        {
            OurAxisSystem.ConvertScene(mFbxScene);
        }

        // Convert Unit System to what is used in this example, if needed
        FbxSystemUnit SceneSystemUnit = mFbxScene->GetGlobalSettings().GetSystemUnit();
        if (SceneSystemUnit.GetScaleFactor() != 1.0)
        {
            //The unit in this example is centimeter.
            FbxSystemUnit::cm.ConvertScene(mFbxScene);
        }
        
//         FbxAxisSystem::OpenGL.ConvertScene(mFbxScene);
        // 统一以1米为单位
//         FbxSystemUnit::m.ConvertScene(mFbxScene);

        // 不是三角形为面的mesh，统一转换成三角形为面的mesh
        FbxGeometryConverter converter(mFbxManager);
        converter.Triangulate(mFbxScene, true);

        pData = mFbxScene;

        return bStatus;
    }

    bool FbxSerializer::save(const String &path, void *pData)
    {
        mFbxScene = static_cast<FbxScene*>(pData);

        return true;
    }

    bool FbxSerializer::initFBXObjects()
    {
        // 创建FBX SDK Manager
        destroyFBXObjects();

        mFbxManager = FbxManager::Create();
        if (mFbxManager == nullptr)
        {
            T3D_LOG_ERROR("Unable to create FBX Manager !");
            return false;
        }

        // 创建IOSettings object
        FbxIOSettings *pIOSettings = FbxIOSettings::Create(mFbxManager, IOSROOT);
        mFbxManager->SetIOSettings(pIOSettings);

        // Load plugins from the executable directory (optional)
        FbxString path = FbxGetApplicationDirectory();
        mFbxManager->LoadPluginsDirectory(path.Buffer());

        // Create an FBX scene. This object holds most objects imported/exported from/to files.
        mFbxScene = FbxScene::Create(mFbxManager, "My Scene");
        if (mFbxScene == nullptr)
        {
            T3D_LOG_ERROR("Unable to create FBX scene !");
            return false;
        }

        return true;
    }

    void FbxSerializer::destroyFBXObjects()
    {
        if (mFbxManager != nullptr)
        {
            mFbxManager->Destroy();
            mFbxManager = nullptr;
        }
    }
}
