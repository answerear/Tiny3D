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

#ifndef __PLATFORM_APP_H__
#define __PLATFORM_APP_H__


#include <Tiny3D.h>

class PlatformApp : public Tiny3D::Application, Tiny3D::ITimerListener
{
public:
    PlatformApp();
    virtual ~PlatformApp();

protected:  /// from Tiny3D::Application
    virtual bool applicationDidFinishLaunching() override;

    virtual void applicationDidEnterBackground() override;

    virtual void applicationWillEnterForeground() override;

    virtual void applicationWillTerminate() override;

    virtual void applicationLowMemory() override;

protected:  /// from Tiny3D::ITimerListener
    virtual void onTimer(uint32_t timerID, int32_t dt);
	
protected:
    uint32_t    mOnceTimerID;
    uint32_t    mLoopTimerID;
};


#endif  /*__PLATFORM_APP_H__*/
