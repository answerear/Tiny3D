

#ifndef __MCONV_T3DSERIALIZER_H__
#define __MCONV_T3DSERIALIZER_H__


#include "mconv_serializer.h"
#include "tinyxml2/tinyxml2.h"


namespace mconv
{
    using namespace tinyxml2;

    class Node;

    class T3DBinSerializer : public Serializer
    {
    public:
        T3DBinSerializer();
        virtual ~T3DBinSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;
    };

    class T3DXMLSerializer : public Serializer
    {
    public:
        static const char * const TAG_SCENE;
        static const char * const TAG_MODEL;
        static const char * const TAG_MESH;
        static const char * const TAG_ATTRIBUTES;
        static const char * const TAG_ATTRIBUTE;
        static const char * const TAG_VERTICES;
        static const char * const TAG_PARTS;
        static const char * const TAG_PART;
        static const char * const TAG_INDICES;
        static const char * const TAG_MATERIALS;
        static const char * const TAG_MATERIAL;
        static const char * const TAG_MODE;
        static const char * const TAG_AMBIENT;
        static const char * const TAG_DIFFUSE;
        static const char * const TAG_SPECULAR;
        static const char * const TAG_EMISSIVE;
        static const char * const TAG_SHININESS;
        static const char * const TAG_TRANSPARENCY;
        static const char * const TAG_REFLECTION;
        static const char * const TAG_TEXTURES;
        static const char * const TAG_TEXTURE;
        static const char * const TAG_EFFECTS;
        static const char * const TAG_EFFECT;
        static const char * const TAG_SKELETON;
        static const char * const TAG_BONE;
        static const char * const TAG_CHILDREN;
        static const char * const TAG_TRANSFORM;
        static const char * const TAG_ANIMATIONS;
        static const char * const TAG_ANIMATION;
        static const char * const TAG_KEYFRAME;

        static const char * const ATTRIB_ID;
        static const char * const ATTRIB_COUNT;
        static const char * const ATTRIB_SIZE;
        static const char * const ATTRIB_TYPE;
        static const char * const ATTRIB_PRIMITIVE;
        static const char * const ATTRIB_MATERIAL;

        T3DXMLSerializer();
        virtual ~T3DXMLSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;

    protected:
        XMLElement *populateXMLNode(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode);
        XMLElement *populateXMLModel(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode);
        XMLElement *populateXMLMesh(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode);
        XMLElement *populateXMLVertexAttributes(XMLDocument *pDoc, XMLElement *pParentElem, Node *pNode);
    };

    class T3DSerializer : public Serializer
    {
    public:
        T3DSerializer();
        virtual ~T3DSerializer();

        virtual bool load(const String &path, void *&pData) override;
        virtual bool save(const String &path, void *pData) override;

    protected:
        T3DBinSerializer    *mBinSerializer;
        T3DXMLSerializer    *mXMLSerializer;
    };
}


#endif  /*__MCONV_T3DSERIALIZER_H__*/
