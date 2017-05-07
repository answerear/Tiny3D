

#ifndef __T3D_TYPEDEF_INTERNAL_H__
#define __T3D_TYPEDEF_INTERNAL_H__


#include "Misc/T3DSmartPtr.h"
#include <sstream>


namespace Tiny3D
{
    T3D_DECLARE_SMART_PTR(ModelData);
    T3D_DECLARE_SMART_PTR(Bone);
    T3D_DECLARE_SMART_PTR(VertexBuffer);
    T3D_DECLARE_SMART_PTR(SubMeshData);
    T3D_DECLARE_SMART_PTR(MeshData);
    T3D_DECLARE_SMART_PTR(SkinData);
    T3D_DECLARE_SMART_PTR(BoneData);
    T3D_DECLARE_SMART_PTR(ActionData);
    T3D_DECLARE_SMART_PTR(KeyFrameData);
    T3D_DECLARE_SMART_PTR(KeyFrameDataT);
    T3D_DECLARE_SMART_PTR(KeyFrameDataR);
    T3D_DECLARE_SMART_PTR(KeyFrameDataS);

    #define T3D_XML_TAG_VERSION                 "version"
    #define T3D_XML_TAG_SCENE                   "scene"
    #define T3D_XML_TAG_MODEL                   "model"
    #define T3D_XML_TAG_MESH                    "mesh"
    #define T3D_XML_TAG_VERTEX_BUFFERS          "vertices"
    #define T3D_XML_TAG_VERTEX_BUFFER           "buffer"
    #define T3D_XML_TAG_DATA                    "data"
    #define T3D_XML_TAG_ATTRIBUTES              "attributes"
    #define T3D_XML_TAG_ATTRIBUTE               "attribute"
    #define T3D_XML_TAG_SUBMESHES               "submeshes"
    #define T3D_XML_TAG_SUBMESH                 "submesh"
    #define T3D_XML_TAG_INDICES                 "indices"
    #define T3D_XML_TAG_SKIN                    "skin"
    #define T3D_XML_TAG_BONE                    "bone"
    #define T3D_XML_TAG_TRANSFORM               "transform"
    #define T3D_XML_TAG_SKELETON                "skeleton"
    #define T3D_XML_TAG_ANIMATION               "animation"
    #define T3D_XML_TAG_ACTION                  "action"
    #define T3D_XML_TAG_KEYFRAME                "keyframe"
    #define T3D_XML_TAG_FRAME                   "frame"

    #define T3D_XML_TAG_MATERIALS               "materials"
    #define T3D_XML_TAG_MATERIAL                "material"

    #define T3D_XML_TAG_MODE                    "mode"
    #define T3D_XML_TAG_AMBIENT                 "ambient"
    #define T3D_XML_TAG_DIFFUSE                 "diffuse"
    #define T3D_XML_TAG_SPECULAR                "specular"
    #define T3D_XML_TAG_EMISSIVE                "emissive"
    #define T3D_XML_TAG_SHININESS               "shininess"
    #define T3D_XML_TAG_TRANSPARENCY            "transparency"
    #define T3D_XML_TAG_REFLECTION              "reflection"
    #define T3D_XML_TAG_TEXTURES                "textures"
    #define T3D_XML_TAG_TEXTURE                 "texture"

    #define T3D_XML_ATTRIB_ID                   "id"
    #define T3D_XML_ATTRIB_SIZE                 "size"
    #define T3D_XML_ATTRIB_TYPE                 "type"
    #define T3D_XML_ATTRIB_COUNT                "count"
    #define T3D_XML_ATTRIB_PRIMITIVE            "primitive"
    #define T3D_XML_ATTRIB_16BITS               "is16bits"
    #define T3D_XML_ATTRIB_SHARED               "shared_vertex"
    #define T3D_XML_ATTRIB_BONE                 "bone"
    #define T3D_XML_ATTRIB_TIME                 "time"
    #define T3D_XML_ATTRIB_DURATION             "duration"

    #define T3D_XML_ATTRIB_WRAP_U               "wrap_u"
    #define T3D_XML_ATTRIB_WRAP_V               "wrap_v"

    #define T3D_VERTEX_SEMANTIC_POSITION        "POSITION"
    #define T3D_VERTEX_SEMANTIC_TEXCOORD        "TEXCOORD"
    #define T3D_VERTEX_SEMANTIC_NORMAL          "NORMAL"
    #define T3D_VERTEX_SEMANTIC_TANGENT         "TANGENT"
    #define T3D_VERTEX_SEMANTIC_BINORMAL        "BINORMAL"
    #define T3D_VERTEX_SEMANTIC_COLOR           "COLOR"
    #define T3D_VERTEX_SEMANTIC_BLEND_WEIGHT    "BLEND_WEIGHT"
    #define T3D_VERTEX_SEMANTIC_BLEND_INDEX     "BLEND_INDEX"

    #define T3D_VALUE_TYPE_FLOAT                "float"
    #define T3D_VALUE_TYPE_DOUBLE               "double"
    #define T3D_VALUE_TYPE_INT                  "int"
    #define T3D_VALUE_TYPE_SHORT                "short"
    #define T3D_VALUE_TYPE_LONG                 "long"

    #define T3D_PRITYPE_TRIANGLE_LIST           "triangles"
    #define T3D_PRITYPE_TRIANGLE_STRIP          "triangle strip"

    #define T3D_BIN_MODEL_FILE_EXT              "tmb"
    #define T3D_TXT_MODEL_FILE_EXT              "tmt"

    #define T3D_BIN_MATERIAL_FILE_EXT           "mtb"
    #define T3D_TXT_MATERIAL_FILE_EXT           "mtt"

    #define T3D_ACTION_TYPE_TRANSLATION         "translation"
    #define T3D_ACTION_TYPE_ROTATION            "rotation"
    #define T3D_ACTION_TYPE_SCALING             "scaling"


    inline size_t getStartPos(const String &text, size_t start)
    {
        size_t pos = start;
        while (start != String::npos && (text[pos] == ' ' || text[pos] == '\n' || text[pos] == '\t'))
        {
            ++pos;
        }

        return pos;
    }

    template <typename T>
    T getValue(const String &text, size_t &start)
    {
        size_t end = text.find(' ', start);
        size_t len = 0;

        if (end == String::npos)
        {
            len = text.length() - start;
        }
        else
        {
            len = end - start;
        }
        String str = text.substr(start, len);
        start = getStartPos(text, end);

        std::stringstream ss(str);
        T value;
        ss>>value;

        return value;
    }
}

#endif  /*__T3D_TYPEDEF_INTERNAL_H__*/
