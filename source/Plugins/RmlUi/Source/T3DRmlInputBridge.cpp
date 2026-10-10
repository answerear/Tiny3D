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

#include "T3DRmlInputBridge.h"

#include "T3DRmlCanvas.h"
#include "T3DRmlTextInputHandler.h"

#include "Component/T3DCamera.h"
#include "Input/T3DInput.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "Render/T3DRenderWindow.h"
#include "Render/T3DViewport.h"

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Input.h>

#include <SDL.h>

#include <algorithm>
#include <cstring>


namespace Tiny3D
{
    namespace
    {
        // 键值与 SDLK 相同，表移植自 RmlUi 的 ConvertKey（MIT）。
        Rml::Input::KeyIdentifier convertKey(int key)
        {
            switch (key)
            {
            case TKEY_UNKNOWN: return Rml::Input::KI_UNKNOWN;
            case TKEY_ESCAPE: return Rml::Input::KI_ESCAPE;
            case TKEY_SPACE: return Rml::Input::KI_SPACE;
            case TKEY_0: return Rml::Input::KI_0;
            case TKEY_1: return Rml::Input::KI_1;
            case TKEY_2: return Rml::Input::KI_2;
            case TKEY_3: return Rml::Input::KI_3;
            case TKEY_4: return Rml::Input::KI_4;
            case TKEY_5: return Rml::Input::KI_5;
            case TKEY_6: return Rml::Input::KI_6;
            case TKEY_7: return Rml::Input::KI_7;
            case TKEY_8: return Rml::Input::KI_8;
            case TKEY_9: return Rml::Input::KI_9;
            case TKEY_a: return Rml::Input::KI_A;
            case TKEY_b: return Rml::Input::KI_B;
            case TKEY_c: return Rml::Input::KI_C;
            case TKEY_d: return Rml::Input::KI_D;
            case TKEY_e: return Rml::Input::KI_E;
            case TKEY_f: return Rml::Input::KI_F;
            case TKEY_g: return Rml::Input::KI_G;
            case TKEY_h: return Rml::Input::KI_H;
            case TKEY_i: return Rml::Input::KI_I;
            case TKEY_j: return Rml::Input::KI_J;
            case TKEY_k: return Rml::Input::KI_K;
            case TKEY_l: return Rml::Input::KI_L;
            case TKEY_m: return Rml::Input::KI_M;
            case TKEY_n: return Rml::Input::KI_N;
            case TKEY_o: return Rml::Input::KI_O;
            case TKEY_p: return Rml::Input::KI_P;
            case TKEY_q: return Rml::Input::KI_Q;
            case TKEY_r: return Rml::Input::KI_R;
            case TKEY_s: return Rml::Input::KI_S;
            case TKEY_t: return Rml::Input::KI_T;
            case TKEY_u: return Rml::Input::KI_U;
            case TKEY_v: return Rml::Input::KI_V;
            case TKEY_w: return Rml::Input::KI_W;
            case TKEY_x: return Rml::Input::KI_X;
            case TKEY_y: return Rml::Input::KI_Y;
            case TKEY_z: return Rml::Input::KI_Z;
            case TKEY_SEMICOLON: return Rml::Input::KI_OEM_1;
            case TKEY_PLUS: return Rml::Input::KI_OEM_PLUS;
            case TKEY_COMMA: return Rml::Input::KI_OEM_COMMA;
            case TKEY_MINUS: return Rml::Input::KI_OEM_MINUS;
            case TKEY_PERIOD: return Rml::Input::KI_OEM_PERIOD;
            case TKEY_SLASH: return Rml::Input::KI_OEM_2;
            case TKEY_BACKQUOTE: return Rml::Input::KI_OEM_3;
            case TKEY_LEFTBRACKET: return Rml::Input::KI_OEM_4;
            case TKEY_BACKSLASH: return Rml::Input::KI_OEM_5;
            case TKEY_RIGHTBRACKET: return Rml::Input::KI_OEM_6;
            case TKEY_QUOTEDBL: return Rml::Input::KI_OEM_7;
            case TKEY_KP_0: return Rml::Input::KI_NUMPAD0;
            case TKEY_KP_1: return Rml::Input::KI_NUMPAD1;
            case TKEY_KP_2: return Rml::Input::KI_NUMPAD2;
            case TKEY_KP_3: return Rml::Input::KI_NUMPAD3;
            case TKEY_KP_4: return Rml::Input::KI_NUMPAD4;
            case TKEY_KP_5: return Rml::Input::KI_NUMPAD5;
            case TKEY_KP_6: return Rml::Input::KI_NUMPAD6;
            case TKEY_KP_7: return Rml::Input::KI_NUMPAD7;
            case TKEY_KP_8: return Rml::Input::KI_NUMPAD8;
            case TKEY_KP_9: return Rml::Input::KI_NUMPAD9;
            case TKEY_KP_ENTER: return Rml::Input::KI_NUMPADENTER;
            case TKEY_KP_MULTIPLY: return Rml::Input::KI_MULTIPLY;
            case TKEY_KP_PLUS: return Rml::Input::KI_ADD;
            case TKEY_KP_MINUS: return Rml::Input::KI_SUBTRACT;
            case TKEY_KP_PERIOD: return Rml::Input::KI_DECIMAL;
            case TKEY_KP_DIVIDE: return Rml::Input::KI_DIVIDE;
            case TKEY_KP_EQUALS: return Rml::Input::KI_OEM_NEC_EQUAL;
            case TKEY_BACKSPACE: return Rml::Input::KI_BACK;
            case TKEY_TAB: return Rml::Input::KI_TAB;
            case TKEY_CLEAR: return Rml::Input::KI_CLEAR;
            case TKEY_RETURN: return Rml::Input::KI_RETURN;
            case TKEY_PAUSE: return Rml::Input::KI_PAUSE;
            case TKEY_CAPSLOCK: return Rml::Input::KI_CAPITAL;
            case TKEY_PAGEUP: return Rml::Input::KI_PRIOR;
            case TKEY_PAGEDOWN: return Rml::Input::KI_NEXT;
            case TKEY_END: return Rml::Input::KI_END;
            case TKEY_HOME: return Rml::Input::KI_HOME;
            case TKEY_LEFT: return Rml::Input::KI_LEFT;
            case TKEY_UP: return Rml::Input::KI_UP;
            case TKEY_RIGHT: return Rml::Input::KI_RIGHT;
            case TKEY_DOWN: return Rml::Input::KI_DOWN;
            case TKEY_INSERT: return Rml::Input::KI_INSERT;
            case TKEY_DELETE: return Rml::Input::KI_DELETE;
            case TKEY_HELP: return Rml::Input::KI_HELP;
            case TKEY_F1: return Rml::Input::KI_F1;
            case TKEY_F2: return Rml::Input::KI_F2;
            case TKEY_F3: return Rml::Input::KI_F3;
            case TKEY_F4: return Rml::Input::KI_F4;
            case TKEY_F5: return Rml::Input::KI_F5;
            case TKEY_F6: return Rml::Input::KI_F6;
            case TKEY_F7: return Rml::Input::KI_F7;
            case TKEY_F8: return Rml::Input::KI_F8;
            case TKEY_F9: return Rml::Input::KI_F9;
            case TKEY_F10: return Rml::Input::KI_F10;
            case TKEY_F11: return Rml::Input::KI_F11;
            case TKEY_F12: return Rml::Input::KI_F12;
            case TKEY_F13: return Rml::Input::KI_F13;
            case TKEY_F14: return Rml::Input::KI_F14;
            case TKEY_F15: return Rml::Input::KI_F15;
            case TKEY_NUMLOCKCLEAR: return Rml::Input::KI_NUMLOCK;
            case TKEY_SCROLLLOCK: return Rml::Input::KI_SCROLL;
            case TKEY_LSHIFT: return Rml::Input::KI_LSHIFT;
            case TKEY_RSHIFT: return Rml::Input::KI_RSHIFT;
            case TKEY_LCTRL: return Rml::Input::KI_LCONTROL;
            case TKEY_RCTRL: return Rml::Input::KI_RCONTROL;
            case TKEY_LALT: return Rml::Input::KI_LMENU;
            case TKEY_RALT: return Rml::Input::KI_RMENU;
            case TKEY_LGUI: return Rml::Input::KI_LMETA;
            case TKEY_RGUI: return Rml::Input::KI_RMETA;
            default: break;
            }
            return Rml::Input::KI_UNKNOWN;
        }

