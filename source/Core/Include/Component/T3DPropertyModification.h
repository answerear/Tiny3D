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

#ifndef __T3D_PROPERTY_MODIFICATION_H__
#define __T3D_PROPERTY_MODIFICATION_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief Prefab 实例相对源资产的单条属性覆盖
     * \remarks targetObjectUUID 为 Prefab 资产内的模板 UUID；value 以类型名 + 编码字符串持久化
     */
    TSTRUCT()
    struct T3D_ENGINE_API PropertyModification
    {
        TRTTI_FRIEND

    public:
        PropertyModification() = default;

        PropertyModification(const UUID &targetObjectUUID, const String &propertyPath,
            const String &valueType, const String &valueData)
            : mTargetObjectUUID(targetObjectUUID)
            , mPropertyPath(propertyPath)
            , mValueType(valueType)
            , mValueData(valueData)
        {
        }

        TPROPERTY(RTTRFuncName="TargetObjectUUID", RTTRFuncType="getter")
        const UUID &getTargetObjectUUID() const { return mTargetObjectUUID; }

        TPROPERTY(RTTRFuncName="TargetObjectUUID", RTTRFuncType="setter")
        void setTargetObjectUUID(const UUID &uuid) { mTargetObjectUUID = uuid; }

        TPROPERTY(RTTRFuncName="PropertyPath", RTTRFuncType="getter")
        const String &getPropertyPath() const { return mPropertyPath; }

        TPROPERTY(RTTRFuncName="PropertyPath", RTTRFuncType="setter")
        void setPropertyPath(const String &path) { mPropertyPath = path; }

        TPROPERTY(RTTRFuncName="ValueType", RTTRFuncType="getter")
        const String &getValueType() const { return mValueType; }

        TPROPERTY(RTTRFuncName="ValueType", RTTRFuncType="setter")
        void setValueType(const String &type) { mValueType = type; }

        TPROPERTY(RTTRFuncName="ValueData", RTTRFuncType="getter")
        const String &getValueData() const { return mValueData; }

        TPROPERTY(RTTRFuncName="ValueData", RTTRFuncType="setter")
        void setValueData(const String &data) { mValueData = data; }

    protected:
        UUID    mTargetObjectUUID {};
        String  mPropertyPath {};
        String  mValueType {};
        String  mValueData {};
    };

    using PropertyModifications = TArray<PropertyModification>;
}


#endif  /*__T3D_PROPERTY_MODIFICATION_H__*/