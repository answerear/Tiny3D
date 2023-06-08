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

#ifndef __T3D_WIN32_STD_THREAD_H__
#define __T3D_WIN32_STD_THREAD_H__


#include "T3DNoncopyable.h"
#include "Adapter/Common/T3DSTDThread.h"


namespace Tiny3D
{
    class Win32STDThread : public STDThread, public Noncopyable
    {
    public:
        Win32STDThread() = default;

        ~Win32STDThread() override = default;

        ThreadPriority getPriority() const override;

        void setPriority(ThreadPriority priority) override;

        uint64_t getAffinityMask() const override;

        void setAffinityMask(uint64_t mask) override;
    };
}

#endif  /*__T3D_WIN32_STD_THREAD_H__*/
