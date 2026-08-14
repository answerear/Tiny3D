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


#include "PlayerApp.h"

#include <cstdlib>
#include <cstring>


#define LOG_TAG_PLAYER          "TinyPlayer"
#define ARCHIVE_TYPE_FS         "FileSystem"
#define ARCHIVE_TYPE_METAFS     "MetaFileSystem"


using namespace Tiny3D;


PlayerApp theApp;


PlayerApp::PlayerApp()
{
}

PlayerApp::~PlayerApp()
{
}

TResult PlayerApp::go(int32_t argc, char *argv[])
{
    TResult ret = T3D_OK;

    do
    {
        if (!parseCommandLine(argc, argv))
        {
            ret = T3D_ERR_INVALID_PARAM;
            break;
        }

        Agent *engine = T3D_NEW Agent();

        Settings settings;
        fillRuntimeSettings(settings);

        ret = engine->init(argc, argv, true, true, settings);
        if (ret == T3D_OK)
        {
            engine->run();
        }

        T3D_SAFE_DELETE(engine);
    } while (false);

    return ret;
}

bool PlayerApp::parseCommandLine(int32_t argc, char *argv[])
{
    mProjectRoot.clear();
    mProjectMode = false;

    for (int32_t i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--project") == 0)
        {
            if (i + 1 >= argc)
            {
                T3D_LOG_ERROR(LOG_TAG_PLAYER, "--project requires a project root path.");
                return false;
            }

            mProjectRoot = Dir::formatPath(argv[++i]);
            mProjectMode = true;
        }
    }

    return true;
}

String PlayerApp::resolvePluginPath() const
{
    if (const char *env = std::getenv("TINY3D_SDK_ROOT"))
    {
        if (env[0] != '\0')
        {
            return Dir::formatPath(env);
        }
    }

#if defined(TINY3D_PLAYER_SDK_BIN)
    return Dir::formatPath(TINY3D_PLAYER_SDK_BIN);
#else
    return ".";
#endif
}

void PlayerApp::fillRuntimeSettings(Settings &settings) const
{
    settings.renderSettings.resizable = true;
    settings.renderSettings.title = "TinyPlayer";
    settings.renderSettings.renderer = RHIRenderer::DIRECT3D11;
    settings.pluginSettings.pluginPath = resolvePluginPath();
    settings.pluginSettings.plugins.emplace_back("FileSystemArchive");
    settings.pluginSettings.plugins.emplace_back("MetaFSArchive");
    settings.pluginSettings.plugins.emplace_back("BundleFSArchive");
    settings.pluginSettings.plugins.emplace_back("D3D11Renderer");
    settings.pluginSettings.plugins.emplace_back("FreeImageCodec");
}

TResult PlayerApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
{
    (void)argc;
    (void)argv;

    TResult ret = T3D_OK;

    do
    {
        if (mProjectMode)
        {
            ret = loadGameSettings();
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = loadGamePlugin();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_WARNING(LOG_TAG_PLAYER,
                    "Game plugin not loaded; custom Behaviour will be missing. Continuing.");
                ret = T3D_OK;
            }

            ret = mountProjectArchives();
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = loadStartupScene();
            break;
        }

        // 导出模式：Tiny3D.cfg 里的 GameSettings。本次只留接口，F5 走 --project。
        mGameSettings = T3D_AGENT.getSettings().gameSettings;
        if (mGameSettings.gamePluginName.empty())
        {
            T3D_LOG_ERROR(LOG_TAG_PLAYER,
                "No --project and Tiny3D.cfg has no gameSettings.gamePluginName. "
                "Launch with --project <project root> when debugging from Visual Studio.");
            ret = T3D_ERR_INVALID_PARAM;
            break;
        }

        T3D_LOG_WARNING(LOG_TAG_PLAYER,
            "Standalone cfg mode is not implemented yet. Use --project for development.");
        ret = T3D_ERR_NOT_IMPLEMENT;
    } while (false);

    return ret;
}

TResult PlayerApp::loadGameSettings()
{
    TResult ret = T3D_OK;

    do
    {
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(
            mProjectRoot, ARCHIVE_TYPE_FS, Archive::AccessMode::kRead);
        if (archive == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_PLAYER, "Failed to open project root [%s].",
                mProjectRoot.c_str());
            ret = T3D_ERR_RES_LOAD_FAILED;
            break;
        }

        const String filename = String("ProjectSettings")
            + Dir::getNativeSeparator() + "Game.tasset";
        ret = archive->read(filename,
            [](DataStream &stream, const String &filename, void *userData)
            {
                GameSettings *settings = static_cast<GameSettings *>(userData);
                return T3D_SERIALIZER_MGR.deserialize(stream, *settings);
            },
            &mGameSettings);
        if (T3D_FAILED(ret))
        {
            T3D_LOG_ERROR(LOG_TAG_PLAYER,
                "Failed to read [%s]. Open the project in TinyEditor once so it can write Game.tasset.",
                filename.c_str());
            break;
        }

        if (mGameSettings.gamePluginName.empty())
        {
            T3D_LOG_ERROR(LOG_TAG_PLAYER, "Game.tasset has an empty gamePluginName.");
            ret = T3D_ERR_INVALID_PARAM;
            break;
        }
    } while (false);

    return ret;
}

