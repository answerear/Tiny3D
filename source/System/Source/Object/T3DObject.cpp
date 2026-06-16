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

#include "Object/T3DObject.h"

#include "../../../Log/Include/T3DLog.h"
#include "Object/T3DObjectTracer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Object::Object()
        : mReferCount(0)
    {
        ObjectTracer::getInstance().addObject(this);
    }

    //--------------------------------------------------------------------------

    Object::~Object()
    {
#if T3D_TRACE_OBJECT
        T3D_LOG_INFO("System", "%s - Class : %s, Pointer : 0x%016p", __FUNCTION__, mClsName.c_str(), this);
#endif
        ObjectTracer::getInstance().removeObject(this);
    }

    //--------------------------------------------------------------------------

    Object *Object::acquire()
    {
        mReferCount.fetch_add(1, std::memory_order_relaxed);
        return this;
    }

    //--------------------------------------------------------------------------

    void Object::release()
    {
        if (mReferCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            delete this;
        }
    }

    //--------------------------------------------------------------------------

    void Object::printDebugInfo()
    {
#if T3D_TRACE_OBJECT
        if (!mHasOutputDebug)
        {
            mClsName = typeid(*this).name();
            T3D_LOG_INFO("System", "Class : %s, Pointer : %p", mClsName.c_str(), this);
            mHasOutputDebug = true;
        }
#endif
    }

    //--------------------------------------------------------------------------

    void Object::onPostInit()
    {
        
    }

    //--------------------------------------------------------------------------

    void Object::onPreSave()
    {
        
    }

    //--------------------------------------------------------------------------

    void Object::onPostSave()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void Object::onPostLoad()
    {
        
    }
    
    //--------------------------------------------------------------------------
}

