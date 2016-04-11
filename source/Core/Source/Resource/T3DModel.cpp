

#include "Resource/T3DModel.h"
#include "DataStruct/T3DMeshData.h"
#include "DataStruct/T3DSubMeshData.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"

#include "Support/tinyxml2/tinyxml2.h"


namespace Tiny3D
{
    #define T3D_XML_TAG_VERSION         "version"
    #define T3D_XML_TAG_SCENE           "scene"
    #define T3D_XML_TAG_MODEL           "model"
    #define T3D_XML_TAG_MESH            "mesh"
    #define T3D_XML_TAG_ATTRIBUTES      "attributes"
    #define T3D_XML_TAG_ATTRIBUTE       "attribute"
    #define T3D_XML_TAG_VERTICES        "vertices"
    #define T3D_XML_TAG_PARTS           "parts"
    #define T3D_XML_TAG_PART            "part"
    #define T3D_XML_TAG_MATERIALS       "materials"
    #define T3D_XML_TAG_MATERIAL        "material"

    #define T3D_XML_ATTRIB_ID           "id"
    #define T3D_XML_ATTRIB_SIZE         "size"
    #define T3D_XML_ATTRIB_TYPE         "type"
    #define T3D_XML_ATTRIB_COUNT        "count"

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

    using namespace tinyxml2;

    ModelPtr Model::create(const String &name)
    {
        ModelPtr model = new Model(name);
        model->release();
        return model;
    }

    Model::Model(const String &name)
        : Resource(name)
    {

    }

    Model::~Model()
    {

    }

    Resource::Type Model::getType() const
    {
        return E_TYPE_MODEL;
    }

