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
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "Input/T3DInput.h"
#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderWindow.h"
#include "Window/T3DWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Input::Input()
    {
        reset();
    }

    //--------------------------------------------------------------------------

    Input::~Input()
    {
    }

    //--------------------------------------------------------------------------

    void Input::onAppEvent(const AppEvent &event)
    {
        processEvent(event);
    }

    //--------------------------------------------------------------------------

    void Input::processEvent(const AppEvent &event)
    {
        switch (event.type)
        {
        case APP_KEYDOWN:
        case APP_KEYUP:
            processKey(event.key);
            break;
        case APP_MOUSEMOTION:
            processMouseMotion(event.motion);
            break;
        case APP_MOUSEBUTTONDOWN:
        case APP_MOUSEBUTTONUP:
            processMouseButton(event.button);
            break;
        case APP_MOUSEWHEEL:
            processMouseWheel(event.wheel);
            break;
        case APP_FINGERDOWN:
        case APP_FINGERUP:
        case APP_FINGERMOTION:
            processTouch(event.tfinger);
            break;
        case APP_WINDOWEVENT:
            processWindow(event.window);
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    void Input::endFrame()
    {
        for (uint32_t i = 0; i < APP_NUM_SCANCODES; ++i)
        {
            mScanKeys[i].down = false;
            mScanKeys[i].up = false;
        }

        for (auto &pair : mKeyCodes)
        {
            pair.second.down = false;
            pair.second.up = false;
        }

        for (uint32_t i = 0; i < kMaxMouseButtons; ++i)
        {
            mMouseButtons[i].down = false;
            mMouseButtons[i].up = false;
        }

        mMouseDelta = Vector2(0.0f, 0.0f);
        mMouseScroll = Vector2(0.0f, 0.0f);

        uint32_t write = 0;
        for (uint32_t i = 0; i < mTouchCount; ++i)
        {
            if (mTouches[i].phase == TouchPhase::Ended
                || mTouches[i].phase == TouchPhase::Canceled)
            {
                continue;
            }

            mTouches[i].delta = Vector2(0.0f, 0.0f);
            mTouches[i].phase = TouchPhase::Stationary;
            if (write != i)
            {
                mTouches[write] = mTouches[i];
            }
            ++write;
        }
        mTouchCount = write;
    }

    //--------------------------------------------------------------------------

    void Input::reset()
    {
        for (uint32_t i = 0; i < APP_NUM_SCANCODES; ++i)
        {
            mScanKeys[i] = ButtonState();
        }
        mKeyCodes.clear();
        mModifiers = 0;

        for (uint32_t i = 0; i < kMaxMouseButtons; ++i)
        {
            mMouseButtons[i] = ButtonState();
        }
        mMouseDelta = Vector2(0.0f, 0.0f);
        mMouseScroll = Vector2(0.0f, 0.0f);

        mTouchCount = 0;
        for (uint32_t i = 0; i < kMaxTouches; ++i)
        {
            mTouches[i] = Touch();
        }
    }

    //--------------------------------------------------------------------------

    bool Input::isScanCodeValid(ScanCode scancode) const
    {
        return static_cast<uint32_t>(scancode) < APP_NUM_SCANCODES;
    }

    //--------------------------------------------------------------------------

    bool Input::isMouseButtonValid(MouseButton button) const
    {
        const uint8_t index = static_cast<uint8_t>(button);
        return index > 0 && index < kMaxMouseButtons;
    }

    //--------------------------------------------------------------------------

    bool Input::getKey(ScanCode scancode) const
    {
        if (!mEnabled || !isScanCodeValid(scancode))
        {
            return false;
        }
        return mScanKeys[scancode].held;
    }

    //--------------------------------------------------------------------------

    bool Input::getKeyDown(ScanCode scancode) const
    {
        if (!mEnabled || !isScanCodeValid(scancode))
        {
            return false;
        }
        return mScanKeys[scancode].down;
    }

    //--------------------------------------------------------------------------

    bool Input::getKeyUp(ScanCode scancode) const
    {
        if (!mEnabled || !isScanCodeValid(scancode))
        {
            return false;
        }
        return mScanKeys[scancode].up;
    }

    //--------------------------------------------------------------------------

    const Input::ButtonState *Input::findKeyState(KeyCode key) const
    {
        auto itr = mKeyCodes.find(static_cast<uint32_t>(key));
        if (itr != mKeyCodes.end())
        {
            return &itr->second;
        }

        // Caps Lock / Shift 时 SDL 可能给出 'W' 而不是 TKEY_w('w')
        const uint32_t code = static_cast<uint32_t>(key);
        if (code >= static_cast<uint32_t>('a') && code <= static_cast<uint32_t>('z'))
        {
            itr = mKeyCodes.find(code - ('a' - 'A'));
            if (itr != mKeyCodes.end())
            {
                return &itr->second;
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool Input::getKey(KeyCode key) const
    {
        if (!mEnabled)
        {
            return false;
        }
        const ButtonState *state = findKeyState(key);
        return state != nullptr && state->held;
    }

    //--------------------------------------------------------------------------

    bool Input::getKeyDown(KeyCode key) const
    {
        if (!mEnabled)
        {
            return false;
        }
        const ButtonState *state = findKeyState(key);
        return state != nullptr && state->down;
    }

    //--------------------------------------------------------------------------

    bool Input::getKeyUp(KeyCode key) const
    {
        if (!mEnabled)
        {
            return false;
        }
        const ButtonState *state = findKeyState(key);
        return state != nullptr && state->up;
    }

    //--------------------------------------------------------------------------

    bool Input::getMouseButton(MouseButton button) const
    {
        if (!mEnabled || !isMouseButtonValid(button))
        {
            return false;
        }
        return mMouseButtons[static_cast<uint8_t>(button)].held;
    }

    //--------------------------------------------------------------------------

    bool Input::getMouseButtonDown(MouseButton button) const
    {
        if (!mEnabled || !isMouseButtonValid(button))
        {
            return false;
        }
        return mMouseButtons[static_cast<uint8_t>(button)].down;
    }

    //--------------------------------------------------------------------------

    bool Input::getMouseButtonUp(MouseButton button) const
    {
        if (!mEnabled || !isMouseButtonValid(button))
        {
            return false;
        }
        return mMouseButtons[static_cast<uint8_t>(button)].up;
    }

    //--------------------------------------------------------------------------

    Vector2 Input::getMousePosition() const
    {
        return mMousePosition;
    }

    //--------------------------------------------------------------------------

    Vector2 Input::getMouseDelta() const
    {
        return mEnabled ? mMouseDelta : Vector2(0.0f, 0.0f);
    }

    //--------------------------------------------------------------------------

    Vector2 Input::getMouseScrollDelta() const
    {
        return mEnabled ? mMouseScroll : Vector2(0.0f, 0.0f);
    }

    //--------------------------------------------------------------------------

    uint32_t Input::getTouchCount() const
    {
        return mEnabled ? mTouchCount : 0;
    }

    //--------------------------------------------------------------------------

    Touch Input::getTouch(uint32_t index) const
    {
        if (!mEnabled || index >= mTouchCount)
        {
            return Touch();
        }
        return mTouches[index];
    }

    //--------------------------------------------------------------------------

    void Input::setCursorVisible(bool visible)
    {
        Window *window = getOSWindow();
        if (window != nullptr)
        {
            window->setCursorVisible(visible);
        }
    }

    //--------------------------------------------------------------------------

    void Input::setMouseCapture(bool enable)
    {
        Window *window = getOSWindow();
        if (window != nullptr)
        {
            window->setRelativeMouseMode(enable);
        }
    }

    //--------------------------------------------------------------------------

    void Input::warpMouse(int32_t x, int32_t y)
    {
        Window *window = getOSWindow();
        if (window != nullptr)
        {
            window->warpMouse(x, y);
        }
        mMousePosition = Vector2(static_cast<Real>(x), static_cast<Real>(y));
    }

    //--------------------------------------------------------------------------

    Window *Input::getOSWindow() const
    {
        if (Agent::getInstancePtr() == nullptr)
        {
            return nullptr;
        }

        RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
        if (rw == nullptr)
        {
            return nullptr;
        }
        return rw->getOSWindow();
    }

    //--------------------------------------------------------------------------

    void Input::getWindowSize(int32_t &width, int32_t &height) const
    {
        width = mWindowWidth;
        height = mWindowHeight;

        if ((width <= 0 || height <= 0) && Agent::getInstancePtr() != nullptr)
        {
            RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
            if (rw != nullptr)
            {
                width = rw->getDescriptor().Width;
                height = rw->getDescriptor().Height;
            }
        }

        if (width <= 0)
        {
            width = 1;
        }
        if (height <= 0)
        {
            height = 1;
        }
    }

    //--------------------------------------------------------------------------

    Vector2 Input::toPixel(float nx, float ny) const
    {
        int32_t width = 1;
        int32_t height = 1;
        getWindowSize(width, height);
        return Vector2(nx * static_cast<Real>(width), ny * static_cast<Real>(height));
    }

    //--------------------------------------------------------------------------

    void Input::processKey(const AppKeyboardEvent &evt)
    {
        mModifiers = evt.keysym.modifiers;

        const bool pressed = (evt.state == TKEY_PRESSED);
        const ScanCode scancode = evt.keysym.scancode;
        const uint32_t key = static_cast<uint32_t>(evt.keysym.sym);

        if (isScanCodeValid(scancode))
        {
            ButtonState &slot = mScanKeys[scancode];
            if (pressed)
            {
                if (!evt.repeat && !slot.held)
                {
                    slot.down = true;
                }
                slot.held = true;
                slot.up = false;
            }
            else
            {
                if (slot.held)
                {
                    slot.up = true;
                }
                slot.held = false;
                slot.down = false;
            }
        }

        ButtonState &keySlot = mKeyCodes[key];
        if (pressed)
        {
            if (!evt.repeat && !keySlot.held)
            {
                keySlot.down = true;
            }
            keySlot.held = true;
            keySlot.up = false;
        }
        else
        {
            if (keySlot.held)
            {
                keySlot.up = true;
            }
            keySlot.held = false;
            keySlot.down = false;
        }
    }

    //--------------------------------------------------------------------------

    void Input::processMouseMotion(const AppMouseMotionEvent &evt)
    {
        if (evt.which == T3D_APP_TOUCH_MOUSEID)
        {
            return;
        }

        mMousePosition = Vector2(static_cast<Real>(evt.x), static_cast<Real>(evt.y));
        mMouseDelta.x() += static_cast<Real>(evt.xrel);
        mMouseDelta.y() += static_cast<Real>(evt.yrel);
    }

    //--------------------------------------------------------------------------

    void Input::processMouseButton(const AppMouseButtonEvent &evt)
    {
        if (evt.which == T3D_APP_TOUCH_MOUSEID)
        {
            return;
        }

        mMousePosition = Vector2(static_cast<Real>(evt.x), static_cast<Real>(evt.y));

        const uint8_t index = evt.button;
        if (index == 0 || index >= kMaxMouseButtons)
        {
            return;
        }

        ButtonState &slot = mMouseButtons[index];
        const bool pressed = (evt.state == TKEY_PRESSED);
        if (pressed)
        {
            if (!slot.held)
            {
                slot.down = true;
            }
            slot.held = true;
            slot.up = false;
        }
        else
        {
            if (slot.held)
            {
                slot.up = true;
            }
            slot.held = false;
            slot.down = false;
        }
    }

    //--------------------------------------------------------------------------

    void Input::processMouseWheel(const AppMouseWheelEvent &evt)
    {
        if (evt.which == T3D_APP_TOUCH_MOUSEID)
        {
            return;
        }

        mMouseScroll.x() += static_cast<Real>(evt.x);
        mMouseScroll.y() += static_cast<Real>(evt.y);
    }

    //--------------------------------------------------------------------------

    int32_t Input::findTouch(int64_t fingerId) const
    {
        for (uint32_t i = 0; i < mTouchCount; ++i)
        {
            if (mTouches[i].fingerId == fingerId)
            {
                return static_cast<int32_t>(i);
            }
        }
        return -1;
    }

    //--------------------------------------------------------------------------

    int32_t Input::addTouch(int64_t fingerId)
    {
        if (mTouchCount >= kMaxTouches)
        {
            return -1;
        }

        const int32_t index = static_cast<int32_t>(mTouchCount);
        mTouches[index] = Touch();
        mTouches[index].fingerId = fingerId;
        ++mTouchCount;
        return index;
    }

    //--------------------------------------------------------------------------

    void Input::processTouch(const AppTouchFingerEvent &evt)
    {
        int32_t index = findTouch(evt.fingerID);
        const Vector2 pos = toPixel(evt.x, evt.y);
        const Vector2 delta = toPixel(evt.dx, evt.dy);

        if (evt.type == APP_FINGERDOWN)
        {
            if (index < 0)
            {
                index = addTouch(evt.fingerID);
            }
            if (index < 0)
            {
                return;
            }

            Touch &touch = mTouches[index];
            touch.position = pos;
            touch.delta = Vector2(0.0f, 0.0f);
            touch.pressure = evt.pressure;
            touch.phase = TouchPhase::Began;
        }
        else if (evt.type == APP_FINGERMOTION)
        {
            if (index < 0)
            {
                index = addTouch(evt.fingerID);
            }
            if (index < 0)
            {
                return;
            }

            Touch &touch = mTouches[index];
            touch.position = pos;
            touch.delta.x() += delta.x();
            touch.delta.y() += delta.y();
            touch.pressure = evt.pressure;
            if (touch.phase != TouchPhase::Began)
            {
                touch.phase = TouchPhase::Moved;
            }
        }
        else if (evt.type == APP_FINGERUP)
        {
            if (index < 0)
            {
                return;
            }

            Touch &touch = mTouches[index];
            touch.position = pos;
            touch.delta.x() += delta.x();
            touch.delta.y() += delta.y();
            touch.pressure = evt.pressure;
            touch.phase = TouchPhase::Ended;
        }
    }

    //--------------------------------------------------------------------------

    void Input::processWindow(const AppWindowEvent &evt)
    {
        switch (evt.event)
        {
        case APP_WINDOWEVENT_RESIZED:
        case APP_WINDOWEVENT_SIZE_CHANGED:
            mWindowWidth = evt.data1;
            mWindowHeight = evt.data2;
            break;
        case APP_WINDOWEVENT_FOCUS_LOST:
            reset();
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    T3D_INSTANTIATE_SINGLETON(Input)
}
