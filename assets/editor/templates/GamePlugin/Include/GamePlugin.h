/*******************************************************************************
 * {ProjectName} —— 业务逻辑插件
 ******************************************************************************/

#pragma once


#include "GamePluginPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief {ProjectName} 业务逻辑插件
     * \remarks
     *   Behaviour 派生类的类型注册是由各自 .cpp 里的 RTTR_REGISTRATION 在 DLL
     *   加载时自动完成的，不需要在这里逐个登记。这个类只负责需要显式初始化的
     *   全局性东西，比如自定义资源管理器、网络连接、全局配置。没有的话留空即可。
     */
    class GamePlugin : public Plugin
    {
    public:
        GamePlugin();

        ~GamePlugin() override;

        /// 获取插件名称
        const String &getName() const override;

        /// 安装插件，此时引擎子系统尚未全部就绪，只做不依赖引擎的初始化
        TResult install() override;

        /// 启动插件，引擎已就绪，可以在这里做需要引擎服务的初始化
        TResult startup() override;

        /// 关闭插件，与 startup 配对
        TResult shutdown() override;

        /// 卸载插件，与 install 配对
        TResult uninstall() override;

    protected:
        String mName {};
    };
}
