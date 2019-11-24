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

#include "Kernel/T3DPass.h"
#include "Resource/T3DGPUProgramManager.h"
#include "Resource/T3DGPUConstBufferManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PassPtr Pass::create(const String &name, Technique *tech)
    {
        PassPtr pass = new Pass(name, tech);
        pass->release();
        return pass;
    }

    //--------------------------------------------------------------------------

    Pass::Pass(const String &name, Technique *tech)
        : mParent(tech)
        , mName(name)
    {

    }

    //--------------------------------------------------------------------------

    Pass::~Pass()
    {

    }

    //--------------------------------------------------------------------------

    TResult Pass::setGPUProgram(GPUProgramRefPtr program)
    {
        TResult ret = T3D_OK;

        do 
        {
            mGPUProgram 
                = T3D_GPU_PROGRAM_MGR.loadGPUProgram(program->getName());
            if (mGPUProgram == nullptr)
            {
                ret = T3D_ERR_RES_LOAD_FAILED;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Load GPU Program [%s] failed !",
                    program->getName().c_str());
                break;
            }

            auto list = program->getBufferRefList();
            mConstBuffers.resize(list.size());

            auto itr = list.begin();

            while (itr != list.end())
            {
                GPUConstBufferRefPtr bufferRef = *itr;
                GPUConstBufferPtr buffer 
                    = T3D_GPU_CONST_BUFFER_MGR.loadBuffer(bufferRef->getName());
                if (buffer == nullptr)
                {
                    ret = T3D_ERR_RES_LOAD_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Load GPU constant buffer [%s] failed !",
                        bufferRef->getName());
                    break;
                }

                if (bufferRef->getSlot() >= mConstBuffers.size())
                {
                    ret = T3D_ERR_OUT_OF_BOUND;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Constant buffer reference is out of bound !");
                    break;
                }
                mConstBuffers[bufferRef->getSlot()] = buffer;
                ++itr;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

}
