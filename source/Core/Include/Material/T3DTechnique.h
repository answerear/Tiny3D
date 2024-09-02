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

#ifndef __T3D_TECHNIQUE_H__
#define __T3D_TECHNIQUE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    /**
     * \brief 渲染技术
     */
    TCLASS()
    class T3D_ENGINE_API Technique
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static TechniquePtr create(const String &name);

        ~Technique() override = default;

        TechniquePtr clone() const;

        TResult compile();

        void setShader(Shader *shader) { mShader = shader; }

        Shader *getShader() const { return mShader; }

        bool addTag(const String &key, const String &value);

        void removeTag(const String &key);
        
        bool getTag(const String &key, String &value) const;

        bool setTag(const String &key, const String &value);

        bool addPass(PassPtr pass);

        void removePass(const String &name);

        bool getPass(const String &name, PassPtr &pass) const;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }
        
        TPROPERTY(RTTRFuncName="LOD", RTTRFuncType="getter")
        uint32_t getLOD() const
        {
            return mLOD;
        }

        TPROPERTY(RTTRFuncName="LOD", RTTRFuncType="setter")
        void setLOD(uint32_t LOD)
        {
            mLOD = LOD;
        }

        uint32_t getRenderQueue() const { return mRenderQueue; }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="getter")
        const ShaderLabTags &getTags() const
        {
            return mTags;
        }

        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="getter")
        RenderStatePtr getRenderState() const
        {
            return mRenderState;
        }

        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="setter")
        void setRenderState(RenderStatePtr state)
        {
            mRenderState = state;
        }

        TPROPERTY(RTTRFuncName="Passes", RTTRFuncType="getter")
        const Passes &getPasses() const
        {
            return mPasses;
        }
        
    private:
        Technique() : Technique("") {}

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="setter")
        void setTags(const ShaderLabTags &tags)
        {
            mTags = tags;
            const auto itr = mTags.find(SHADER_TAG_QUEUE);
            if (itr != mTags.end())
            {
                mRenderQueue = toRenderQueue(itr->second);
            }
            else
            {
                mRenderQueue = -1;
            }
        }

        TPROPERTY(RTTRFuncName="Passes", RTTRFuncType="setter")
        void setPasses(const Passes &passes)
        {
            mPasses = passes;
        }
        
    protected:
        Technique(const String &name);

        uint32_t toRenderQueue(const String &tag);

        void onPostLoad() override;
        
    protected:
        Shader          *mShader {nullptr};
        String          mName {};
        uint32_t        mLOD {0};
        uint32_t        mRenderQueue {0};
        ShaderLabTags   mTags {};
        /// 渲染状态
        RenderStatePtr  mRenderState {nullptr};
        Passes          mPasses {};
    };
}


#endif    /*__T3D_TECHNIQUE_H__*/
