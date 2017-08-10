/***************************************************************************************************
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
 **************************************************************************************************/

#ifndef __T3D_FONT_MANAGER_H__
#define __T3D_FONT_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DFont.h"


namespace Tiny3D
{
    /**
     * @class FontConfig
     * @brief 字体配置项，用于给FontManager读取用户对字体的配置
     */
    struct T3D_ENGINE_API FontConfig
    {
        /**
         * @brief 字符内存管理策略，主要用于内部纹理不足的情况如何重新调整分配空间
         */
        enum Strategy
        {
            E_STRATEGY_AUTO = 0,    /// 内部自动选择
            E_STRATEGY_LRU,         /// 当纹理空间不够时，使用LRU算法淘汰最长时间没有渲染的纹理
            E_STRATEGY_APPEND,      /// 新增另外一个材质和纹理
        };

        /**
         * @brief 字体配置中的默认值
         */
        enum DefaultValue
        {
            E_DEFAULT_TEX_SIZE = 512,       /// 默认初始的纹理大小
            E_DEFAULT_TEX_MAX = 2048,       /// 默认最大纹理大小，这会跟硬件支持的最大值取个最小值
            E_DEFAULT_TEX_INC_SIZE = 0,     /// 默认纹理增长大小，用于纹理空间不足自动增长的时候
            E_DEFAULT_BLOCK_SIZE = 128,     /// 默认区块大小
        };

        FontConfig()
            : initTexWidth(E_DEFAULT_TEX_SIZE)
            , initTexHeight(E_DEFAULT_TEX_SIZE)
            , maxTexWidth(E_DEFAULT_TEX_MAX)
            , maxTexHeight(E_DEFAULT_TEX_MAX)
            , blockWidth(E_DEFAULT_BLOCK_SIZE)
            , blockHeight(E_DEFAULT_BLOCK_SIZE)
            , strategy(E_STRATEGY_AUTO)
        {
        }

        size_t      initTexWidth;       /// 初始纹理宽度，默认是E_DEFAULT_TEX_SIZE
        size_t      initTexHeight;      /// 初始纹理高度，默认是E_DEFAULT_TEX_SIZE
        size_t      maxTexWidth;        /// 最大纹理宽度，此值会跟硬件支持最大纹理宽度比较取最小值，
                                        /// 默认是E_DEFAULT_TEX_MAX
        size_t      maxTexHeight;       /// 最大纹理高度，此值会跟硬件支持最大纹理高度比较取最小值，
                                        /// 默认是E_DEFAULT_TEX_MAX
        size_t      blockWidth;         /// 子纹理区块宽度，默认是E_DEFAULT_BLOCK_SIZE
        size_t      blockHeight;        /// 子纹理区块高度，默认是E_DEFAULT_BLOCK_SIZE
        Strategy    strategy;           /// 管理策略，默认是E_STRATEGY_AUTO，
                                        /// 具体会根据不同的字体实现来定制
    };

    /**
     * @class FontManager
     * @brief 字体资源管理器，主要用于加载、卸载等各种字体资源的管理
     * @note 本类是单例，无法在外部实例化，否则会抛出异常，当引擎启动后，
     *      请直接使用T3D_FONT_MGR宏来访问，
     *      或者使用FontManager::getInstance()和FontManager::getInstancePtr()来访问。
     * @see class Singleton
     */
    class T3D_ENGINE_API FontManager
        : public Singleton<FontManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 默认构造函数
         * @param [in] config : 字体配置项，默认用引擎默认配置
         * @see struct FontConfig
         */
        FontManager(const FontConfig &config = FontConfig());

        /**
         * @brief 析构函数
         */
        virtual ~FontManager();

        /**
         * @brief 根据指定字体名称加载一种字体
         * @param [in] fontSize : 字体大小
         * @param [in] name : 字体名称
         * @return 加载成功返回字体资源对象
         * @see void unloadFont(FontPtr &font)
         * @see class Font
         */
        virtual FontPtr loadFont(int32_t fontSize, const String &name = "", Font::FontType fontType = Font::E_FT_TRUETYPE);

        /**
         * @brief 卸载字体资源
         * @param [in] font : 字体对象
         * @return void
         * @see FontPtr loadFont(FontPtr &font);
         * @see class Font
         */
        virtual void unloadFont(FontPtr &font);

        /**
         * @brief 获取字体配置项
         * @return 返回字体配置项
         * @see struct FontConfig
         */
        const FontConfig &getConfig() const { return mFontConfig; }

        /**
         * @brief 设置默认字体名称
         * @param [in] fontName : 默认字体名称字符串
         * @return void
         * @see const String &getDefaultFontName() const
         */
        void setDefaultFontName(const String &fontName) { mDefaultFontName = fontName; }

        /**
         * @brief 获取默认字体名称
         * @return 返回默认字体名称
         * @see void setDefaultFontName(const String &fontName)
         */
        const String &getDefaultFontName() const { return mDefaultFontName; }

    protected:
        /**
         * @brief 从父类继承，重现以实现创建具体资源对象
         * @param [in] name : 资源名称
         * @param [in] argc : 可变参数长度
         * @param [in] args : 可变参数列表
         * @return 调用成功返回具体资源对象，本接口重写后返回的是Font及其子类
         * @see class Font
         */
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;

    protected:
        FontConfig  mFontConfig;        /// 字体配置项
        String      mDefaultFontName;   /// 默认字体名称
    };

    #define T3D_FONT_MGR    (FontManager::getInstance())
}


#endif  /*__T3D_FONT_MANAGER_H__*/
