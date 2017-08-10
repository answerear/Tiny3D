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

#ifndef __T3D_FONT_MANAGER_H__
#define __T3D_FONT_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DFont.h"


namespace Tiny3D
{
    /**
     * @class FontConfig
     * @brief ������������ڸ�FontManager��ȡ�û������������
     */
    struct T3D_ENGINE_API FontConfig
    {
        /**
         * @brief �ַ��ڴ������ԣ���Ҫ�����ڲ�����������������µ�������ռ�
         */
        enum Strategy
        {
            E_STRATEGY_AUTO = 0,    /// �ڲ��Զ�ѡ��
            E_STRATEGY_LRU,         /// ������ռ䲻��ʱ��ʹ��LRU�㷨��̭�ʱ��û����Ⱦ������
            E_STRATEGY_APPEND,      /// ��������һ�����ʺ�����
        };

        /**
         * @brief ���������е�Ĭ��ֵ
         */
        enum DefaultValue
        {
            E_DEFAULT_TEX_SIZE = 512,       /// Ĭ�ϳ�ʼ�������С
            E_DEFAULT_TEX_MAX = 2048,       /// Ĭ����������С������Ӳ��֧�ֵ����ֵȡ����Сֵ
            E_DEFAULT_TEX_INC_SIZE = 0,     /// Ĭ������������С����������ռ䲻���Զ�������ʱ��
            E_DEFAULT_BLOCK_SIZE = 128,     /// Ĭ�������С
        };

        FontConfig()
            : initTexWidth(E_DEFAULT_TEX_SIZE)
            , initTexHeight(E_DEFAULT_TEX_SIZE)
            , maxTexWidth(E_DEFAULT_TEX_MAX)
            , maxTexHeight(E_DEFAULT_TEX_MAX)
            , blockWidth(E_DEFAULT_BLOCK_SIZE)
            , blockHeight(E_DEFAULT_BLOCK_SIZE)
            , strategy(E_STRATEGY_AUTO)
        {
        }

        size_t      initTexWidth;       /// ��ʼ�����ȣ�Ĭ����E_DEFAULT_TEX_SIZE
        size_t      initTexHeight;      /// ��ʼ����߶ȣ�Ĭ����E_DEFAULT_TEX_SIZE
        size_t      maxTexWidth;        /// ��������ȣ���ֵ���Ӳ��֧����������ȱȽ�ȡ��Сֵ��
                                        /// Ĭ����E_DEFAULT_TEX_MAX
        size_t      maxTexHeight;       /// �������߶ȣ���ֵ���Ӳ��֧���������߶ȱȽ�ȡ��Сֵ��
                                        /// Ĭ����E_DEFAULT_TEX_MAX
        size_t      blockWidth;         /// �����������ȣ�Ĭ����E_DEFAULT_BLOCK_SIZE
        size_t      blockHeight;        /// ����������߶ȣ�Ĭ����E_DEFAULT_BLOCK_SIZE
        Strategy    strategy;           /// ������ԣ�Ĭ����E_STRATEGY_AUTO��
                                        /// �������ݲ�ͬ������ʵ��������
    };

    /**
     * @class FontManager
     * @brief ������Դ����������Ҫ���ڼ��ء�ж�صȸ���������Դ�Ĺ���
     * @note �����ǵ������޷����ⲿʵ������������׳��쳣��������������
     *      ��ֱ��ʹ��T3D_FONT_MGR�������ʣ�
     *      ����ʹ��FontManager::getInstance()��FontManager::getInstancePtr()�����ʡ�
     * @see class Singleton
     */
    class T3D_ENGINE_API FontManager
        : public Singleton<FontManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief Ĭ�Ϲ��캯��
         * @param [in] config : ���������Ĭ��������Ĭ������
         * @see struct FontConfig
         */
        FontManager(const FontConfig &config = FontConfig());

        /**
         * @brief ��������
         */
        virtual ~FontManager();

        /**
         * @brief ����ָ���������Ƽ���һ������
         * @param [in] fontSize : �����С
         * @param [in] name : ��������
         * @return ���سɹ�����������Դ����
         * @see void unloadFont(FontPtr &font)
         * @see class Font
         */
        virtual FontPtr loadFont(int32_t fontSize, const String &name = "", Font::FontType fontType = Font::E_FT_TRUETYPE);

        /**
         * @brief ж��������Դ
         * @param [in] font : �������
         * @return void
         * @see FontPtr loadFont(FontPtr &font);
         * @see class Font
         */
        virtual void unloadFont(FontPtr &font);

        /**
         * @brief ��ȡ����������
         * @return ��������������
         * @see struct FontConfig
         */
        const FontConfig &getConfig() const { return mFontConfig; }

        /**
         * @brief ����Ĭ����������
         * @param [in] fontName : Ĭ�����������ַ���
         * @return void
         * @see const String &getDefaultFontName() const
         */
        void setDefaultFontName(const String &fontName) { mDefaultFontName = fontName; }

        /**
         * @brief ��ȡĬ����������
         * @return ����Ĭ����������
         * @see void setDefaultFontName(const String &fontName)
         */
        const String &getDefaultFontName() const { return mDefaultFontName; }

    protected:
        /**
         * @brief �Ӹ���̳У�������ʵ�ִ���������Դ����
         * @param [in] name : ��Դ����
         * @param [in] argc : �ɱ��������
         * @param [in] args : �ɱ�����б�
         * @return ���óɹ����ؾ�����Դ���󣬱��ӿ���д�󷵻ص���Font��������
         * @see class Font
         */
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;

    protected:
        FontConfig  mFontConfig;        /// ����������
        String      mDefaultFontName;   /// Ĭ����������
    };

    #define T3D_FONT_MGR    (FontManager::getInstance())
}


#endif  /*__T3D_FONT_MANAGER_H__*/
