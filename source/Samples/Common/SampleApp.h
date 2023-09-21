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

#ifndef __SAMPLE_APP_H__
#define __SAMPLE_APP_H__


#include <Tiny3D.h>

#define LOG_TAG_APP     "APP"


class SampleApp
{
public:
    virtual ~SampleApp() = default;
    
    virtual TResult go(int32_t argc, char *argv[]) = 0;
};

class SampleWindowApp : public Tiny3D::WindowApplication, public SampleApp
{
public:
    SampleWindowApp();
    ~SampleWindowApp() override;

    TResult go(int32_t argc, char *argv[]) override;

protected:  /// from Tiny3D::Application
    bool applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void applicationDidEnterBackground() override;

    void applicationWillEnterForeground() override;

    void applicationWillTerminate() override;

    void applicationLowMemory() override;

protected:
};


class SampleConsoleApp : public Tiny3D::ConsoleApplication, public SampleApp
{
public:
    SampleConsoleApp();
    ~SampleConsoleApp() override;

    TResult go(int32_t argc, char *argv[]) override;

protected:
    bool applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void applicationDidEnterBackground() override;

    void applicationWillEnterForeground() override;

    void applicationWillTerminate() override;

    void applicationLowMemory() override;
};


#endif  /*__SAMPLE_APP_H__*/
