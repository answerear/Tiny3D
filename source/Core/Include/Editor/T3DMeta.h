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

#ifndef __T3D_META_H__
#define __T3D_META_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
#if defined(T3D_EDITOR)

    TSTRUCT()
    struct T3D_ENGINE_API Meta
    {
        TENUM()
        enum Type
        {
            kUnknown = 0,
            kFolder,
            kDylib,
            kMaterial,
            kTexture,
            kShader,
            kMesh,
            kPrefab,
            kScene,
        };

        Meta() = default;

        Meta(Type t, const UUID &uid)
            : type(t)
            , uuid(uid)
        {}

        TPROPERTY()
        Type type {kUnknown};

        TPROPERTY()
        UUID uuid {UUID::INVALID};
    };
    
#endif
}


#endif    /*__T3D_META_H__*/
