/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Answer Wong
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

#ifndef __MCONV_LOG_H__
#define __MCONV_LOG_H__



#include "mconv_prerequisites.h"


namespace mconv
{
#define MCONV_LOG_ERROR(fmt, ...)   \
    T3D_LOG_ERROR(fmt, ##__VA_ARGS__);  \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

#define MCONV_LOG_WARNING(fmt, ...) \
    T3D_LOG_WARNING(fmt, ##__VA_ARGS__);    \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

#define MCONV_LOG_INFO(fmt, ...)    \
    T3D_LOG_INFO(fmt, ##__VA_ARGS__);   \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

#define MCONV_LOG_DEBUG(fmt, ...)   \
    T3D_LOG_DEBUG(fmt, ##__VA_ARGS__);  \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

}


#endif  /*__MCONV_LOG_H__*/
