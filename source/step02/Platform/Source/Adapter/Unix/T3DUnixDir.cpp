/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
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

#include "Adapter/Unix/T3DUnixDir.h"


namespace Tiny3D
{
	UnixDir::UnixDir()
	{
		
	}
	
	UnixDir::~UnixDir()
	{
		
	}
	
	bool UnixDir::findFile(const String &strPath)
	{
		return false;
	}
	
	bool UnixDir::findNextFile()
	{
		return false;
	}
	
	void UnixDir::close()
	{
		
	}
	
	String UnixDir::getRoot() const
	{
		return "";
	}
	
	String UnixDir::getFileName() const
	{
		return "";
	}
	
	String UnixDir::getFilePath() const
	{
		return "";
	}
	
	String UnixDir::getFileTitle() const
	{
		return "";
	}
	
	uint32_t UnixDir::getLength() const
	{
		return 0;
	}
	
	bool UnixDir::isDots() const
	{
		return false;
	}
	
	bool UnixDir::isDirectory() const
	{
		return false;
	}
	
	long_t UnixDir::getCreationTime() const
	{
		return 0;
	}
	
	long_t UnixDir::getLastAccessTime() const
	{
		return 0;
	}
	
	long_t UnixDir::getLastWriteTime() const
	{
		return 0;
	}
	
	bool UnixDir::makeDir(const String &strDir)
	{
		return false;
	}
	
	bool UnixDir::removeDir(const String &strDir)
	{
		return false;
	}
	
	bool UnixDir::remove(const String &strFileName)
	{
		return false;
	}
	
	bool UnixDir::exists(const String &strPath) const
	{
		return false;
	}
	
	String UnixDir::getCachePath() const
	{
		return "";
	}
	
	String UnixDir::getAppPath() const
	{
		return "";
	}
	
	char UnixDir::getNativeSeparator() const
	{
		return 0;
	}
}

