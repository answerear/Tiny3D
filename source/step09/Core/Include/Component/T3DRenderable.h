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


#ifndef __T3D_RENDERABLE_H__
#define __T3D_RENDERABLE_H__


#include "Component/T3DComponent.h"
#include "Render/T3DRenderer.h"
#include "Render/T3DVertexArrayObject.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    /**
     * @brief 场景中 3D 可渲染对象基类，用于渲染 3D 物体
     */
    class T3D_ENGINE_API Renderable : public Component
    {
        friend class DefaultSceneMgr;

    public:
        /**
         * @brief 析构函数
         */
        virtual ~Renderable();

        /**
         * @brief 获取渲染使用的材质
         * @return 返回渲染用的材质对象
         */
        virtual MaterialPtr getMaterial() const = 0;

        /**
         * @brief 获取渲染使用的VAO对象
         * @return 返回渲染数据的VAO对象
         */
        virtual VertexArrayObjectPtr getVertexArrayObject() const = 0;

        virtual void frustumCulling(BoundPtr bound, RenderQueuePtr queue) = 0;

        virtual void updateBound() = 0;

    protected:
        /**
         * @brief 构造函数
         */
        Renderable(ID uID = E_CID_AUTOMATIC);

        virtual TResult cloneProperties(ComponentPtr newObj) const;

        virtual void onAttachSceneNode(SceneNode *node) override;

    private:
        Renderable  *mPrev;      /**< 前一个可渲染对象 */
        Renderable  *mNext;      /**< 下一个可渲染对象 */
    };
}


#endif  /*__T3D_RENDERABLE_H__*/
