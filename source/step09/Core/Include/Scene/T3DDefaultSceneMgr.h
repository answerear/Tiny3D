/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


#include "Scene/T3DSceneManagerBase.h"


namespace Tiny3D
{
    /**
     * @brief 默认场景管理器
     * @remarks 当没有任何场景管理器插件设置时候，会自动使用默认场景管理器。
     *      默认场景管理器实际上是什么场景管理都没有，只是简单的一个渲染树。
     */
    class T3D_ENGINE_API DefaultSceneMgr 
        : public SceneManagerBase
        , public Singleton<DefaultSceneMgr>
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
         * @brief 初始化
         */
        virtual TResult init();

        /**
         * @brief 更新场景树，实现基类接口
         * @see TResult SceneManagerBase::update()
         */
        virtual TResult update() override;

        /**
         * @brief 实现基类接口
         * @see TResult SceneManagerBase::render(ViewportPtr viewport)
         */
        virtual TResult render(ViewportPtr viewport) override;

        /**
         * @brief 实现基类接口
         * @see SceneNodePtr SceneManagerBase::getRoot() const
         */
        virtual SceneNodePtr getRoot() const override;

        /**
         * @brief 实现基类接口
         * @see SceneTransform3DPtr SceneManagerBase::createTransform3D(
         *      SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneTransform3DPtr createTransform3D(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneCameraPtr SceneManagerBase::createCamera(SceneNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneCameraPtr createCamera(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneLightPtr SceneManagerBase::createLight(SceneNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneLightPtr createLight(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneModelPtr SceneManagerBase::createModel(SceneNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneModelPtr createModel(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneMeshPtr SceneManagerBase::createMesh(SceneNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneMeshPtr createMesh(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneAxisPtr SceneManagerBase::createAxis(SceneNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneAxisPtr createAxis(Real X, Real Y, Real Z, SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneQuadPtr SceneManagerBase::createQuad(SceneNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneQuadPtr createQuad(const SceneQuad::Quad &quad,
            const String &materialName, SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneBillboardPtr SceneManagerBase::createBillboard(
         *      SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneBillboardPtr createBillboard(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneBoxPtr SceneManagerBase::createBox(SceneNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneBoxPtr createBox(const Vector3 &center, const Vector3 &extent,
            SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 实现基类接口
         * @see SceneSpherePtr SceneManagerBase::createSphere(SceneNodePtr parent,
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneSpherePtr createSphere(const Vector3 &center, Real radius,
            SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @brief 添加可渲染对象到对应相机队列，用于视锥体剔除
         * @param [in] renderable : 可渲染对象
         * @return 调用成功返回 T3D_OK
         */
        TResult addRenderable(SceneRenderablePtr renderable);

        /**
         * @brief 根据camera mask来移除可渲染对象
         * @param [in] renderable : 可渲染对象
         * @return 调用成功返回 T3D_OK
         */
        TResult removeRenderable(SceneRenderablePtr renderable);

    protected:
        /**
         * @brief 构造函数
         */
        DefaultSceneMgr();

        TResult frustumCulling(SceneCameraPtr camera);

    protected:
        struct Slot
        {
            Slot()
                : count(0)
                , first(nullptr)
                , last(nullptr)
            {}

            size_t              count;
            SceneRenderablePtr     first;
            SceneRenderablePtr     last;
        };

        typedef TArray<Slot>                Renderables;
        typedef Renderables::iterator       RenderablesItr;
        typedef Renderables::const_iterator RenderablesConstItr;

        SceneNodePtr       mRoot;          /**< 根结点 */
        RenderQueuePtr  mRenderQueue;   /**< 渲染队列 */
        Renderables     mRenderables;   /**< 可渲染对象列表，用于做视锥体剔除 */
    };
}


#endif  /*__T3D_DEFAULT_SCENE_MGR_H__*/
