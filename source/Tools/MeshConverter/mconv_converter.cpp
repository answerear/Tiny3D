/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
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

#include "mconv_converter.h"
#include "mconv_command.h"
#include "mconv_settings.h"
#include "mconv_fbxconverter.h"
#include "mconv_ogreconverter.h"
#include "mconv_serializer.h"
#include "mconv_log.h"


namespace mconv
{
    Converter::Converter()
        : mConverter(nullptr)
    {

    }

    Converter::~Converter()
    {
        delete mConverter;
        mConverter = nullptr;
    }

    bool Converter::execute(int argc, char *argv[])
    {
        bool result = false;

        Settings settings;
        Command command;
        result = command.parse(argc, argv, settings);

        ConverterImpl *converter = nullptr;

        if (result)
        {
            switch (settings.mSrcType)
            {
            case E_FILETYPE_FBX:
            case E_FILETYPE_DAE:
                {
                    converter = new FBXConverter(settings);
                }
                break;
            case E_FILETYPE_OGRE:
                {
                    converter = new OgreConverter(settings);
                }
                break;
            }
        }

        result = result && converter->convert();

        delete converter;
        converter = nullptr;

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////

    ConverterImpl::ConverterImpl(const Settings &settings)
        : mSettings(settings)
        , mImporter(nullptr)
        , mExporter(nullptr)
    {

    }

    ConverterImpl::~ConverterImpl()
    {
        delete mImporter;
        mImporter = nullptr;

        delete mExporter;
        mExporter = nullptr;
    }

    bool ConverterImpl::convert()
    {
        bool ret = true;

        MCONV_LOG_INFO("Starting converting ......");
        ret = ret && importScene();
        ret = ret && convertToT3D();
        ret = ret && exportScene();

        cleanup();

        if (ret)
        {
            MCONV_LOG_INFO("Completed converting !");
        }

        return ret;
    }
}
