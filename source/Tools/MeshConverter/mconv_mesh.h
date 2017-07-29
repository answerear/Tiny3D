/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#ifndef __MCONV_MESH_H__
#define __MCONV_MESH_H__


#include "mconv_node.h"
#include "mconv_vertexbuffer.h"
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

        bool searchMaterial(Model *pModel, int nMaterialIdx, String &name)
        {
            if (nMaterialIdx == -1)
            {
                name = "No material";
                return true;
            }

            bool result = false;

            Materials *pMaterials = nullptr;
            size_t i = 0;
            for (i = 0; i < pModel->getChildrenCount(); ++i)
            {
                Node *pNode = pModel->getChild(i);
                if (pNode->getNodeType() == Node::E_TYPE_MATERIALS)
                {
                    pMaterials = (Materials *)pNode;
                    break;
                }
            }

            if (pMaterials != nullptr)
            {
                int index = 0;
                for (i = 0; i < pMaterials->getChildrenCount(); ++i)
                {
                    Node *pNode = pMaterials->getChild(i);
                    if (pNode->getNodeType() == Node::E_TYPE_MATERIAL)
                    {
                        if (index == nMaterialIdx)
                        {
                            name = pNode->getID();
                            result = true;
                            break;
                        }

                        index++;
                    }
                }
            }

            return result;
        }

        bool optimize()
        {
            bool ret = false;
            size_t count = getChildrenCount();
            size_t i = 0, j = 0, k = 0;
            size_t idx = 0;

            SubMeshes *pSubMeshes = nullptr;

            for (i = 0; i < count; ++i)
            {
                Node *pNode = getChild(i);
                if (pNode->getNodeType() == Node::E_TYPE_SUBMESHES)
                {
                    pSubMeshes = (SubMeshes *)pNode;
                    ret = true;
                    break;
                }
            }

            for (i = 0; i < count; ++i)
            {
                VertexBuffers *vbs = dynamic_cast<VertexBuffers *>(getChild(i));

                if (vbs != nullptr)
                {
                    ret = ret && (vbs->getChildrenCount() > 0);

                    for (j = 0; j < vbs->getChildrenCount(); ++j)
                    {
                        VertexBuffer *vb = dynamic_cast<VertexBuffer *>(vbs->getChild(j));
                        vb->calcAttributesHash();

                        if (vb != nullptr)
                        {
                            ret = ret && vb->optimize();

                            for (k = 0; k < pSubMeshes->getChildrenCount(); ++k)
                            {
                                SubMesh *pSubMesh = (SubMesh *)pSubMeshes->getChild(k);
                                T3D_ASSERT(pSubMesh->getNodeType() == Node::E_TYPE_SUBMESH);
                                Model *pModel = (Model *)getParent();
                                String name;
                                searchMaterial(pModel, pSubMesh->mMaterialIdx, name);
                                std::stringstream ss;
                                ss << name << "#" << idx;
                                pSubMesh->mMaterialName = name;
                                pSubMesh->mVB->calcAttributesHash();
                                ret = ret && pSubMesh->generateIndices(ss.str(), vb);
                                idx++;
                            }
                        }
                    }
                }
            }

            return ret;
        }
    };
}


#endif  /*__MCONV_MESH_H__*/
