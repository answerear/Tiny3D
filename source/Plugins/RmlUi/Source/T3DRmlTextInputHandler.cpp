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

#include "T3DRmlTextInputHandler.h"

#include "Input/T3DInput.h"
#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderWindow.h"
#include "Window/T3DWindow.h"

#include <RmlUi/Core/StringUtilities.h>
#include <RmlUi/Core/TextInputContext.h>

#include <algorithm>


namespace Tiny3D
{
    namespace
    {
        Window *activeWindow()
        {
            if (Agent::getInstancePtr() == nullptr)
            {
                return nullptr;
            }

            const RenderWindowPtr window = T3D_AGENT.getDefaultRenderWindow();
            if (window == nullptr)
            {
                return nullptr;
            }
            return window->getOSWindow();
        }

        Vector2 toWindow(const Vector2 &target)
        {
            if (Input::getInstancePtr() == nullptr)
            {
                return target;
            }
            return T3D_INPUT.unmapPointer(target);
        }
    }

    //--------------------------------------------------------------------------

    void RmlTextInputHandler::OnActivate(Rml::TextInputContext *inputContext)
    {
        mContext = inputContext;
        mStart = 0;
        mEnd = 0;
        placeCandidateWindow();

        Window *window = activeWindow();
        if (window != nullptr)
        {
            window->startTextInput();
        }
    }

    //--------------------------------------------------------------------------

    void RmlTextInputHandler::OnDeactivate(Rml::TextInputContext *inputContext)
    {
        if (mContext == inputContext)
        {
            stopInput();
        }
    }

    //--------------------------------------------------------------------------

    void RmlTextInputHandler::OnDestroy(Rml::TextInputContext *inputContext)
    {
        if (mContext == inputContext)
        {
            stopInput();
        }
    }

    //--------------------------------------------------------------------------

    void RmlTextInputHandler::handleEdit(const char *text, int32_t editStart, int32_t editLength)
    {
        if (mContext == nullptr)
        {
            return;
        }

        const Rml::String value(text != nullptr ? text : "");
        const int length = static_cast<int>(Rml::StringUtilities::LengthUTF8(value));
        const bool composing = mStart != mEnd;

        if (!composing)
        {
            mContext->GetSelectionRange(mStart, mEnd);
        }

        if (composing || length > 0)
        {
            mContext->SetText(Rml::StringView(value), mStart, mEnd);
        }

        mEnd = mStart + length;
        mContext->SetCompositionRange(mStart, mEnd);

        if (length > 0 && editStart >= 0 && editLength >= 0)
        {
            mContext->SetSelectionRange(mStart + editStart, mStart + editStart + editLength);
        }
        else if (composing)
        {
            mContext->SetCursorPosition(mEnd);
        }

        // 提交时 SDL 先发空的 TEXTEDITING，再发 TEXTINPUT。
        if (composing && length == 0)
        {
            mContext->CommitComposition(Rml::StringView());
            mStart = 0;
            mEnd = 0;
        }
    }

    //--------------------------------------------------------------------------

    bool RmlTextInputHandler::commitText(const char *text)
    {
        if (mContext == nullptr || mStart == mEnd)
        {
            return false;
        }

        const Rml::String value(text != nullptr ? text : "");
        mContext->CommitComposition(Rml::StringView(value));
        mStart = 0;
        mEnd = 0;
        return true;
    }

    //--------------------------------------------------------------------------

    void RmlTextInputHandler::placeCandidateWindow()
    {
        if (mContext == nullptr)
        {
            return;
        }

        Window *window = activeWindow();
        if (window == nullptr)
        {
            return;
        }

        Rml::Rectanglef box;
        if (!mContext->GetBoundingBox(box))
        {
            return;
        }

        const Vector2 topLeft = toWindow(Vector2(box.Left(), box.Top()));
        const Vector2 bottomRight = toWindow(Vector2(box.Right(), box.Bottom()));
        const int32_t width = std::max(1, static_cast<int32_t>(bottomRight.x() - topLeft.x()));
        const int32_t height = std::max(1, static_cast<int32_t>(bottomRight.y() - topLeft.y()));
        window->setTextInputRect(
            static_cast<int32_t>(topLeft.x()),
            static_cast<int32_t>(topLeft.y()),
            width,
            height);
    }

    //--------------------------------------------------------------------------

    void RmlTextInputHandler::stopInput()
    {
        mContext = nullptr;
        mStart = 0;
        mEnd = 0;

        Window *window = activeWindow();
        if (window != nullptr)
        {
            window->stopTextInput();
        }
    }

    //--------------------------------------------------------------------------
}
