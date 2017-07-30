/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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

#ifndef __T3D_RENDER_QUEUE_H__
#define __T3D_RENDER_QUEUE_H__


#include "Misc/T3DObject.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RenderGroup : public Object
    {
    public:
        RenderGroup();
        virtual ~RenderGroup();

        void addRenderable(const MaterialPtr &material, const SGRenderablePtr &renderable);

        void clear();

        void render(uint32_t groupID, const RendererPtr &renderer);

    protected:
        size_t calcPrimitiveCount(Renderer::PrimitiveType priType, size_t indexCount, size_t vertexCount, bool useIndex);

    protected:
        typedef std::list<SGRenderablePtr>              RenderableList;
        typedef RenderableList::iterator                RenderableListItr;
        typedef RenderableList::const_iterator          RenderableListConstItr;

        typedef std::map<MaterialPtr, RenderableList>   Renderables;
        typedef Renderables::iterator                   RenderablesItr;
        typedef Renderables::const_iterator             RenderablesConstItr;

        typedef std::pair<MaterialPtr, RenderableList>  RenderablesValue;

        Renderables     mRenderables;
    };

    class T3D_ENGINE_API RenderQueue : public Object
    {
    public:
        enum GroupID
        {
            E_GRPID_NONE = 0,
            E_GRPID_BACKGROUND = 10,
            E_GRPID_LIGHT = 15,
            E_GRPID_SKY_BOX = 20,
            E_GRPID_INDICATOR = 30,
            E_GRPID_AUTOMATIC = 50,
            E_GRPID_SOLID = 60,
            E_GRPID_WIREFRAME = 65,
            E_GRPID_TRANSPARENT = 70,
            E_GRPID_TRANSPARENT_EFFECT = 80,
            E_GRPID_SHADOW = 90,
            E_GRPID_OVERLAY = 100
        };


        static RenderQueuePtr create();

        virtual ~RenderQueue();

        void addRenderable(GroupID groupID, const SGRenderablePtr &renderable);

        void clear();

        void render(const RendererPtr &renderer);

    protected:
        RenderQueue();

        typedef std::map<GroupID, RenderGroupPtr>   RenderableGroup;
        typedef RenderableGroup::iterator           RenderableGroupItr;
        typedef RenderableGroup::const_iterator     RenderableGroupConstItr;

        typedef std::pair<GroupID, RenderGroupPtr>  RenderableGroupValue;

        RenderableGroup     mGroups;
    };
}


#endif  /*__T3D_RENDER_QUEUE_H__*/
