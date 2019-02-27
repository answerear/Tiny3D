/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_DEFAULT_SCENE_MGR_H__
#define __T3D_DEFAULT_SCENE_MGR_H__


#include "SceneGraph/T3DSceneManagerBase.h"


namespace Tiny3D
{
    /**
     * @brief 默认场景管理器
     * @remarks 当没有任何场景管理器插件设置时候，会自动使用默认场景管理器。
     *      默认场景管理器实际上是什么场景管理都没有，只是简单的一个渲染树。
     */
    class T3D_ENGINE_API DefaultSceneMgr : public SceneManagerBase
    {
    public:
        /**
         * @brief 创建默认场景管理器
         * @return 返回一个默认场景管理器对象
         */
        static DefaultSceneMgrPtr create();

        /**
         * @brief 析构函数
         */
        virtual ~DefaultSceneMgr();

        /**
         * @brief 实现基类接口
         * @see TResult SceneManagerBase::renderScene(SGCameraPtr camera)
         */
        virtual TResult renderScene(SGCameraPtr camera) override;

        /**
         * @brief 实现基类接口
         * @see SGNodePtr SceneManagerBase::getRoot() const
         */
        virtual SGNodePtr getRoot() const override;

        /**
         * @brief 实现基类接口
         * @see SGTransform3DPtr SceneManagerBase::createTransform3D(
         *      SGNodePtr parent, ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGTransform3DPtr createTransform3D(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGCameraPtr SceneManagerBase::createCamera(SGNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGCameraPtr createCamera(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGLightPtr SceneManagerBase::createLight(SGNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGLightPtr createLight(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGModelPtr SceneManagerBase::createModel(SGNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGModelPtr createModel(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGMeshPtr SceneManagerBase::createMesh(SGNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGMeshPtr createMesh(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGAxisPtr SceneManagerBase::createAxis(SGNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGAxisPtr createAxis(Real X, Real Y, Real Z, SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGQuadPtr SceneManagerBase::createQuad(SGNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGQuadPtr createQuad(const SGQuad::Quad &quad,
            const String &materialName, SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGBillboardPtr SceneManagerBase::createBillboard(
         *      SGNodePtr parent, ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGBillboardPtr createBillboard(SGNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGBoxPtr SceneManagerBase::createBox(SGNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGBoxPtr createBox(const Vector3 &center, const Vector3 &extent,
            SGNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SGSpherePtr SceneManagerBase::createSphere(SGNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SGSpherePtr createSphere(const Vector3 &center, Real radius,
            SGNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) override;

    protected:
        /**
         * @brief 构造函数
         */
        DefaultSceneMgr();

    protected:
        SGNodePtr       mRoot;          /**< 根结点 */
        SGCameraPtr     mCurCamera;     /**< 当前渲染相机 */

        RenderQueuePtr  mRenderQueue;   /**< 渲染队列 */
    };
}


#endif  /*__T3D_DEFAULT_SCENE_MGR_H__*/
