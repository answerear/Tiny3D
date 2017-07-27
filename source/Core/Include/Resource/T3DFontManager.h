

#ifndef __T3D_FONT_MANAGER_H__
#define __T3D_FONT_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DFont.h"


namespace Tiny3D
{
    struct T3D_ENGINE_API FontConfig
    {
        enum Strategy
        {
            E_STRATEGY_AUTO = 0,    /// 内部自动选择
            E_STRATEGY_LRU,         /// 当纹理空间不够时，使用LRU算法淘汰最长时间没有渲染的纹理
            E_STRATEGY_APPEND,      /// 新增另外一个材质和纹理
        };

        enum DefaultValue
        {
            E_DEFAULT_TEX_SIZE = 512,
            E_DEFAULT_TEX_MAX = 2048,
            E_DEFAULT_TEX_INC_SIZE = 0,
            E_DEFAULT_BLOCK_SIZE = 128,
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
        size_t      maxTexWidth;        /// 最大纹理宽度，此值会跟硬件支持最大纹理宽度比较取最小值，默认是E_DEFAULT_TEX_MAX
        size_t      maxTexHeight;       /// 最大纹理高度，此值会跟硬件支持最大纹理高度比较取最小值，默认是E_DEFAULT_TEX_MAX
        size_t      blockWidth;         /// 子纹理区块宽度，默认是E_DEFAULT_BLOCK_SIZE
        size_t      blockHeight;        /// 子纹理区块高度，默认是E_DEFAULT_BLOCK_SIZE
        Strategy    strategy;           /// 管理策略，默认是E_STRATEGY_AUTO，具体会根据不同的字体实现来定制
    };

    class T3D_ENGINE_API FontManager
        : public Singleton<FontManager>
        , public ResourceManager
    {
    public:
        FontManager(const FontConfig &config = FontConfig());
        virtual ~FontManager();

        virtual FontPtr loadFont(const String &name);

        const FontConfig &getConfig() const { return mFontConfig; }

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;

        FontConfig  mFontConfig;
    };

    #define T3D_FONT_MGR    (FontManager::getInstance())
}


#endif  /*__T3D_FONT_MANAGER_H__*/
