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

#ifndef __T3D_WIN32_FACTORY_H__
#define __T3D_WIN32_FACTORY_H__


#include "Adapter/T3DFactoryInterface.h"


namespace Tiny3D
{
    class Win32Factory : public IFactory
    {
    public:
        Win32Factory() = default;
        virtual ~Win32Factory() override = default;

        virtual IPlatform *createPlatform() override;

        virtual IApplication *createPlatformApplication() override;

        virtual IWindow *createPlatformWindow() override;

        virtual ITime *createPlatformTime() override;

        virtual ITimerService *createPlatformTimerService() override;

        virtual IDir *createPlatformDir() override;

        virtual IDeviceInfo *createPlatformDeviceInfo() override;

        virtual IConsole *createPlatformConsole() override;

        virtual IThread *createPlatformThread() override;

        virtual ICriticalSection *createPlatformCriticalSection() override;

        virtual IMutex *createPlatformMutex() override;

        virtual IRecursiveMutex *createPlatformRecursiveMutex() override;
        
        virtual ISemaphore *createPlatformSemaphore() override;

        virtual IEvent *createPlatformEvent() override;

        virtual IWaitCondition *createPlatformWaitCondition() override;
        
        virtual EPlatform getPlatform() override;

    protected:

    };
}


#endif  /*__T3D_WIN32_FACTORY_H__*/