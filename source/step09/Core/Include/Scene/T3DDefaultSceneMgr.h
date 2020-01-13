/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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
     * @class   T3D_ENGINE_API
     * @brief   默认场景管理器
     * @remarks 当没有任何场景管理器插件设置时候，会自动使用默认场景管理器。 
     *          默认场景管理器实际上是什么场景管理都没有，只是简单的一个渲染树.
     */
    class T3D_ENGINE_API DefaultSceneMgr 
        : public SceneManagerBase
        , public Singleton<DefaultSceneMgr>
    {
        T3D_CLASS(DefaultSceneMgr, SceneManagerBase);

    public:
        /**
         * @fn  static DefaultSceneMgrPtr create();
         * @brief   创建默认场景管理器
         * @return  返回一个默认场景管理器对象.
         */
        static DefaultSceneMgrPtr create();

        /**
         * @fn  virtual ~DefaultSceneMgr();
         * @brief   析构函数
         */
        virtual ~DefaultSceneMgr();

        /**
         * @fn  virtual TResult init();
         * @brief   初始化
         * @return  A TResult.
         */
        virtual TResult init();

        /**
         * @fn  virtual TResult update() override;
         * @brief   更新场景树，实现基类接口
         * @return  A TResult.
         * @sa  TResult SceneManagerBase::update()
         */
        virtual TResult update() override;

        /**
         * @fn  virtual TResult render(ViewportPtr viewport) override;
         * @brief   实现基类接口
         * @param   viewport    The viewport.
         * @return  A TResult.
         * @sa  TResult SceneManagerBase::render(ViewportPtr viewport)
         */
        virtual TResult render(ViewportPtr viewport) override;

        /**
         * @fn  virtual SceneNodePtr getRoot() const override;
         * @brief   实现基类接口
         * @return  The root.
         * @sa  SceneNodePtr SceneManagerBase::getRoot() const
         */
        virtual SceneNodePtr getRoot() const override;

        /**
         * @fn  virtual SceneTransform3DPtr createTransform3D(
         *      SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new transform 3D.
         * @sa  SceneTransform3DPtr SceneManagerBase::createTransform3D(
         *      SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneTransform3DPtr createTransform3D(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneCameraPtr createCamera(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new camera.
         * @sa  SceneCameraPtr SceneManagerBase::createCamera(
         *      SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneCameraPtr createCamera(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneLightPtr createLight(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new light.
         * @sa  SceneLightPtr SceneManagerBase::createLight(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneLightPtr createLight(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneModelPtr createModel(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new model.
         * @sa  SceneModelPtr SceneManagerBase::createModel(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneModelPtr createModel(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneMeshPtr createMesh(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new mesh.
         * @sa  SceneMeshPtr SceneManagerBase::createMesh(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneMeshPtr createMesh(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneAxisPtr createAxis(Real X, Real Y, Real Z, 
         *      SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   X       A Real to process.
         * @param   Y       A Real to process.
         * @param   Z       A Real to process.
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new axis.
         * @sa  SceneAxisPtr SceneManagerBase::createAxis(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneAxisPtr createAxis(Real X, Real Y, Real Z, 
            SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneQuadPtr createQuad(const SceneQuad::Quad &quad, 
         *      const String &materialName, SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   quad            The quad.
         * @param   materialName    Name of the material.
         * @param   parent          The parent.
         * @param   uID             (Optional) The identifier.
         * @return  The new quad.
         * @sa  SceneQuadPtr SceneManagerBase::createQuad(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneQuadPtr createQuad(const SceneQuad::Quad &quad,
            const String &materialName, SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneBillboardPtr createBillboard(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new billboard.
         * @sa  SceneBillboardPtr SceneManagerBase::createBillboard(
         *      SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneBillboardPtr createBillboard(SceneNodePtr parent,
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneBoxPtr createBox(const Vector3 &center, 
         *      const Vector3 &extent, SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   center  The center.
         * @param   extent  The extent.
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new box.
         * @sa  SceneBoxPtr SceneManagerBase::createBox(SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneBoxPtr createBox(const Vector3 &center, 
            const Vector3 &extent, SceneNodePtr parent, 
            ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  virtual SceneSpherePtr createSphere(const Vector3 &center, 
         *      Real radius, SceneNodePtr parent, 
         *      ID uID = Node::E_NID_AUTOMATIC) override;
         * @brief   实现基类接口
         * @param   center  The center.
         * @param   radius  The radius.
         * @param   parent  The parent.
         * @param   uID     (Optional) The identifier.
         * @return  The new sphere.
         * @sa  SceneSpherePtr SceneManagerBase::createSphere(
         *      SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC)
         */
        virtual SceneSpherePtr createSphere(const Vector3 &center, Real radius,
            SceneNodePtr parent, ID uID = Node::E_NID_AUTOMATIC) override;

        /**
         * @fn  TResult addRenderable(SceneRenderablePtr renderable);
         * @brief   添加可渲染对象到对应相机队列，用于视锥体剔除
         * @param [in]  renderable  : 可渲染对象.
         * @return  调用成功返回 T3D_OK.
         */
        TResult addRenderable(SceneRenderablePtr renderable);

        /**
         * @fn  TResult removeRenderable(SceneRenderablePtr renderable);
         * @brief   根据camera mask来移除可渲染对象
         * @param [in]  renderable  : 可渲染对象.
         * @return  调用成功返回 T3D_OK.
         */
        TResult removeRenderable(SceneRenderablePtr renderable);

    protected:
        /**
         * @fn  DefaultSceneMgr();
         * @brief   构造函数
         */
        DefaultSceneMgr();

        /**
         * @fn  TResult frustumCulling(SceneCameraPtr camera);
         * @brief   Frustum culling
         * @param   camera  The camera.
         * @return  A TResult.
         */
        TResult frustumCulling(SceneCameraPtr camera);

    protected:
        /**
         * @struct  Slot
         * @brief   A slot.
         */
        struct Slot
        {
            Slot()
                : count(0)
                , first(nullptr)
                , last(nullptr)
            {}

            size_t              count;  /**< Number of  */
            SceneRenderablePtr  first;  /**< The first */
            SceneRenderablePtr  last;   /**< The last */
        };

        typedef TArray<Slot>                Renderables;
        typedef Renderables::iterator       RenderablesItr;
        typedef Renderables::const_iterator RenderablesConstItr;

        SceneNodePtr    mRoot;          /**< 根结点 */
        RenderQueuePtr  mRenderQueue;   /**< 渲染队列 */
        Renderables     mRenderables;   /**< 可渲染对象列表，用于做视锥体剔除 */
    };
}


#endif  /*__T3D_DEFAULT_SCENE_MGR_H__*/
