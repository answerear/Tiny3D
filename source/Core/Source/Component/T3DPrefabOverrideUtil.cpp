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

#include "Component/T3DPrefabOverrideUtil.h"
#include "Component/T3DPrefabInstance.h"
#include "Component/T3DComponent.h"
#include "Kernel/T3DGameObject.h"
#include "Resource/T3DPrefab.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    bool PrefabOverrideUtil::shouldSkipProperty(const String &propName)
    {
        return propName == "UUID"
            || propName == "Children"
            || propName == "Components"
            || propName == "GameObjects"
            || propName == "Modifications"
            || propName == "SourcePrefabUUID"
            || propName == "RootGameObjectUUID"
            || propName == "BasePrefabUUID";
    }

    //--------------------------------------------------------------------------

    bool PrefabOverrideUtil::encodeVariant(const RTTRVariant &value, String &outType, String &outData)
    {
        if (!value.is_valid())
        {
            return false;
        }

        const RTTRType t = value.get_type();
        outType = t.get_name().to_string();

        if (t == rttr::type::get<bool>())
        {
            outData = value.to_bool() ? "1" : "0";
            return true;
        }
        if (t == rttr::type::get<float>() || t == rttr::type::get<double>())
        {
            outData = StringConverter::toString(static_cast<float>(value.to_double()));
            return true;
        }
        if (t == rttr::type::get<int32_t>() || t == rttr::type::get<int>()
            || t == rttr::type::get<uint32_t>())
        {
            outData = StringConverter::toString(static_cast<int32_t>(value.to_int32()));
            return true;
        }
        if (t == rttr::type::get<String>() || t == rttr::type::get<std::string>())
        {
            outData = value.to_string();
            return true;
        }
        if (t == rttr::type::get<UUID>())
        {
            outData = value.get_value<UUID>().toString();
            return true;
        }
        if (t == rttr::type::get<Vector3>())
        {
            const Vector3 v = value.get_value<Vector3>();
            outData = StringConverter::toString(v.x()) + ","
                + StringConverter::toString(v.y()) + ","
                + StringConverter::toString(v.z());
            return true;
        }
        if (t == rttr::type::get<Quaternion>())
        {
            const Quaternion q = value.get_value<Quaternion>();
            outData = StringConverter::toString(q.w()) + ","
                + StringConverter::toString(q.x()) + ","
                + StringConverter::toString(q.y()) + ","
                + StringConverter::toString(q.z());
            return true;
        }
        if (t == rttr::type::get<ColorRGB>())
        {
            const ColorRGB c = value.get_value<ColorRGB>();
            outData = StringConverter::toString(c.red()) + ","
                + StringConverter::toString(c.green()) + ","
                + StringConverter::toString(c.blue());
            return true;
        }

        outData = value.to_string();
        return true;
    }

    //--------------------------------------------------------------------------

    bool PrefabOverrideUtil::decodeVariant(const String &valueType, const String &valueData, RTTRVariant &outValue)
    {
        if (valueType == "bool")
        {
            outValue = (valueData == "1" || valueData == "true");
            return true;
        }
        if (valueType == "float" || valueType == "double")
        {
            outValue = StringConverter::parseValue<float>(valueData);
            return true;
        }
        if (valueType == "int" || valueType == "int32_t" || valueType == "uint32_t")
        {
            outValue = StringConverter::parseValue<int32_t>(valueData);
            return true;
        }
        if (valueType == "std::string" || valueType.find("basic_string") != String::npos
            || valueType == "String")
        {
            outValue = valueData;
            return true;
        }
        if (valueType.find("UUID") != String::npos)
        {
            UUID uuid;
            uuid.fromString(valueData);
            outValue = uuid;
            return true;
        }
        if (valueType.find("Vector3") != String::npos)
        {
            StringArray parts = StringUtil::split(valueData, ",");
            if (parts.size() >= 3)
            {
                outValue = Vector3(
                    StringConverter::parseValue<float>(parts[0]),
                    StringConverter::parseValue<float>(parts[1]),
                    StringConverter::parseValue<float>(parts[2]));
                return true;
            }
            return false;
        }
        if (valueType.find("Quaternion") != String::npos)
        {
            StringArray parts = StringUtil::split(valueData, ",");
            if (parts.size() >= 4)
            {
                outValue = Quaternion(
                    StringConverter::parseValue<float>(parts[0]),
                    StringConverter::parseValue<float>(parts[1]),
                    StringConverter::parseValue<float>(parts[2]),
                    StringConverter::parseValue<float>(parts[3]));
                return true;
            }
            return false;
        }
        if (valueType.find("ColorRGB") != String::npos)
        {
            StringArray parts = StringUtil::split(valueData, ",");
            if (parts.size() >= 3)
            {
                outValue = ColorRGB(
                    StringConverter::parseValue<float>(parts[0]),
                    StringConverter::parseValue<float>(parts[1]),
                    StringConverter::parseValue<float>(parts[2]));
                return true;
            }
            return false;
        }

        outValue = valueData;
        return true;
    }

    //--------------------------------------------------------------------------

    bool PrefabOverrideUtil::variantsEqual(const RTTRVariant &a, const RTTRVariant &b)
    {
        if (!a.is_valid() || !b.is_valid())
        {
            return a.is_valid() == b.is_valid();
        }

        String ta, da, tb, db;
        if (!encodeVariant(a, ta, da) || !encodeVariant(b, tb, db))
        {
            return false;
        }
        return ta == tb && da == db;
    }

    //--------------------------------------------------------------------------

    TResult PrefabOverrideUtil::recordObjectOverrides(Object *templateObj, Object *instanceObj,
        const UUID &templateUUID, PropertyModifications &outMods)
    {
        if (templateObj == nullptr || instanceObj == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        rttr::instance srcInst(*templateObj);
        rttr::instance dstInst(*instanceObj);
        rttr::type t = srcInst.get_derived_type();

        for (auto &prop : t.get_properties())
        {
            const String propName = prop.get_name().to_string();
            if (shouldSkipProperty(propName) || prop.is_readonly())
            {
                continue;
            }

            RTTRVariant srcVal = prop.get_value(srcInst);
            RTTRVariant dstVal = prop.get_value(dstInst);
            if (!srcVal.is_valid() || !dstVal.is_valid())
            {
                continue;
            }

            if (!variantsEqual(srcVal, dstVal))
            {
                String valueType, valueData;
                if (encodeVariant(dstVal, valueType, valueData))
                {
                    outMods.emplace_back(templateUUID, propName, valueType, valueData);
                }
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PrefabOverrideUtil::collectOverrides(Prefab *sourcePrefab, PrefabInstance *instance,
        PropertyModifications &outMods)
    {
        if (sourcePrefab == nullptr || instance == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        GameObject *templateRoot = sourcePrefab->getRootGameObject();
        if (templateRoot == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        outMods.clear();
        const auto &map = instance->getTemplateToInstanceMap();
        for (const auto &item : map)
        {
            const UUID &templateUUID = item.first;
            Object *instanceObj = item.second;
            Object *templateObj = nullptr;

            GameObject *go = findGameObjectByTemplateUUID(templateRoot, templateUUID);
            if (go != nullptr)
            {
                templateObj = go;
            }
            else
            {
                Component *comp = findComponentByTemplateUUID(templateRoot, templateUUID);
                templateObj = comp;
            }

            if (templateObj != nullptr && instanceObj != nullptr)
            {
                recordObjectOverrides(templateObj, instanceObj, templateUUID, outMods);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PrefabOverrideUtil::writeModificationToObject(Object *obj, const PropertyModification &mod)
    {
        if (obj == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        RTTRVariant value;
        if (!decodeVariant(mod.getValueType(), mod.getValueData(), value))
        {
            return T3D_ERR_INVALID_PARAM;
        }

        rttr::instance inst(*obj);
        rttr::type t = inst.get_derived_type();
        rttr::property prop = t.get_property(mod.getPropertyPath());
        if (!prop.is_valid())
        {
            return T3D_ERR_NOT_FOUND;
        }

        if (!prop.set_value(inst, value))
        {
            return T3D_ERR_FAIL;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PrefabOverrideUtil::applyModification(PrefabInstance *instance, const PropertyModification &mod)
    {
        if (instance == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        Object *obj = instance->resolveInstanceObject(mod.getTargetObjectUUID());
        if (obj == nullptr)
        {
            return T3D_ERR_NOT_FOUND;
        }

        return writeModificationToObject(obj, mod);
    }

    //--------------------------------------------------------------------------

    TResult PrefabOverrideUtil::applyModifications(PrefabInstance *instance, const PropertyModifications &mods)
    {
        if (instance == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        TResult ret = T3D_OK;
        for (const auto &mod : mods)
        {
            TResult r = applyModification(instance, mod);
            if (T3D_FAILED(r))
            {
                ret = r;
            }
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    GameObject *PrefabOverrideUtil::findGameObjectByTemplateUUID(GameObject *templateRoot, const UUID &templateUUID)
    {
        if (templateRoot == nullptr)
        {
            return nullptr;
        }

        if (templateRoot->getUUID() == templateUUID)
        {
            return templateRoot;
        }

        GameObjects objs;
        GameObject::collectHierarchy(templateRoot, objs);
        const auto it = objs.find(templateUUID);
        return it != objs.end() ? it->second.get() : nullptr;
    }

    //--------------------------------------------------------------------------

    Component *PrefabOverrideUtil::findComponentByTemplateUUID(GameObject *templateRoot, const UUID &templateUUID)
    {
        if (templateRoot == nullptr)
        {
            return nullptr;
        }

        GameObjects objs;
        GameObject::collectHierarchy(templateRoot, objs);
        for (const auto &item : objs)
        {
            GameObject *go = item.second.get();
            for (const auto &compItem : go->getAllComponents())
            {
                Component *comp = compItem.second.get();
                if (comp != nullptr && comp->getUUID() == templateUUID)
                {
                    return comp;
                }
            }
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------
}