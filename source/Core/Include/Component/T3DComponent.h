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


#ifndef __T3D_COMPONENT_H__
#define __T3D_COMPONENT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DGameObject.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API Component : public Object, public Noncopyable
    {
        friend class GameObject;
        
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
     
    public:
        virtual ComponentPtr clone() const = 0;

        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="getter", "Description"="UUID value")
        const UUID &getUUID() const { return mUUID; }

        GameObjectPtr getGameObject() const { return mGameObject; }

        void setGameObject(GameObject *gameObject) { mGameObject = gameObject; }

        virtual void onStart();
        
        virtual void onUpdate();

        virtual void onLoadResource(Archive *archive);

    protected:
        Component();
        
        Component(const UUID &uuid);

        ~Component() override;

        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter", "Description"="UUID value")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

        virtual TResult cloneProperties(const Component * const src);

        virtual void onDestroy();

    protected:
        /// 唯一 ID
        UUID    mUUID {UUID::INVALID};
        /// 绑定的 game object
        GameObjectPtr   mGameObject {nullptr};
    };
}


#endif  /*__T3D_COMPONENT_H__*/
