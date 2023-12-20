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

#ifndef __T3D_SCENE_MANAGER_H__
#define __T3D_SCENE_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SceneManagerImpl : public ResourceManager
    {
    public:
        virtual ScenePtr createScene(const String &name) = 0;

        virtual ScenePtr loadScene(Archive *archive, const String &name) = 0;

        virtual TResult saveScene(Scene *scene, Archive *archive) = 0;

        virtual TResult unloadScene(ScenePtr &scene);
    };

    class T3D_ENGINE_API SceneManager : public Object, public Singleton<SceneManager>
    {
    public:
        static SceneManagerPtr create();

        ~SceneManager() override = default;

        void setSceneManagerImpl(SceneManagerImpl *impl) { mImpl = impl; }

        ScenePtr createScene(const String &name);

        ScenePtr loadScene(Archive *archive, const String &name);

        TResult saveScene(Scene *scene, Archive *archive);

        ScenePtr getCurrentScene() const { return mCurrentScene; }

        void setCurrentScene(Scene *scene) { mCurrentScene = scene; }

        TResult unloadScene();
        
    protected:
        SceneManager() = default;

    protected:
        SceneManagerImplPtr mImpl {nullptr};
        ScenePtr            mCurrentScene {nullptr};
    };

    
    #define T3D_SCENE_MGR   (SceneManager::getInstance())
    

    class BuiltinSceneManager : public SceneManagerImpl
    {
    public:
        static BuiltinSceneManagerPtr create();

        ~BuiltinSceneManager() override = default;

        ScenePtr createScene(const String &name) override;

        ScenePtr loadScene(Archive *archive, const String &name) override;

        TResult saveScene(Scene *scene, Archive *archive) override;
        
    protected:
        BuiltinSceneManager() = default;
        
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        ResourcePtr loadResource(const String &name, DataStream &stream, int32_t argc, va_list args) override;
        
        TResult saveResource(DataStream &stream, Resource *res) override;
    };
}


#endif    /*__T3D_SCENE_MANAGER_H__*/
