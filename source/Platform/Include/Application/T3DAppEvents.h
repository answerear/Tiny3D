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


#ifndef __T3D_APP_EVENTS_H__
#define __T3D_APP_EVENTS_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DAppKeyboard.h"


namespace Tiny3D
{
    enum AppEventType : uint32_t
    {
        APP_FIRSTEVENT = 0,

        /// App 退出
        APP_QUIT = 0x100,

        /// App 被 OS 终止事件。
        /// iOS 在 applicationWillTerminate() 中调用
        /// Android 在 OnDestroy() 中调用
        APP_TERMINATING,
        
        /// 低电量事件。
        /// iOS 在 applicationDidReceiveMemoryWarning() 中调用
        /// Android 在 onLowMemory() 中调用
        APP_LOWMEMROY,

        /// App 进入后台事件
        /// iOS 在 applicationDidEnterBackground() 中调用
        /// Android 在 onPause() 中调用
        APP_WILLENTERBACKGROUND,

        /// App 回到前台事件
        /// iOS 在 applicationWillEnterForeground() 中调用
        /// Android 在 onResume() 中调用
        APP_DIDENTERBACKGROUND,

        /// 窗口事件
        APP_WINDOWEVENT = 0x200,

        /// OS 相关的事件，平台相关
        APP_SYSWMEVENT,

        /// 键盘事件

        /// 按键按下
        APP_KEYDOWN = 0x300,
        /// 按钮松开
        APP_KEYUP,
        /// 编辑框编辑
        APP_TEXTEDITING,
        /// 编辑框输入
        APP_TEXTINPUT,
        /// 按键映射变化，主要取决于系统事件，例如输入语言、按键布局等变化
        APP_KEYMAPCHANGED,

        /// 鼠标事件

        /// 鼠标移动
        APP_MOUSEMOTION = 0x400,
        /// 鼠标键按下
        APP_MOUSEBUTTONDOWN,
        /// 鼠标键松开
        APP_MOUSEBUTTONUP,
        /// 鼠标滚轮
        APP_MOUSEWHEEL,

        /// 摇杆事件

        /// Joystick axis 移动
        APP_JOYAXISMOTION = 0x600,
        /// Joystick trackball 移动
        APP_JOYBALLMOTION,
        /// Joystick hat 位置变化
        APP_JOYHATMOTION,
        /// Joystick 按钮按下
        APP_JOYBUTTONDOWN,
        /// Joystick 按钮松开
        APP_JOYBUTTONUP,
        /// 一个新的游戏手柄插入
        APP_JOYDEVICEADDED,
        /// 游戏手柄拔除
        APP_JOYDEVICEREMOVED,

        /// 游戏手柄事件

        /// Game controller axis 移动
        APP_CONTROLLERAXISMOTION = 0x650,
        /// Game controller 按钮按下
        APP_COTNROLLERRBUTTONDOWN,
        /// Game controller 按钮松开
        APP_CONTROLLERRBUTTONUP,
        /// 一个新的 Game controller 插入
        APP_CONTROLLERDEVICEADDED,
        /// Game controller 拔除
        APP_CONTROLLERDEVICEREMOVED,
        /// Game controller 映射更新
        APP_CONTROLLERDEVICEREMAPPED,

        /// 触摸事件
        APP_FINGERDOWN = 0x700,
        APP_FINGERUP,
        APP_FINGERMOTION,

        /// 手势事件
        APP_DOLLARGESTURE = 0x800,
        APP_DOLLARRECORD,
        APP_MULTIGESTURE,

        /// 粘贴板事件
        APP_CLIPBOARDUPDATE = 0x900,

        /// 拖动事件

        /// 系统请求打开文件
        APP_DROPFILE = 0x1000,
        /// 文本 drag-and-drop 事件
        APP_DROPTEXT,
        /// 开始一个新的拖动
        APP_DROPBEGIN,
        /// 完成当前的拖动
        APP_DROPCOMPLETED,

        /// 新音频设备加入系统
        APP_AUDIODEVICEADDED = 0x1100,
        APP_AUDIODEVICEREMOVED,
    };

    struct AppCommonEvent
    {
        uint32_t type {APP_FIRSTEVENT};
        uint32_t timestamp {0};
    };

