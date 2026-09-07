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

#ifndef __LOG_CAMERA_BEHAVIOUR_H__
#define __LOG_CAMERA_BEHAVIOUR_H__


#include "Behaviour/T3DCameraBehaviour.h"


namespace Tiny3D
{
    /**
     * \brief 相机时序日志：默认只打前 3 帧，L 键切换为逐帧输出
     */
    TCLASS()
    class LogCameraBehaviour : public CameraBehaviour
    {
        TRTTI_ENABLE(CameraBehaviour)
        TRTTI_FRIEND

    public:
        ~LogCameraBehaviour() override = default;

        /// 当前帧是否应输出时序日志（启动前 3 帧，或已打开逐帧开关）
        bool shouldLog() const;

        /// 打开 / 关闭逐帧日志
        void setFrameLogEnabled(bool enabled);

        bool isFrameLogEnabled() const { return mFrameLogEnabled; }

        void toggleFrameLog();

    protected:
        LogCameraBehaviour() = default;
        explicit LogCameraBehaviour(const UUID &uuid);

        void onPreRender(RHIContext *ctx) override;
        void onPostRender(RHIContext *ctx) override;

    private:
        static const int32_t kStartupLogFrames = 3;

        int32_t mLoggedFrames {0};
        bool    mFrameLogEnabled {false};
    };
}


#endif  /*__LOG_CAMERA_BEHAVIOUR_H__*/
