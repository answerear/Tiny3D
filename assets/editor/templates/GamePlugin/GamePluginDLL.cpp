/*******************************************************************************
 * {ProjectName} —— 动态库入口
 *
 * 编辑器与 Player 都通过这两个符号加载业务代码，和引擎自带插件走的是同一套约定。
 * 一般不需要改这个文件。
 ******************************************************************************/

#include "GamePlugin.h"


Tiny3D::GamePlugin *gPlugin = nullptr;

extern "C"
{
    GAME_PLUGIN_API TResult dllStartPlugin()
    {
        gPlugin = T3D_NEW Tiny3D::GamePlugin();
        return Tiny3D::Agent::getInstance().installPlugin(gPlugin);
    }

    GAME_PLUGIN_API TResult dllStopPlugin()
    {
        TResult ret = Tiny3D::Agent::getInstance().uninstallPlugin(gPlugin);
        if (ret == Tiny3D::T3D_OK)
        {
            T3D_SAFE_DELETE(gPlugin);
        }
        return ret;
    }
}
