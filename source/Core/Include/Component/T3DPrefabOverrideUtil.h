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

#ifndef __T3D_PREFAB_OVERRIDE_UTIL_H__
#define __T3D_PREFAB_OVERRIDE_UTIL_H__


#include "Component/T3DPropertyModification.h"


namespace Tiny3D
{
    class Prefab;
    class PrefabInstance;
    class GameObject;
    class Component;

    /**
     * \brief Prefab override 编解码与 diff / 应用工具
     */
    class T3D_ENGINE_API PrefabOverrideUtil
    {
    public:
        static bool encodeVariant(const RTTRVariant &value, String &outType, String &outData);

        static bool decodeVariant(const String &valueType, const String &valueData, RTTRVariant &outValue);

        static bool variantsEqual(const RTTRVariant &a, const RTTRVariant &b);

        static bool shouldSkipProperty(const String &propName);

        static TResult collectOverrides(Prefab *sourcePrefab, PrefabInstance *instance,
            PropertyModifications &outMods);

        static TResult recordObjectOverrides(Object *templateObj, Object *instanceObj,
            const UUID &templateUUID, PropertyModifications &outMods);

        static TResult applyModification(PrefabInstance *instance, const PropertyModification &mod);

        static TResult applyModifications(PrefabInstance *instance, const PropertyModifications &mods);

        static TResult writeModificationToObject(Object *obj, const PropertyModification &mod);

        static Component *findComponentByTemplateUUID(GameObject *templateRoot, const UUID &templateUUID);

        static GameObject *findGameObjectByTemplateUUID(GameObject *templateRoot, const UUID &templateUUID);
    };
}


#endif  /*__T3D_PREFAB_OVERRIDE_UTIL_H__*/