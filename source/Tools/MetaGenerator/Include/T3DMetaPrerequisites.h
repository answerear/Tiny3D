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

#ifndef __T3D_META_PREREQUISITES_H__
#define __T3D_META_PREREQUISITES_H__


#include <Tiny3D.h>


namespace Tiny3D
{
    #define LOG_TAG                 "MetaGenerator"

    #define MG_LOG_ERROR(fmt, ...)   \
        T3D_LOG_ERROR(LOG_TAG, fmt, ##__VA_ARGS__);
        // printf(fmt, ##__VA_ARGS__); \
        // printf("\n");   \
        //

    #define MG_LOG_WARNING(fmt, ...) \
        T3D_LOG_WARNING(LOG_TAG, fmt, ##__VA_ARGS__);
        // printf(fmt, ##__VA_ARGS__); \
        // printf("\n");   \
        //

    #define MG_LOG_INFO(fmt, ...)    \
        T3D_LOG_INFO(LOG_TAG, fmt, ##__VA_ARGS__);
        // printf(fmt, ##__VA_ARGS__); \
        // printf("\n");   \
        //

    #define MG_LOG_DEBUG(fmt, ...)   \
        T3D_LOG_DEBUG(LOG_TAG, fmt, ##__VA_ARGS__);
        // printf(fmt, ##__VA_ARGS__); \
        // printf("\n");   \
        //
}

#endif  /*__T3D_META_PREREQUISITES_H__*/
