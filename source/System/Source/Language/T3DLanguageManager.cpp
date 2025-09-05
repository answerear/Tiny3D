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


#include "Language/T3DLanguageManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    LanguageManagerPtr LanguageManager::create()
    {
        return T3D_NEW LanguageManager();
    }

    //--------------------------------------------------------------------------

    LanguageManager::~LanguageManager()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult LanguageManager::init(const String &languageFilePath)
    {
        TResult ret = T3D_OK;

        do
        {
            FileDataStream fs;
            if (!fs.open(languageFilePath.c_str(), FileDataStream::EOpenMode::E_MODE_TEXT|FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            ret = init(fs);
            
            fs.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult LanguageManager::init(DataStream &stream)
    {
        mTextCache.clear();
        
        uint32_t index = 0;
        
        while (!stream.eof())
        {
            String s = stream.readLine();
            mTextCache.emplace(index, s);
            ++index;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    const String &LanguageManager::getText(uint32_t txtID)
    {
        static String empty;

        auto itr = mTextCache.find(txtID);
        if (itr == mTextCache.end())
        {
            return empty;
        }

        return itr->second;
    }

    //--------------------------------------------------------------------------
}