    bool Model::load()
    {
        bool ret = false;

        ArchivePtr archive;
        MemoryDataStream stream;

        if (T3D_ARCHIVE_MGR.getArchive(mName, archive))
        {
            if (archive->read(mName, stream))
            {
                FileType fileType = parseFileType(mName);

                switch (fileType)
                {
                case Tiny3D::Model::E_FILETYPE_UNKNOWN:
                    break;
                case Tiny3D::Model::E_FILETYPE_TMB:
                    {
                        ret = loadFromBinary(stream);
                    }
                    break;
                case Tiny3D::Model::E_FILETYPE_TMT:
                    {
                        ret = loadFromXML(stream);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        return ret;
    }

    void Model::unload()
    {
        Resource::unload();
    }

    ResourcePtr Model::clone() const
    {
        ModelPtr model = create(mName);
        return model;
    }

    Model::FileType Model::parseFileType(const String &name) const
    {
        FileType fileType = E_FILETYPE_UNKNOWN;

        size_t pos = name.rfind(".");
        if (pos > 0)
        {
            String ext = name.substr(pos+1);

            if (ext == "tmb")
            {
                fileType = E_FILETYPE_TMB;
            }
            else if (ext == "tmt")
            {
                fileType = E_FILETYPE_TMT;
            }
        }

        return fileType;
    }

    bool Model::loadFromXML(MemoryDataStream &stream)
    {
        bool ret = false;
        uint8_t *buffer = nullptr;
        size_t bufSize = 0;
        stream.getBuffer(buffer, bufSize);

        if (bufSize > 0 && buffer != nullptr)
        {
            XMLDocument *pDoc = new XMLDocument();

            if (pDoc != nullptr && pDoc->Parse((const char *)buffer, bufSize) == XML_SUCCESS)
            {
                ret = true;

                XMLDeclaration *pDeclaration = pDoc->ToDeclaration();
                XMLElement *pVersionElement = pDoc->FirstChildElement(T3D_XML_TAG_VERSION);

                String version = pVersionElement->GetText();

                XMLElement *pSceneElement = pDoc->FirstChildElement(T3D_XML_TAG_SCENE);
                XMLElement *pModelElement = pSceneElement->FirstChildElement(T3D_XML_TAG_MODEL);
                XMLElement *pMeshElement = pModelElement->FirstChildElement(T3D_XML_TAG_MESH);
                ret = ret && parseMesh(pMeshElement);

                XMLElement *pMaterialsElement = pModelElement->FirstChildElement(T3D_XML_TAG_MATERIALS);
                ret = ret && parseMaterials(pMaterialsElement);
            }

            delete pDoc;
        }

        return ret;
    }

    bool Model::parseMesh(tinyxml2::XMLElement *pMeshElement)
    {
        XMLElement *pAttribsElement = pMeshElement->FirstChildElement(T3D_XML_TAG_ATTRIBUTES);

        // 先设置属性列表并且设置其数量
        int32_t count = pAttribsElement->IntAttribute(T3D_XML_ATTRIB_COUNT);
        MeshData::VertexAttributes attributes(count);

        XMLElement *pAttribElement = pAttribsElement->FirstChildElement(T3D_XML_TAG_ATTRIBUTE);
        int32_t i = 0;
        size_t offset = 0;

        while (pAttribElement != nullptr && i < count)
        {
            VertexElement::Semantic vertexSemantic = parseVertexSemantic(pAttribElement->Attribute(T3D_XML_ATTRIB_ID));
            size_t valueCount = pAttribElement->IntAttribute(T3D_XML_ATTRIB_SIZE);
            size_t vertexSize = 0;
            VertexElement::Type vertexType = parseVertexType(pAttribElement->Attribute(T3D_XML_ATTRIB_TYPE), valueCount, vertexSize);
            attributes[i] = VertexElement(offset, vertexType, vertexSemantic);
            offset += vertexSize;
            pAttribElement = pAttribElement->NextSiblingElement(T3D_XML_TAG_ATTRIBUTE);
            i++;
        }

        return true;
    }

    VertexElement::Semantic Model::parseVertexSemantic(const String &name)
    {
        VertexElement::Semantic semantic = VertexElement::E_VES_POSITION;

        if (name == T3D_VERTEX_SEMANTIC_POSITION)
        {
            semantic = VertexElement::E_VES_POSITION;
        }
        else if (name == T3D_VERTEX_SEMANTIC_TEXCOORD)
        {
            semantic = VertexElement::E_VES_TEXCOORD;
        }
        else if (name == T3D_VERTEX_SEMANTIC_NORMAL)
        {
            semantic = VertexElement::E_VES_NORMAL;
        }
        else if (name == T3D_VERTEX_SEMANTIC_TANGENT)
        {
            semantic = VertexElement::E_VES_TANGENT;
        }
        else if (name == T3D_VERTEX_SEMANTIC_BINORMAL)
        {
            semantic = VertexElement::E_VES_BINORMAL;
        }
        else if (name == T3D_VERTEX_SEMANTIC_COLOR)
        {
            semantic = VertexElement::E_VES_DIFFUSE;
        }
        else if (name == T3D_VERTEX_SEMANTIC_BLEND_WEIGHT)
        {
            semantic = VertexElement::E_VES_BLENDWEIGHT;
        }
        else if (name == T3D_VERTEX_SEMANTIC_BLEND_INDEX)
        {
            semantic = VertexElement::E_VES_BLENDINDICES;
        }

        return semantic;
    }

    VertexElement::Type Model::parseVertexType(const String &name, size_t valueCount, size_t &vertexSize)
    {
        VertexElement::Type type = VertexElement::E_VET_FLOAT3;

        if (name == T3D_VALUE_TYPE_FLOAT)
        {
            vertexSize = sizeof(float) * valueCount;

            switch (valueCount)
            {
            case 1:
                {
                    type = VertexElement::E_VET_FLOAT1;
                }
                break;
            case 2:
                {
                    type = VertexElement::E_VET_FLOAT2;
                }
                break;
            case 3:
                {
                    type = VertexElement::E_VET_FLOAT3;
                }
                break;
            case 4:
                {
                    type = VertexElement::E_VET_FLOAT4;
                }
                break;
            }
        }
        else if (name == T3D_VALUE_TYPE_DOUBLE)
        {
            vertexSize = sizeof(double) * valueCount;

            switch (valueCount)
            {
            case 1:
                {
                    type = VertexElement::E_VET_DOUBLE1;
                }
                break;
            case 2:
                {
                    type = VertexElement::E_VET_DOUBLE2;
                }
                break;
            case 3:
                {
                    type = VertexElement::E_VET_DOUBLE3;
                }
                break;
            case 4:
                {
                    type = VertexElement::E_VET_DOUBLE4;
                }
                break;
            }
        }

        return type;
    }

    bool Model::parseSubMesh(tinyxml2::XMLElement *pSubMeshElement)
    {
        return true;
    }

    bool Model::parseMaterials(tinyxml2::XMLElement *pMatsElement)
    {
        return true;
    }

    bool Model::loadFromBinary(DataStream &stream)
    {
        return true;
    }
}
