/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
 **************************************************************************************************/

#ifndef __T3D_RENDER_TARGET_H__
#define __T3D_RENDER_TARGET_H__


#include "T3DPrerequisites.h"
#include "Render/T3DViewport.h"
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RenderTarget : public Object
    {
    public:
        static RenderTargetPtr create();

        virtual ~RenderTarget();

        const String &getName() const;

        void getMetrics(int32_t &nWidth, int32_t &nHeight,
            int32_t &nColorDepth) const;

        int32_t getWidth() const;
        int32_t getHeight() const;

        int32_t getColorDepth() const;

        bool isActive() const       { return mIsActive; }
        void setActive(bool active) { mIsActive = active; }

        virtual void update();

        ViewportPtr addViewport(SGCamera *camera, int32_t nZOrder, 
            Real left, Real top, Real width, Real height);

        void removeViewport(int32_t nZOrder);

        void removeAllViewports();

        uint32_t getNumViewports() const;

        ViewportPtr getViewport(uint32_t unIndex) const;

        void addListener(RenderTargetListener *pListener);
        void removeListener(RenderTargetListener *pListener);
        void removeAllListener();

    protected:
        RenderTarget();

    protected:
        typedef std::map<int32_t, ViewportPtr>  ViewportList;
        typedef ViewportList::iterator          ViewportListItr;
        typedef ViewportList::const_iterator    ViewportListConstItr;
        typedef ViewportList::value_type        ViewportValue;

        typedef std::list<RenderTargetListener*>    ListenerList;
        typedef ListenerList::iterator              ListenerListItr;

        int32_t         mWidth;
        int32_t         mHeight;
        int32_t         mColorDepth;

        bool            mIsActive;

        String          mName;

        ViewportList    mViewportList;

        ListenerList    mListenerList;
    };
}


#include "T3DRenderTarget.inl"


#endif  /*__T3D_RENDER_TARGET_H__*/
