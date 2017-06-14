

#ifndef __MCONV_SUBMESH_H__
#define __MCONV_SUBMESH_H__


#include "mconv_node.h"
#include "mconv_vertexbuffer.h"
#include "mconv_material.h"


namespace mconv
{
    class SubMesh : public Node
    {
    public:
        SubMesh(const String &ID)
            : Node(ID)
            , mMaterialIdx(0)
        {
            mVB = new VertexBuffer(ID);
        }

        virtual ~SubMesh()
        {
            delete mVB;
            mVB = nullptr;
        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SUBMESH;
        }

        bool generateIndices(const String &name, VertexBuffer *pVB)
        {
            if (pVB->getAttributesHash() != mVB->getAttributesHash())
            {
                return false;
            }

            // 重新声明名字
            this->mID = name;

            // 生成顶点索引
            auto itr = mVB->mVertices.begin();

            while (itr != mVB->mVertices.end())
            {
                Vertex &vertex = *itr;
                vertex.hash();

                int nIndex = 0;
                auto i = pVB->mVertices.begin();

                while (i != pVB->mVertices.end())
                {
                    Vertex &other = *i;

                    if (vertex == other)
                    {
                       mIndices.push_back(nIndex);
                       break;
                    }
                    ++nIndex;
                    ++i;
                }

                ++itr;
            }

            return true;
        }

        int             mMaterialIdx;
        String          mMaterialName;
        VertexBuffer    *mVB;
        Indices         mIndices;
    };

    class SubMeshes : public Node
    {
    public:
        SubMeshes(const String &ID)
            : Node(ID)
        {

        }

        virtual ~SubMeshes()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SUBMESHES;
        }
    };
}

#endif  /*__MCONV_SUBMESH_H__*/
