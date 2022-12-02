/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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

#ifndef __T3D_REFLECTION_PREPROCESSOR_H__
#define __T3D_REFLECTION_PREPROCESSOR_H__


#include "T3DRPPrerequisites.h"
#include "T3DPreprocessorOptions.h"


namespace Tiny3D
{
    class ReflectionGenerator;
    
    class ReflectionPreprocessor
    {
    public:
        ReflectionPreprocessor();

        virtual ~ReflectionPreprocessor();

        TResult execute(int32_t argc, char *argv[]);

    protected:
        ClangArgs parseSettingsFile(const String &path);

        TResult collectProjectHeaders(const String &path);
        
        TResult generateAST(const String &path, const ClangArgs &args);

        TResult generateSource(const String &path);

    protected:
        static const String kReflectionSettingsFile;
        
        typedef TList<String> SettingsPathes;

        SettingsPathes mPathes;
        ReflectionGenerator *mGenerator;
        StringList mArgs;
        String mGeneratedPath;
    };
}


#endif  /*__T3D_REFLECTION_PREPROCESSOR_H__*/
