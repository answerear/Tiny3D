/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Component/T3DGeometry.h"
#include "Kernel/T3DArchive.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DMesh.h"
#include "Resource/T3DSubMesh.h"
#include "Resource/T3DMeshManager.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Geometry::Geometry(const UUID &uuid)
        : Renderable(uuid)
    {
        
    }

    //--------------------------------------------------------------------------

    Geometry::~Geometry()
    {
        
    }

    //--------------------------------------------------------------------------

    void Geometry::onDestroy()
    {
        if (!mIsDynamicBatch && mMaterial != nullptr)
        {
            // 不是动态合批，要自己卸载掉
            T3D_MATERIAL_MGR.unload(mMaterial);
            mMaterial = nullptr;
        }
        else
        {
            mMaterial = nullptr;
        }

        Renderable::onDestroy();
    }

    //--------------------------------------------------------------------------

    ComponentPtr Geometry::clone() const
    {
        GeometryPtr geometry = T3D_NEW Geometry();
        TResult ret = geometry->cloneProperties(this);
        if (T3D_FAILED(ret))
        {
            geometry = nullptr;
        }
        return geometry;
    }

    //--------------------------------------------------------------------------

    TResult Geometry::cloneProperties(const Component *const src)
    {
        TResult ret;

        do
        {
            ret = Component::cloneProperties(src);
            if (T3D_FAILED(ret))
            {
                break;
            }

            const Geometry * const other = static_cast<const Geometry * const>(src);
            mMeshUUID = other->mMeshUUID;
            mSubMeshName = other->mSubMeshName;

            mMesh = other->mMesh;
            mSubMesh = other->mSubMesh;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    Material *Geometry::getMaterial()
    {
        return mMaterial;
    }

    //--------------------------------------------------------------------------

    PrimitiveType Geometry::getPrimitiveType() const
    {
        if (mMesh != nullptr && mSubMesh != nullptr)
        {
            return mSubMesh->getPrimitiveType();
        }
        return PrimitiveType::kTriangleList;
    }

    //--------------------------------------------------------------------------

    VertexDeclaration *Geometry::getVertexDeclaration() const
    {
        if (mMesh != nullptr)
        {
            return mMesh->getVertexDeclaration();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    const VertexBuffers &Geometry::getVertexBuffers() const
    {
        if (mMesh != nullptr)
        {
            return mMesh->getVertexBuffers();
        }
        static VertexBuffers vbuffers;
        return vbuffers;
    }

    //--------------------------------------------------------------------------

    IndexBuffer *Geometry::getIndexBuffer() const
    {
        if (mMesh != nullptr && mSubMesh != nullptr)
        {
            return mSubMesh->getIndexBuffer();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------

    const VertexStrides &Geometry::getVertexStrides() const
    {
        if (mMesh != nullptr)
        {
            return mMesh->getVertexStrides();
        }
        static VertexStrides strides;
        return strides;
    }

    //--------------------------------------------------------------------------

    const VertexOffsets &Geometry::getVertexOffsets() const
    {
        if (mMesh != nullptr)
        {
            return mMesh->getVertexOffsets();
        }
        static VertexOffsets offsets;
        return offsets;
    }

    //--------------------------------------------------------------------------

    void Geometry::setMeshObject(Mesh *mesh, SubMesh *submesh)
    {
        if (mesh == nullptr || submesh == nullptr)
        {
            mMesh = nullptr;
            mSubMesh = nullptr;
            mMeshUUID = UUID::INVALID;
            mSubMeshName = "";

            if (!mIsDynamicBatch && mMaterial != nullptr)
            {
                T3D_MATERIAL_MGR.unload(mMaterial);
                mMaterial = nullptr;
            }
        }
        else
        {
            const auto &submeshes = mesh->getSubMeshes();
            const auto &itr = submeshes.find(submesh->getName());
            if (itr != submeshes.end())
            {
                mMesh = mesh;
                mSubMesh = submesh;
                mMeshUUID = mMesh->getUUID();
                mSubMeshName = mSubMesh->getName();

                generateRenderMaterial();
            }
        }
    }
    
    //--------------------------------------------------------------------------

    void Geometry::onPostLoad()
    {
        Renderable::onPostLoad();
        // Archive *archive = nullptr;
        // mMesh = T3D_MESH_MGR.loadMesh(archive, mMeshName);
        // if (mMesh != nullptr)
        // {
        //     mSubMesh = mMesh->getSubMesh(mSubMeshName);
        // }
    }

    //--------------------------------------------------------------------------

    void Geometry::onLoadResource(Archive *archive)
    {
        if (mMesh != nullptr)
        {
            if (mMesh->getUUID() != mMeshUUID)
            {
                if (!mIsDynamicBatch && mMaterial != nullptr)
                {
                    T3D_MATERIAL_MGR.unload(mMaterial);
                    mMaterial = nullptr;
                }
                
                mMesh = nullptr;
            }
        }

        if (mMesh == nullptr)
        {
            mMesh = T3D_MESH_MGR.loadMesh(archive, mMeshUUID);

            if (mMesh != nullptr)
            {
                mSubMesh = mMesh->getSubMesh(mSubMeshName);

                if (mSubMesh == nullptr)
                {
                    // 记录的 sub mesh 不在这个 mesh 里，退化到第一个 sub mesh。
                    // 换网格（如编辑器里改 MeshUUID）时旧的 sub mesh 名必然对不上，
                    // 若就这么留着空 sub mesh，该几何体既画不出来也没有材质
                    const auto &submeshes = mMesh->getSubMeshes();

                    if (!submeshes.empty())
                    {
                        const String requested = mSubMeshName;

                        mSubMesh = submeshes.begin()->second;
                        mSubMeshName = mSubMesh->getName();

                        T3D_LOG_WARNING(LOG_TAG_COMPONENT,
                            "Sub mesh (%s) not found in mesh (%s), fallback to (%s) !",
                            requested.c_str(), mMeshUUID.toString().c_str(),
                            mSubMeshName.c_str());
                    }
                }

                if (mSubMesh != nullptr && archive != nullptr)
                {
                    // 加载出来的，那就顺便把材质加载一下
                    T3D_MATERIAL_MGR.loadMaterial(archive, mSubMesh->getMaterialUUID());
                }

                generateRenderMaterial();
            }
            else
            {
                T3D_LOG_ERROR(LOG_TAG_COMPONENT, "Failed to load mesh (%s) !", mMeshUUID.toString().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------

    void Geometry::generateRenderMaterial()
    {
        // 材质挂在 sub mesh 上，没有 sub mesh 就无从生成。onLoadResource 里 mesh 加载
        // 成功但 sub mesh 解析不到时也会走到这里（例如 mesh 一个 sub mesh 都没有），
        // 不先挡住就会解引用空指针
        if (mSubMesh == nullptr)
        {
            T3D_LOG_WARNING(LOG_TAG_COMPONENT,
                "No sub mesh to generate render material (mesh : %s) !",
                mMeshUUID.toString().c_str());
            return;
        }

        Material *material = static_cast<Material *>(T3D_MATERIAL_MGR.getResource(mSubMesh->getMaterialUUID()));
        T3D_ASSERT(material != nullptr);

        // sub mesh 引用的材质没能加载进来（资产缺失或未随 mesh 一起加载）时提前退出。
        // 断言已经在 debug 下把数据问题暴露出来了，release 下继续走会把空指针交给
        // clone
        if (material == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_COMPONENT,
                "Material (%s) of sub mesh (%s) is not loaded !",
                mSubMesh->getMaterialUUID().toString().c_str(),
                mSubMeshName.c_str());
            return;
        }

        mIsDynamicBatch = false;

        if (mIsDynamicBatch)
        {
            // 动态合批，直接使用 submesh 的材质即可
            mMaterial = material;
        }
        else
        {
            // 不合批，生成一个新材质
            mMaterial = T3D_MATERIAL_MGR.clone(material);
        }
    }

    //--------------------------------------------------------------------------
}
