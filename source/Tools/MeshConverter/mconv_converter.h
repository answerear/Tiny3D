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

#ifndef __MCONV_CONVERTER_H__
#define __MCONV_CONVERTER_H__


#include "mconv_prerequisites.h"
#include "mconv_settings.h"


namespace mconv
{
    class ConverterImpl;
    class Serializer;

    class Converter
    {
    public:
        Converter();
        virtual ~Converter();

        bool execute(int argc, char *argv[]);

    protected:
        ConverterImpl   *mConverter;
    };

    class ConverterImpl
    {
    public:
        ConverterImpl(const Settings &settings);
        virtual ~ConverterImpl();

        virtual bool convert();

    protected:
        virtual bool importScene() = 0;
        virtual bool convertToT3D() = 0;
        virtual bool exportScene() = 0;
        virtual void cleanup() = 0;

    protected:
        const Settings    &mSettings;

        Serializer  *mImporter;
        Serializer  *mExporter;
    };
}


#endif  /*__MCONV_CONVERTER_H__*/
