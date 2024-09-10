/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_PROJECT_MANAGER_INTERFACE_H__
#define __T3D_PROJECT_MANAGER_INTERFACE_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
#if defined (T3D_EDITOR)
    class IProjectManager
    {
    public:
        virtual ~IProjectManager() = default;

        virtual TResult createProject(const String &path, const String &name) = 0;

        virtual TResult openProject(const String &path, const String &name) = 0;

        virtual TResult closeProject() = 0;

        virtual const String &getProjectPath() const = 0;

        virtual const String &getProjectName() const = 0;

        virtual const String &getAssetsPath() const = 0;

        virtual const String &getTempPath() const = 0;
    };
#endif
}


#endif    /*__T3D_PROJECT_MANAGER_INTERFACE_H__*/
