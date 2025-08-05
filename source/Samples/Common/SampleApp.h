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

#ifndef __SAMPLE_APP_H__
#define __SAMPLE_APP_H__


#include <Tiny3D.h>

#define LOG_TAG_APP     "APP"

#define APP_LOG_DEBUG(fmt, ...)  T3D_LOG_DEBUG(LOG_TAG_APP, fmt, ##__VA_ARGS__)


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
