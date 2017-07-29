/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#include "Resource/T3DModel.h"
#include "Misc/T3DModelData.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DXMLModelSerializer.h"
#include "Resource/T3DBinModelSerializer.h"


#include <sstream>


namespace Tiny3D
{
    ModelPtr Model::create(const String &name)
    {
        ModelPtr model = new Model(name);
        model->release();
        return model;
    }

    Model::Model(const String &name)
        : Resource(name)
    {

    }

    Model::~Model()
    {

    }

    Resource::Type Model::getType() const
    {
        return E_TYPE_MODEL;
    }

    bool Model::load()
    {
        bool ret = false;

        ArchivePtr archive;
        MemoryDataStream stream;

        if (T3D_ARCHIVE_MGR.getArchive(mName, archive))
        {
            if (archive->read(mName, stream))
            {
                FileType fileType = parseFileType(mName);

                mModelData = ModelData::create();

                switch (fileType)
                {
                case E_FILETYPE_UNKNOWN:
                    break;
                case E_FILETYPE_T3B:
                    {
                        BinModelSerializer serializer;
                        ret = serializer.load(stream, smart_pointer_cast<ModelData>(mModelData));
                    }
                    break;
                case E_FILETYPE_T3T:
                    {
                        XMLModelSerializer serializer;
                        ret = serializer.load(stream, smart_pointer_cast<ModelData>(mModelData));
                    }
                    break;
                default:
                    break;
                }

                if (!ret)
                {
                    mModelData = nullptr;
                }
            }
        }

        return ret;
    }

    void Model::unload()
    {
        Resource::unload();
    }

    ResourcePtr Model::clone() const
    {
        ModelPtr model = create(mName);
        return model;
    }

    Model::FileType Model::parseFileType(const String &name) const
    {
        FileType fileType = E_FILETYPE_UNKNOWN;

        size_t pos = name.rfind(".");
        if (pos > 0)
        {
            String ext = name.substr(pos+1);

            if (ext == T3D_BIN_MODEL_FILE_EXT)
            {
                fileType = E_FILETYPE_T3B;
            }
            else if (ext == T3D_TXT_MODEL_FILE_EXT)
            {
                fileType = E_FILETYPE_T3T;
            }
        }

        return fileType;
    }
}
