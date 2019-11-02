/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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
#include "T3DObject.h"

namespace Tiny3D
{
    class T3D_ENGINE_API Technique : public Object
    {
    public:
        /** 创建 Technique 对象 */
        static TechniquePtr create();

        /** 析构函数 */
        virtual ~Technique();

    protected:
        /** 构造函数 */
        Technique();

        typedef TArray<Pass*>   Passes;

        Material    *mParent;

        Passes      mPasses;

        String      mName;

        //---------------------------------------
        // Command : lod_index
        // Usage : lod_index <number>
        uint16_t    mLodIndex;

        uint16_t    mSchemeIndex;
    };
}


#endif  /*__T3D_TECHNIQUE_H__*/
