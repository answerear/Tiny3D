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

#ifndef __T3D_RML_CANVAS_H__
#define __T3D_RML_CANVAS_H__


#include "T3DRmlUiPrerequisites.h"
#include "Component/T3DBehaviour.h"
#include "UI/T3DUISystem.h"


namespace Rml
{
    class Context;
}


namespace Tiny3D
{
    /**
     * \brief 挂在相机物体上的 RmlUi 画布
     * \remarks Phase 1 只在 kOverlay 绘制。文档在第一次 update 里加载。
     */
    TCLASS()
    class T3D_RMLUI_API RmlCanvas : public Behaviour
    {
        TRTTI_ENABLE(Behaviour)
        TRTTI_FRIEND

    public:
        ~RmlCanvas() override = default;

        bool executeInEditMode() const override { return true; }

        TPROPERTY(RTTRFuncName="Documents", RTTRFuncType="getter")
        const TArray<String> &getDocuments() const { return mDocuments; }

        TPROPERTY(RTTRFuncName="Documents", RTTRFuncType="setter")
        void setDocuments(const TArray<String> &documents);

        TPROPERTY(RTTRFuncName="SortOrder", RTTRFuncType="getter")
        int32_t getSortOrder() const { return mSortOrder; }

        TPROPERTY(RTTRFuncName="SortOrder", RTTRFuncType="setter")
        void setSortOrder(int32_t order) { mSortOrder = order; }

        TPROPERTY(RTTRFuncName="DpRatio", RTTRFuncType="getter")
        Real getDpRatio() const { return mDpRatio; }

        TPROPERTY(RTTRFuncName="DpRatio", RTTRFuncType="setter")
        void setDpRatio(Real ratio) { mDpRatio = ratio; }

        /// Phase 1 固定为叠在后处理之后
        UIRenderPhase getRenderPhase() const { return UIRenderPhase::kOverlay; }

        Rml::Context *getContext() const { return mContext; }

        bool documentsLoaded() const { return mDocumentsLoaded; }
        void setDocumentsLoaded(bool loaded) { mDocumentsLoaded = loaded; }

        /// 在 Rml 仍存活时卸掉 context。shutdown 会先走这里。
        void releaseContext();

    protected:
        RmlCanvas() = default;
        explicit RmlCanvas(const UUID &uuid);

        void onAwake() override;
        void onDestroy() override;

    private:
        TArray<String> mDocuments {};
        int32_t mSortOrder {0};
        /// <= 0 时按平台 DPI 换算
        Real mDpRatio {0.0f};
        bool mDocumentsLoaded {false};
        String mContextName {};
        Rml::Context *mContext {nullptr};
    };
}


#endif  /*__T3D_RML_CANVAS_H__*/
