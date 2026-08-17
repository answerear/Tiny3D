/*******************************************************************************
 * {ProjectName} —— 业务逻辑插件
 ******************************************************************************/

#include "GamePlugin.h"


namespace Tiny3D
{
    GamePlugin::GamePlugin()
        : mName("{ProjectName}")
    {
    }

    //--------------------------------------------------------------------------

    GamePlugin::~GamePlugin()
    {
    }

    //--------------------------------------------------------------------------

    const String &GamePlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult GamePlugin::install()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GamePlugin::startup()
    {
        GAME_LOG_INFO("{ProjectName} game plugin started.");
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GamePlugin::shutdown()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GamePlugin::uninstall()
    {
        return T3D_OK;
    }
}
