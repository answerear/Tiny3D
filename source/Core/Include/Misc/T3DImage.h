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

#ifndef __T3D_IMAGE_H__
#define __T3D_IMAGE_H__

#include "Misc/T3DObject.h"
#include "Misc/T3DCommon.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Image : public Object
    {
        friend class ImageCodecBase;

    public:
        static const char * const FILETYPE_PNG;
        static const char * const FILETYPE_JPG;
        static const char * const FILETYPE_BMP;
        static const char * const FILETYPE_TGA;
        static const char * const FILETYPE_DDS;

        enum Filter
        {
            E_FILTER_BOX = 0,           /// Box, pulse, Fourier window, 1st order (constant) b-spline
            E_FILTER_BICUBIC = 1,       /// Mitchell & Netravali's two-param cubic filter
            E_FILTER_BILINEAR = 2,      /// Bilinear filter
            E_FILTER_BSPLINE = 3,       /// 4th order (cubic) b-spline
            E_FILTER_CATMULLROM = 4,    /// Catmull-Rom spline, Overhauser spline
            E_FILTER_LANCZOS3 = 5       /// Lanczos3 filter
        };

        /**
         * @brief Ĭ�Ϲ��캯��
         */
        Image();

        /**
         * @brief ����һ��ָ�����ͼ�����
         */
        Image(int32_t width, int32_t height, int32_t bpp, PixelFormat format);

        /**
         * @brief �������캯��
         */
        Image(const Image &other);

        /**
         * @brief ��������
         */
        virtual ~Image();

        /**
         * @brief �ӱ��ͼ������ƹ���
         */
        Image &operator =(const Image &other);

        /**
         * @brief �Ƚ�ͼ���Ƿ���ͬ����Ҫʹ��ͼ��MD5ֵ���ȶ�
         */
        bool operator ==(const Image &other) const;

        /**
         * @brief �Ƚ�ͼ���Ƿ�ͬ����Ҫʹ��ͼ��MD5ֵ���ȶ�
         */
        bool operator !=(const Image &other) const;

        /**
         * @brief ���ļ�����ͼ������
         * @param [in] path : �ļ�ȫ·��
         * @return ���óɹ�����true�����򷵻�false
         * @note ���ӿڶ�ȡ�ļ�����Զ�ʶ���ļ���ʽ������
         * @see bool load(DataStream &stream)
         */
        bool load(const String &path);

        /**
         * @brief ���ڴ����ͼ������
         * @param [in] stream : ����������
         * @return ���óɹ�����true�����򷵻�false
         * @note ���ӿ��Զ�ʶ���������ĸ�ʽ�����ҽ���������
         * @see bool load(const String &path);
         */
        bool load(DataStream &stream);

        /**
         * @brief ���ڴ�Raw���ݼ���ͼ��
         * @param [in] data : ͼ��ԭʼ����
         * @param [in] width : ͼ����
         * @param [in] height : ͼ��߶�
         * @param [in] bpp : ͼ��ɫ��
         * @param [in] pitch : ͼ����
         * @param [in] format : ͼ�����ظ�ʽ
         * @param [in] copySource : �Ƿ����һ�����ݵ��ڲ�ά��
         * @return ���óɹ�����true�����򷵻�false
         */
        bool load(uint8_t *data, int32_t width, int32_t height, int32_t bpp, int32_t pitch, PixelFormat format, bool copySource = false);

        /**
         * @brief ��ͼ�񱣴浽�ļ���
         * @param [in] path : �ļ�ȫ·��
         * @param [in] fileType : Ҫ������ļ����ͣ�Ĭ����PNG�ļ�
         * @return ���óɹ�����true�����򷵻�false
         */
        bool save(const String &path, const String &fileType = FILETYPE_PNG) const;

        /**
         * @brief ��ͼ�񱣴浽�ļ���
         * @param [in] stream : ���������������
         * @param [in] fileType : Ҫ������ļ����ͣ�Ĭ����PNG�ļ�
         * @return ���óɹ�����true�����򷵻�false
         */
        bool save(DataStream &stream, const String &fileType = FILETYPE_PNG) const;

        /**
         * @brief ����ͼ��ʹ���Ϊ��ͼ�����
         * @return ���ٳɹ�����true�����򷵻�false
         */
        bool destroy();

        /**
         * @brief ���·�תͼ��
         * @return ���óɹ�����true�����򷵻�false
         */
        bool flip();

        /**
         * @brief ���Ҿ���ͼ��
         * @return ���óɹ�����true�����򷵻�false
         */
        bool mirror();

        /**
         * @brief ��ͼ�����ָ����ɫ
         */
        bool fill(const Color4 &rkColor);

        /**
         * @brief �ӱ��ͼ����������ݹ���
         * @param [in] image : Դͼ��
         * @param [in] srcRect : Ҫ���Ƶ�Դ����Ĭ��nullptrΪ��������
         * @param [in] dstRect : Ҫ���ƹ�ȥ��Ŀ������Ĭ��nullptrΪ��������
         * @return ���óɹ�����true�����򷵻�false
         * @note ��Դ��Ŀ��������ͬ��Сʱ���Զ����������ƣ�Դ��Ŀ��ͼ�����Ҫ���ظ�ʽһ�£��������ʧ��
         * @note ���Զ�����ʱ���ᵼ�������½�
         */
        bool copy(const Image &image, Rect *srcRect = nullptr, Rect *dstRect = nullptr, Filter filter = E_FILTER_BILINEAR);

        /**
         * @brief ת��Ŀ�����ظ�ʽ��ͼ��
         * @param [in] format : Ŀ�����ظ�ʽ
         * @return ���óɹ�����true�����򷵻�false
         */
        bool convert(PixelFormat format);

        /**
         * @brief ת��Ŀ�����ظ�ʽ��ͼ��
         * @param [in] format : Ŀ�����ظ�ʽ
         * @return ���óɹ�����true�����򷵻�false
         */
        bool convert(Image &image, PixelFormat format) const;

        /**
         * @brief �Ƚ�ͼ��
         * @param [in] other : ����һ��ͼ�����
         * @param [in] compareAlpha : �������alphaͨ�����Ƿ�Ƚ�alphaͨ�������������alphaͨ������������Ч
         * @return �Ƚϳ���ͬ����true����ͬ����false
         * @note ��Ҫ�ǱȽ�ͼ��MD5ֵ
         */
        bool compare(const Image &other, bool compareAlpha = true) const;

        /**
         * @brief ����ͼ������
         */
        uint8_t *getData();

        /**
         * @brief ����ͼ������
         */
        const uint8_t * const getData() const;

        /**
         * @brief ����ͼ�����ݴ�С
         */
        size_t getSize() const;

        /**
         * @brief ����ͼ����
         */
        int32_t getWidth() const;

        /**
         * @brief ����ͼ��߶�
         */
        int32_t getHeight() const;

        /**
         * @brief ����ͼ����
         */
        int32_t getPitch() const;

        /**
         * @brief ����ͼ��ɫ��
         */
        int32_t getBPP() const;

        /**
         * @brief ����ͼ�����ظ�ʽ
         */
        PixelFormat getFormat() const;

        /**
         * @brief ����ͼ���Ƿ��alphaͨ��
         */
        bool hasAlpha() const;

        /**
         * @brief ����ͼ���Ƿ�����alphaԤ��
         */
        bool isPremultipliedAlpha() const;

        /**
         * @brief �����Ƿ��ͼ��
         */
        bool isEmpty() const;

        /**
        * @brief ��ȡ������ɫͨ����һ��͸��ͨ������ɫmask
        */
        void getColorMask(uint32_t &redMask, uint32_t &greenMask, uint32_t &blueMask, uint32_t &alphaMask) const;

        /**
         * @brief ����ɫ�����ͼ���п��
         */
        static int32_t calcPitch(int32_t width, int32_t bpp);

        /**
         * @brief ����ͼ�����ظ�ʽ��ȡɫ��
         */
        static int32_t getBPP(PixelFormat format);

    protected:
        /**
         * @brief �����ļ���չ���ͻ�ȡ�ļ�����ö��
         */
        uint32_t getFileType(const String &fileExt) const;

        /**
         * @brief ����ͼ���п��
         */
        int32_t calcPitch() const;

        /**
         * @brief ��ȡ��ǰ���ظ�ʽ��һ������ռ�õ��ֽ���
         */
        int32_t getBytesPerPixel() const;

    private:
        int32_t     mWidth;             /// ͼ����
        int32_t     mHeight;            /// ͼ��߶�
        int32_t     mBPP;               /// ͼ��ɫ��
        int32_t     mPitch;             /// ͼ���п��

        PixelFormat mFormat;            /// ͼ�����ظ�ʽ

        bool        mHasAlpha;          /// �Ƿ��alphaͨ��
        bool        mIsPreMulti;        /// �Ƿ�Ԥ��alpha
        bool        mIsDataExternal;    /// �����Ƿ��ⲿ�����
        bool        mIsEmpty;           /// ��ͼ�񣨼�û���κ����ݵ�ͼ��

        uint8_t     *mData;             /// ͼ������
        size_t      mDataSize;          /// ͼ�����ݴ�С
    };
}


#include "T3DImage.inl"


#endif  /*__T3D_IMAGE_H__*/

