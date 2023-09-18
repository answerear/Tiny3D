/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __T3D_TECHNIQUE_H__
#define __T3D_TECHNIQUE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Material/T3DPass.h"


namespace Tiny3D
{
    using Passes = TList<PassPtr>;
    
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

        bool addTag(const String &key, const String &value);

        void removeTag(const String &key);
        
        bool getTag(const String &key, String &value) const;

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

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="getter")
        const ShaderLabTags &getTags() const
        {
            return mTags;
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
        }

        TPROPERTY(RTTRFuncName="Passes", RTTRFuncType="setter")
        void setPasses(const Passes &passes)
        {
            mPasses = passes;
        }
        
    protected:
        Technique(const String &name);

        String          mName {};
        uint32_t        mLOD {0};
        ShaderLabTags   mTags {};
        Passes          mPasses {};
    };
}


#endif    /*__T3D_TECHNIQUE_H__*/
