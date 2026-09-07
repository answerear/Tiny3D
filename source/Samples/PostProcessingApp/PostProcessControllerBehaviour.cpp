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

#include "PostProcessControllerBehaviour.h"
#include "InvertEffectBehaviour.h"
#include "TintEffectBehaviour.h"
#include "LogCameraBehaviour.h"

#include "CopyEffectBehaviour.h"
#include "GrayscaleEffectBehaviour.h"
#include "Input/T3DInput.h"


#define LOG_TAG_APP     "APP"


namespace Tiny3D
{
    PostProcessControllerBehaviour::PostProcessControllerBehaviour(const UUID &uuid)
        : Behaviour(uuid)
    {
    }

    void PostProcessControllerBehaviour::onAwake()
    {
        GameObject *go = getGameObject();
        if (go != nullptr)
        {
            mCopy = go->getComponent<CopyEffectBehaviour>().get();
            mGray = go->getComponent<GrayscaleEffectBehaviour>().get();
            mInvert = go->getComponent<InvertEffectBehaviour>().get();
            mTint = go->getComponent<TintEffectBehaviour>().get();
            mLog = go->getComponent<LogCameraBehaviour>().get();
        }

        applyPreset(0);
    }

    void PostProcessControllerBehaviour::onUpdate()
    {
        if (Input::getInstancePtr() == nullptr)
        {
            return;
        }

        if (T3D_INPUT.getKeyDown(APP_SCANCODE_0))
        {
            applyPreset(0);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_1))
        {
            applyPreset(1);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_2))
        {
            applyPreset(2);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_3))
        {
            applyPreset(3);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_4))
        {
            applyPreset(4);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_5))
        {
            applyPreset(5);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_6))
        {
            applyPreset(6);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_7))
        {
            applyPreset(7);
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_O))
        {
            swapEffectOrder();
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_L))
        {
            if (mLog != nullptr)
            {
                mLog->toggleFrameLog();
            }
        }
    }

    void PostProcessControllerBehaviour::applyPreset(int32_t preset)
    {
        if (preset < 0 || preset > 7)
        {
            return;
        }

        if (mCopy != nullptr)
        {
            mCopy->setEnabled(false);
            mCopy->setEffectOrder(0);
        }
        if (mGray != nullptr)
        {
            mGray->setEnabled(false);
            mGray->setEffectOrder(0);
        }
        if (mInvert != nullptr)
        {
            mInvert->setEnabled(false);
            mInvert->setEffectOrder(0);
        }
        if (mTint != nullptr)
        {
            mTint->setEnabled(false);
            mTint->setEffectOrder(0);
        }

        switch (preset)
        {
        case 1:
            if (mCopy != nullptr)
            {
                mCopy->setEnabled(true);
                mCopy->setEffectOrder(0);
            }
            break;
        case 2:
            if (mGray != nullptr)
            {
                mGray->setEnabled(true);
                mGray->setEffectOrder(0);
            }
            break;
        case 3:
            if (mInvert != nullptr)
            {
                mInvert->setEnabled(true);
                mInvert->setEffectOrder(0);
            }
            break;
        case 4:
            if (mGray != nullptr)
            {
                mGray->setEnabled(true);
                mGray->setEffectOrder(0);
            }
            if (mInvert != nullptr)
            {
                mInvert->setEnabled(true);
                mInvert->setEffectOrder(1);
            }
            break;
        case 5:
            if (mInvert != nullptr)
            {
                mInvert->setEnabled(true);
                mInvert->setEffectOrder(0);
            }
            if (mGray != nullptr)
            {
                mGray->setEnabled(true);
                mGray->setEffectOrder(1);
            }
            break;
        case 6:
            if (mGray != nullptr)
            {
                mGray->setEnabled(false);
                mGray->setEffectOrder(0);
            }
            break;
        case 7:
            if (mTint != nullptr)
            {
                mTint->setEnabled(true);
                mTint->setEffectOrder(0);
            }
            break;
        default:
            break;
        }

        mPreset = preset;
        logPreset();

        GameObject *go = getGameObject();
        if (go != nullptr)
        {
            uint32_t chainCount = 0;
            for (const auto &item : go->getAllComponents())
            {
                if (item.second == nullptr)
                {
                    continue;
                }
                Behaviour *behaviour = item.second->asBehaviour();
                if (behaviour != nullptr && behaviour->asCameraEffectBehaviour() != nullptr
                    && behaviour->isActiveAndEnabled())
                {
                    ++chainCount;
                }
            }
            T3D_LOG_INFO(LOG_TAG_APP, "[PostProcess] active effects in chain: %u", chainCount);
        }
    }

    void PostProcessControllerBehaviour::swapEffectOrder()
    {
        if (mGray == nullptr || mInvert == nullptr)
        {
            return;
        }

        const int32_t grayOrder = mGray->getEffectOrder();
        const int32_t invertOrder = mInvert->getEffectOrder();
        mGray->setEffectOrder(invertOrder);
        mInvert->setEffectOrder(grayOrder);

        if (mPreset == 4)
        {
            mPreset = 5;
        }
        else if (mPreset == 5)
        {
            mPreset = 4;
        }

        T3D_LOG_INFO(LOG_TAG_APP, "[PostProcess] swap order Gray:%d Invert:%d",
            mGray->getEffectOrder(), mInvert->getEffectOrder());
        logPreset();
    }

    void PostProcessControllerBehaviour::logPreset() const
    {
        const int32_t copyEn = (mCopy != nullptr && mCopy->isEnabled()) ? 1 : 0;
        const int32_t grayEn = (mGray != nullptr && mGray->isEnabled()) ? 1 : 0;
        const int32_t invertEn = (mInvert != nullptr && mInvert->isEnabled()) ? 1 : 0;
        const int32_t tintEn = (mTint != nullptr && mTint->isEnabled()) ? 1 : 0;
        const int32_t grayOrder = (mGray != nullptr) ? mGray->getEffectOrder() : 0;
        const int32_t invertOrder = (mInvert != nullptr) ? mInvert->getEffectOrder() : 0;

        T3D_LOG_INFO(LOG_TAG_APP,
            "[PostProcess] preset=%d %s enabled={Copy:%d Gray:%d Invert:%d Tint:%d} order={Gray:%d Invert:%d}",
            mPreset, presetName(mPreset), copyEn, grayEn, invertEn, tintEn, grayOrder, invertOrder);
    }

    const char *PostProcessControllerBehaviour::presetName(int32_t preset)
    {
        switch (preset)
        {
        case 0: return "None";
        case 1: return "Copy";
        case 2: return "Grayscale";
        case 3: return "Invert";
        case 4: return "GrayThenInvert";
        case 5: return "InvertThenGray";
        case 6: return "GrayDisabled";
        case 7: return "Tint";
        default: return "Unknown";
        }
    }
}
