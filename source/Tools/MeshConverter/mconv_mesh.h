

#ifndef __MCONV_MESH_H__
#define __MCONV_MESH_H__


#include "mconv_node.h"
#include "mconv_vertex.h"
#include "mconv_submesh.h"


namespace mconv
{
    class Mesh : public Node
    {
    public:
        Mesh(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Mesh()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_MESH;
        }

        bool split()
        {
            typedef std::map<uint32_t, Vertex>      VerticesDict;
            typedef VerticesDict::iterator          VerticesDictItr;
            typedef VerticesDict::const_iterator    VerticesDictConstItr;
            typedef std::pair<uint32_t, Vertex>     VerticesValue;

            typedef std::map<int, SubMesh*>         SubMeshDict;
            typedef SubMeshDict::iterator           SubMeshDictItr;
            typedef SubMeshDict::const_iterator     SubMeshDictConstItr;
            typedef std::pair<int, SubMesh*>        SubMeshValue;

            SubMeshes *pSubMeshes = new SubMeshes("SubMeshes");
            addChild(pSubMeshes);

            SubMeshDict submeshes;

            VerticesDict vertices;

            // 第一遍扫描，先生成所有submesh，并且剔除所有相同的顶点
            int count = 0;
            auto itr = mVertices.begin();
            while (itr != mVertices.end())
            {
                Vertex &vertex = *itr;

                auto i = submeshes.find(vertex.mMaterialIdx);
                if (i == submeshes.end())
                {
                    std::stringstream ss;
                    ss<<count;
                    SubMesh *pSubMesh = new SubMesh(ss.str());
                    pSubMeshes->addChild(pSubMesh);

                    SubMeshValue value(vertex.mMaterialIdx, pSubMesh);
                    submeshes.insert(value);
                    ++count;
                }

                vertex.hash();
                VerticesValue value(vertex.mHash, vertex);
                vertices.insert(value);

                ++itr;
            }

            // 第二遍扫描，生成顶点索引
            itr = mVertices.begin();
            while (itr != mVertices.end())
            {
                Vertex &vertex = *itr;

                auto it = submeshes.find(vertex.mMaterialIdx);
                SubMesh *pSubMesh = it->second;

                int nIndex = 0;
                auto i = vertices.begin();
                while (i != vertices.end())
                {
                    Vertex &other = i->second;
                    if (vertex == other)
                    {
                        pSubMesh->mIndices.push_back(nIndex);
                    }
                    ++nIndex;
                    ++i;
                }

                ++itr;
            }

            // 重置顶点缓存
            mVertices.clear();
            auto i = vertices.begin();
            while (i != vertices.end())
            {
                mVertices.push_back(i->second);
                ++i;
            }

            return true;
        }

        FbxAMatrix          mWorldMatrix;
        VertexAttributes    mAttributes;
        Vertices            mVertices;
    };
}


#endif  /*__MCONV_MESH_H__*/