    enum AppWindowEventID
    {
        APP_WINDOWEVENT_NONE,           /**< Never used */
        APP_WINDOWEVENT_SHOWN,          /**< Window has been shown */
        APP_WINDOWEVENT_HIDDEN,         /**< Window has been hidden */
        APP_WINDOWEVENT_EXPOSED,        /**< Window has been exposed and should be
                                             redrawn */
        APP_WINDOWEVENT_MOVED,          /**< Window has been moved to data1, data2
                                         */
        APP_WINDOWEVENT_RESIZED,        /**< Window has been resized to data1xdata2 */
        APP_WINDOWEVENT_SIZE_CHANGED,   /**< The window size has changed, either as
                                             a result of an API call or through the
                                             system or user changing the window size. */
        APP_WINDOWEVENT_MINIMIZED,      /**< Window has been minimized */
        APP_WINDOWEVENT_MAXIMIZED,      /**< Window has been maximized */
        APP_WINDOWEVENT_RESTORED,       /**< Window has been restored to normal size
                                             and position */
        APP_WINDOWEVENT_ENTER,          /**< Window has gained mouse focus */
        APP_WINDOWEVENT_LEAVE,          /**< Window has lost mouse focus */
        APP_WINDOWEVENT_FOCUS_GAINED,   /**< Window has gained keyboard focus */
        APP_WINDOWEVENT_FOCUS_LOST,     /**< Window has lost keyboard focus */
        APP_WINDOWEVENT_CLOSE,          /**< The window manager requests that the window be closed */
        APP_WINDOWEVENT_TAKE_FOCUS,     /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
        APP_WINDOWEVENT_HIT_TEST        /**< Window had a hit test that wasn't SDL_HITTEST_NORMAL. */
    };
    
    /**
     * 窗口状态变化事件数据
     */
    struct AppWindowEvent
    {
        /// 事件类型，这里是 APP_WINDOWEVENT
        AppEventType type {APP_FIRSTEVENT};
        /// 事件触发的时间戳
        uint32_t timestamp {0};
        /// 事件触发的窗口
        uint32_t windowID {0};
        /// 窗口事件 ID
        uint8_t event {0};
        /// 用于字节对齐
        uint8_t padding1 {0};
        uint8_t padding2 {0};
        uint8_t padding3 {0};
        /// 窗口事件 ID 对应的数据
        int32_t data1 {0};
        /// 窗口事件 ID 对应的数据
        int32_t data2 {0};
    };

    /**
     * keysym，在按键事件里面使用
     */
    struct AppKeysym
    {
        /// 物理按键码
        ScanCode scancode {APP_SCANCODE_UNKNOWN};
        /// 虚拟按键码
        KeyCode sym {TKEY_UNKNOWN};
        /// 按键修饰键，如 Shift、Ctrl、Alt 等
        uint16_t modifiers {0};
        /// 没使用
        uint32_t unused {0};
    };

    #define TKEY_RELEASED   0
    #define TKEY_PRESSED    1

    /**
     * 键盘按钮事件数据
     */
    struct AppKeyboardEvent
    {
        /// 事件类型，这里是 APP_KEYDOWN 或者 APP_KEYUP
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触发事件的窗口 ID
        uint32_t windowID {0};
        /// 按键状态，这里是 TKEY_PRESSED 或者 TKEY_RELEASED
        uint8_t state {0};
        /// 是否重复
        bool repeat {false};
        /// 用于字节对齐
        uint8_t padding2 {0};
        uint8_t padding3 {0};
        /// 具体的按钮信息
        AppKeysym keysym {};
    };

    #define T3D_APP_TEXTEDITINGEVENT_TEXT_SIZE (32)
    
    /**
     * 键盘文本编辑中事件数据
     */
    struct AppTextEditingEvent
    {
        /// 事件类型，这里是 APP_TEXTEDITING
        AppEventType type {APP_TEXTEDITING};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触发事件的窗口 ID
        uint32_t windowID {0};
        /// 编辑的文本
        char text[T3D_APP_TEXTEDITINGEVENT_TEXT_SIZE] {0};
        /// 选中文本的光标的开始位置
        int32_t start {0};
        /// 选中文本的长度
        int32_t length {0};
    };

    #define T3D_APP_TEXTINPUTEVENT_TEXT_SIZE (32)
    
    /**
     * 键盘文本输入事件数据
     */
    struct AppTextInputEvent
    {
        /// 事件类型，这里是 APP_TEXTINPUT
        AppEventType type {APP_TEXTINPUT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触发事件的窗口 ID
        uint32_t windowID {0};
        /// 输入的文本
        char text[T3D_APP_TEXTINPUTEVENT_TEXT_SIZE] {0};
    };

