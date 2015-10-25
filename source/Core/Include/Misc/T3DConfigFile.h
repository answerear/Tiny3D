

#ifndef __T3D_CONFIG_FILE_H__
#define __T3D_CONFIG_FILE_H__


#include "T3DPrerequisites.h"


namespace tinyxml2
{
    class XMLDocument;
    class XMLElement;
}


namespace Tiny3D
{
    class T3D_ENGINE_API ConfigFile
    {
    public:
        ConfigFile(const String &filename);
        virtual ~ConfigFile();

        bool loadXML(Settings &settings);
        bool saveXML(const Settings &settings);

        bool loadBinary(Settings &settings);
        bool saveBinary(const Settings &settings);

    protected:
        bool parseXML(const tinyxml2::XMLDocument &doc, Settings &settings);
        bool parseDict(const tinyxml2::XMLElement *root, VariantMap &dict);
        bool parseArray(const tinyxml2::XMLElement *root, VariantArray &arr);

        bool buildXML(tinyxml2::XMLDocument &doc, const Settings &settings);
        bool buildDict(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root, 
            const VariantMap &dict);
        bool buildArray(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root, 
            const VariantArray &arr);
        bool buildList(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root,
            const VariantList &list);
        bool buildVariant(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root,
            const Variant &value);

    private:
        String  mFilename;
    };
}


#endif  /*__T3D_CONFIG_FILE_H__*/
