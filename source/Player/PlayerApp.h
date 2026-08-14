/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to furnish persons to whom the Software is
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

#ifndef __TINY_PLAYER_APP_H__
#define __TINY_PLAYER_APP_H__


#include <Tiny3D.h>


namespace Tiny3D
{
    /**
     * \brief 游戏 Runtime 宿主。开发期用 --project 打开工程目录，加载 Runtime
     *        变体业务 DLL 和启动场景；导出后读旁边的 Tiny3D.cfg。
     */
    class PlayerApp : public WindowApplication
    {
    public:
        PlayerApp();

        ~PlayerApp() override;

        TResult go(int32_t argc, char *argv[]);

    protected:
        TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

        void applicationDidEnterBackground() override;

        void applicationWillEnterForeground() override;

        void applicationWillTerminate() override;

        void applicationLowMemory() override;

        bool parseCommandLine(int32_t argc, char *argv[]);

        String resolvePluginPath() const;

        void fillRuntimeSettings(Settings &settings) const;

        TResult loadGameSettings();

        TResult loadGamePlugin();

        TResult mountProjectArchives();

        TResult loadStartupScene();

        void bindCameras(Scene *scene);

    protected:
        String          mProjectRoot {};
        GameSettings    mGameSettings {};
        bool            mProjectMode {false};
        ScenePtr        mScene {};
        RenderTargetPtr mRenderTarget {};
    };
}


#endif  /*__TINY_PLAYER_APP_H__*/