    /**
     * 鼠标移动事件数据
     */
    struct AppMouseMotionEvent
    {
        /// 事件类型，这里是 APP_MOUSEMOTION
        AppEventType type {APP_MOUSEMOTION};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触发事件的窗口 ID
        uint32_t windowID {0};
        /// 鼠标实例 ID，或者是 T3D_APP_TOUCH_MOUSEID
        uint32_t which {0};
        /// 状态
        uint32_t state {0};
        /// X 坐标，相对窗口的
        int32_t x {0};
        /// Y 坐标，相对窗口的
        int32_t y {0};
        /// X 方向上的相对位移
        int32_t xrel {0};
        /// Y 方向上的相对位置
        int32_t yrel {0};
    };

    #define T3D_APP_TOUCH_MOUSEID   ((uint32_t)-1)

    /**
     * 鼠标点击事件数据
     */
    struct AppMouseButtonEvent
    {
        /// 事件类型，这里是 APP_MOUSEEBUTTONDOWN or APP_MOUSEBUTTONUP
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触发事件的窗口 ID
        uint32_t windowID {0};
        /// 鼠标实例 ID，或者是 T3D_APP_TOUCH_MOUSEID
        uint32_t which {0};
        /// 状态，TKEY_PRESSED 或者 TKEY_RELEASED
        uint32_t state {0};
        /// 点击次数，1 为单机，2为双击
        uint8_t clicks {0};
        /// 用于字节对齐
        uint8_t padding1 {0};
        /// X 坐标，相对于窗口坐标
        int32_t x {0};
        /// Y 坐标，相对于窗口坐标
        int32_t y {0};
    };

    /**
     * 鼠标滚轮事件数据
     */
    struct AppMouseWheelEvent
    {
        /// 事件类型，这里是 APP_MOUSEWHEEL
        AppEventType type {APP_MOUSEWHEEL};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触发事件的窗口 ID
        uint32_t windowID {0};
        /// 鼠标实例 ID，或者是 T3D_APP_TOUCH_MOUSEID
        uint32_t which {0};
        /// 水平方向偏移总量。 右是正，左是负
        int32_t x {0};
        /// 垂直方向偏移总量。 远离用户是正，靠近用户是负
        int32_t y {0};
    };

    using AppJoystickID = int32_t;

    /**
     * 摇杆移动事件数据
     */
    struct AppJoyAxisEvent
    {
        /// 事件类型，这里是 APP_JOYAXISMOTION
        AppEventType type {APP_JOYAXISMOTION};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 摇杆实例 ID
        AppJoystickID which {0};
        /// 摇杆轴索引
        uint8_t axis {0};
        /// 用于字节对齐
        uint8_t padding1 {0};
        uint8_t padding2 {0};
        uint8_t padding3 {0};
        /// 轴上的值，介于 -32768 到 32767 之间
        int16_t value {0};
        /// 用于字节对齐
        uint16_t padding4 {0};
    };

    /**
     * 摇杆轨迹球移动事件数据
     */
    struct AppJoyBallEvent
    {
        /// 事件类型，这里是 APP_JOYBALLMOTION
        AppEventType type {APP_JOYBALLMOTION};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 摇杆实例 ID
        AppJoystickID which {0};
        /// 摇杆轨迹球索引
        uint8_t ball {0};
        /// 用于字节对齐
        uint8_t padding1 {0};
        uint8_t padding2 {0};
        uint8_t padding3 {0};
        /// X 方向上的相对偏移
        int16_t xrel {0};
        /// Y 方向上的相对偏移
        int16_t yrel {0};
    };

    #define T3D_APP_HAT_CENTERED    0x00
    #define T3D_APP_HAT_UP          0x01
    #define T3D_APP_HAT_RIGHT       0x02
    #define T3D_APP_HAT_DOWN        0x04
    #define T3D_APP_HAT_LEFT        0x08
    #define T3D_APP_HAT_RIGHTUP     (T3D_APP_HAT_RIGHT|T3D_APP_HAT_UP)
    #define T3D_APP_HAT_RIGHTDOWN   (T3D_APP_HAT_RIGHT|T3D_APP_HAT_DOWN)
    #define T3D_APP_HAT_LEFTUP      (T3D_APP_HAT_LEFT|T3D_APP_HAT_UP)
    #define T3D_APP_HAT_LEFTDOWN    (T3D_APP_HAT_LEFT|T3D_APP_HAT_DOWN)
    
    /**
     * 摇杆 hat 位置变化事件数据
     */
    struct AppJoyHatEvent
    {
        /// 事件类型，这里是 APP_JOYHATMOTION
        AppEventType type {APP_JOYHATMOTION};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 摇杆实例 ID
        AppJoystickID which {0};
        /// joystick hat 索引
        uint8_t hat {0};
        /// hat 位置值，一般是 T3D_HAT_LEFTUP T3D_HAT_UP T3D_HAT_RIGHTUP 等值
        uint8_t value {0};
        /// 用于字节对齐
        uint8_t padding1 {0};
        uint8_t padding2 {0};
    };

