

#include "mconv_t3dserializer.h"
#include "tinyxml2/tinyxml2.h"

#include "mconv_scene.h"
#include "mconv_model.h"
#include "mconv_mesh.h"


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
    const char * const T3DXMLSerializer::TAG_SKELETON = "skeleton";
    const char * const T3DXMLSerializer::TAG_BONE = "bone";
    const char * const T3DXMLSerializer::TAG_CHILDREN = "children";
    const char * const T3DXMLSerializer::TAG_TRANSFORM = "transform";
    const char * const T3DXMLSerializer::TAG_ANIMATIONS = "animations";
    const char * const T3DXMLSerializer::TAG_ANIMATION = "animation";
    const char * const T3DXMLSerializer::TAG_KEYFRAME = "keyframe";

    const char * const T3DXMLSerializer::ATTRIB_ID = "id";
    const char * const T3DXMLSerializer::ATTRIB_COUNT = "count";
    const char * const T3DXMLSerializer::ATTRIB_SIZE = "size";
    const char * const T3DXMLSerializer::ATTRIB_TYPE = "type";
    const char * const T3DXMLSerializer::ATTRIB_PRIMITIVE = "primitive";
    const char * const T3DXMLSerializer::ATTRIB_MATERIAL = "material";

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
            XMLDocument *pDoc = new XMLDocument();
            XMLDeclaration *pDecl = pDoc->NewDeclaration();
            pDoc->LinkEndChild(pDecl);

            XMLElement *pSceneElem = pDoc->NewElement("scene");
            pSceneElem->SetAttribute("ID", pScene->getID().c_str());
            pDoc->LinkEndChild(pSceneElem);

            pDoc->SaveFile(path.c_str());
            delete pDoc;
        }

        return true;
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
