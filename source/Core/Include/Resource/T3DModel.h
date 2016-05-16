

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
        typedef std::vector<ObjectPtr>              GeometryDataList;
        typedef GeometryDataList::iterator          GeometryDataListItr;
        typedef GeometryDataList::const_iterator    GeometryDataListConstItr;

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

        const GeometryDataList &getMeshDataList() const
        {
            return mGeometryData;
        }

    protected:
        Model(const String &name);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

        FileType parseFileType(const String &name) const;

        bool loadFromXML(MemoryDataStream &stream);

        size_t parseVertexValue(const String &text, size_t &start, const VertexElement &attribute, void *value);
        size_t parseIndexValue(const String &text, size_t &start, bool is16bits, void *value);

        VertexElement::Semantic parseVertexSemantic(const String &name);
        VertexElement::Type parseVertexType(VertexElement::Semantic semantic, const String &name, size_t valueCount);
        Renderer::PrimitiveType parsePrimitiveType(const String &name);

        bool parseMesh(tinyxml2::XMLElement *pMeshElement, bool sharedVertex);

        bool parseSubMeshes(tinyxml2::XMLElement *pMeshElement);
        bool parseSubMesh(tinyxml2::XMLElement *pSubMeshElement);

        bool parseMaterials(tinyxml2::XMLElement *pMatsElement);

        bool loadFromBinary(DataStream &stream);

    protected:
        bool                mIsVertexShared;    /// 是否共享顶点数据
        GeometryDataList    mGeometryData;      /// 渲染几何数据
    };
}

#endif  /*__T3D_MODEL_H__*/
