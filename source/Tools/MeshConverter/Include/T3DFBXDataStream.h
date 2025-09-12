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

#pragma once

#include "T3DConverterPrerequisites.h"


namespace Tiny3D
{
    class FbxDataStream : public FbxStream
    {
    public:
        FbxDataStream(DataStream &stream, FbxManager *manager, bool readable);

        ~FbxDataStream() override;

        EState GetState() override;

        bool Open(void *pStreamData) override;

        bool Close() override;

        bool Flush() override;

        size_t Write(const void *pData, FbxUInt64 size) override;

        size_t Read(void *pData, FbxUInt64 size) const override;

        int GetReaderID() const override;

        int GetWriterID() const override;

        void Seek(const FbxInt64 &pOffset, const FbxFile::ESeekPos &pSeekPos) override;

        FbxInt64 GetPosition() const override;

        void SetPosition(FbxInt64 pPosition) override;

        int GetError() const override;

        void ClearError() override;

    protected:
        DataStream &mStream;
        int mReaderID;
        int mWriterID;
    };
}
