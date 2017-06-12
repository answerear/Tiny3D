

#ifndef __T3D_MESH_DATA_H__
#define __T3D_MESH_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DRenderer.h"
#include "T3DSubMeshData.h"


namespace Tiny3D
{
    /**
     * @brief 顶点缓冲区
     */
    class VertexBuffer : public Object
    {
    public:
        typedef std::list<VertexElement>        Attributes;
        typedef Attributes::iterator            AttributesItr;
        typedef Attributes::const_iterator      AttributesConstItr;

        typedef std::vector<uint8_t>            Vertices;
        typedef Vertices::iterator              VerticesItr;
        typedef Vertices::const_iterator        VerticesConstItr;

        static VertexBufferPtr create();

        String                  mName;          /// 缓冲区名称
        Attributes              mAttributes;    /// 缓冲区中顶点属性
        Vertices                mVertices;      /// 缓冲区顶点数据
        size_t                  mVertexSize;    /// 每个顶点的大小，单位：字节

    protected:
        VertexBuffer();

    private:
        VertexBuffer(const VertexBuffer &rkOther);
        VertexBuffer &operator =(const VertexBuffer &rkOther);
    };

    /**
     * @brief 网格数据
     */
    class MeshData : public Object
    {
    public:
        typedef std::list<VertexBufferPtr>      VertexBuffers;
        typedef VertexBuffers::iterator         VertexBuffersItr;
        typedef VertexBuffers::const_iterator   VertexBuffersConstItr;

        typedef std::list<SubMeshDataPtr>       SubMeshDataList;
        typedef SubMeshDataList::iterator       SubMeshDataListItr;
        typedef SubMeshDataList::const_iterator SubMeshDataListConstItr;

        static MeshDataPtr create();

        String              mName;              /// 网格名称
        VertexBuffers       mBuffers;           /// 网格拥有的顶点缓冲区列表
        SubMeshDataList     mSubMeshes;         /// 子网格列表

    protected:
        MeshData();

    private:
        MeshData(const MeshData &rkOther);
        MeshData &operator =(const MeshData &rkOther);
    };
}


#endif  /*__T3D_MESH_DATA_H__*/
