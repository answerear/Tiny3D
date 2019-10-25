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


#include "Resource/T3DMaterial.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DScriptParser.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MaterialPtr Material::create(const String &name, MaterialType type)
    {
        MaterialPtr material = new Material(name, type);
        material->release();
        return material;
    }

    //--------------------------------------------------------------------------

    Material::Material(const String &name, MaterialType type)
        : Resource(name)
        , mMaterialType(type)
    {

    }

    //--------------------------------------------------------------------------

    Material::~Material()
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type Material::getType() const
    {
        return E_RT_MATERIAL;
    }

    //--------------------------------------------------------------------------

    TResult Material::load()
    {
        TResult ret = T3D_OK;

        do 
        {
            if (E_MT_DEFAULT == mMaterialType)
            {
                // 默认类型，从文件加载

                // 加载材质文件
                ArchivePtr archive = T3D_AGENT.getAssetsArchive(mName);
                if (archive == nullptr)
                {
                    ret = T3D_ERR_IMG_NOT_FOUND;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Could not find the archive for file %s !", 
                        mName.c_str());
                    break;
                }

                String path = T3D_AGENT.getMainAssetsPath(mName);
                MemoryDataStream stream;
                ret = archive->read(path, stream);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read material content failed from file %s ! ", 
                        mName.c_str());
                    break;
                }

                // 交给脚本解析器解析
                ret = ScriptParser::getInstance().parse(stream, *this);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Could not parse material file %s !", mName);
                    break;
                }
            }
            else if (E_MT_MANUAL == mMaterialType)
            {
                // 手动创建，不从文件加载
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_TYPE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid material type !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Material::unload()
    {
        return Resource::unload();
    }

    //--------------------------------------------------------------------------

    ResourcePtr Material::clone() const
    {
        return Material::create(mName, mMaterialType);
    }
}
