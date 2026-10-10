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

#ifndef __T3D_RML_TEXT_INPUT_HANDLER_H__
#define __T3D_RML_TEXT_INPUT_HANDLER_H__


#include "T3DRmlUiPrerequisites.h"



namespace Tiny3D
{
    /**
     * \brief 把平台文本事件写回 RmlUi 输入框，并开关软键盘 / IME。
     */
    class RmlTextInputHandler : public Rml::TextInputHandler
    {
    public:
        void OnActivate(Rml::TextInputContext *inputContext) override;
        void OnDeactivate(Rml::TextInputContext *inputContext) override;
        void OnDestroy(Rml::TextInputContext *inputContext) override;

        void handleEdit(const char *text, int32_t start, int32_t length);

        /// 正在组字时提交并返回 true。否则由调用方走 ProcessTextInput。
        bool commitText(const char *text);

        bool isActive() const { return mContext != nullptr; }

    private:
        void placeCandidateWindow();
        void stopInput();

        Rml::TextInputContext *mContext {nullptr};
        int mStart {0};
        int mEnd {0};
    };
}


#endif  /*__T3D_RML_TEXT_INPUT_HANDLER_H__*/
