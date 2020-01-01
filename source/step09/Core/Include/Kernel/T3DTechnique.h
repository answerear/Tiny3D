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


#ifndef __T3D_TECHNIQUE_H__
#define __T3D_TECHNIQUE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "T3DObject.h"

namespace Tiny3D
{
    class T3D_ENGINE_API Technique : public Object
    {
    public:
        typedef TArray<PassPtr>                 Passes;
        typedef Passes::iterator                PassesItr;
        typedef Passes::const_iterator          PassesConstItr;

        /** 創建 Technique 對象 */
        static TechniquePtr create(const String &name, Material *material);

        /** 析構函數 */
        virtual ~Technique();

        /**
         * @brief   编译
         * @returns 调用成功返回 T3D_OK.
         */
        TResult compile();

        /**
         * @brief 獲取 Technique 的名稱
         */
        const String &getName() const;

        /**
         * @brief 獲取擁有該 Technique 對象的材質對象
         */
        Material *getMaterial() const;

        /**
         * @brief 新增一個 Pass 對象
         * @param [in] name : 新增的 Pass 對象名稱
         * @param [in][out] pass : 返回新增的 Pass 對象
         * @return 調用成功返回 T3D_OK
         */
        TResult addPass(const String &name, PassPtr &pass);

        /**
         * @brief 移除指定名稱的 Pass 對象
         * @param [in] name : Pass 對象名稱
         * @return 調用成功返回 T3D_OK
         */
        TResult removePass(const String &name);

        /**
         * @brief 移除指定索引的 Pass 對象
         * @param [in] index : 索引
         * @return 調用成功返回 T3D_OK
         */
        TResult removePass(size_t index);

        /**
         * @brief 獲取指定名稱的 Pass 對象
         * @param [in] name : Pass 名稱
         * @return 調用成功返回 Pass 對象，否則返回 nullptr
         */
        PassPtr getPass(const String &name) const;

        /**
         * @brief 獲取指定索引的 Pass 對象
         * @param [in] index : Pass 對象所在索引
         * @return 調用成功返回 Pass 對象，否則返回 nullptr
         */
        PassPtr getPass(size_t index) const;

        /**
         * @brief 獲取所有 Pass 對象
         */
        const Passes &getPasses() const;

        /**
         * @brief 獲取 Pass 對象數量
         */
        size_t getPassCount() const;

    protected:
        /** 構造函數 */
        Technique(const String &name, Material *material);

    protected:
        Material    *mParent;
        String      mName;
        Passes      mPasses;

        //---------------------------------------
        // Command : lod_index
        // Usage : lod_index <number>
        uint16_t    mLodIndex;

        uint16_t    mSchemeIndex;

        bool        mIsSupported;   /**< GPU 是否支持本 Technique */
    };
}


#include "T3DTechnique.inl"


#endif  /*__T3D_TECHNIQUE_H__*/
