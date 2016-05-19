

#include "Resource/T3DModel.h"
#include "DataStruct/T3DGeometryData.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"

#include "Support/tinyxml2/tinyxml2.h"

#include <sstream>


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
    #define T3D_XML_TAG_INDICES         "indices"
    #define T3D_XML_TAG_MATERIALS       "materials"
    #define T3D_XML_TAG_MATERIAL        "material"

    #define T3D_XML_ATTRIB_ID           "id"
    #define T3D_XML_ATTRIB_SIZE         "size"
    #define T3D_XML_ATTRIB_TYPE         "type"
    #define T3D_XML_ATTRIB_COUNT        "count"
    #define T3D_XML_ATTRIB_PRIMITIVE    "primitive"
    #define T3D_XML_ATTRIB_16BITS       "is16bits"
    #define T3D_XML_ATTRIB_SHARED       "shared_vertex"

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

    using namespace tinyxml2;

    size_t getStartPos(const String &text, size_t start)
    {
        size_t pos = start;
        while (text[pos] == ' ' || text[pos] == '\n' || text[pos] == '\t')
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

        if (end == -1)
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
                case E_FILETYPE_UNKNOWN:
                    break;
                case E_FILETYPE_TMB:
                    {
                        ret = loadFromBinary(stream);
                    }
                    break;
                case E_FILETYPE_TMT:
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

            if (ext == T3D_BIN_MODEL_FILE_EXT)
            {
                fileType = E_FILETYPE_TMB;
            }
            else if (ext == T3D_TXT_MODEL_FILE_EXT)
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

                int32_t count = pModelElement->IntAttribute(T3D_XML_ATTRIB_COUNT);
                mIsVertexShared = pModelElement->BoolAttribute(T3D_XML_ATTRIB_SHARED);

                if (!mIsVertexShared)
                {
                    // 不是共享顶点模式，则有几个mesh就有几个GeometryData，共享顶点模式要看索引buffer的数量来定GeometryData数量
                    mGeometryData.reserve(count);
                }

                while (pMeshElement != nullptr)
                {
                    ret = ret && parseMesh(pMeshElement);
                    pMeshElement = pMeshElement->NextSiblingElement(T3D_XML_TAG_MESH);
                }

                XMLElement *pMaterialsElement = pModelElement->FirstChildElement(T3D_XML_TAG_MATERIALS);
                ret = ret && parseMaterials(pMaterialsElement);
            }

            delete pDoc;
        }

        return ret;
    }

    size_t Model::parseVertexValue(const String &text, size_t &start, const VertexElement &attribute, void *value)
    {
        size_t step = 0;

        switch (attribute.getSemantic())
        {
        case VertexElement::E_VES_POSITION:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT3:
                    {
                        float pos[3];
                        pos[0] = getValue<float>(text, start);
                        pos[1] = getValue<float>(text, start);
                        pos[2] = getValue<float>(text, start);
                        step = sizeof(pos);
                        memcpy(value, pos, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE3:
                    {
                        double pos[3];
                        pos[0] = getValue<double>(text, start);
                        pos[1] = getValue<double>(text, start);
                        pos[2] = getValue<double>(text, start);
                        step = sizeof(pos);
                        memcpy(value, pos, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_BLENDWEIGHT:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT4:
                    {
                        float weight[4];
                        weight[0] = getValue<float>(text, start);
                        weight[1] = getValue<float>(text, start);
                        weight[2] = getValue<float>(text, start);
                        weight[3] = getValue<float>(text, start);
                        step = sizeof(weight);
                        memcpy(value, weight, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE4:
                    {
                        double weight[4];
                        weight[0] = getValue<double>(text, start);
                        weight[1] = getValue<double>(text, start);
                        weight[2] = getValue<double>(text, start);
                        weight[3] = getValue<double>(text, start);
                        step = sizeof(weight);
                        memcpy(value, weight, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_BLENDINDICES:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_INT4:
                    {
                        int32_t indices[4];
                        indices[0] = getValue<int32_t>(text, start);
                        indices[1] = getValue<int32_t>(text, start);
                        indices[2] = getValue<int32_t>(text, start);
                        indices[3] = getValue<int32_t>(text, start);
                        memcpy(value, indices, sizeof(indices));
                        step += sizeof(indices);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_NORMAL:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT3:
                    {
                        float normal[3];
                        normal[0] = getValue<float>(text, start);
                        normal[1] = getValue<float>(text, start);
                        normal[2] = getValue<float>(text, start);
                        step = sizeof(normal);
                        memcpy(value, normal, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE3:
                    {
                        double normal[3];
                        normal[0] = getValue<double>(text, start);
                        normal[1] = getValue<double>(text, start);
                        normal[2] = getValue<double>(text, start);
                        step = sizeof(normal);
                        memcpy(value, normal, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_DIFFUSE:
        case VertexElement::E_VES_SPECULAR:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_COLOR:
                    {
                        float color[4];
                        color[0] = getValue<float>(text, start);
                        color[1] = getValue<float>(text, start);
                        color[2] = getValue<float>(text, start);
                        color[3] = getValue<float>(text, start);
                        uint8_t *c = (uint8_t *)value;
                        c[3] = (uint8_t)(color[0] * 255);
                        c[2] = (uint8_t)(color[1] * 255);
                        c[1] = (uint8_t)(color[2] * 255);
                        c[0] = (uint8_t)(color[3] * 255);
                        step = sizeof(uint32_t);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_TEXCOORD:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT2:
                    {
                        float texcoord[2];
                        texcoord[0] = getValue<float>(text, start);
                        texcoord[1] = getValue<float>(text, start);
                        step = sizeof(texcoord);
                        memcpy(value, texcoord, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE2:
                    {
                        double texcoord[2];
                        texcoord[0] = getValue<double>(text, start);
                        texcoord[1] = getValue<double>(text, start);
                        step = sizeof(texcoord);
                        memcpy(value, texcoord, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_TANGENT:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT3:
                    {
                        float tangent[3];
                        tangent[0] = getValue<float>(text, start);
                        tangent[1] = getValue<float>(text, start);
                        tangent[2] = getValue<float>(text, start);
                        step = sizeof(tangent);
                        memcpy(value, tangent, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE3:
                    {
                        double tangent[3];
                        tangent[0] = getValue<double>(text, start);
                        tangent[1] = getValue<double>(text, start);
                        tangent[2] = getValue<double>(text, start);
                        step = sizeof(tangent);
                        memcpy(value, tangent, step);
                    }
                    break;
                }
            }
            break;
        case VertexElement::E_VES_BINORMAL:
            {
                switch (attribute.getType())
                {
                case VertexElement::E_VET_FLOAT3:
                    {
                        float binormal[3];
                        binormal[0] = getValue<float>(text, start);
                        binormal[1] = getValue<float>(text, start);
                        binormal[2] = getValue<float>(text, start);
                        step = sizeof(binormal);
                        memcpy(value, binormal, step);
                    }
                    break;
                case VertexElement::E_VET_DOUBLE3:
                    {
                        double binormal[3];
                        binormal[0] = getValue<double>(text, start);
                        binormal[1] = getValue<double>(text, start);
                        binormal[2] = getValue<double>(text, start);
                        step = sizeof(binormal);
                        memcpy(value, binormal, step);
                    }
                    break;
                }
            }
            break;
        }

        return step;
    }

    size_t Model::parseIndexValue(const String &text, size_t &start, bool is16bits, void *value)
    {
        size_t step = 0;

        if (is16bits)
        {
            uint16_t index = getValue<uint16_t>(text, start);
            step = sizeof(uint16_t);
            memcpy(value, &index, step);
        }
        else
        {
            uint32_t index = getValue<uint32_t>(text, start);
            step = sizeof(uint32_t);
            memcpy(value, &index, step);
        }

        return step;
    }

    Renderer::PrimitiveType Model::parsePrimitiveType(const String &name)
    {
        Renderer::PrimitiveType primitiveType = Renderer::E_PT_TRIANGLE_LIST;

        if (name == T3D_PRITYPE_TRIANGLE_LIST)
        {
            primitiveType = Renderer::E_PT_TRIANGLE_LIST;
        }

        return primitiveType;
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

    VertexElement::Type Model::parseVertexType(VertexElement::Semantic semantic, const String &name, size_t valueCount)
    {
        bool bColorValue = false;

        bColorValue = (semantic == VertexElement::E_VES_DIFFUSE || semantic == VertexElement::E_VES_SPECULAR);

        VertexElement::Type type = VertexElement::E_VET_FLOAT3;

        if (bColorValue)
        {
            type = VertexElement::E_VET_COLOR;
        }
        else
        {
            if (name == T3D_VALUE_TYPE_FLOAT)
            {
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
            else if (name == T3D_VALUE_TYPE_INT)
            {
                switch (valueCount)
                {
                case 1:
                    {
                        type = VertexElement::E_VET_INT1;
                    }
                    break;
                case 2:
                    {
                        type = VertexElement::E_VET_INT2;
                    }
                    break;
                case 3:
                    {
                        type = VertexElement::E_VET_INT3;
                    }
                    break;
                case 4:
                    {
                        type = VertexElement::E_VET_INT4;
                    }
                    break;
                }
            }
            else if (name == T3D_VALUE_TYPE_LONG)
            {

            }

        }

        return type;
    }

    bool Model::parseMesh(tinyxml2::XMLElement *pMeshElement)
    {
        XMLElement *pAttribsElement = pMeshElement->FirstChildElement(T3D_XML_TAG_ATTRIBUTES);

        // 解析顶点属性列表大小
        int32_t count = pAttribsElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        if (count == 0)
        {
            T3D_LOG_ERROR("The size of vertex attributes is zero !!!");
            return false;
        }

        GeometryData::VertexAttributes attributes(count);

        // 解析顶点属性
        XMLElement *pAttribElement = pAttribsElement->FirstChildElement(T3D_XML_TAG_ATTRIBUTE);
        int32_t i = 0;
        size_t offset = 0;

        while (pAttribElement != nullptr && i < count)
        {
            VertexElement::Semantic vertexSemantic = parseVertexSemantic(pAttribElement->Attribute(T3D_XML_ATTRIB_ID));
            size_t valueCount = pAttribElement->IntAttribute(T3D_XML_ATTRIB_SIZE);
            VertexElement::Type vertexType = parseVertexType(vertexSemantic, pAttribElement->Attribute(T3D_XML_ATTRIB_TYPE), valueCount);
            attributes[i] = VertexElement(offset, vertexType, vertexSemantic);
            offset += attributes[i].getSize();
            i++;
            pAttribElement = pAttribElement->NextSiblingElement(T3D_XML_TAG_ATTRIBUTE);
        }

        // 解析顶点数据
        XMLElement *pVerticesElement = pMeshElement->FirstChildElement(T3D_XML_TAG_VERTICES);
        size_t vertexCount = pVerticesElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        if (count == 0)
        {
            T3D_LOG_ERROR("Size of vertex data is zero !!!");
            return false;
        }

        size_t vertexSize = offset;
        size_t valueCount = vertexCount * vertexSize;
        GeometryData::Vertices vertices(valueCount);

        String text = pVerticesElement->GetText();
        size_t start = 0;
        start = getStartPos(text, start);
        i = 0;

        do
        {
            size_t j = 0;

            for (j = 0; j < attributes.size(); ++j)
            {
                auto attribute = attributes[j];
                size_t step = parseVertexValue(text, start, attribute, &vertices[i]);
                i += step;
            }
        } while (i < valueCount);

        bool ret = parseSubMeshes(pMeshElement, &attributes, &vertices, vertexSize);

        return true;
    }

    bool Model::parseSubMeshes(tinyxml2::XMLElement *pMeshElement, void *attributes, void *vertices, size_t vertexSize)
    {
        XMLElement *pSubMeshesElement = pMeshElement->FirstChildElement(T3D_XML_TAG_PARTS);
        int32_t count = pSubMeshesElement->IntAttribute(T3D_XML_ATTRIB_COUNT);

        if (mIsVertexShared)
        {
            mGeometryData.reserve(count);
        }

        XMLElement *pSubMeshElement = pSubMeshesElement->FirstChildElement(T3D_XML_TAG_PART);
        while (pSubMeshElement != nullptr)
        {
            parseSubMesh(pSubMeshElement, attributes, vertices, vertexSize);
            pSubMeshElement = pSubMeshElement->NextSiblingElement(T3D_XML_TAG_PART);
        }

        return true;
    }

    bool Model::parseSubMesh(tinyxml2::XMLElement *pSubMeshElement, void *attributes, void *vertices, size_t vertexSize)
    {
        Renderer::PrimitiveType primitiveType = parsePrimitiveType(pSubMeshElement->Attribute(T3D_XML_ATTRIB_PRIMITIVE));
        int32_t primitiveCount = pSubMeshElement->IntAttribute(T3D_XML_ATTRIB_COUNT);
        String materialName = pSubMeshElement->Attribute(T3D_XML_TAG_MATERIAL) + String(".") + T3D_TXT_MATERIAL_FILE_EXT;

        XMLElement *pIndicesElement = pSubMeshElement->FirstChildElement(T3D_XML_TAG_INDICES);
        int32_t indexCount = pIndicesElement->IntAttribute(T3D_XML_ATTRIB_COUNT);
        bool is16bits = pIndicesElement->BoolAttribute(T3D_XML_ATTRIB_16BITS);

        size_t indexSize = indexCount * sizeof(uint32_t);
        if (is16bits)
        {
            indexSize = indexCount * sizeof(uint16_t);
        }

        GeometryData::Indices indices(indexSize);

        String text = pIndicesElement->GetText();
        size_t start = 0;
        size_t i = 0;

        do 
        {
            size_t step = parseIndexValue(text, start, is16bits, &indices[i]);
            i += step;
        } while (i < indexSize);

        GeometryData::VertexAttributes *vertexAttrib = (GeometryData::VertexAttributes *)attributes;
        GeometryData::Vertices *vertexData = (GeometryData::Vertices *)vertices;

        GeometryDataPtr geometryData = GeometryData::create(primitiveType, *vertexAttrib, *vertexData, vertexSize, indices, is16bits, materialName);
        mGeometryData.push_back(geometryData);

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
