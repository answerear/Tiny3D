﻿/*******************************************************************************
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


#include "T3DScriptTranslator.h"
#include "T3DScriptError.h"
#include "T3DScriptCompiler.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    size_t ScriptTranslator::processNode(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node)
    {
        if (node->type != ANT_OBJECT)
            return 0;

        // Abstract objects are completely skipped
        ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(node.get());
        if (obj->abstrct)
            return 0;

        // Retrieve the translator to use
        ScriptTranslator *translator = compiler->getTranslator(node);

        size_t ret = 0;

        if (translator)
        {
            ret = translator->translate(compiler, stream, node);
        }
        else
        {
            ScriptError::printError(CERR_UNEXPECTEDTOKEN, obj->name, obj->file, obj->line);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    size_t ScriptTranslator::translateObjectHeader(ObjectAbstractNode *obj, DataStream &stream)
    {
        size_t totalBytes = 0;
        size_t bytesOfWritten = 0;

        // 类型
        uint16_t type = (uint16_t)obj->type;
        bytesOfWritten = stream.write(&type, sizeof(type));
        totalBytes += bytesOfWritten;
        // ID
        uint16_t id = (uint16_t)obj->id;
        bytesOfWritten = stream.write(&id, sizeof(id));
        totalBytes += bytesOfWritten;
        // 名称
        bytesOfWritten = writeString(obj->name, stream);
        totalBytes += bytesOfWritten;

        return totalBytes;
    }

    AbstractNodeList::const_iterator ScriptTranslator::getNodeAt(const AbstractNodeList &nodes, int index)
    {
        AbstractNodeList::const_iterator i = nodes.begin();
        int n = 0;
        while (i != nodes.end())
        {
            if (n == index)
                return i;
            ++i;
            ++n;
        }
        return nodes.end();
    }

    bool ScriptTranslator::getBoolean(const AbstractNodePtr &node, bool *result)
    {
        if (node->type != ANT_ATOM)
            return false;

        AtomAbstractNode *atom = (AtomAbstractNode*)node.get();

        if (atom->id == ID_TRUE || atom->id == ID_FALSE)
        {
            *result = atom->id == ID_TRUE ? true : false;
        }
        else
        {
            String string(atom->value.c_str());
            *result = StringConverter::parseBool(string);
        }

        return true;
    }

    bool ScriptTranslator::getString(const AbstractNodePtr &node, String *result)
    {
        if (node->type != ANT_ATOM)
            return false;

        AtomAbstractNode *atom = (AtomAbstractNode*)node.get();
        *result = atom->value;
        return true;
    }

    bool ScriptTranslator::getSingle(const AbstractNodePtr &node, float32_t *result)
    {
        if (node->type != ANT_ATOM)
            return false;

        AtomAbstractNode *atom = (AtomAbstractNode*)node.get();
        int n = sscanf(atom->value.c_str(), "%f", result);

        if (n == 0 || n == EOF)
            return false; // Conversion failed

        return true;
    }

    bool ScriptTranslator::getDouble(const AbstractNodePtr &node, float64_t *result)
    {
        if (node->type != ANT_ATOM)
            return false;

        AtomAbstractNode *atom = (AtomAbstractNode*)node.get();
        int n = sscanf(atom->value.c_str(), "%lf", result);

        if (n == 0 || n == EOF)
            return false; // Conversion failed

        return true;
    }

    bool ScriptTranslator::getInt(const AbstractNodePtr &node, int32_t *result)
    {
        if (node->type != ANT_ATOM)
            return false;

        AtomAbstractNode *atom = (AtomAbstractNode*)node.get();
        int n = sscanf(atom->value.c_str(), "%d", result);

        if (n == 0 || n == EOF)
            return false; // Conversion failed

        return true;
    }

    bool ScriptTranslator::getUInt(const AbstractNodePtr &node, uint32_t *result)
    {
        if (node->type != ANT_ATOM)
            return false;

        AtomAbstractNode *atom = (AtomAbstractNode*)node.get();
        int n = sscanf(atom->value.c_str(), "%u", result);

        if (n == 0 || n == EOF)
            return false; // Conversion failed

        return true;
    }

    bool ScriptTranslator::getHex(const AbstractNodePtr &node, uint32_t *result)
    {
        if (node->type != ANT_ATOM)
            return false;

        AtomAbstractNode *atom = (AtomAbstractNode*)node.get();

        char *end;
        *result = static_cast<uint32_t>(strtoul(atom->value.c_str(), &end, 16));

        return !(*end);
    }

    bool ScriptTranslator::getColor(AbstractNodeList::const_iterator i, AbstractNodeList::const_iterator end, ColorARGB *result, int32_t maxEntries /* = 4 */)
    {
        int32_t n = 0;
        while (i != end && n < maxEntries)
        {
            float v = 0;
            if (getSingle(*i, &v))
            {
                switch (n)
                {
                case 0:
                    result->red() = v;
                    break;
                case 1:
                    result->green() = v;
                    break;
                case 2:
                    result->blue() = v;
                    break;
                case 3:
                    result->alpha() = v;
                    break;
                }
            }
            else
            {
                return false;
            }
            ++n;
            ++i;
        }
        // return error if we found less than rgb before end, unless constrained
        return (n >= 3 || n == maxEntries);
    }

    bool ScriptTranslator::getMatrix4(AbstractNodeList::const_iterator i, AbstractNodeList::const_iterator end, Matrix4 *m)
    {
        int n = 0;
        while (i != end && n < 16)
        {
            if (i != end)
            {
                Real r = 0;
                if (getSingle(*i, &r))
                    (*m)[n / 4][n % 4] = r;
                else
                    return false;
            }
            else
            {
                return false;
            }
            ++i;
            ++n;
        }
        return true;
    }

    //--------------------------------------------------------------------------

    size_t ScriptTranslator::writeString(const String &str, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        uint16_t len = str.length();
        if (len > 0)
        {
            bytesOfWritten = stream.write(&len, sizeof(len));
            totalBytes += bytesOfWritten;
            bytesOfWritten = stream.write((void*)str.c_str(), len);
            totalBytes += bytesOfWritten;
        }

        return totalBytes;
    }

    size_t ScriptTranslator::writeColor(const ColorARGB &clr, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        float32_t component = clr.blue();
        bytesOfWritten = stream.write(&component, sizeof(component));
        totalBytes += bytesOfWritten;
        component = clr.green();
        bytesOfWritten = stream.write(&component, sizeof(component));
        totalBytes += bytesOfWritten;
        component = clr.red();
        bytesOfWritten = stream.write(&component, sizeof(component));
        totalBytes += bytesOfWritten;
        component = clr.alpha();
        bytesOfWritten = stream.write(&component, sizeof(component));
        totalBytes += bytesOfWritten;

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t MaterialTranslator::translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node)
    {
        ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(node.get());

        if (obj->name.empty())
        {
            ScriptError::printError(CERR_OBJECTNAMEEXPECTED, "No name object", obj->file, obj->line);
            return 0;
        }

        size_t totalBytes = 0;
        size_t bytesOfWritten = 0;

        // 对象头数据
        bytesOfWritten = translateObjectHeader(obj, stream);
        totalBytes += bytesOfWritten;

        // 属性
        for (auto i = obj->children.begin(); i != obj->children.end(); ++i)
        {
            if ((*i)->type == ANT_PROPERTY)
            {
                PropertyAbstractNode *prop = static_cast<PropertyAbstractNode*>((*i).get());

                // ID
                uint16_t id = prop->id;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // Values
                switch (prop->id)
                {
                case ID_LOD_VALUES:
                    {
                        bytesOfWritten = translateLODValues(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_RECEIVE_SHADOWS:
                    {
                        bytesOfWritten = translateReceiveShadow(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_TRANSPARENCY_CASTS_SHADOWS:
                    {
                        bytesOfWritten = translateTransparentCastsShadow(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SET_TEXTURE_ALIAS:
                    {
                        bytesOfWritten = translateSetTextuerAlias(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    {
                        ScriptError::printError(CERR_UNEXPECTEDTOKEN, prop->name, prop->file, prop->line,
                            "token \"" + prop->name + "\" is not recognized");
                    }
                    break;
                }
            }
            else if ((*i)->type == ANT_OBJECT)
            {
                bytesOfWritten = processNode(compiler, stream, *i);
                totalBytes += bytesOfWritten;
            }
        }

        return bytesOfWritten;
    }

    size_t MaterialTranslator::translateLODValues(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t totalBytes = 0;
        size_t bytesOfWritten = 0;
        
        // LOD数量
        uint16_t len = prop->values.size();
        bytesOfWritten = stream.write(&len, sizeof(len));
        totalBytes += bytesOfWritten;
        // LOD值
        if (len > 0)
        {
            for (auto j = prop->values.begin(); j != prop->values.end(); ++j)
            {
                float32_t val;
                if (getSingle(*j, &val))
                {
                    bytesOfWritten = stream.write(&val, sizeof(val));
                    totalBytes += bytesOfWritten;
                }
                else
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        "lod_values expects only numbers as arguments");
                }
            }
        }
        else
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }

        return totalBytes;
    }

    size_t MaterialTranslator::translateReceiveShadow(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t totalBytes = 0;
        size_t bytesOfWritten = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "receive_shadows only supports 1 argument");
        }
        else
        {
            bool val = true;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "receive_shadows argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\"");
            }
        }

        return totalBytes;
    }

    size_t MaterialTranslator::translateTransparentCastsShadow(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t totalBytes = 0;
        size_t bytesOfWritten = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "transparency_casts_shadows only supports 1 argument");
        }
        else
        {
            bool val = true;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "transparency_casts_shadows argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\"");
            }
        }

        return totalBytes;
    }

    size_t MaterialTranslator::translateSetTextuerAlias(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t totalBytes = 0;
        size_t bytesOfWritten = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 3)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "set_texture_alias only supports 2 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            String name, value;
            if (getString(*i0, &name) && getString(*i1, &value))
            {
                // name
                bytesOfWritten = writeString(name, stream);
                totalBytes += bytesOfWritten;
                // value
                bytesOfWritten = writeString(value, stream);
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "set_texture_alias must have 2 string argument");
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TechniqueTranslator::translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node)
    {
        ObjectAbstractNode *obj = reinterpret_cast<ObjectAbstractNode*>(node.get());

        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        // 对象头数据
        bytesOfWritten = translateObjectHeader(obj, stream);
        totalBytes += bytesOfWritten;


        for (AbstractNodeList::iterator i = obj->children.begin(); i != obj->children.end(); ++i)
        {
            if ((*i)->type == ANT_PROPERTY)
            {
                PropertyAbstractNode *prop = reinterpret_cast<PropertyAbstractNode*>((*i).get());

                // ID
                uint16_t id = prop->id;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // 属性
                switch (prop->id)
                {
                case ID_SCHEME:
                    {
                        bytesOfWritten = translateScene(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_LOD_INDEX:
                    {
                        bytesOfWritten = translateLODIndex(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SHADOW_CASTER_MATERIAL:
                    {
                        bytesOfWritten = translateShadowCasterMaterial(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SHADOW_RECEIVER_MATERIAL:
                    {
                        bytesOfWritten = translateShadowReceiveMaterial(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_GPU_VENDOR_RULE:
                    {
                        bytesOfWritten = translateGPUVendorRule(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_GPU_DEVICE_RULE:
                    {
                        bytesOfWritten = translateGPUDeviceRule(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_UNEXPECTEDTOKEN, prop->name, prop->file, prop->line,
                        "token \"" + prop->name + "\" is not recognized");
                    break;
                }
            }
            else if ((*i)->type == ANT_OBJECT)
            {
                bytesOfWritten = processNode(compiler, stream, *i);
                totalBytes += bytesOfWritten;
            }
        }

        return totalBytes;
    }

    size_t TechniqueTranslator::translateScene(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "scheme only supports 1 argument");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0);
            String scheme;
            if (getString(*i0, &scheme))
            {
                bytesOfWritten = writeString(scheme, stream);
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "scheme must have 1 string argument");
            }
        }

        return totalBytes;
    }

    size_t TechniqueTranslator::translateLODIndex(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "lod_index only supports 1 argument");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0);
            uint32_t v = 0;
            if (getUInt(*i0, &v))
            {
                bytesOfWritten = stream.write(&v, sizeof(v));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "lod_index cannot accept argument \"" + (*i0)->getValue() + "\"");
            }
        }

        return totalBytes;
    }

    size_t TechniqueTranslator::translateShadowCasterMaterial(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "shadow_caster_material only accepts 1 argument");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0);
            String matName;
            if (getString(*i0, &matName))
            {
                bytesOfWritten = writeString(matName, stream);
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "shadow_caster_material cannot accept argument \"" + (*i0)->getValue() + "\"");
            }
        }

        return totalBytes;
    }

    size_t TechniqueTranslator::translateShadowReceiveMaterial(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "shadow_receiver_material only accepts 1 argument");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0);
            String matName;
            if (getString(*i0, &matName))
            {
                bytesOfWritten = writeString(matName, stream);
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "shadow_receiver_material cannot accept argument \"" + (*i0)->getValue() + "\"");
            }
        }

        return totalBytes;
    }

    size_t TechniqueTranslator::translateGPUVendorRule(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.size() < 2)
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line,
                "gpu_vendor_rule must have 2 arguments");
        }
        else if (prop->values.size() > 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "gpu_vendor_rule must have 2 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0);
            AbstractNodeList::const_iterator i1 = getNodeAt(prop->values, 1);

            if ((*i0)->type == ANT_ATOM)
            {
                AtomAbstractNode *atom0 = (AtomAbstractNode*)(*i0).get();
                if (atom0->id == ID_INCLUDE || atom0->id == ID_EXCLUDE)
                {
                    // ID
                    uint16_t id = atom0->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                    
                    String vendor;
                    if (getString(*i1, &vendor))
                    {
                        bytesOfWritten = writeString(vendor, stream);
                        totalBytes += bytesOfWritten;
                    }
                    else
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            "gpu_vendor_rule cannot accept \"" + (*i1)->getValue() + "\" as second argument");
                    }
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "gpu_vendor_rule cannot accept \"" + (*i0)->getValue() + "\" as first argument");
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "gpu_vendor_rule cannot accept \"" + (*i0)->getValue() + "\" as first argument");
            }

        }

        return totalBytes;
    }

    size_t TechniqueTranslator::translateGPUDeviceRule(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.size() < 2)
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line,
                "gpu_device_rule must have at least 2 arguments");
        }
        else if (prop->values.size() > 3)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "gpu_device_rule must have at most 3 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0);
            AbstractNodeList::const_iterator i1 = getNodeAt(prop->values, 1);

            if ((*i0)->type == ANT_ATOM)
            {
                AtomAbstractNode *atom0 = (AtomAbstractNode*)(*i0).get();

                if (atom0->id == ID_INCLUDE || atom0->id == ID_EXCLUDE)
                {
                    // ID
                    uint16_t id = atom0->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;

                    String pattern;
                    if (getString(*i1, &pattern))
                    {
                        bytesOfWritten = writeString(pattern, stream);
                        totalBytes += bytesOfWritten;
                    }
                    else
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            "gpu_device_rule cannot accept \"" + (*i1)->getValue() + "\" as second argument");
                    }

                    if (prop->values.size() == 3)
                    {
                        AbstractNodeList::const_iterator i2 = getNodeAt(prop->values, 2);
                        bool caseSensitive;
                        if (getBoolean(*i2, &caseSensitive))
                        {
                            bytesOfWritten = stream.write(&caseSensitive, sizeof(caseSensitive));
                            totalBytes += bytesOfWritten;
                        }
                        else
                        {
                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                "gpu_device_rule third argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\"");
                        }
                    }
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "gpu_device_rule cannot accept \"" + (*i0)->getValue() + "\" as first argument");
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "gpu_device_rule cannot accept \"" + (*i0)->getValue() + "\" as first argument");
            }

        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t PassTranslator::translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node)
    {
        ObjectAbstractNode *obj = reinterpret_cast<ObjectAbstractNode*>(node.get());

        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        // 对象头数据
        bytesOfWritten = translateObjectHeader(obj, stream);
        totalBytes += bytesOfWritten;

        // Set the properties for the material
        for (AbstractNodeList::iterator i = obj->children.begin(); i != obj->children.end(); ++i)
        {
            if ((*i)->type == ANT_PROPERTY)
            {
                PropertyAbstractNode *prop = reinterpret_cast<PropertyAbstractNode*>((*i).get());

                // ID
                uint16_t id = prop->id;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // 属性
                switch (prop->id)
                {
                case ID_AMBIENT:
                    {
                        bytesOfWritten = translateAmbient(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_DIFFUSE:
                    {
                        bytesOfWritten = translateDiffuse(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SPECULAR:
                    {
                        bytesOfWritten = translateSpecular(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_EMISSIVE:
                    {
                        bytesOfWritten = translateEmissive(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SCENE_BLEND:
                    {
                        bytesOfWritten = translateSceneBlend(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SEPARATE_SCENE_BLEND:
                    {
                        bytesOfWritten = translateSeparateSceneBlend(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SCENE_BLEND_OP:
                    {
                        bytesOfWritten = translateSceneBlendOp(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SEPARATE_SCENE_BLEND_OP:
                    {
                        bytesOfWritten = translateSeparateSceneBlendOp(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_DEPTH_CHECK:
                    {
                        bytesOfWritten = translateDepthCheck(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_DEPTH_WRITE:
                    {
                        bytesOfWritten = translateDepthWrite(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_DEPTH_BIAS:
                    {
                        bytesOfWritten = translateDepthBias(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_DEPTH_FUNC:
                    {
                        bytesOfWritten = translateDepthFunc(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ITERATION_DEPTH_BIAS:
                    {
                        bytesOfWritten = translateIterationDepthBias(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ALPHA_REJECTION:
                    {
                        bytesOfWritten = translateAlphaRejection(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ALPHA_TO_COVERAGE:
                    {
                        bytesOfWritten = translateAlphaToCoverage(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_LIGHT_SCISSOR:
                    {
                        bytesOfWritten = translateLightScissor(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_LIGHT_CLIP_PLANES:
                    {
                        bytesOfWritten = translateLightClipPlanes(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_TRANSPARENT_SORTING:
                    {
                        bytesOfWritten = translateTransparentSorting(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ILLUMINATION_STAGE:
                    {
                        bytesOfWritten = translateIlluminationStage(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_CULL_HARDWARE:
                    {
                        bytesOfWritten = translateCullHardware(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_CULL_SOFTWARE:
                    {
                        bytesOfWritten = translateCullSoftware(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_NORMALISE_NORMALS:
                    {
                        bytesOfWritten = translateNormalizeNormals(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_LIGHTING:
                    {
                        bytesOfWritten = translateLighting(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SHADING:
                    {
                        bytesOfWritten = translateShading(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_POLYGON_MODE:
                    {
                        bytesOfWritten = translatePolygonMode(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_POLYGON_MODE_OVERRIDEABLE:
                    {
                        bytesOfWritten = translatePolygonModeOverridable(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_FOG_OVERRIDE:
                    {
                        bytesOfWritten = translateFogOverride(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_COLOUR_WRITE:
                    {
                        bytesOfWritten = translateColorWrite(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_MAX_LIGHTS:
                    {
                        bytesOfWritten = translateMaxLights(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_START_LIGHT:
                    {
                        bytesOfWritten = translateStartLight(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_LIGHT_MASK:
                    {
                        bytesOfWritten = translateLightMask(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ITERATION:
                    {
                        bytesOfWritten = translateIteration(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_POINT_SIZE:
                    {
                        bytesOfWritten = translatePointSize(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_POINT_SPRITES:
                    {
                        bytesOfWritten = translatePointSprites(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_POINT_SIZE_ATTENUATION:
                    {
                        bytesOfWritten = translatePointSizeAttenuation(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_POINT_SIZE_MIN:
                    {
                        bytesOfWritten = translatePointSizeMin(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_POINT_SIZE_MAX:
                    {
                        bytesOfWritten = translatePointSizeMax(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_UNEXPECTEDTOKEN, prop->name, prop->file, prop->line,
                        "token \"" + prop->name + "\" is not recognized");
                }
            }
            else if ((*i)->type == ANT_OBJECT)
            {
                ObjectAbstractNode *child = reinterpret_cast<ObjectAbstractNode*>((*i).get());
                switch (child->id)
                {
                case ID_FRAGMENT_PROGRAM_REF:
//                     translateFragmentProgramRef(compiler, child);
                    break;
                case ID_VERTEX_PROGRAM_REF:
//                     translateVertexProgramRef(compiler, child);
                    break;
                case ID_GEOMETRY_PROGRAM_REF:
//                     translateGeometryProgramRef(compiler, child);
                    break;
                case ID_TESSELLATION_HULL_PROGRAM_REF:
//                     translateTessellationHullProgramRef(compiler, child);
                    break;
                case ID_TESSELLATION_DOMAIN_PROGRAM_REF:
//                     translateTessellationDomainProgramRef(compiler, child);
                    break;
                case ID_COMPUTE_PROGRAM_REF:
//                     translateComputeProgramRef(compiler, child);
                    break;
                case ID_SHADOW_CASTER_VERTEX_PROGRAM_REF:
//                     translateShadowCasterVertexProgramRef(compiler, child);
                    break;
                case ID_SHADOW_CASTER_FRAGMENT_PROGRAM_REF:
//                     translateShadowCasterFragmentProgramRef(compiler, child);
                    break;
                default:
                    {
                        bytesOfWritten = processNode(compiler, stream, *i);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                }
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateAmbient(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 4)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "ambient must have at most 4 parameters");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM &&
                ((AtomAbstractNode*)prop->values.front().get())->id == ID_VERTEXCOLOUR)
            {
                // vertex color
                uint16_t id = ID_VERTEXCOLOUR;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;
            }
            else
            {
                // not vertex color
                uint16_t id = 0;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // ambient
                ColorARGB val = ColorARGB::WHITE;
                if (getColor(prop->values.begin(), prop->values.end(), &val))
                {
                    bytesOfWritten = writeColor(val, stream);
                    totalBytes += bytesOfWritten;
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "ambient requires 3 or 4 colour arguments, or a \"vertexcolour\" directive");
                }
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateDiffuse(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 4)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "diffuse must have at most 4 arguments");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM &&
                ((AtomAbstractNode*)prop->values.front().get())->id == ID_VERTEXCOLOUR)
            {
                // vertex color
                uint16_t id = ID_VERTEXCOLOUR;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;
            }
            else
            {
                // not vertex color
                uint16_t id = 0;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // diffuse
                ColorARGB val = ColorARGB::WHITE;
                if (getColor(prop->values.begin(), prop->values.end(), &val))
                {
                    bytesOfWritten = writeColor(val, stream);
                    totalBytes += bytesOfWritten;
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "diffuse requires 3 or 4 colour arguments, or a \"vertexcolour\" directive");
                }
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateSpecular(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 5)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "specular must have at most 5 arguments");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM &&
                ((AtomAbstractNode*)prop->values.front().get())->id == ID_VERTEXCOLOUR)
            {
                // vertex color
                uint16_t id = ID_VERTEXCOLOUR;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // shininess
                if (prop->values.size() >= 2)
                {
                    float32_t shininess = 0.0f;
                    if (getSingle(prop->values.back(), &shininess))
                    {
                        bytesOfWritten = stream.write(&shininess, sizeof(shininess));
                        totalBytes += bytesOfWritten;
                    }
                    else
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            "specular does not support \"" + prop->values.back()->getValue() + "\" as its second argument");
                    }
                }
            }
            else
            {
                if (prop->values.size() < 4)
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        "specular expects at least 4 arguments");
                }
                else
                {
                    // not vertex color
                    uint16_t id = 0;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;

                    AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0),
                        i1 = getNodeAt(prop->values, 1),
                        i2 = getNodeAt(prop->values, 2);
                    ColorARGB val(0.0f, 0.0f, 0.0f, 1.0f);
                    if (getSingle(*i0, &val.red()) && getSingle(*i1, &val.green()) && getSingle(*i2, &val.blue()))
                    {
                        if (prop->values.size() == 4)
                        {
                            // specular
                            bytesOfWritten = writeColor(val, stream);
                            totalBytes += bytesOfWritten;

                            // shininess
                            AbstractNodeList::const_iterator i3 = getNodeAt(prop->values, 3);
                            float32_t shininess = 0.0f;
                            if (getSingle(*i3, &shininess))
                            {
                                bytesOfWritten = stream.write(&shininess, sizeof(shininess));
                                totalBytes += bytesOfWritten;
                            }
                            else
                            {
                                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                    "specular fourth argument must be a valid number for shininess attribute");
                            }
                        }
                        else
                        {
                            AbstractNodeList::const_iterator i3 = getNodeAt(prop->values, 3);
                            if (!getSingle(*i3, &val.alpha()))
                            {
                                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                    "specular fourth argument must be a valid color component value");
                            }
                            else
                            {
                                // specular
                                bytesOfWritten = writeColor(val, stream);
                                totalBytes += bytesOfWritten;
                            }

                            // shininess
                            AbstractNodeList::const_iterator i4 = getNodeAt(prop->values, 4);
                            float32_t shininess = 0.0f;
                            if (getSingle(*i4, &shininess))
                            {
                                bytesOfWritten = stream.write(&shininess, sizeof(shininess));
                                totalBytes += bytesOfWritten;
                            }
                            else
                            {
                                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                    "specular fourth argument must be a valid number for shininess attribute");
                            }
                        }
                    }
                    else
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            "specular must have first 3 arguments be a valid colour");
                    }
                }

            }
        }

        return bytesOfWritten;
    }

    size_t PassTranslator::translateEmissive(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 4)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "emissive must have at most 4 arguments");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM &&
                ((AtomAbstractNode*)prop->values.front().get())->id == ID_VERTEXCOLOUR)
            {
                // vertex color
                uint16_t id = ID_VERTEXCOLOUR;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;
            }
            else
            {
                // not vertex color
                uint16_t id = 0;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // emissive
                ColorARGB val(0.0f, 0.0f, 0.0f, 1.0f);
                if (getColor(prop->values.begin(), prop->values.end(), &val))
                {
                    bytesOfWritten = writeColor(val, stream);
                    totalBytes += bytesOfWritten;
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "emissive requires 3 or 4 colour arguments, or a \"vertexcolour\" directive");
                }
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateSceneBlend(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "scene_blend supports at most 2 arguments");
        }
        else if (prop->values.size() == 1)
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();

                switch (atom->id)
                {
                case ID_ADD:
                case ID_MODULATE:
                case ID_COLOUR_BLEND:
                case ID_ALPHA_BLEND:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "scene_blend does not support \"" + prop->values.front()->getValue() + "\" for argument 1");
                    break;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "scene_blend does not support \"" + prop->values.front()->getValue() + "\" for argument 1");
            }
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            AtomAbstractNode *atom0 = (AtomAbstractNode *)(i0->get());
            AtomAbstractNode *atom1 = (AtomAbstractNode *)(i1->get());
            if (atom0->id >= ID_ONE && atom0->id <= ID_ONE_MINUS_SRC_ALPHA
                && atom1->id >= ID_ONE && atom1->id <= ID_ONE_MINUS_SRC_ALPHA)
            {
                uint16_t id = atom0->id;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;
                id = atom1->id;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "scene_blend does not support \"" + (*i0)->getValue() + "\" and \"" + (*i1)->getValue() + "\" as arguments");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateSeparateSceneBlend(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 3)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "separate_scene_blend must have 2 or 4 arguments");
        }
        else if (prop->values.size() > 4)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "separate_scene_blend must have 2 or 4 arguments");
        }
        else if (prop->values.size() == 2)
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            if ((*i0)->type == ANT_ATOM && (*i1)->type == ANT_ATOM)
            {
                AtomAbstractNode *atom0 = (AtomAbstractNode*)(*i0).get(), *atom1 = (AtomAbstractNode*)(*i1).get();

                switch (atom0->id)
                {
                case ID_ADD:
                case ID_MODULATE:
                case ID_COLOUR_BLEND:
                case ID_ALPHA_BLEND:
                    {
                        uint16_t id = atom0->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "separate_scene_blend does not support \"" + atom0->value + "\" as argument 1");
                    return 0;
                }

                switch (atom1->id)
                {
                case ID_ADD:
                case ID_MODULATE:
                case ID_COLOUR_BLEND:
                case ID_ALPHA_BLEND:
                    {
                        uint16_t id = atom1->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "separate_scene_blend does not support \"" + atom1->value + "\" as argument 2");
                    return 0;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "separate_scene_blend does not support \"" + (*i0)->getValue() + "\" as argument 1");
            }
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1),
                i2 = getNodeAt(prop->values, 2), i3 = getNodeAt(prop->values, 3);
            AtomAbstractNode *atom0 = (AtomAbstractNode *)(i0->get());
            AtomAbstractNode *atom1 = (AtomAbstractNode *)(i1->get());
            AtomAbstractNode *atom2 = (AtomAbstractNode *)(i2->get());
            AtomAbstractNode *atom3 = (AtomAbstractNode *)(i3->get());
            if ((*i0)->type == ANT_ATOM && (*i1)->type == ANT_ATOM && (*i2)->type == ANT_ATOM && (*i3)->type == ANT_ATOM)
            {
                if (atom0->id >= ID_ONE && atom0->id <= ID_ONE_MINUS_SRC_ALPHA
                    && atom1->id >= ID_ONE && atom1->id <= ID_ONE_MINUS_SRC_ALPHA
                    && atom2->id >= ID_ONE && atom2->id <= ID_ONE_MINUS_SRC_ALPHA
                    && atom3->id >= ID_ONE && atom3->id <= ID_ONE_MINUS_SRC_ALPHA)
                {
                    uint16_t id = atom0->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                    id = atom1->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                    id = atom2->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                    id = atom3->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "one of the arguments to separate_scene_blend is not a valid scene blend factor directive");
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "one of the arguments to separate_scene_blend is not a valid scene blend factor directive");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateSceneBlendOp(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "scene_blend_op must have 1 argument");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = reinterpret_cast<AtomAbstractNode*>(prop->values.front().get());
                switch (atom->id)
                {
                case ID_ADD:
                case ID_SUBTRACT:
                case ID_REVERSE_SUBTRACT:
                case ID_MIN:
                case ID_MAX:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom->value + ": unrecognized argument");
                    break;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + ": unrecognized argument");
            }
        }

        return bytesOfWritten;
    }

    size_t PassTranslator::translateSeparateSceneBlendOp(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() != 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "separate_scene_blend_op must have 2 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            if ((*i0)->type == ANT_ATOM && (*i1)->type == ANT_ATOM)
            {
                AtomAbstractNode *atom0 = reinterpret_cast<AtomAbstractNode*>((*i0).get()),
                    *atom1 = reinterpret_cast<AtomAbstractNode*>((*i1).get());

                switch (atom0->id)
                {
                case ID_ADD:
                case ID_SUBTRACT:
                case ID_REVERSE_SUBTRACT:
                case ID_MIN:
                case ID_MAX:
                    {
                        uint16_t id = atom0->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom0->value + ": unrecognized first argument");
                    break;
                }

                switch (atom1->id)
                {
                case ID_ADD:
                case ID_SUBTRACT:
                case ID_REVERSE_SUBTRACT:
                case ID_MIN:
                case ID_MAX:
                    {
                        uint16_t id = atom1->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom1->value + ": unrecognized second argument");
                    break;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + ": unrecognized argument");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateDepthCheck(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "depth_check must have 1 argument");
        }
        else
        {
            bool val = true;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "depth_check third argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\"");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateDepthWrite(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "depth_write must have 1 argument");
        }
        else
        {
            bool val = true;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "depth_write third argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\"");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateDepthBias(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "depth_bias must have at most 2 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            float32_t val0, val1 = 0.0f;
            if (getSingle(*i0, &val0))
            {
                if (i1 != prop->values.end() && getSingle(*i1, &val1))
                {
                    uint16_t argc = 2;
                    bytesOfWritten = stream.write(&argc, sizeof(argc));
                    totalBytes += bytesOfWritten;
                    bytesOfWritten = stream.write(&val0, sizeof(val0));
                    totalBytes += bytesOfWritten;
                    bytesOfWritten = stream.write(&val1, sizeof(val1));
                    totalBytes += bytesOfWritten;
                }
                else
                {
                    uint16_t argc = 1;
                    bytesOfWritten = stream.write(&argc, sizeof(argc));
                    totalBytes += bytesOfWritten;
                    bytesOfWritten = stream.write(&val0, sizeof(val0));
                    totalBytes += bytesOfWritten;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "depth_bias does not support \"" + (*i0)->getValue() + "\" for argument 1");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateDepthFunc(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "depth_func must have 1 argument");
        }
        else
        {
            AtomAbstractNode *atom = (AtomAbstractNode*)(prop->values.front().get());
            switch (atom->id)
            {
            case ID_ALWAYS_FAIL:
            case ID_ALWAYS_PASS:
            case ID_LESS:
            case ID_LESS_EQUAL:
            case ID_EQUAL:
            case ID_NOT_EQUAL:
            case ID_GREATER_EQUAL:
            case ID_GREATER:
                {
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                break;
            default:
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid CompareFunction");
                break;
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateIterationDepthBias(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "iteration_depth_bias must have 1 argument");
        }
        else
        {
            float32_t val = 0.0f;
            if (getSingle(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid float value");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateAlphaRejection(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "alpha_rejection must have at most 2 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            AtomAbstractNode *atom0 = (AtomAbstractNode*)((*i0).get());

            int16_t argc = 1;
            uint32_t val = 0;

            if (i1 != prop->values.end())
            {
                if (getUInt(*i1, &val))
                {
                    argc = 2;
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        (*i1)->getValue() + " is not a valid integer");
                }
            }

            bytesOfWritten = stream.write(&argc, sizeof(argc));
            totalBytes += bytesOfWritten;

            switch (atom0->id)
            {
            case ID_ALWAYS_FAIL:
            case ID_ALWAYS_PASS:
            case ID_LESS:
            case ID_LESS_EQUAL:
            case ID_EQUAL:
            case ID_NOT_EQUAL:
            case ID_GREATER_EQUAL:
            case ID_GREATER:
                {
                    uint16_t id = atom0->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                break;
            default:
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    (*i0)->getValue() + " is not a valid CompareFunction");
                break;
            }

            if (argc == 2)
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateAlphaToCoverage(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "alpha_to_coverage must have 1 argument");
        }
        else
        {
            bool val = true;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "alpha_to_coverage argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\"");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateLightScissor(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "light_scissor must have only 1 argument");
        }
        else
        {
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateLightClipPlanes(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "light_clip_planes must have at most 1 argument");
        }
        else
        {
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
            }
        }

        return bytesOfWritten;
    }

    size_t PassTranslator::translateTransparentSorting(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "transparent_sorting must have at most 1 argument");
        }
        else
        {
            bool val = true;
            if (getBoolean(prop->values.front(), &val))
            {
                uint8_t v = val ? 1 : 0;
                bytesOfWritten = stream.write(&v, sizeof(v));
                totalBytes += bytesOfWritten;
            }
            else
            {
                String val2;
                if (getString(prop->values.front(), &val2) && val2 == "force")
                {
                    uint8_t v = 2;
                    bytesOfWritten = stream.write(&v, sizeof(v));
                    totalBytes += bytesOfWritten;
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        prop->values.front()->getValue() + " must be boolean or force");
                }
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateIlluminationStage(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "illumination_stage must have at most 1 argument");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();
                switch (atom->id)
                {
                case ID_AMBIENT:
                case ID_PER_LIGHT:
                case ID_DECAL:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        prop->values.front()->getValue() + " is not a valid IlluminationStage");
                    break;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid IlluminationStage");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateCullHardware(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "cull_hardware must have at most 1 argument");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();
                switch (atom->id)
                {
                case ID_CLOCKWISE:
                case ID_ANTICLOCKWISE:
                case ID_NONE:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        prop->values.front()->getValue() + " is not a valid CullingMode");
                    break;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid CullingMode");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateCullSoftware(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "cull_software must have at most 1 argument");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();
                switch (atom->id)
                {
                case ID_FRONT:
                case ID_BACK:
                case ID_NONE:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        prop->values.front()->getValue() + " is not a valid ManualCullingMode");
                    break;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid ManualCullingMode");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateNormalizeNormals(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "normalise_normals must have at most 1 argument");
        }
        else
        {
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateLighting(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "lighting must have at most 1 argument");
        }
        else
        {
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateShading(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "shading must have at most 1 argument");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();
                switch (atom->id)
                {
                case ID_FLAT:
                case ID_GOURAUD:
                case ID_PHONG:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        prop->values.front()->getValue() + " is not a valid shading mode (flat, gouraud, or phong)");
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid shading mode (flat, gouraud, or phong)");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translatePolygonMode(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "polygon_mode must have at most 1 argument");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();
                switch (atom->id)
                {
                case ID_SOLID:
                case ID_POINTS:
                case ID_WIREFRAME:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        prop->values.front()->getValue() + " is not a valid polygon mode (solid, points, or wireframe)");
                    break;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid polygon mode (solid, points, or wireframe)");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translatePolygonModeOverridable(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "polygon_mode_overrideable must have at most 1 argument");
        }
        else
        {
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateFogOverride(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 8)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "fog_override must have at most 8 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i1 = getNodeAt(prop->values, 1), i2 = getNodeAt(prop->values, 2);
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;

                ColorARGB clr = ColorARGB::WHITE;
                float32_t dens = 0.001, start = 0.0f, end = 1.0f;

                if (i1 != prop->values.end())
                {
                    if ((*i1)->type == ANT_ATOM)
                    {
                        AtomAbstractNode *atom = (AtomAbstractNode*)(*i1).get();
                        switch (atom->id)
                        {
                        case ID_NONE:
                        case ID_LINEAR:
                        case ID_EXP:
                        case ID_EXP2:
                            {
                                uint16_t id = atom->id;
                                bytesOfWritten = stream.write(&id, sizeof(id));
                                totalBytes += bytesOfWritten;
                            }
                            break;
                        default:
                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                (*i1)->getValue() + " is not a valid FogMode");
                            break;
                        }
                    }
                    else
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            (*i1)->getValue() + " is not a valid FogMode");
                        return 0;
                    }
                }

                if (i2 != prop->values.end())
                {
                    if (!getColor(i2, prop->values.end(), &clr, 3))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            (*i2)->getValue() + " is not a valid colour");
                        return 0;
                    }

                    bytesOfWritten = writeColor(clr, stream);
                    totalBytes += bytesOfWritten;

                    i2 = getNodeAt(prop->values, 5);
                }

                if (i2 != prop->values.end())
                {
                    if (!getSingle(*i2, &dens))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            (*i2)->getValue() + " is not a valid number");
                        return 0;
                    }

                    bytesOfWritten = stream.write(&dens, sizeof(dens));
                    totalBytes += bytesOfWritten;

                    ++i2;
                }

                if (i2 != prop->values.end())
                {
                    if (!getSingle(*i2, &start))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            (*i2)->getValue() + " is not a valid number");
                        return 0;
                    }

                    bytesOfWritten = stream.write(&start, sizeof(start));
                    totalBytes += bytesOfWritten;

                    ++i2;
                }

                if (i2 != prop->values.end())
                {
                    if (!getSingle(*i2, &end))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            (*i2)->getValue() + " is not a valid number");
                        return 0;
                    }

                    bytesOfWritten = stream.write(&end, sizeof(end));
                    totalBytes += bytesOfWritten;

                    ++i2;
                }

            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
                return 0;
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateColorWrite(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "colour_write must have at most 1 argument");
        }
        else
        {
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateMaxLights(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "max_lights must have at most 1 argument");
        }
        else
        {
            uint32_t val = 0;
            if (getUInt(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid integer");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translateStartLight(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "start_light must have at most 1 argument");
        }
        else
        {
            uint32_t val = 0;
            if (getUInt(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid integer");
            }
        }

        return bytesOfWritten;
    }

    size_t PassTranslator::translateLightMask(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else
        {
            uint32_t val = 0;
            if (getUInt(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid integer");
            }
        }

        return bytesOfWritten;
    }

    size_t PassTranslator::translateIteration(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0);

            if ((*i0)->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)(*i0).get();
                if (atom->id == ID_ONCE)
                {
                    // once
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                else if (atom->id == ID_ONCE_PER_LIGHT)
                {
                    // once_per_light
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;

                    AbstractNodeList::const_iterator i1 = getNodeAt(prop->values, 1);
                    if (i1 != prop->values.end() && (*i1)->type == ANT_ATOM)
                    {
                        atom = (AtomAbstractNode*)(*i1).get();
                        switch (atom->id)
                        {
                        case ID_POINT:
                        case ID_DIRECTIONAL:
                        case ID_SPOT:
                            {
                                id = atom->id;
                                bytesOfWritten = stream.write(&id, sizeof(id));
                                totalBytes += bytesOfWritten;
                            }
                            break;
                        default:
                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                prop->values.front()->getValue() + " is not a valid light type (point, directional, or spot)");
                            break;
                        }
                    }
                    else
                    {
                        id = 0;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }

                }
                else if (StringConverter::isNumber(atom->value))
                {
                    // number
                    uint16_t id = 0;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;

                    // value of the number
                    int32_t val = StringConverter::parseInt32(atom->value);
                    bytesOfWritten = stream.write(&val, sizeof(val));
                    totalBytes += bytesOfWritten;

                    AbstractNodeList::const_iterator i1 = getNodeAt(prop->values, 1);
                    if (i1 != prop->values.end() && (*i1)->type == ANT_ATOM)
                    {
                        atom = (AtomAbstractNode*)(*i1).get();
                        if (atom->id == ID_PER_LIGHT)
                        {
                            // per_light
                            id = atom->id;
                            bytesOfWritten = stream.write(&id, sizeof(id));
                            totalBytes += bytesOfWritten;

                            // light type
                            AbstractNodeList::const_iterator i2 = getNodeAt(prop->values, 2);
                            if (i2 != prop->values.end() && (*i2)->type == ANT_ATOM)
                            {
                                atom = (AtomAbstractNode*)(*i2).get();
                                switch (atom->id)
                                {
                                case ID_POINT:
                                case ID_DIRECTIONAL:
                                case ID_SPOT:
                                    {
                                        id = atom->id;
                                        bytesOfWritten = stream.write(&id, sizeof(id));
                                        totalBytes += bytesOfWritten;
                                    }
                                    break;
                                default:
                                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                        (*i2)->getValue() + " is not a valid light type (point, directional, or spot)");
                                    break;
                                }
                            }
                            else
                            {
                                id = 0;
                                bytesOfWritten = stream.write(&id, sizeof(id));
                                totalBytes += bytesOfWritten;
                            }
                        }
                        else if (ID_PER_N_LIGHTS)
                        {
                            // per_n_lights
                            id = atom->id;
                            bytesOfWritten = stream.write(&id, sizeof(id));
                            totalBytes += bytesOfWritten;

                            AbstractNodeList::const_iterator i2 = getNodeAt(prop->values, 2);
                            if (i2 != prop->values.end() && (*i2)->type == ANT_ATOM)
                            {
                                atom = (AtomAbstractNode*)(*i2).get();
                                if (StringConverter::isNumber(atom->value))
                                {
                                    // value
                                    int32_t val = StringConverter::parseInt32(atom->value);
                                    bytesOfWritten = stream.write(&val, sizeof(val));
                                    totalBytes += bytesOfWritten;

                                    // light type
                                    AbstractNodeList::const_iterator i3 = getNodeAt(prop->values, 3);
                                    if (i3 != prop->values.end() && (*i3)->type == ANT_ATOM)
                                    {
                                        atom = (AtomAbstractNode*)(*i3).get();
                                        switch (atom->id)
                                        {
                                        case ID_POINT:
                                        case ID_DIRECTIONAL:
                                        case ID_SPOT:
                                            {
                                                id = atom->id;
                                                bytesOfWritten = stream.write(&id, sizeof(id));
                                                totalBytes += bytesOfWritten;
                                            }
                                            break;
                                        default:
                                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                                (*i3)->getValue() + " is not a valid light type (point, directional, or spot)");
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        id = 0;
                                        bytesOfWritten = stream.write(&id, sizeof(id));
                                        totalBytes += bytesOfWritten;
                                    }
                                }
                                else
                                {
                                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                                        (*i2)->getValue() + " is not a valid number");
                                }
                            }
                            else
                            {
                                ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                                    prop->values.front()->getValue() + " is not a valid number");
                            }
                        }
                    }
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translatePointSize(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "point_size must have at most 1 argument");
        }
        else
        {
            float32_t val = 0.0f;
            if (getSingle(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid number");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translatePointSprites(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "point_sprites must have at most 1 argument");
        }
        else
        {
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translatePointSizeAttenuation(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 4)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "point_size_attenuation must have at most 4 arguments");
        }
        else
        {
            bool val = false;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;

                if (val)
                {
                    AbstractNodeList::const_iterator i1 = getNodeAt(prop->values, 1), i2 = getNodeAt(prop->values, 2),
                        i3 = getNodeAt(prop->values, 3);

                    if (prop->values.size() > 1)
                    {
                        Real constant = 0.0f, linear = 1.0f, quadratic = 0.0f;

                        if (i1 != prop->values.end() && (*i1)->type == ANT_ATOM)
                        {
                            AtomAbstractNode *atom = (AtomAbstractNode*)(*i1).get();
                            if (StringConverter::isNumber(atom->value))
                            {
                                constant = StringConverter::parseSingle(atom->value);
                                bytesOfWritten = stream.write(&constant, sizeof(constant));
                                totalBytes += bytesOfWritten;
                            }
                            else
                            {
                                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
                            }
                        }
                        else
                        {
                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                (*i1)->getValue() + " is not a valid number");
                        }

                        if (i2 != prop->values.end() && (*i2)->type == ANT_ATOM)
                        {
                            AtomAbstractNode *atom = (AtomAbstractNode*)(*i2).get();
                            if (StringConverter::isNumber(atom->value))
                            {
                                linear = StringConverter::parseSingle(atom->value);
                                bytesOfWritten = stream.write(&linear, sizeof(linear));
                                totalBytes += bytesOfWritten;
                            }
                            else
                            {
                                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
                            }
                        }
                        else
                        {
                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                (*i2)->getValue() + " is not a valid number");
                        }

                        if (i3 != prop->values.end() && (*i3)->type == ANT_ATOM)
                        {
                            AtomAbstractNode *atom = (AtomAbstractNode*)(*i3).get();
                            if (StringConverter::isNumber(atom->value))
                            {
                                quadratic = StringConverter::parseSingle(atom->value);
                                bytesOfWritten = stream.write(&quadratic, sizeof(quadratic));
                                totalBytes += bytesOfWritten;
                            }
                            else
                            {
                                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
                            }
                        }
                        else
                        {
                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                (*i3)->getValue() + " is not a valid number");
                        }
                    }
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid boolean");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translatePointSizeMin(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "point_size_min must have at most 1 argument");
        }
        else
        {
            float32_t val = 0.0f;
            if (getSingle(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid number");
            }
        }

        return totalBytes;
    }

    size_t PassTranslator::translatePointSizeMax(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "point_size_max must have at most 1 argument");
        }
        else
        {
            float32_t val = 0.0f;
            if (getSingle(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid number");
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node)
    {
        ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(node.get());

        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        // 对象头数据
        bytesOfWritten = translateObjectHeader(obj, stream);
        totalBytes += bytesOfWritten;

        Real fval;
        uint32_t uival;
        String sval;

        // Set the properties for the material
        for (AbstractNodeList::iterator i = obj->children.begin(); i != obj->children.end(); ++i)
        {
            if ((*i)->type == ANT_PROPERTY)
            {
                PropertyAbstractNode *prop = static_cast<PropertyAbstractNode*>((*i).get());

                // ID
                uint16_t id = prop->id;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // 属性
                switch (prop->id)
                {
                case ID_TEX_ADDRESS_MODE:
                case ID_TEX_BORDER_COLOUR:
                case ID_FILTERING:
                case ID_CMPTEST:
                case ID_CMPFUNC:
                case ID_COMP_FUNC:
                case ID_MAX_ANISOTROPY:
                case ID_MIPMAP_BIAS:
                    {
                        SamplerTranslator *translator = static_cast<SamplerTranslator*>(compiler->getTranslator(*i));
                        bytesOfWritten = translator->translateSamplerParams(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SAMPLER_REF:
                    {
                        bytesOfWritten = translateSamplerRef(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_TEXTURE_ALIAS:
                    {
                        bytesOfWritten = translateTextureAlias(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_TEXTURE:
                    {
                        bytesOfWritten = translateTexture(compiler, prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ANIM_TEXTURE:
                    {
                        bytesOfWritten = translateAnimTexture(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_CUBIC_TEXTURE:
                    {
                        bytesOfWritten = translateCubicTexture(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_TEX_COORD_SET:
                    {
                        bytesOfWritten = translateTexCoordSet(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_COLOUR_OP:
                    {
                        bytesOfWritten = translateColorOp(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_COLOUR_OP_EX:
                    {
                        bytesOfWritten = translateColorOpEx(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_COLOUR_OP_MULTIPASS_FALLBACK:
                    {
                        bytesOfWritten = translateColorOpMultiPassFallback(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ALPHA_OP_EX:
                    {
                        bytesOfWritten = translateAlphaOpEx(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ENV_MAP:
                    {
                        bytesOfWritten = translateEnvMap(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SCROLL:
                    {
                        bytesOfWritten = translateScroll(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SCROLL_ANIM:
                    {
                        bytesOfWritten = translateScrollAnim(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ROTATE:
                    {
                        bytesOfWritten = translateRotate(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_ROTATE_ANIM:
                    {
                        bytesOfWritten = translateRotateAnim(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_SCALE:
                    {
                        bytesOfWritten = translateScale(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_WAVE_XFORM:
                    {
                        bytesOfWritten = translateWaveXform(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_TRANSFORM:
                    {
                        bytesOfWritten = translateTransform(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_BINDING_TYPE:
                    {
                        bytesOfWritten = translateBindingType(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_CONTENT_TYPE:
                    {
                        bytesOfWritten = translateContentType(prop, stream);
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_UNEXPECTEDTOKEN, prop->name, prop->file, prop->line,
                        "token \"" + prop->name + "\" is not recognized");
                }
            }
            else if ((*i)->type == ANT_OBJECT)
            {
                bytesOfWritten = processNode(compiler, stream, *i);
                totalBytes += bytesOfWritten;
            }
        }
    
        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateSamplerRef(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "sampler_ref must have at most 1 argument");
        }
        else
        {
            String val;
            if (getString(prop->values.front(), &val))
            {
                bytesOfWritten = writeString(val, stream);
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_REFERENCETOANONEXISTINGOBJECT,
                    prop->name, prop->file, prop->line);
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateTextureAlias(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "texture_alias must have at most 1 argument");
        }
        else
        {
            String val;
            if (getString(prop->values.front(), &val))
            {
                bytesOfWritten = writeString(val, stream);
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_REFERENCETOANONEXISTINGOBJECT,
                    prop->name, prop->file, prop->line);
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateTexture(ScriptCompiler *compiler, PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 5)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "texture must have at most 5 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator j = prop->values.begin();
            String val;
            if (getString(*j, &val))
            {
                bytesOfWritten = writeString(val, stream);
                totalBytes += bytesOfWritten;

                ++j;

                while (j != prop->values.end())
                {
                    if ((*j)->type == ANT_ATOM)
                    {
                        AtomAbstractNode *atom = (AtomAbstractNode*)(*j).get();

                        switch (atom->id)
                        {
                        case ID_1D:
                        case ID_2D:
                        case ID_3D:
                        case ID_CUBIC:
                        case ID_2DARRAY:
                        case ID_UNLIMITED:
                        case ID_ALPHA:
                        case ID_GAMMA:
                            {
                                uint16_t id = atom->id;
                                bytesOfWritten = stream.write(&id, sizeof(id));
                                totalBytes += bytesOfWritten;
                            }
                            break;
                        default:
                            {
                                if (StringConverter::isNumber(atom->value))
                                {
                                    uint16_t mipmaps = StringConverter::parseInt32(atom->value);
                                    bytesOfWritten = stream.write(&mipmaps, sizeof(mipmaps));
                                    totalBytes += bytesOfWritten;
                                }
                                else
                                {
                                    uint16_t id = ID_PIXELFORMAT;
                                    bytesOfWritten = stream.write(&id, sizeof(id));
                                    totalBytes += bytesOfWritten;

                                    auto r = compiler->mPixelFormat.find(atom->value);
                                    if (r != compiler->mPixelFormat.end())
                                    {
                                        uint32_t pf = r->second;
                                        bytesOfWritten = stream.write(&pf, sizeof(pf));
                                        totalBytes += bytesOfWritten;
                                    }
                                    else
                                    {
                                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                            (*j)->getValue() + " is not a supported argument to the texture property");
                                    }
                                }
                            }
                            break;
                        }
                    }
                    else
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            (*j)->getValue() + " is not a supported argument to the texture property");
                    }
                    ++j;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    (*j)->getValue() + " is not a valid texture name");
            }   
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateAnimTexture(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.size() < 3)
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else
        {
            AbstractNodeList::const_iterator i1 = getNodeAt(prop->values, 1);

            if ((*i1)->type == ANT_ATOM && StringConverter::isNumber(((AtomAbstractNode*)(*i1).get())->value))
            {
                // Short form
                uint16_t type = 0;
                bytesOfWritten = stream.write(&type, sizeof(type));
                totalBytes += bytesOfWritten;

                AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i2 = getNodeAt(prop->values, 2);

                if ((*i0)->type == ANT_ATOM && (*i1)->type == ANT_ATOM)
                {
                    String val0;
                    uint32_t val1;
                    float32_t val2;
                    if (getString(*i0, &val0) && getUInt(*i1, &val1) && getSingle(*i2, &val2))
                    {
                        // name
                        bytesOfWritten = writeString(val0, stream);
                        totalBytes += bytesOfWritten;

                        // numFrames
                        uint16_t frames = val1;
                        bytesOfWritten = stream.write(&frames, sizeof(frames));
                        totalBytes += bytesOfWritten;

                        // duration
                        bytesOfWritten = stream.write(&val2, sizeof(val2));
                        totalBytes += bytesOfWritten;
                    }
                    else
                    {
                        ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                            "anim_texture short form requires a texture name, number of frames, and animation duration");
                    }
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "anim_texture short form requires a texture name, number of frames, and animation duration");
                }
            }
            else
            {
                // Long form has n number of frames
                uint16_t type = 1;
                bytesOfWritten = stream.write(&type, sizeof(type));
                totalBytes += bytesOfWritten;

                // number of frames
                uint16_t frames = prop->values.size() - 1;
                bytesOfWritten = stream.write(&frames, sizeof(frames));
                totalBytes += bytesOfWritten;

                float32_t duration = 0;
                AbstractNodeList::const_iterator in = getNodeAt(prop->values, static_cast<int>(prop->values.size()) - 1);

                if (getSingle(*in, &duration))
                {
                    // duration
                    bytesOfWritten = stream.write(&duration, sizeof(duration));
                    totalBytes += bytesOfWritten;

                    // names
                    AbstractNodeList::iterator j = prop->values.begin();
                    while (j != in)
                    {
                        if ((*j)->type == ANT_ATOM)
                        {
                            String name = ((AtomAbstractNode*)(*j).get())->value;
                            bytesOfWritten = writeString(name, stream);
                            totalBytes += bytesOfWritten;
                        }
                        else
                        {
                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                (*j)->getValue() + " is not supported as a texture name");
                        }
                        ++j;
                    }
                }
                else
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        (*in)->getValue() + " is not supported for the duration argument");
                }
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateCubicTexture(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 2)
        {
            // type
            uint16_t type = 2;
            bytesOfWritten = stream.write(&type, sizeof(type));
            totalBytes += bytesOfWritten;

            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0),
                i1 = getNodeAt(prop->values, 1);

            if ((*i0)->type == ANT_ATOM && (*i1)->type == ANT_ATOM)
            {
                AtomAbstractNode *atom0 = (AtomAbstractNode*)(*i0).get();
                AtomAbstractNode *atom1 = (AtomAbstractNode*)(*i1).get();

                String name;
                if (getString(*i0, &name))
                {
                    bytesOfWritten = writeString(name, stream);
                    totalBytes += bytesOfWritten;

                    uint16_t id = atom1->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                else
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
            }
        }
        else if (prop->values.size() == 7)
        {
            // type
            uint16_t type = 7;
            bytesOfWritten = stream.write(&type, sizeof(type));
            totalBytes += bytesOfWritten;

            AbstractNodeList::const_iterator 
                i0 = getNodeAt(prop->values, 0),
                i1 = getNodeAt(prop->values, 1),
                i2 = getNodeAt(prop->values, 2),
                i3 = getNodeAt(prop->values, 3),
                i4 = getNodeAt(prop->values, 4),
                i5 = getNodeAt(prop->values, 5),
                i6 = getNodeAt(prop->values, 6);
            if ((*i0)->type == ANT_ATOM && (*i1)->type == ANT_ATOM 
                && (*i2)->type == ANT_ATOM && (*i3)->type == ANT_ATOM 
                && (*i4)->type == ANT_ATOM && (*i5)->type == ANT_ATOM 
                && (*i6)->type == ANT_ATOM)
            {
                AtomAbstractNode *atom0 = (AtomAbstractNode*)(*i0).get(), 
                    *atom1 = (AtomAbstractNode*)(*i1).get(),
                    *atom2 = (AtomAbstractNode*)(*i2).get(), 
                    *atom3 = (AtomAbstractNode*)(*i3).get(),
                    *atom4 = (AtomAbstractNode*)(*i4).get(), 
                    *atom5 = (AtomAbstractNode*)(*i5).get(),
                    *atom6 = (AtomAbstractNode*)(*i6).get();

                bytesOfWritten = writeString(atom0->value, stream);
                totalBytes += bytesOfWritten;

                bytesOfWritten = writeString(atom1->value, stream);
                totalBytes += bytesOfWritten;
                    
                bytesOfWritten = writeString(atom2->value, stream);
                totalBytes += bytesOfWritten;

                bytesOfWritten = writeString(atom3->value, stream);
                totalBytes += bytesOfWritten;

                bytesOfWritten = writeString(atom4->value, stream);
                totalBytes += bytesOfWritten;

                bytesOfWritten = writeString(atom5->value, stream);
                totalBytes += bytesOfWritten;
            }

        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "cubic_texture must have at most 7 arguments");
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateTexCoordSet(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            uint32_t val = 0;
            if (getUInt(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
            }
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "tex_coord_set must have only 1 arguments");
        }
        
        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateColorOp(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            AtomAbstractNode *atom = (AtomAbstractNode *)(prop->values.front()).get();
            uint16_t id = atom->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "colour_op must have only 1 arguments");
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateColorOpEx(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.size() < 3)
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line,
                "colour_op_ex must have at least 3 arguments");
        }
        else if (prop->values.size() > 6)
        {
            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                "colour_op_ex must have at most 10 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0),
                i1 = getNodeAt(prop->values, 1),
                i2 = getNodeAt(prop->values, 2);

            AtomAbstractNode *atom0 = (AtomAbstractNode *)(*i0).get();
            AtomAbstractNode *atom1 = (AtomAbstractNode *)(*i1).get();
            AtomAbstractNode *atom2 = (AtomAbstractNode *)(*i2).get();

            // op
            uint16_t id = atom0->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;

            // source1
            id = atom1->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;

            // source2
            id = atom2->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;

            if (atom0->id == ID_BLEND_MANUAL)
            {
                AbstractNodeList::const_iterator i3 = getNodeAt(prop->values, 3);
                if (i3 != prop->values.end())
                {
                    float32_t manualBlend;

                    if (!getSingle(*i3, &manualBlend))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            (*i3)->getValue() + " is not a valid number argument");
                    }
                    else
                    {
                        bytesOfWritten = stream.write(&manualBlend, sizeof(manualBlend));
                        totalBytes += bytesOfWritten;
                    }
                }
                else
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        "fourth argument expected when blend_manual is used");
                }
            }
            
            AbstractNodeList::const_iterator j = getNodeAt(prop->values, 3);
            if (atom0->id == ID_BLEND_MANUAL)
                j++;

            ColorARGB arg1 = ColorARGB::WHITE, arg2 = ColorARGB::WHITE;

            if (atom1->id == ID_SRC_MANUAL)
            {
                if (j != prop->values.end())
                {
                    if (!getColor(j, prop->values.end(), &arg1, 3))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            "valid colour expected when src_manual is used");
                    }
                    else
                    {
                        bytesOfWritten = writeColor(arg1, stream);
                        totalBytes += bytesOfWritten;
                    }
                }
                else
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        "valid colour expected when src_manual is used");
                }
            }

            if (atom2->id == ID_SRC_MANUAL)
            {
                if (j != prop->values.end())
                {
                    if (!getColor(j, prop->values.end(), &arg2, 3))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            "valid colour expected when src_manual is used");
                    }
                    else
                    {
                        bytesOfWritten = writeColor(arg2, stream);
                        totalBytes += bytesOfWritten;
                    }
                }
                else
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        "valid colour expected when src_manual is used");
                }
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateColorOpMultiPassFallback(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "colour_op_multiplass_fallback must have at most 2 arguments");
        }
        else if (prop->values.size() == 1)
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();
                switch (atom->id)
                {
                case ID_ADD:
                case ID_MODULATE:
                case ID_COLOUR_BLEND:
                case ID_ALPHA_BLEND:
                case ID_REPLACE:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "argument must be a valid scene blend type (add, modulate, colour_blend, alpha_blend, or replace)");
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "argument must be a valid scene blend type (add, modulate, colour_blend, alpha_blend, or replace)");
            }
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            AtomAbstractNode *atom0 = (AtomAbstractNode *)(*i0).get();
            AtomAbstractNode *atom1 = (AtomAbstractNode *)(*i1).get();
            
            uint16_t id = atom0->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;

            id = atom1->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateAlphaOpEx(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.size() < 3)
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line,
                "alpha_op_ex must have at least 3 arguments");
        }
        else if (prop->values.size() > 6)
        {
            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                "alpha_op_ex must have at most 10 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0),
                i1 = getNodeAt(prop->values, 1),
                i2 = getNodeAt(prop->values, 2);

            AtomAbstractNode *atom0 = (AtomAbstractNode *)(*i0).get();
            AtomAbstractNode *atom1 = (AtomAbstractNode *)(*i1).get();
            AtomAbstractNode *atom2 = (AtomAbstractNode *)(*i2).get();

            // op
            uint16_t id = atom0->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;

            // source1
            id = atom1->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;

            // source2
            id = atom2->id;
            bytesOfWritten = stream.write(&id, sizeof(id));
            totalBytes += bytesOfWritten;

            if (atom0->id == ID_BLEND_MANUAL)
            {
                AbstractNodeList::const_iterator i3 = getNodeAt(prop->values, 3);
                if (i3 != prop->values.end())
                {
                    float32_t manualBlend;

                    if (!getSingle(*i3, &manualBlend))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            (*i3)->getValue() + " is not a valid number argument");
                    }
                    else
                    {
                        bytesOfWritten = stream.write(&manualBlend, sizeof(manualBlend));
                        totalBytes += bytesOfWritten;
                    }
                }
                else
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        "fourth argument expected when blend_manual is used");
                }
            }

            AbstractNodeList::const_iterator j = getNodeAt(prop->values, 3);
            if (atom0->id == ID_BLEND_MANUAL)
                j++;

            float32_t arg1, arg2;

            if (atom1->id == ID_SRC_MANUAL)
            {
                if (j != prop->values.end())
                {
                    if (!getSingle(*j, &arg1))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            "valid colour expected when src_manual is used");
                    }
                    else
                    {
                        bytesOfWritten = stream.write(&arg1, sizeof(arg1));
                        totalBytes += bytesOfWritten;
                        ++j;
                    }
                }
                else
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        "valid colour expected when src_manual is used");
                }
            }

            if (atom2->id == ID_SRC_MANUAL)
            {
                if (j != prop->values.end())
                {
                    if (!getSingle(*j, &arg2))
                    {
                        ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                            "valid colour expected when src_manual is used");
                    }
                    else
                    {
                        bytesOfWritten = stream.write(&arg2, sizeof(arg2));
                        totalBytes += bytesOfWritten;
                    }
                }
                else
                {
                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line,
                        "valid colour expected when src_manual is used");
                }
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateEnvMap(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 1)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "env_map must have at most 1 argument");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();

                switch (atom->id)
                {
                case ID_OFF:
                case ID_SPHERICAL:
                case ID_PLANAR:
                case ID_CUBIC_REFLECTION:
                case ID_CUBIC_NORMAL:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        prop->values.front()->getValue() + " is not a valid argument (must be \"off\", \"spherical\", \"planar\", \"cubic_reflection\", or \"cubic_normal\")");
                    break;
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid argument (must be \"off\", \"spherical\", \"planar\", \"cubic_reflection\", or \"cubic_normal\")");
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateScroll(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "scroll must have at most 2 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            float32_t x, y;
            if (getSingle(*i0, &x) && getSingle(*i1, &y))
            {
                bytesOfWritten = stream.write(&x, sizeof(x));
                totalBytes += bytesOfWritten;
                bytesOfWritten = stream.write(&y, sizeof(y));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    (*i0)->getValue() + " and/or " + (*i1)->getValue() + " is invalid; both must be numbers");
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateScrollAnim(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "scroll_anim must have at most 2 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            float32_t x, y;
            if (getSingle(*i0, &x) && getSingle(*i1, &y))
            {
                bytesOfWritten = stream.write(&x, sizeof(x));
                totalBytes += bytesOfWritten;
                bytesOfWritten = stream.write(&y, sizeof(y));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    (*i0)->getValue() + " and/or " + (*i1)->getValue() + " is invalid; both must be numbers");
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateRotate(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            float32_t val;
            if (getSingle(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
            }
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "rotate must have only 1 arguments");
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateRotateAnim(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            float32_t val;
            if (getSingle(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
            }
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "rotate_anim must have only 1 arguments");
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateScale(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 2)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "scale must have at most 2 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1);
            float32_t x, y;
            if (getSingle(*i0, &x) && getSingle(*i1, &y))
            {
                bytesOfWritten = stream.write(&x, sizeof(x));
                totalBytes += bytesOfWritten;
                bytesOfWritten = stream.write(&y, sizeof(y));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "first and second arguments must both be valid number values (received " + (*i0)->getValue() + ", " + (*i1)->getValue() + ")");
            }
        }
        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateWaveXform(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 6)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "wave_xform must have at most 6 arguments");
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0), i1 = getNodeAt(prop->values, 1),
                i2 = getNodeAt(prop->values, 2), i3 = getNodeAt(prop->values, 3),
                i4 = getNodeAt(prop->values, 4), i5 = getNodeAt(prop->values, 5);
            if ((*i0)->type == ANT_ATOM && (*i1)->type == ANT_ATOM && (*i2)->type == ANT_ATOM &&
                (*i3)->type == ANT_ATOM && (*i4)->type == ANT_ATOM && (*i5)->type == ANT_ATOM)
            {
                AtomAbstractNode *atom0 = (AtomAbstractNode*)(*i0).get(), *atom1 = (AtomAbstractNode*)(*i1).get();

                float32_t base = 0.0f, freq = 0.0f, phase = 0.0f, amp = 0.0f;

                switch (atom0->id)
                {
                case ID_SCROLL_X:
                case ID_SCROLL_Y:
                case ID_SCALE_X:
                case ID_SCALE_Y:
                case ID_ROTATE:
                    {
                        uint16_t id = atom0->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom0->value + " is not a valid transform type (must be \"scroll_x\", \"scroll_y\", \"scale_x\", \"scale_y\", or \"rotate\")");
                    break;
                }

                switch (atom1->id)
                {
                case ID_SINE:
                case ID_TRIANGLE:
                case ID_SQUARE:
                case ID_SAWTOOTH:
                case ID_INVERSE_SAWTOOTH:
                    {
                        uint16_t id = atom1->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom1->value + " is not a valid waveform type (must be \"sine\", \"triangle\", \"square\", \"sawtooth\", or \"inverse_sawtooth\")");
                }

                if (!getSingle(*i2, &base) || !getSingle(*i3, &freq) 
                    || !getSingle(*i4, &phase) || !getSingle(*i5, &amp))
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        "arguments 3, 4, 5, and 6 must be valid numbers; received " + (*i2)->getValue() + ", " + (*i3)->getValue() + ", " + (*i4)->getValue() + ", " + (*i5)->getValue());

                bytesOfWritten = stream.write(&base, sizeof(base));
                totalBytes += bytesOfWritten;
                bytesOfWritten = stream.write(&freq, sizeof(freq));
                totalBytes += bytesOfWritten;
                bytesOfWritten = stream.write(&phase, sizeof(phase));
                totalBytes += bytesOfWritten;
                bytesOfWritten = stream.write(&amp, sizeof(amp));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateTransform(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        Matrix4 m;
        if (getMatrix4(prop->values.begin(), prop->values.end(), &m))
        {
            float val = m[0][0];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[0][1];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[0][2];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[0][3];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[1][0];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[1][1];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[1][2];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[1][3];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[2][0];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[2][1];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[2][2];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[2][3];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[3][0];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[3][1];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[3][2];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
            val = m[3][3];
            bytesOfWritten = stream.write(&val, sizeof(val));
            totalBytes += bytesOfWritten;
        }
        else
        {
            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateBindingType(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();

            switch (atom->id)
            {
            case ID_FRAGMENT:
            case ID_VERTEX:
                {
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                break;
            default:
                {
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom->value + " is not a valid binding type (must be \"fragment\", \"vertex\")");
                }
                break;
            }
        }
        else
        {
            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line);
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t TextureUnitTranslator::translateContentType(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() > 4)
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "content_type must have at most 4 arguments");
        }
        else
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();
                switch (atom->id)
                {
                case ID_NAMED:
                case ID_SHADOW:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                case ID_COMPOSITOR:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;

                        if (prop->values.size() >= 3)
                        {
                            uint16_t argc = prop->values.size() - 1;
                            bytesOfWritten = stream.write(&argc, sizeof(argc));
                            totalBytes += bytesOfWritten;

                            String compositorName;
                            getString(*getNodeAt(prop->values, 1), &compositorName);
                            bytesOfWritten = writeString(compositorName, stream);
                            totalBytes += bytesOfWritten;

                            String textureName;
                            getString(*getNodeAt(prop->values, 2), &textureName);
                            bytesOfWritten = writeString(textureName, stream);
                            totalBytes += bytesOfWritten;

                            if (prop->values.size() == 4)
                            {
                                uint32_t mrtIndex;
                                if (getUInt(*getNodeAt(prop->values, 3), (uint32_t*)&mrtIndex))
                                {
                                    bytesOfWritten = stream.write(&mrtIndex, sizeof(mrtIndex));
                                    totalBytes += bytesOfWritten;
                                }
                                else
                                {
                                    ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
                                }
                            }
                        }
                        else
                        {
                            ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                                "content_type compositor must have an additional 2 or 3 parameters");
                        }
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom->value + " is not a valid content type (must be \"named\" or \"shadow\" or \"compositor\")");
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " is not a valid content type");
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node)
    {
        ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(node.get());

        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        // 对象头数据
        bytesOfWritten = translateObjectHeader(obj, stream);
        totalBytes += bytesOfWritten;

        // Set the properties for the material
        for (AbstractNodeList::iterator i = obj->children.begin(); i != obj->children.end(); ++i)
        {
            if ((*i)->type == ANT_PROPERTY)
            {
                PropertyAbstractNode *prop = static_cast<PropertyAbstractNode*>((*i).get());

                // ID
                uint16_t id = prop->id;
                bytesOfWritten = stream.write(&id, sizeof(id));
                totalBytes += bytesOfWritten;

                // 属性
                switch (prop->id)
                {
                case ID_TEX_ADDRESS_MODE:
                case ID_TEX_BORDER_COLOUR:
                case ID_FILTERING:
                case ID_CMPTEST:
                case ID_COMP_FUNC:
                case ID_MAX_ANISOTROPY:
                case ID_MIPMAP_BIAS:
                    bytesOfWritten = translateSamplerParams(prop, stream);
                    totalBytes += bytesOfWritten;
                    break;
                default:
                    ScriptError::printError(CERR_UNEXPECTEDTOKEN, prop->name, prop->file, prop->line,
                        "token \"" + prop->name + "\" is not recognized");
                }
            }
            else if ((*i)->type == ANT_OBJECT)
            {
                bytesOfWritten = processNode(compiler, stream, *i);
                totalBytes += bytesOfWritten;
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translateSamplerParams(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        switch (prop->id)
        {
        case ID_TEX_ADDRESS_MODE:
            {
                bytesOfWritten = translateTexAddressMode(prop, stream);
                totalBytes += bytesOfWritten;
            }
            break;
        case ID_TEX_BORDER_COLOUR:
            {
                bytesOfWritten = translateTexBorderColor(prop, stream);
                totalBytes += bytesOfWritten;
            }
            break;
        case ID_FILTERING:
            {
                bytesOfWritten = translateFiltering(prop, stream);
                totalBytes += bytesOfWritten;
            }
            break;
        case ID_CMPTEST:
            {
                bytesOfWritten = translateCompareTest(prop, stream);
                totalBytes += bytesOfWritten;
            }
            break;
        case ID_CMPFUNC:
            ScriptError::printError(CERR_DEPRECATEDSYMBOL, prop->name, prop->file,
                prop->line,
                "compare_func. Use comp_func.");
        case ID_COMP_FUNC:
            {
                bytesOfWritten = translateCompareFunc(prop, stream);
                totalBytes += bytesOfWritten;
            }
            break;
        case ID_MAX_ANISOTROPY:
            {
                bytesOfWritten = translateMaxAnisotropy(prop, stream);
                totalBytes += bytesOfWritten;
            }
            break;
        case ID_MIPMAP_BIAS:
            {
                bytesOfWritten = translateMipmapBias(prop, stream);
                totalBytes += bytesOfWritten;
            }
            break;
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translateTexAddressMode(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else
        {
            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0),
                i1 = getNodeAt(prop->values, 1),
                i2 = getNodeAt(prop->values, 2);
            float32_t u, v, w;

            uint16_t argc = prop->values.size();
            bytesOfWritten = stream.write(&argc, sizeof(argc));
            totalBytes += bytesOfWritten;

            AtomAbstractNode *atom = (AtomAbstractNode *)(*i0).get();

            switch (atom->id)
            {
            case ID_WRAP:
            case ID_MIRROR:
            case ID_CLAMP:
            case ID_BORDER:
                {
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                break;
            default:
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    atom->getValue() + " not supported as first argument (must be \"wrap\", \"clamp\", \"mirror\", or \"border\")");
                break;
            }

            if (i1 != prop->values.end())
            {
                atom = (AtomAbstractNode *)(*i1).get();

                switch (atom->id)
                {
                case ID_WRAP:
                case ID_MIRROR:
                case ID_CLAMP:
                case ID_BORDER:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom->getValue() + " not supported as second argument (must be \"wrap\", \"clamp\", \"mirror\", or \"border\")");
                    break;
                }
            }


            if (i2 != prop->values.end())
            {
                atom = (AtomAbstractNode *)(*i2).get();

                switch (atom->id)
                {
                case ID_WRAP:
                case ID_MIRROR:
                case ID_CLAMP:
                case ID_BORDER:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        atom->getValue() + " not supported as third argument (must be \"wrap\", \"clamp\", \"mirror\", or \"border\")");
                    break;
                }
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translateTexBorderColor(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else
        {
            ColorARGB val;
            if (getColor(prop->values.begin(), prop->values.end(), &val))
            {
                bytesOfWritten = writeColor(val, stream);
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "tex_border_colour only accepts a colour argument");
            }
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translateFiltering(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_STRINGEXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            if (prop->values.front()->type == ANT_ATOM)
            {
                uint16_t argc = 1;
                bytesOfWritten = stream.write(&argc, sizeof(argc));
                totalBytes += bytesOfWritten;

                AtomAbstractNode *atom = (AtomAbstractNode*)prop->values.front().get();

                switch (atom->id)
                {
                case ID_NONE:
                case ID_BILINEAR:
                case ID_TRILINEAR:
                case ID_ANISOTROPIC:
                    {
                        uint16_t id = atom->id;
                        bytesOfWritten = stream.write(&id, sizeof(id));
                        totalBytes += bytesOfWritten;
                    }
                    break;
                default:
                    ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                        prop->values.front()->getValue() + " not supported as first argument (must be \"none\", \"bilinear\", \"trilinear\", or \"anisotropic\")");
                }
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " not supported as first argument (must be \"none\", \"bilinear\", \"trilinear\", or \"anisotropic\")");
            }
        }
        else if (prop->values.size() == 3)
        {
            uint16_t argc = 3;
            bytesOfWritten = stream.write(&argc, sizeof(argc));
            totalBytes += bytesOfWritten;

            AbstractNodeList::const_iterator i0 = getNodeAt(prop->values, 0),
                i1 = getNodeAt(prop->values, 1),
                i2 = getNodeAt(prop->values, 2);

            AtomAbstractNode *atom = (AtomAbstractNode *)(*i0).get();

            switch (atom->id)
            {
            case ID_NONE:
            case ID_LINEAR:
            case ID_BILINEAR:
            case ID_TRILINEAR:
            case ID_ANISOTROPIC:
                {
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                break;
            default:
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " not supported as first argument (must be \"none\", \"bilinear\", \"trilinear\", or \"anisotropic\")");
            }

            atom = (AtomAbstractNode *)(*i1).get();
            switch (atom->id)
            {
            case ID_NONE:
            case ID_LINEAR:
            case ID_BILINEAR:
            case ID_TRILINEAR:
            case ID_ANISOTROPIC:
                {
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                break;
            default:
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " not supported as second argument (must be \"none\", \"bilinear\", \"trilinear\", or \"anisotropic\")");
            }

            atom = (AtomAbstractNode *)(*i2).get();
            switch (atom->id)
            {
            case ID_NONE:
            case ID_LINEAR:
            case ID_BILINEAR:
            case ID_TRILINEAR:
            case ID_ANISOTROPIC:
                {
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                break;
            default:
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " not supported as third argument (must be \"none\", \"bilinear\", \"trilinear\", or \"anisotropic\")");
            }
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "filtering must have either 1 or 3 arguments");
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translateCompareTest(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            bool val;
            if (getBoolean(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "compare_test only accepts a boolean argument");
            }
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "compare_test must have only 1 argument");
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translateCompareFunc(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            AtomAbstractNode *atom = (AtomAbstractNode *)prop->values.front().get();

            switch (atom->id)
            {
            case ID_ALWAYS_FAIL:
            case ID_ALWAYS_PASS:
            case ID_LESS_EQUAL:
            case ID_LESS:
            case ID_EQUAL:
            case ID_NOT_EQUAL:
            case ID_GREATER_EQUAL:
            case ID_GREATER:
                {
                    uint16_t id = atom->id;
                    bytesOfWritten = stream.write(&id, sizeof(id));
                    totalBytes += bytesOfWritten;
                }
                break;
            default:
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    prop->values.front()->getValue() + " not supported as argument (must be \"always_fail\", \"always_pass\", \"less_equal\", \"equal\", \"not_equal\", \"greater_equal\", or \"greater\")");
            }
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "comp_func must have only 1 argument");
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translateMaxAnisotropy(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            float32_t val;
            if (getSingle(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "max_anisotropy only accepts a float argument");
            }
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "max_anisotropy must have only 1 argument");
        }

        return totalBytes;
    }

    //--------------------------------------------------------------------------

    size_t SamplerTranslator::translateMipmapBias(PropertyAbstractNode *prop, DataStream &stream)
    {
        size_t bytesOfWritten = 0;
        size_t totalBytes = 0;

        if (prop->values.empty())
        {
            ScriptError::printError(CERR_NUMBEREXPECTED, prop->name, prop->file, prop->line);
        }
        else if (prop->values.size() == 1)
        {
            float32_t val;
            if (getSingle(prop->values.front(), &val))
            {
                bytesOfWritten = stream.write(&val, sizeof(val));
                totalBytes += bytesOfWritten;
            }
            else
            {
                ScriptError::printError(CERR_INVALIDPARAMETERS, prop->name, prop->file, prop->line,
                    "mipmap_bias only accepts a float argument");
            }
        }
        else
        {
            ScriptError::printError(CERR_FEWERPARAMETERSEXPECTED, prop->name, prop->file, prop->line,
                "mipmap_bias must have only 1 argument");
        }

        return bytesOfWritten;
    }
}

