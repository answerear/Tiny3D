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



#ifndef __T3D_CCT_PREREQUISITES_H__
#define __T3D_CCT_PREREQUISITES_H__


#include <stdio.h>

#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <functional>

#define RAPIDJSON_HAS_STDSTRING 1
#include <prettywriter.h>
#include <document.h>
#include <rapidjson.h>
#include <error/en.h>
#include <istreamwrapper.h>
#include <ostreamwrapper.h>


typedef std::string String;
typedef std::list<std::string> StringList;


#endif  /*__T3D_CCT_PREREQUISITES_H__*/

