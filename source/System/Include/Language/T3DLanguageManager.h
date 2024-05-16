/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
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

#ifndef __T3D_LANGUAGE_MANAGER_H__
#define __T3D_LANGUAGE_MANAGER_H__


#include "T3DSystemPrerequisites.h"
#include "Object/T3DObject.h"
#include "Object/T3DSmartPtr.h"


namespace Tiny3D
{
    class LanguageManager;
    T3D_DECLARE_SMART_PTR(LanguageManager);
    
    class T3D_SYSTEM_API LanguageManager
        : public Object
        , public Singleton<LanguageManager>
    {
    public:
        static LanguageManagerPtr create();
        
        ~LanguageManager() override;

        TResult init(const String &languageFilePath);

        TResult init(DataStream &stream);

        const String &getText(uint32_t txtID);

    protected:
        LanguageManager() = default;
        
        using TextCache = TUnorderedMap<uint32_t, String>;

        TextCache  mTextCache {};
    };

    #define T3D_LANG_MGR    (LanguageManager::getInstance())
}


#endif  /*__T3D_LANGUAGE_MANAGER_H__*/
