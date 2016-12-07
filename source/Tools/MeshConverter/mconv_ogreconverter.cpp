

#include "mconv_ogreconverter.h"
#include "mconv_command.h"
#include "mconv_settings.h"
#include "mconv_ogreserializer.h"
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


namespace mconv
{
    OgreConverter::OgreConverter(const Settings &settings)
        : ConverterImpl(settings)
    {

    }

    OgreConverter::~OgreConverter()
    {

    }

    bool OgreConverter::importScene()
    {
        bool result = true;

        delete mImporter;
        mImporter = nullptr;

        if (mSettings.mSrcType & E_FILETYPE_OGRE)
        {
            mImporter = new OgreSerializer();
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

    bool OgreConverter::exportScene()
    {
        bool result = false;

        delete mExporter;
        mExporter = nullptr;

        if ((mSettings.mDstType & E_FILETYPE_T3D) == E_FILETYPE_T3D)
        {
            mExporter = new T3DSerializer();
            result = (mExporter != nullptr);
        }
        else if (mSettings.mDstType & E_FILETYPE_TMB)
        {
            mExporter = new T3DBinSerializer();
            result = (mExporter != nullptr);
        }
        else if (mSettings.mDstType & E_FILETYPE_TMT)
        {
            mExporter = new T3DXMLSerializer();
            result = (mExporter != nullptr);
        }
        else
        {
            T3D_LOG_ERROR("Create exporter failed ! Because of invalid destination file format !");
            T3D_ASSERT(0);
            result = false;
        }

        result = result && mExporter->save(mSettings.mDstPath, mDstData);

        return result;
    }

    bool OgreConverter::convertToT3D()
    {
        bool result = true;

        OgreMesh *pOgreMesh = (OgreMesh *)mSrcData;

        return result;
    }

    void OgreConverter::cleanup()
    {
        OgreMesh *pOgreMesh = (OgreMesh *)mSrcData;

        delete pOgreMesh;
        mSrcData = nullptr;
    }
}
