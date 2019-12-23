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


#ifndef __T3D_SCENE_MANAGER_H__
#define __T3D_SCENE_MANAGER_H__


#include "Scene/T3DSceneManagerBase.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SceneManager 
        : public SceneManagerBase
        , public Singleton<SceneManager>
    {
    public:
        /**
         * @brief 创建场景管理器对象
         * @param [in] impl : 场景管理器具体实现的对象
         * @return 返回一个场景管理器对象
         */
        static SceneManagerPtr create(SceneManagerBase *impl);

        /**
         * @brief 析构函数
         */
        virtual ~SceneManager();

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

    protected:
        SceneManager(SceneManagerBase *impl);

        SceneManagerBasePtr     mImpl;  /**< 具体场景管理器对象 */
    };

    #define T3D_SCENE_MGR       SceneManager::getInstance()
}


#endif  /*__T3D_SCENE_MANAGER_H__*/
