/*******************************************************************************
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
 ******************************************************************************/

#ifndef __HELLO_APP_H__
#define __HELLO_APP_H__


#include <Tiny3D.h>

class HelloApp : public Tiny3D::Application, public Tiny3D::ITimerListener
{
public:
    HelloApp();
    virtual ~HelloApp();

protected:  /// from Tiny3D::Application
    virtual bool applicationDidFinishLaunching() override;

    virtual void applicationDidEnterBackground() override;

    virtual void applicationWillEnterForeground() override;

    virtual void applicationWillTerminate() override;

    virtual void applicationLowMemory() override;
	
	virtual void onTimer(uint32_t timerID, int32_t dt) override;
	
protected:
	uint32_t	mTimerID1;
	uint32_t	mTimerID2;
};


#endif  /*__HELLO_APP_H__*/