    /**
     * 摇杆按钮事件数据
     */
    struct AppJoyButtonEvent
    {
        /// 事件类型，这里是 APP_JOYBUTTONDOWN or APP_JOYBUTTONUP
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 摇杆实例 ID
        AppJoystickID which {0};
        /// 按钮索引
        uint8_t button {0};
        /// 按钮状态，TKEY_PRESSED or TKEY_RELEASED
        uint8_t state {0};
        /// 用于字节对齐
        uint8_t padding1 {0};
        uint8_t padding2 {0};
    };

    /**
     * 摇杆设备事件数据
     */
    struct AppJoyDeviceEvent
    {
        /// 事件类型，这里是 APP_JOYDEVICEADDED or APP_JOYDEVICEREMOVED
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 摇杆实例 ID
        AppJoystickID which {0};
    };

    enum AppGameControllerAxis : uint8_t
    {
        APP_CONTROLLER_AXIS_INVALID = 0xFF,
        APP_CONTROLLER_AXIS_LEFTX = 0,
        APP_CONTROLLER_AXIS_LEFTY,
        APP_CONTROLLER_AXIS_RIGHTX,
        APP_CONTROLLER_AXIS_RIGHTY,
        APP_CONTROLLER_AXIS_TRIGGERLEFT,
        APP_CONTROLLER_AXIS_TRIGGERRIGHT,
        APP_CONTROLLER_AXIS_MAX
    };
    
    /**
     * 游戏手柄移动事件数据
     */
    struct AppControllerAxisEvent
    {
        /// 事件类型，这里是 APP_CONTROLLERAXISMOTION
        AppEventType type {APP_CONTROLLERAXISMOTION};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 手柄实例 ID
        AppJoystickID which {0};
        /// 手柄移动轴索引
        uint8_t axis {APP_CONTROLLER_AXIS_INVALID};
        /// 用于字节对齐
        uint8_t padding1 {0};
        uint8_t padding2 {0};
        uint8_t padding3 {0};
        /// 轴上的值，介于 -32768 到 32767 之间
        int16_t value {0};
        /// 用于字节对齐
        uint16_t padding4 {0};
    };

    enum AppGameControllerButton
    {
        APP_CONTROLLER_BUTTON_INVALID = -1,
        APP_CONTROLLER_BUTTON_A,
        APP_CONTROLLER_BUTTON_B,
        APP_CONTROLLER_BUTTON_X,
        APP_CONTROLLER_BUTTON_Y,
        APP_CONTROLLER_BUTTON_BACK,
        APP_CONTROLLER_BUTTON_GUIDE,
        APP_CONTROLLER_BUTTON_START,
        APP_CONTROLLER_BUTTON_LEFTSTICK,
        APP_CONTROLLER_BUTTON_RIGHTSTICK,
        APP_CONTROLLER_BUTTON_LEFTSHOULDER,
        APP_CONTROLLER_BUTTON_RIGHTSHOULDER,
        APP_CONTROLLER_BUTTON_DPAD_UP,
        APP_CONTROLLER_BUTTON_DPAD_DOWN,
        APP_CONTROLLER_BUTTON_DPAD_LEFT,
        APP_CONTROLLER_BUTTON_DPAD_RIGHT,
        APP_CONTROLLER_BUTTON_MAX
    };

    /**
     * 游戏手柄按键事件数据
     */
    struct AppControllerButtonEvent
    {
        /// 事件类型，这里是 APP_CONTROLLERRBUTTONDOWN or APP_CONTROLLERRBUTTONUP
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 手柄实例 ID
        AppJoystickID which {0};
        /// 手柄按键
        uint8_t button {0};
        /// 按键状态，TKEY_PRESSED or TKEY_RELEASED
        uint8_t state {0};
        /// 用于字节对齐
        uint8_t padding1 {0};
        uint8_t padding2 {0};
    };

    /**
     * 游戏手柄设备事件数据
     */
    struct AppControllerDeviceEvent
    {
        /// 事件类型，这里是 APP_CONTROLLERDEVICEADDED or APP_CONTROLLERDEVICEREMOVED or APP_CONTROLLERDEVICEREMAPPED
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 音频设备索引
        uint32_t which {0};
        /// false 是输出设备，true 是捕捉设备
        bool iscapture {false};
        /// 用于字节对齐
        uint8_t padding1 {0};
        uint8_t padding2 {0};
        uint8_t padding3 {0};
    };

