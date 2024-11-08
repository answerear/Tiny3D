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


#include "ImInputText.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    ImInputText::~ImInputText()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ImInputText::create(uint32_t id, const String &name, int32_t maxNumberOfChars, const ImInputTextCallback &callback, ImWidget *parent)
    {
        return ImWidget::createInternal(id, name, parent, 2, maxNumberOfChars, &callback);
    }

    //--------------------------------------------------------------------------

    TResult ImInputText::createInternal(uint32_t id, const String &name, ImWidget *parent, int32_t argc, va_list &args)
    {
        TResult ret = T3D_OK;

        do
        {
            T3D_ASSERT(argc >= 2);
            mMaxNumberOfChars = va_arg(args, int32_t);

            if (mMaxNumberOfChars <= 0)
            {
                ret = T3D_ERR_INVALID_PARAM;
                IMGUI_LOG_ERROR("Invalid the number of max characters !");
                break;
            }

            ImInputTextCallback *callback = va_arg(args, ImInputTextCallback*);
            if (callback != nullptr)
            {
                mCallback = *callback;
            }

            mText = new char[mMaxNumberOfChars + 1];
            memset(mText, 0, sizeof(char) * (mMaxNumberOfChars + 1));
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ImInputText::onDestroy()
    {
        T3D_SAFE_DELETE_ARRAY(mText);
    }

    //--------------------------------------------------------------------------

    bool ImInputText::onGUIBegin()
    {
        return true;
    }

    //--------------------------------------------------------------------------

    void ImInputText::onGUI()
    {
        ImGui::InputText(getName().c_str(), mText, mMaxNumberOfChars+1,
            ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_EnterReturnsTrue,
            ImInputText::inputTextCallback,
            this);
    }

    //--------------------------------------------------------------------------

    void ImInputText::onGUIEnd()
    {
    }

    //--------------------------------------------------------------------------

    int32_t ImInputText::inputTextCallback(ImGuiInputTextCallbackData *data)
    {
        ImInputText *inputText = static_cast<ImInputText *>(data->UserData);
        if (inputText != nullptr)
        {
            return inputText->inputTextCallback();
        }
        return 0;
    }

    //--------------------------------------------------------------------------

    int32_t ImInputText::inputTextCallback()
    {
        if (mCallback != nullptr)
        {
            mCallback(this, mText);
        }
        return 0;
    }

    //--------------------------------------------------------------------------

}
