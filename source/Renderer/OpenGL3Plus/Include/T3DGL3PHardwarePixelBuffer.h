/***************************************************************************************************
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
 **************************************************************************************************/

#ifndef __T3D_GL3P_HARDWARE_PIXEL_BUFFER_H__
#define __T3D_GL3P_HARDWARE_PIXEL_BUFFER_H__

#include "T3DGL3PPrerequisites.h"


namespace Tiny3D
{
    class GL3PHardwarePixelBuffer : public HardwarePixelBuffer
    {
    public:
        GL3PHardwarePixelBuffer(uint32_t width, uint32_t height,
            PixelFormat format, HardwareBuffer::Usage usage,
            bool useSystemMemory, bool useShadowBuffer);
        virtual ~GL3PHardwarePixelBuffer();

        bool createTexture();

        /**
        * @brief ��ȡ����Ӳ����������ͬ��Ⱦ��ʵ�ֽӿ�
        * @param [in] rect : Ҫ��ȡ���ݵ�����
        * @param [in] options : ��ȡ����ѡ��
        * @param [out] lockedPitch : �������������pitch
        * @return ����������Ӳ�����ݵ�ַ
        * @see enum LockOptions
        */
        virtual void *lockImpl(const Rect &rect, LockOptions options, int32_t &lockedPitch) override;

        /**
        * @brief ����Ӳ��������
        */
        virtual void unlockImpl() override;

        /**
        * @brief ����Դ�����image��ȡ���ݵ�Ŀ������
        * @param [in] image : Ҫ��ȡ��ͼ�����
        * @param [in] srcRect : Դ��������Ĭ��Ϊnullptr��ʱ�򣬱�ʾ����ԴĿ�����򣬻��Զ�����ƥ��Ŀ������
        * @param [in] dstRect : Ŀ����������Ĭ��Ϊnullptr��ʱ�򣬱�ʾ����Ŀ�����򣬻��Զ�����ƥ��Դ����
        * @return ���óɹ�����true�����򷵻�false
        * @note dstRect �� srcRect ��Ϊnullptr��ʱ�򣬻��Զ�����ƥ��Դ��Ŀ������
        */
        virtual bool readImage(const Image &image, Rect *srcRect = nullptr, Rect *dstRect = nullptr) override;

        /**
        * @brief ��ָ��Դ����Χ����д��image��Ŀ������
        * @param [in] image : Ҫд���ͼ�����
        * @param [in] dstRect : Ŀ������Ĭ��Ϊnullptr��ʱ�򣬱�ʾ����Ŀ�����򣬻��Զ�����ƥ��Դ����
        * @param [in] srcRect : Դ����Ĭ��Ϊnullptr��ʱ�򣬱�ʾ����ԴĿ�����򣬻��Զ�����ƥ��Ŀ������
        * @return ���óɹ�����true�����򷵻�false
        * @note dstRect �� srcRect ��Ϊnullptr��ʱ�򣬻��Զ�����ƥ��Դ��Ŀ������
        */
        virtual bool writeImage(Image &image, Rect *dstRect = nullptr, Rect *srcRect = nullptr) override;

    protected:
    };
}


#endif  /*__T3D_GL3P_HARDWARE_PIXEL_BUFFER_H__*/