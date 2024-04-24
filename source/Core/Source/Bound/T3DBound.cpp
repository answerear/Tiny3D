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


#include "Bound/T3DBound.h"
#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Bound::~Bound()
    {
    }

    //--------------------------------------------------------------------------

    bool Bound::test(Bound *bound) const
    {
        bool ret = false;

        switch (bound->getType())
        {
        case Type::SPHERE:
            {
                SphereBound *sphere = static_cast<SphereBound*>(bound);
                ret = testSphere(sphere->getSphere());
            }
            break;
        case Type::AABB:
            {
                AabbBound *aabb = static_cast<AabbBound*>(bound);
                ret = testAabb(aabb->getAabb());
            }
            break;
        case Type::OBB:
            {
                ObbBound *obb = static_cast<ObbBound*>(bound);
                ret = testObb(obb->getObb());
            }
            break;
        case Type::FRUSTUM:
            {
                FrustumBound *frustum
                    = static_cast<FrustumBound*>(bound);
                ret = testFrustum(frustum->getFrustum());
            }
            break;
        default:
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Bound::cloneProperties(const Component * const src)
    {
        TResult ret = Component::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const Bound * const bound = static_cast<const Bound* const>(src);
            mGroupID = bound->mGroupID;
            mIsMovable = bound->mIsMovable;
            mIsCollisionSource = bound->mIsCollisionSource;
            mIsEnabled = bound->mIsEnabled;
        }

        return ret;
    }

    //--------------------------------------------------------------------------
}
