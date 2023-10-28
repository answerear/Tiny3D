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


#ifndef __T3D_COMPONENT_H__
#define __T3D_COMPONENT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API Component : public Object, public Noncopyable
    {
        friend class SceneNode;

        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
     
    public:
        virtual ComponentPtr clone() const = 0;

        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="getter", "Description"="UUID value")
        const UUID &getUUID() const { return mUUID; }

        SceneNode *getSceneNode() const { return mSceneNode; }

    protected:
        Component(const UUID &uuid = UUID::INVALID);

        ~Component() override;

        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter", "Description"="UUID value")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

        virtual TResult cloneProperties(const Component * const src);

        virtual void onAttachSceneNode(SceneNode *node);

        virtual void onDetachSceneNode(SceneNode *node);

        virtual void update();

    protected:
        /**< The identifier */
        UUID        mUUID {};
        /**< The scene node */
        SceneNode   *mSceneNode {nullptr};
    };
}


#endif  /*__T3D_COMPONENT_H__*/