    struct AppAudioDeviceEvent
    {
        /// 事件类型，这里是 APP_AUDIODEVICEADDED or APP_AUDIODEVICEREMOVED
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 手柄实例 ID
        AppJoystickID which {0};
    };

    using AppTouchID = int64_t;
    using AppFingerID = int64_t;

    /**
     * 触摸事件数据
     */
    struct AppTouchFingerEvent
    {
        /// 事件类型，这里是 APP_FINGERMOTION or APP_FINGERDOWN or APP_FINGERUP
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触摸设备 ID
        AppTouchID touchID {0};
        /// 手指 ID
        AppFingerID fingerID {0};
        /// X 轴，规范化在 [0,1]
        float x {0.0f};
        /// Y 轴，规范化在 [0,1]
        float y {0.0f};
        /// X轴变化值，规范化在 [-1,1]
        float dx {0.0f};
        /// Y轴变化值，规范化在 [-1,1]
        float dy {0.0f};
        float pressure {0.0f};
    };

    using AppGestureID = int64_t;
    
    /**
     * 多手指手势事件数据
     */
    struct AppMultiGestureEvent
    {
        /// 事件类型，这里是 APP_MULTIGESTURE
        AppEventType type {APP_MULTIGESTURE};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触摸设备 ID
        AppTouchID touchID {0};
        float dTheta {0};
        float dDist {0};
        float x {0};
        float y {0};
        /// 手指数量
        uint16_t numFingers {0};
        /// 用于字节对齐
        uint16_t padding {0};
    };

    /**
     * 美元手势事件数据
     */
    struct AppDollarGestureEvent
    {
        /// 事件类型，这里是 APP_MULTIGESTURE
        AppEventType type {APP_MULTIGESTURE};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 触摸设备 ID
        AppTouchID touchID {0};
        /// 手势 ID
        AppGestureID gestureID {0};
        /// 手指数量
        uint32_t numFingers {0};
        float error {0};
        /// 规范化的手势中点 X 位置
        float x {0};
        /// 规范化的手势中点 Y 位置
        float y {0};
    };

    /**
     * 拖动事件数据
     */
    struct AppDropEvent
    {
        /// 事件类型，这里是 APP_DROPBEGIN or APP_DROPTEXT or APP_DROPCOMPLETE
        AppEventType type {APP_FIRSTEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// 文件名，需要自己释放，当时 APP_DROPBEGIN or APP_DROPCOMPLETE 的时候是 nullptr
        char *file {nullptr};
        /// 被拖进去的窗口 ID
        uint32_t windowID {0};
    };

    /**
     * 退出 App 事件数据
     */
    struct AppQuitEvent
    {
        /// 事件类型，这里是 APP_QUIT
        AppEventType type {APP_QUIT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
    };

    /**
     * OS 对应的事件数据，平台相关 
     */
    struct AppOSEvent
    {
        /// 事件类型，这里是 APP_QUIT
        AppEventType type {APP_QUIT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
    };

    struct AppSysWMMsg;
    using AppSysWMMsg = AppSysWMMsg;

    struct AppSysWMEvent
    {
        /// 事件类型，这里是 APP_QUIT
        AppEventType type {APP_SYSWMEVENT};
        /// 触发事件的时间戳
        uint32_t timestamp {0};
        /// OS 对应的事件数据，依赖 OS 
        AppSysWMMsg *msg {nullptr};
    };

    /**
     * 通用事件数据
     */
    union AppEvent
    {
        AppEventType type {APP_FIRSTEVENT};
        AppCommonEvent common;
        AppWindowEvent window;
        AppKeyboardEvent key;
        AppTextEditingEvent edit;
        AppTextInputEvent text;
        AppMouseMotionEvent motion;
        AppMouseButtonEvent button;
        AppMouseWheelEvent wheel;
        AppJoyAxisEvent jaxis;
        AppJoyBallEvent jball;
        AppJoyHatEvent jhat;
        AppJoyButtonEvent jbutton;
        AppJoyDeviceEvent jdevice;
        AppControllerAxisEvent caxis;
        AppControllerButtonEvent cbutton;
        AppControllerDeviceEvent cdevice;
        AppAudioDeviceEvent adevice;
        AppQuitEvent quit;
        AppSysWMEvent syswm;
        AppTouchFingerEvent tfinger;
        AppMultiGestureEvent mgesture;
        AppDollarGestureEvent dgesture;
        AppDropEvent drop;

        /// 为兼容不同编译器，自行字节对齐，强制到 56 字节
        uint8_t padding[56];
    };
}



#endif  /*__T3D_APP_EVENTS_H__*/
