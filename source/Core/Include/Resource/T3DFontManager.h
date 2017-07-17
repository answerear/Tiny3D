

#ifndef __T3D_FONT_MANAGER_H__
#define __T3D_FONT_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DFont.h"


namespace Tiny3D
{
    class T3D_ENGINE_API FontManager
        : public Singleton<FontManager>
        , public ResourceManager
    {
    public:
        FontManager();
        virtual ~FontManager();

        virtual FontPtr loadFont(const String &name);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;
    };

    #define T3D_FONT_MGR    (FontManager::getInstance())
}


#endif  /*__T3D_FONT_MANAGER_H__*/
