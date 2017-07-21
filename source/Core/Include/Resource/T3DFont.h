

#ifndef __T3D_FONT_H__
#define __T3D_FONT_H__


#include "T3DResource.h"


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

        static FontPtr create(const String &name, int32_t fontSize, FontType fontType);

        virtual ~Font();

        virtual Type getType() const override;

        int32_t getFontSize() const { return mFontSize; }
        FontType getFontType() const { return mFontType; }

        /**
         * @brief 更新内容，主要是更新字体纹理
         * @param [in] text : UTF8的文本字符串
         * @param [out] material : 返回的材质
         * @param [out] u : 返回的纹理坐标u
         * @param [out] v : 返回的纹理坐标v
         */
        virtual void updateContent(const String &text, MaterialPtr &material, size_t &u, size_t &v) = 0;

    protected:
        Font(const String &name, int32_t fontSize, FontType fontType);

    protected:
        int32_t     mFontSize;
        FontType    mFontType;
    };
}


#endif  /*__T3D_FONT_H__*/