        int convertMouseButton(uint8_t button)
        {
            switch (button)
            {
            case static_cast<uint8_t>(MouseButton::Left): return 0;
            case static_cast<uint8_t>(MouseButton::Right): return 1;
            case static_cast<uint8_t>(MouseButton::Middle): return 2;
            default: return 3;
            }
        }

        int keyModifiers()
        {
            if (Input::getInstancePtr() == nullptr)
            {
                return 0;
            }

            const uint16_t mods = T3D_INPUT.getModifiers();
            int result = 0;
            if ((mods & KMOD_CTRL) != 0)
            {
                result |= Rml::Input::KM_CTRL;
            }
            if ((mods & KMOD_SHIFT) != 0)
            {
                result |= Rml::Input::KM_SHIFT;
            }
            if ((mods & KMOD_ALT) != 0)
            {
                result |= Rml::Input::KM_ALT;
            }
            if ((mods & KMOD_GUI) != 0)
            {
                result |= Rml::Input::KM_META;
            }
            if ((mods & KMOD_NUM) != 0)
            {
                result |= Rml::Input::KM_NUMLOCK;
            }
            if ((mods & KMOD_CAPS) != 0)
            {
                result |= Rml::Input::KM_CAPSLOCK;
            }
            return result;
        }

        bool targetPixelSize(RenderTarget *target, int32_t &width, int32_t &height)
        {
            width = 0;
            height = 0;
            if (target == nullptr)
            {
                return false;
            }

            if (target->getType() == RenderTarget::Type::E_RT_WINDOW)
            {
                const RenderWindowPtr window = target->getRenderWindow();
                if (window == nullptr)
                {
                    return false;
                }
                width = window->getDescriptor().Width;
                height = window->getDescriptor().Height;
            }
            else
            {
                const RenderTexturePtr texture = target->getRenderTexture();
                if (texture == nullptr)
                {
                    return false;
                }
                width = static_cast<int32_t>(texture->getWidth());
                height = static_cast<int32_t>(texture->getHeight());
            }
            return width > 0 && height > 0;
        }

