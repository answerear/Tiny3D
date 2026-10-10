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

#include "T3DRmlUiPrerequisites.h"
#include "T3DRmlSystemInterface.h"
#include <RmlUi/Core/StringUtilities.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    double RmlSystemInterface::GetElapsedTime()
    {
        return static_cast<double>(Time::unscaledTime()) * 0.001;
    }

    //--------------------------------------------------------------------------

    bool RmlSystemInterface::LogMessage(Rml::Log::Type type, const Rml::String &message)
    {
        switch (type)
        {
        case Rml::Log::LT_ERROR:
        case Rml::Log::LT_ASSERT:
            T3D_LOG_ERROR(LOG_TAG_RMLUI, "%s", message.c_str());
            break;
        case Rml::Log::LT_WARNING:
            T3D_LOG_WARNING(LOG_TAG_RMLUI, "%s", message.c_str());
            break;
        default:
            T3D_LOG_INFO(LOG_TAG_RMLUI, "%s", message.c_str());
            break;
        }
        return true;
    }

    //--------------------------------------------------------------------------

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

        SystemCursor cursorFromName(const Rml::String &name)
        {
            if (name.empty() || name == "arrow")
            {
                return SystemCursor::Arrow;
            }
            if (name == "pointer")
            {
                return SystemCursor::Hand;
            }
            if (name == "text")
            {
                return SystemCursor::IBeam;
            }
            if (name == "move" || Rml::StringUtilities::StartsWith(name, "rmlui-scroll"))
            {
                return SystemCursor::SizeAll;
            }
            if (name == "resize")
            {
                return SystemCursor::SizeNWSE;
            }
            if (name == "cross")
            {
                return SystemCursor::Crosshair;
            }
            if (name == "unavailable")
            {
                return SystemCursor::No;
            }
            return SystemCursor::Arrow;
        }
    }

    void RmlSystemInterface::SetMouseCursor(const Rml::String &cursorName)
    {
        Window *window = activeWindow();
        if (window != nullptr)
        {
            window->setSystemCursor(cursorFromName(cursorName));
        }
    }

    //--------------------------------------------------------------------------

    void RmlSystemInterface::SetClipboardText(const Rml::String &text)
    {
        Window *window = activeWindow();
        if (window != nullptr)
        {
            window->setClipboardText(text.c_str());
        }
    }

    //--------------------------------------------------------------------------

    void RmlSystemInterface::GetClipboardText(Rml::String &text)
    {
        Window *window = activeWindow();
        text = (window != nullptr) ? window->getClipboardText() : "";
    }

    //--------------------------------------------------------------------------

    void RmlSystemInterface::ActivateKeyboard(Rml::Vector2f caretPosition, float lineHeight)
    {
        Window *window = activeWindow();
        if (window == nullptr)
        {
            return;
        }

        Vector2 origin(caretPosition.x, caretPosition.y);
        Vector2 extent(caretPosition.x + 1.0f, caretPosition.y + lineHeight);
        if (Input::getInstancePtr() != nullptr)
        {
            origin = T3D_INPUT.unmapPointer(origin);
            extent = T3D_INPUT.unmapPointer(extent);
        }

        window->setTextInputRect(
            static_cast<int32_t>(origin.x()),
            static_cast<int32_t>(origin.y()),
            std::max(1, static_cast<int32_t>(extent.x() - origin.x())),
            std::max(1, static_cast<int32_t>(extent.y() - origin.y())));
        window->startTextInput();
    }

    //--------------------------------------------------------------------------

    void RmlSystemInterface::DeactivateKeyboard()
    {
        Window *window = activeWindow();
        if (window != nullptr)
        {
            window->stopTextInput();
        }
    }

    //--------------------------------------------------------------------------
}
