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

#ifndef __T3D_INPUT_H__
#define __T3D_INPUT_H__


#include "T3DPrerequisites.h"
#include "T3DSingleton.h"
#include "Application/T3DApplication.h"


namespace Tiny3D
{
    /**
     * \brief 鼠标按键，数值与 SDL_BUTTON_* 对齐
     */
    enum class MouseButton : uint8_t
    {
        Left = 1,
        Middle = 2,
        Right = 3,
        X1 = 4,
        X2 = 5
    };

    /**
     * \brief 触摸阶段
     */
    enum class TouchPhase : uint8_t
    {
        Began = 0,
        Moved,
        Stationary,
        Ended,
        Canceled
    };

    /**
     * \brief 单指触摸状态（窗口像素坐标）
     */
    struct Touch
    {
        int64_t     fingerId {0};
        Vector2     position {0.0f, 0.0f};
        Vector2     delta {0.0f, 0.0f};
        Real        pressure {0.0f};
        TouchPhase  phase {TouchPhase::Canceled};
    };

    /**
     * \brief 设备级输入轮询单例（对齐 Unity Input / Godot Input）
     * \remarks 由 Agent 创建并注册为 IAppEventListener；脚本只读查询。
     *          setEnabled(false) 只影响查询结果，内部状态仍随 OS 事件更新。
     */
    class T3D_ENGINE_API Input : public Singleton<Input>, public IAppEventListener
    {
        friend class Agent;

    public:
        static const uint32_t kMaxTouches = 16;
        static const uint32_t kMaxMouseButtons = 6;

        Input();
        ~Input() override;

        void onAppEvent(const AppEvent &event) override;

        /// 消费一条 OS 事件，更新本帧设备状态
        void processEvent(const AppEvent &event);

        /// 帧末清除 Down/Up 边沿、鼠标 delta/滚轮，并移除已结束的触摸
        void endFrame();

        /// 清空所有按下状态（失焦 / 进后台）
        void reset();

        void setEnabled(bool enabled) { mEnabled = enabled; }
        bool isEnabled() const { return mEnabled; }

        bool getKey(KeyCode key) const;
        bool getKeyDown(KeyCode key) const;
        bool getKeyUp(KeyCode key) const;

        bool getKey(ScanCode scancode) const;
        bool getKeyDown(ScanCode scancode) const;
        bool getKeyUp(ScanCode scancode) const;

        uint16_t getModifiers() const { return mEnabled ? mModifiers : 0; }

        bool getMouseButton(MouseButton button) const;
        bool getMouseButtonDown(MouseButton button) const;
        bool getMouseButtonUp(MouseButton button) const;

        Vector2 getMousePosition() const;
        Vector2 getMouseDelta() const;
        Vector2 getMouseScrollDelta() const;

        uint32_t getTouchCount() const;
        Touch getTouch(uint32_t index) const;

        void setCursorVisible(bool visible);
        void setMouseCapture(bool enable);
        void warpMouse(int32_t x, int32_t y);

    private:
        struct ButtonState
        {
            bool held {false};
            bool down {false};
            bool up {false};
        };

        bool isScanCodeValid(ScanCode scancode) const;
        bool isMouseButtonValid(MouseButton button) const;
        const ButtonState *findKeyState(KeyCode key) const;
        void getWindowSize(int32_t &width, int32_t &height) const;
        Window *getOSWindow() const;
        Vector2 toPixel(float nx, float ny) const;

        void processKey(const AppKeyboardEvent &evt);
        void processMouseMotion(const AppMouseMotionEvent &evt);
        void processMouseButton(const AppMouseButtonEvent &evt);
        void processMouseWheel(const AppMouseWheelEvent &evt);
        void processTouch(const AppTouchFingerEvent &evt);
        void processWindow(const AppWindowEvent &evt);

        int32_t findTouch(int64_t fingerId) const;
        int32_t addTouch(int64_t fingerId);

    private:
        bool        mEnabled {true};
        uint16_t    mModifiers {0};
        int32_t     mWindowWidth {0};
        int32_t     mWindowHeight {0};

        ButtonState mScanKeys[APP_NUM_SCANCODES] {};
        TUnorderedMap<uint32_t, ButtonState> mKeyCodes {};

        ButtonState mMouseButtons[kMaxMouseButtons] {};
        Vector2     mMousePosition {0.0f, 0.0f};
        Vector2     mMouseDelta {0.0f, 0.0f};
        Vector2     mMouseScroll {0.0f, 0.0f};

        Touch       mTouches[kMaxTouches] {};
        uint32_t    mTouchCount {0};
    };

    #define T3D_INPUT   (Input::getInstance())

    T3D_EXTERN_SINGLETON(Input)
}


#endif  /*__T3D_INPUT_H__*/
