

#ifndef __T3D_MODEL_H__
#define __T3D_MODEL_H__


#include "Resource/T3DResource.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DRenderer.h"


namespace tinyxml2
{
    class XMLElement;
}


namespace Tiny3D
{
    class T3D_ENGINE_API Model : public Resource
    {
    public:
        typedef std::vector<ObjectPtr>          SubMeshDataList;
        typedef SubMeshDataList::iterator       SubMeshDataListItr;
        typedef SubMeshDataList::const_iterator SubMeshDataListConstItr;

        typedef std::vector<String>             MaterialList;
        typedef MaterialList::iterator          MaterialListItr;
        typedef MaterialList::const_iterator    MaterialListConstItr;

        enum FileType
        {
            E_FILETYPE_UNKNOWN = 0,
            E_FILETYPE_TMB,
            E_FILETYPE_TMT
        };

        static ModelPtr create(const String &name);

        virtual ~Model();

        virtual Type getType() const override;

        const ObjectPtr &getMeshData() const
        {
            return mMeshData;
        }

        const SubMeshDataList &getSubMeshDataList() const
        {
            return mSubMeshData;
        }

        const MaterialList &getMaterialList() const
        {
            return mMaterials;
        }

    protected:
        Model(const String &name);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

        bool loadFromXML(MemoryDataStream &stream);

        bool parseMesh(tinyxml2::XMLElement *pMeshElement);
        VertexElement::Semantic parseVertexSemantic(const String &name);
        VertexElement::Type parseVertexType(VertexElement::Semantic semantic, const String &name, size_t valueCount, size_t &vertexSize);

        bool parseSubMeshes(tinyxml2::XMLElement *pMeshElement);
        bool parseSubMesh(tinyxml2::XMLElement *pSubMeshElement);
        Renderer::PrimitiveType parsePrimitiveType(const String &name);

        bool parseMaterials(tinyxml2::XMLElement *pMatsElement);

        bool loadFromBinary(DataStream &stream);

        FileType parseFileType(const String &name) const;

    protected:
        ObjectPtr       mMeshData;      // 网格顶点数据
        SubMeshDataList mSubMeshData;   // 根据材质划分的子网格数据
        MaterialList    mMaterials;     // 材质列表
    };
}

#endif  /*__T3D_MODEL_H__*/
