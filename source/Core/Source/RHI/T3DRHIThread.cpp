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


#include "RHI/T3DRHIThread.h"
#include "RHI/T3DRHICommand.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RHIThread::RHIThread()
    {
        
    }

    //--------------------------------------------------------------------------

    RHIThread::~RHIThread()
    {
        stop();
    }

    //--------------------------------------------------------------------------

    bool RHIThread::init()
    {
        mCurrentCommandList = 0;
        mIsRunning = true;
        return true;
    }

    //--------------------------------------------------------------------------

    TResult RHIThread::run()
    {
        while (mIsRunning)
        {
            // 循环执行 RHI 命令
            int32_t current = (mCurrentCommandList + 1) % kMaxCommandLists;

            for (auto command : mCommandLists[current])
            {
                command->execute();
            }

            mCommandLists[current].clear();
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void RHIThread::stop()
    {
        mIsRunning = false;
    }
    
    //--------------------------------------------------------------------------

    void RHIThread::exit()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void RHIThread::addCommand(RHICommand *command)
    {
        mCommandLists[mCurrentCommandList].push_back(command);
    }

    //--------------------------------------------------------------------------
}