TResult PlayerApp::loadGamePlugin()
{
    const String sep(1, Dir::getNativeSeparator());
    const String dllDir = mProjectRoot + sep + "Library" + sep
        + "ScriptAssemblies" + sep + "Runtime";

    TResult ret = T3D_AGENT.loadPluginFromPath(mGameSettings.gamePluginName, dllDir);
    if (T3D_FAILED(ret))
    {
        T3D_LOG_ERROR(LOG_TAG_PLAYER,
            "Failed to load game plugin [%s] from [%s]. Build the Runtime variant first.",
            mGameSettings.gamePluginName.c_str(), dllDir.c_str());
    }

    return ret;
}

TResult PlayerApp::mountProjectArchives()
{
    TResult ret = T3D_OK;
    const String sep(1, Dir::getNativeSeparator());

    do
    {
        const String assetsPath = mProjectRoot + sep + "Assets";
        const String shadersPath = mProjectRoot + sep + "Temp" + sep + "shaders";
        const String builtinPath = mProjectRoot + sep + "Temp" + sep + "builtin";

        ArchivePtr assets = T3D_ARCHIVE_MGR.loadArchive(
            assetsPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadOnly);
        if (assets == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_PLAYER, "Failed to mount Assets at [%s].",
                assetsPath.c_str());
            ret = T3D_ERR_RES_LOAD_FAILED;
            break;
        }

        T3D_ASSET_MGR.init(AssetManager::Mode::kRuntime);
        T3D_ASSET_MGR.unmountAll();
        T3D_ASSET_MGR.mount(assets, 0);

        if (Dir::exists(shadersPath))
        {
            ArchivePtr shaders = T3D_ARCHIVE_MGR.loadArchive(
                shadersPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadOnly);
            if (shaders != nullptr)
            {
                T3D_ASSET_MGR.mount(shaders, 1);
            }
        }

        if (Dir::exists(builtinPath))
        {
            ArchivePtr builtin = T3D_ARCHIVE_MGR.loadArchive(
                builtinPath, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kReadOnly);
            if (builtin != nullptr)
            {
                T3D_ASSET_MGR.mount(builtin, 2);
            }
        }
    } while (false);

    return ret;
}

TResult PlayerApp::loadStartupScene()
{
    TResult ret = T3D_OK;

    do
    {
        if (mGameSettings.startupSceneUUID.empty())
        {
            T3D_LOG_ERROR(LOG_TAG_PLAYER, "Game.tasset has an empty startupSceneUUID.");
            ret = T3D_ERR_INVALID_PARAM;
            break;
        }

        UUID uuid;
        uuid.fromString(mGameSettings.startupSceneUUID);

        mScene = T3D_ASSET_MGR.loadScene(uuid);
        if (mScene == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_PLAYER, "Failed to load startup scene [%s].",
                mGameSettings.startupSceneUUID.c_str());
            ret = T3D_ERR_RES_LOAD_FAILED;
            break;
        }

        T3D_SCENE_MGR.setCurrentScene(mScene);
        bindCameras(mScene.get());
    } while (false);

    return ret;
}

void PlayerApp::bindCameras(Scene *scene)
{
    if (scene == nullptr)
    {
        return;
    }

    RenderWindowPtr window = T3D_AGENT.getDefaultRenderWindow();
    if (window == nullptr)
    {
        T3D_LOG_ERROR(LOG_TAG_PLAYER, "No default render window, cannot bind cameras.");
        return;
    }

    mRenderTarget = RenderTarget::create(window.get());
    for (auto item : scene->getCameras())
    {
        if (item.second != nullptr)
        {
            item.second->setRenderTarget(mRenderTarget);
        }
    }
}

void PlayerApp::applicationDidEnterBackground()
{
    T3D_AGENT.appDidEnterBackground();
}

void PlayerApp::applicationWillEnterForeground()
{
    T3D_AGENT.appWillEnterForeground();
}

void PlayerApp::applicationWillTerminate()
{
    mScene = nullptr;
    mRenderTarget = nullptr;
}

void PlayerApp::applicationLowMemory()
{
}
