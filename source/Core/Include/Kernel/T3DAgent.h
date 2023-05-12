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

#ifndef __T3D_AGENT_H__
#define __T3D_AGENT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class Agent : public Singleton<Agent>
    {
    public:
        Agent();

        virtual ~Agent();

        /**
         * \brief 获取版本号
         * \return 返回整数型版本号
         */
        uint32_t getVersion() const;

        const char *getVersionString() const;

        const char *getVersionName() const;

        TResult init(const String &appPath, bool autoCreateWindow,
                     const String &config = "Tiny3D.cfg");

        const String &getPluginsPath() const { return mPluginsPath; }

    protected:
        String  mPluginsPath;
    };
}


#endif    /*__T3D_AGENT_H__*/
