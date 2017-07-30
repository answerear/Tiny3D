/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __MCONV_VERTEX_BUFFER_H__
#define __MCONV_VERTEX_BUFFER_H__


#include "mconv_node.h"
#include "mconv_vertex.h"
// #include "mconv_submesh.h"
// #include "mconv_material.h"
#include "mconv_model.h"


namespace mconv
{
    class VertexBuffer : public Node
    {
    public:
        VertexBuffer(const String &ID)
            : Node(ID)
        {

        }

        virtual ~VertexBuffer()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_VERTEX_BUFFER;
        }

        void calcAttributesHash()
        {
            auto itr = mAttributes.begin();

            mAttributesHash = 0;

            while (itr != mAttributes.end())
            {
                auto attribute = *itr;
                mAttributesHash |= (1 << attribute.mVertexType);
                ++itr;
            }
        }

//         bool searchMaterial(Model *pModel, int nMaterialIdx, String &name)
//         {
//             bool result = false;
// 
//             Materials *pMaterials = nullptr;
//             size_t i = 0;
//             for (i = 0; i < pModel->getChildrenCount(); ++i)
//             {
//                 Node *pNode = pModel->getChild(i);
//                 if (pNode->getNodeType() == Node::E_TYPE_MATERIALS)
//                 {
//                     pMaterials = (Materials *)pNode;
//                     break;
//                 }
//             }
// 
//             if (pMaterials != nullptr)
//             {
//                 int index = 0;
//                 for (i = 0; i < pMaterials->getChildrenCount(); ++i)
//                 {
//                     Node *pNode = pMaterials->getChild(i);
//                     if (pNode->getNodeType() == Node::E_TYPE_MATERIAL)
//                     {
//                         if (index == nMaterialIdx)
//                         {
//                             name = pNode->getID();
//                             result = true;
//                             break;
//                         }
// 
//                         index++;
//                     }
//                 }
//             }
// 
//             return result;
//         }

        bool optimize()
        {
            typedef std::map<uint32_t, Vertex>      VerticesDict;
            typedef VerticesDict::iterator          VerticesDictItr;
            typedef VerticesDict::const_iterator    VerticesDictConstItr;
            typedef std::pair<uint32_t, Vertex>     VerticesValue;

//             typedef std::map<int, SubMesh*>         SubMeshDict;
//             typedef SubMeshDict::iterator           SubMeshDictItr;
//             typedef SubMeshDict::const_iterator     SubMeshDictConstItr;
//             typedef std::pair<int, SubMesh*>        SubMeshValue;

            Model *pModel = (Model *)getParent()->getParent()->getParent();
            T3D_ASSERT(pModel->getNodeType() == Node::E_TYPE_MODEL);

//             SubMeshes *pSubMeshes = new SubMeshes("SubMeshes");

//             Node *pMesh = getParent()->getParent();
//             T3D_ASSERT(pMesh->getNodeType() == Node::E_TYPE_MESH);
//             pMesh->addChild(pSubMeshes);

//             SubMeshDict submeshes;

            VerticesDict vertices;

            // 第一遍扫描，剔除所有相同的顶点
            int count = 0;
            auto itr = mVertices.begin();
            while (itr != mVertices.end())
            {
                Vertex &vertex = *itr;

//                 auto i = submeshes.find(vertex.mMaterialIdx);
//                 if (i == submeshes.end())
//                 {
//                     std::stringstream ss;
//                     ss<<count;
//                     SubMesh *pSubMesh = new SubMesh(ss.str());
//                     pSubMeshes->addChild(pSubMesh);
// 
//                     String name;
//                     if (searchMaterial(pModel, vertex.mMaterialIdx, name))
//                     {
//                         pSubMesh->mMaterialName = name;
//                     }
// 
//                     SubMeshValue value(vertex.mMaterialIdx, pSubMesh);
//                     submeshes.insert(value);
//                     ++count;
//                 }

                vertex.hash();
                VerticesValue value(vertex.mHash, vertex);
                auto r = vertices.insert(value);
                if (!r.second)
                {
                    int a = 0;
                }
                ++itr;
            }

//             // 第二遍扫描，生成顶点索引
//             itr = mVertices.begin();
//             while (itr != mVertices.end())
//             {
//                 Vertex &vertex = *itr;
// 
//                 auto it = submeshes.find(vertex.mMaterialIdx);
//                 SubMesh *pSubMesh = it->second;
// 
//                 int nIndex = 0;
//                 auto i = vertices.begin();
//                 while (i != vertices.end())
//                 {
//                     Vertex &other = i->second;
//                     if (vertex == other)
//                     {
//                         pSubMesh->mIndices.push_back(nIndex);
//                     }
//                     ++nIndex;
//                     ++i;
//                 }
// 
//                 ++itr;
//             }

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

        uint32_t getAttributesHash() const
        {
            return mAttributesHash;
        }

        VertexAttributes    mAttributes;
        Vertices            mVertices;

    protected:
        uint32_t            mAttributesHash;
    };

    class VertexBuffers : public Node
    {
    public:
        VertexBuffers(const String &ID)
            : Node(ID)
        {

        }

        virtual ~VertexBuffers()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_VERTEX_BUFFERS;
        }
    };
}

#endif  /*__MCONV_VERTEX_BUFFER_H__*/
