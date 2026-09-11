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

#ifndef __POST_PROCESS_CONTROLLER_BEHAVIOUR_H__
#define __POST_PROCESS_CONTROLLER_BEHAVIOUR_H__


#include "Component/T3DBehaviour.h"
#include "PostProcessCommand.h"


namespace Tiny3D
{
    class CopyEffectBehaviour;
    class GrayscaleEffectBehaviour;
    class InvertEffectBehaviour;
    class TintEffectBehaviour;
    class LogCameraBehaviour;

    /**
     * \brief 后处理预设控制器：从命令源（键盘 / 触摸 / 自动轮播）切预设
     */
    TCLASS()
    class PostProcessControllerBehaviour : public Behaviour
    {
        TRTTI_ENABLE(Behaviour)
        TRTTI_FRIEND

    public:
        ~PostProcessControllerBehaviour() override = default;

        void applyPreset(int32_t preset);
        void addCommandSource(IPresetCommandSource *src);

    protected:
        PostProcessControllerBehaviour() = default;
        explicit PostProcessControllerBehaviour(const UUID &uuid);

        void onAwake() override;
        void onUpdate() override;

    private:
        void dispatch(const PresetCommand &cmd);
        void swapEffectOrder();
        void logPreset() const;
        static const char *presetName(int32_t preset);

    private:
        CopyEffectBehaviour        *mCopy {nullptr};
        GrayscaleEffectBehaviour   *mGray {nullptr};
        InvertEffectBehaviour      *mInvert {nullptr};
        TintEffectBehaviour        *mTint {nullptr};
        LogCameraBehaviour         *mLog {nullptr};
        int32_t                     mPreset {0};
        TArray<IPresetCommandSource*> mSources;
    };
}


#endif  /*__POST_PROCESS_CONTROLLER_BEHAVIOUR_H__*/
