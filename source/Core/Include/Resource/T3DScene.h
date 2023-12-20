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

#ifndef __T3D_SCENE_H__
#define __T3D_SCENE_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API Scene : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND

    public:
        static ScenePtr create(const String &name);
        
        ~Scene() override = default;

        Type getType() const override;
        
        const GameObjectList &getRootGameObjects() const { return mRootGameObjects; }

        void addRootGameObject(GameObject *gameObject);

        virtual GameObjectPtr addRootGameObject(const String &name);

        void removeRootGameObject(const String &name);

        void removeRootGameObject(GameObject *gameObject);

        void removeRootGameObject(const UUID &uuid);

        virtual void update();

        TResult addCamera(Camera *camera);

        TResult removeCamera(Camera *camera);

        TResult removeCamera(const UUID &uuid);

        TResult removeCamera(const String &name);

        const CameraList &getCameras() const { return mCameras; }
        
    protected:
        Scene(const String &name);

        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onLoad() override;
        
        TResult onUnload() override;
        
    private:
        Scene() : Scene("") {}
        
    protected:
        /// 场景根节点对应的 game object
        GameObjectList  mRootGameObjects {};
        /// 场景相机列表
        CameraList      mCameras {};
    };
}


#endif    /*__T3D_SCENE_H__*/
