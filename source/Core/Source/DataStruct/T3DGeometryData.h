

#ifndef __T3D_GEOMETRY_DATA_H__
#define __T3D_GEOMETRY_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "DataStruct/T3DVertexData.h"
#include "DataStruct/T3DIndexData.h"
#include "T3DTypedefInternal.h"
#include "Support/tinyxml2/tinyxml2.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    class GeometryData : public Object
    {
    public:
        typedef std::vector<VertexElement>          VertexAttributes;
        typedef VertexAttributes::iterator          VertexAttributesItr;
        typedef VertexAttributes::const_iterator    VertexAttributesConstItr;

        typedef std::vector<uint8_t>        Vertices;
        typedef Vertices::iterator          VerticesItr;
        typedef Vertices::const_iterator    VerticesConstItr;

        typedef std::vector<uint8_t>        Indices;
        typedef Indices::iterator           IndicesItr;
        typedef Indices::const_iterator     IndicesConstItr;

        /**
         * @brief 创建几何物体数据对象
         * @param [in] primitiveType : 渲染用的图元类型
         * @param [in] attributes : 顶点属性集合
         * @param [in] vertices : 顶点数据缓冲
         * @param [in] vertexSize : 单个顶点数据占用的字节大小
         * @param [in] indices : 索引数据缓冲
         * @param [in] is16bits : 索引数据是16位还是32位
         * @param [in] materialName : 材质资源名称
         * @return 返回一个几何渲染数据对象
         */
        static GeometryDataPtr create(Renderer::PrimitiveType primitiveType, const VertexAttributes &attributes, const Vertices &vertices, size_t vertexSize, const Indices &indices, bool is16bits, const String &materialName);

        virtual ~GeometryData();

        Renderer::PrimitiveType getPrimitiveType() const
        {
            return mPrimitiveType;
        }

        const VertexDataPtr &getVertexData() const
        {
            return mVertexData;
        }

        const IndexDataPtr &getIndexData() const
        {
            return mIndexData;
        }

        const String &getMaterialName() const
        {
            return mMaterialName;
        }

    protected:
        GeometryData();
        bool init(Renderer::PrimitiveType primitiveType, const VertexAttributes &attributes, const Vertices &vertices, size_t vertexSize, const Indices &indices, bool is16bits, const String &materialName);

    private:
        GeometryData(const GeometryData &);
        GeometryData &operator =(const GeometryData &);

    protected:
        Renderer::PrimitiveType mPrimitiveType;
        VertexDataPtr           mVertexData;
        IndexDataPtr            mIndexData;
        String                  mMaterialName;
    };
}


#endif  /*__T3D_GEOMETRY_DATA_H__*/
