

#ifndef __T3D_FONT_H__
#define __T3D_FONT_H__


#include "T3DResource.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Font : public Resource
    {
    public:
        enum FontType
        {
            E_FT_TRUETYPE = 1,
            E_FT_IMAGE = 2,
        };

        struct FontConfig
        {
            enum Strategy
            {
                E_STRATEGY_AUTO = 0,    /// 内部自动选择
                E_STRATEGY_LRU,         /// 当纹理空间不够时，使用LRU算法淘汰最长时间没有渲染的纹理
                E_STRATEGY_APPEND,      /// 新增另外一个材质和纹理
            };

            enum DefaultValue
            {
                E_DEFAULT_SIZE = 512,
                E_DEFAULT_MAX = 2048,
            };

            FontConfig()
                : initTexWidth(E_DEFAULT_SIZE)
                , initTexHeight(E_DEFAULT_SIZE)
                , maxTexWidth(E_DEFAULT_MAX)
                , maxTexHeight(E_DEFAULT_MAX)
                , strategy(E_STRATEGY_AUTO)
            {
            }

            size_t      initTexWidth;       /// 初始纹理宽度，默认是512
            size_t      initTexHeight;      /// 初始纹理高度，默认是512
            size_t      maxTexWidth;        /// 最大纹理宽度，此值会跟硬件支持最大纹理宽度比较取最小值，默认是2048
            size_t      maxTexHeight;       /// 最大纹理高度，此值会跟硬件支持最大纹理高度比较取最小值，默认是2048
            Strategy    strategy;           /// 管理策略，默认是E_STRATEGY_AUTO，具体会根据不同的字体实现来定制
        };

        class Char : public Object
        {
        public:
            Char()
                : mCode(0)
            {}

            MaterialPtr     mMaterial;      /// 材质对象
            int32_t         mCode;          /// 字符编码
            Rect            mArea;          /// 字符对应材质中纹理信息
        };

        T3D_DECLARE_SMART_PTR(Char);

        typedef std::vector<CharPtr>    CharSet;
        typedef CharSet::iterator       CharSetItr;
        typedef CharSet::const_iterator CharSetConstItr;
        typedef CharSet::value_type     CharSetValue;

        static FontPtr create(const String &name, int32_t fontSize, FontType fontType, const FontConfig &fontConfig);

        virtual ~Font();

        virtual Type getType() const override;

        /**
         * @brief 返回字体大小
         */
        int32_t getFontSize() const { return mFontSize; }

        /**
         * @brief 返回字体类型，是TRUETYPE还是IMAGE图像
         * @see enum FontType
         */
        FontType getFontType() const { return mFontType; }

        /**
         * @brief 更新内容，主要是更新字体纹理
         * @param [in] text : UTF8的文本字符串
         * @param [out] material : 返回包含文本的材质对象
         * @param [out] set : 返回字符串信息
         * @return 成功更新字体材质和纹理，则返回true，否则返回false.
         * @see class Char
         */
        virtual bool updateContent(const String &text, MaterialPtr &material, CharSet &set) = 0;

    protected:
        Font(const String &name, int32_t fontSize, FontType fontType, const FontConfig &fontConfig);

    protected:
        int32_t     mFontSize;
        FontType    mFontType;
        FontConfig  mFontConfig;
    };
}


#endif  /*__T3D_FONT_H__*/