        void contextPoint(RmlCanvas *canvas, const Vector2 &windowPos, int &x, int &y)
        {
            Vector2 mapped = windowPos;
            if (Input::getInstancePtr() != nullptr)
            {
                mapped = T3D_INPUT.mapPointer(windowPos);
            }

            int32_t originX = 0;
            int32_t originY = 0;
            GameObject *owner = canvas->getGameObject();
            if (owner != nullptr)
            {
                CameraPtr camera = owner->getComponent<Camera>();
                if (camera != nullptr && camera->getRenderTarget() != nullptr)
                {
                    int32_t targetWidth = 0;
                    int32_t targetHeight = 0;
                    if (targetPixelSize(camera->getRenderTarget(), targetWidth, targetHeight))
                    {
                        const Viewport &viewport = camera->getViewport();
                        originX = static_cast<int32_t>(static_cast<Real>(targetWidth) * viewport.Left);
                        originY = static_cast<int32_t>(static_cast<Real>(targetHeight) * viewport.Top);
                    }
                }
            }

            x = static_cast<int>(mapped.x()) - originX;
            y = static_cast<int>(mapped.y()) - originY;
        }

        Vector2 windowSize()
        {
            if (Agent::getInstancePtr() == nullptr)
            {
                return Vector2(1.0f, 1.0f);
            }
            const RenderWindowPtr window = T3D_AGENT.getDefaultRenderWindow();
            if (window == nullptr)
            {
                return Vector2(1.0f, 1.0f);
            }
            return Vector2(
                static_cast<Real>(window->getDescriptor().Width),
                static_cast<Real>(window->getDescriptor().Height));
        }
    }

    //--------------------------------------------------------------------------

    void RmlInputBridge::onAppEvent(const AppEvent &event)
    {
        StoredEvent stored;
        std::memcpy(stored.bytes, &event, sizeof(AppEvent));
        mQueue.push_back(stored);
    }

    //--------------------------------------------------------------------------

    void RmlInputBridge::clear()
    {
        mQueue.clear();
    }

    //--------------------------------------------------------------------------

    void RmlInputBridge::dispatch(const TArray<RmlCanvas *> &canvases, RmlTextInputHandler *textInput)
    {
        TArray<RmlCanvas *> ordered;
        ordered.reserve(canvases.size());
        for (RmlCanvas *canvas : canvases)
        {
            if (canvas != nullptr && canvas->isActiveAndEnabled() && canvas->getContext() != nullptr)
            {
                ordered.push_back(canvas);
            }
        }
        std::stable_sort(ordered.begin(), ordered.end(), [](RmlCanvas *lhs, RmlCanvas *rhs)
        {
            return lhs->getSortOrder() > rhs->getSortOrder();
        });

        const int mods = keyModifiers();

        auto deliver = [&ordered](auto &&visit)
        {
            for (RmlCanvas *canvas : ordered)
            {
                // Process* 返回 true 表示事件还在向下传。
                if (!visit(canvas, canvas->getContext()))
                {
                    return;
                }
            }
        };

        for (const StoredEvent &stored : mQueue)
        {
            const AppEvent *event = reinterpret_cast<const AppEvent *>(stored.bytes);
            switch (event->type)
            {
            case APP_MOUSEMOTION:
                deliver([&](RmlCanvas *canvas, Rml::Context *context)
                {
                    int x = 0;
                    int y = 0;
                    contextPoint(canvas, Vector2(static_cast<Real>(event->motion.x), static_cast<Real>(event->motion.y)), x, y);
                    return context->ProcessMouseMove(x, y, mods);
                });
                break;
            case APP_MOUSEBUTTONDOWN:
                deliver([&](RmlCanvas *canvas, Rml::Context *context)
                {
                    (void)canvas;
                    return context->ProcessMouseButtonDown(convertMouseButton(event->button.button), mods);
                });
                break;
            case APP_MOUSEBUTTONUP:
                deliver([&](RmlCanvas *canvas, Rml::Context *context)
                {
                    (void)canvas;
                    return context->ProcessMouseButtonUp(convertMouseButton(event->button.button), mods);
                });
                break;
            case APP_MOUSEWHEEL:
                deliver([&](RmlCanvas *canvas, Rml::Context *context)
                {
                    (void)canvas;
                    const Rml::Vector2f delta(
                        -static_cast<float>(event->wheel.x),
                        -static_cast<float>(event->wheel.y));
                    return context->ProcessMouseWheel(delta, mods);
                });
                break;
            case APP_FINGERDOWN:
            case APP_FINGERMOTION:
            case APP_FINGERUP:
            {
                const Vector2 pixels = windowSize();
                deliver([&](RmlCanvas *canvas, Rml::Context *context)
                {
                    int x = 0;
                    int y = 0;
                    contextPoint(canvas, Vector2(event->tfinger.x * pixels.x(), event->tfinger.y * pixels.y()), x, y);
                    Rml::TouchList touches;
                    touches.push_back(Rml::Touch {
                        static_cast<Rml::TouchId>(event->tfinger.fingerID),
                        Rml::Vector2f(static_cast<float>(x), static_cast<float>(y))});
                    if (event->type == APP_FINGERDOWN)
                    {
                        return context->ProcessTouchStart(touches, mods);
                    }
                    if (event->type == APP_FINGERMOTION)
                    {
                        return context->ProcessTouchMove(touches, mods);
                    }
                    return context->ProcessTouchEnd(touches, mods);
                });
                break;
            }
            case APP_KEYDOWN:
#if defined(T3D_RMLUI_DEBUGGER)
                if (static_cast<int>(event->key.keysym.sym) == TKEY_F8)
                {
                    break;
                }
#endif
                deliver([&](RmlCanvas *canvas, Rml::Context *context)
                {
                    (void)canvas;
                    const int key = static_cast<int>(event->key.keysym.sym);
                    bool propagating = context->ProcessKeyDown(convertKey(key), mods);
                    if (key == TKEY_RETURN || key == TKEY_KP_ENTER)
                    {
                        propagating = context->ProcessTextInput('\n') && propagating;
                    }
                    return propagating;
                });
                break;
            case APP_KEYUP:
#if defined(T3D_RMLUI_DEBUGGER)
                if (static_cast<int>(event->key.keysym.sym) == TKEY_F8)
                {
                    break;
                }
#endif
                deliver([&](RmlCanvas *canvas, Rml::Context *context)
                {
                    (void)canvas;
                    return context->ProcessKeyUp(convertKey(static_cast<int>(event->key.keysym.sym)), mods);
                });
                break;
            case APP_TEXTEDITING:
                if (textInput != nullptr)
                {
                    textInput->handleEdit(event->edit.text, event->edit.start, event->edit.length);
                }
                break;
            case APP_TEXTINPUT:
                if (textInput != nullptr && textInput->commitText(event->text.text))
                {
                    break;
                }
                deliver([&](RmlCanvas *canvas, Rml::Context *context)
                {
                    (void)canvas;
                    return context->ProcessTextInput(Rml::String(event->text.text));
                });
                break;
            case APP_WINDOWEVENT:
                if (event->window.event == APP_WINDOWEVENT_LEAVE
                    || event->window.event == APP_WINDOWEVENT_FOCUS_LOST)
                {
                    deliver([&](RmlCanvas *canvas, Rml::Context *context)
                    {
                        (void)canvas;
                        return context->ProcessMouseLeave();
                    });
                }
                break;
            default:
                break;
            }
        }

        mQueue.clear();
    }

    //--------------------------------------------------------------------------
}
