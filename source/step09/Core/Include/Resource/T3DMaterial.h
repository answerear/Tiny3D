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


#ifndef __T3D_MATERIAL_H__
#define __T3D_MATERIAL_H__


#include "Resource/T3DResource.h"
#include "Resource/T3DGPUProgram.h"


namespace Tiny3D
{
    /**
     * @brief 材質資源
     */
    class T3D_ENGINE_API Material : public Resource
    {
    public:
        /**
         * @brief 材質來源類型
         */
        enum MaterialType
        {
            E_MT_DEFAULT = 0,   /**< 默認是從資源文件加載資源 */
            E_MT_MANUAL,        /**< 由調用者自己創建 */
        };

        /** 創建 Material 对象 */
        static MaterialPtr create(const String &name, MaterialType type);

        /** 析構函數 */
        virtual ~Material();

        /** 重寫 Resource::getType() */
        virtual Type getType() const override;

        /** 獲取材質名稱 */
        const String &getMaterialName() const { return mMaterialName; }

        /** 設置材質名稱 */
        void setMaterialName(const String &name) { mMaterialName = name; }

        /**
         * @brief 新增一個 Technique 對象到材質中
         * @param [in] tech : 新創建的 Technique 對象
         * @return 創建成功返回 T3D_OK
         */
        TResult addTechnique(const String &name, TechniquePtr &tech);

        /**
         * @brief 根據名稱移除一個 Technique 對象
         */
        TResult removeTechnique(const String &name);

        /**
         * @brief 根據索引移除一個 Technique 對象
         */
        TResult removeTechnique(size_t index);

        /**
         * @brief 根據名稱獲取 Technique 對象
         */
        TechniquePtr getTechnique(const String &name) const;

        /**
         * @brief 根據索引獲取 Technique 對象
         */
        TechniquePtr getTechnique(size_t index) const;

        /**
         * @brief 獲取材質中 Technique 對象數量
         */
        size_t getTechniqueCount() const { return mTechniques.size(); }

        /**
         * @brief 獲取所有 Technique 對象
         */
        const Techniques &getTechniques() const { return mTechniques; }

        /**
         * @brief 新增一個 GPUProgram 對象到材質中
         */
        TResult addGPUProgram(const String &name, GPUProgramPtr &program);

        /**
         * @brief 根據名稱移除一個 GPUProgram 對象
         */
        TResult removeGPUProgram(const String &name);

        /**
         * @brief 根據名稱獲取 GPUProgram 對象
         */
        GPUProgramPtr getGPUProgram(const String &name) const;

        /**
         * @brief 獲取材質中 GPUProgram 對象數量
         */
        size_t getGPUProgramCount() const { return mGPUPrograms.size(); }

        /**
         * @brief 獲取所有 GPUProgram 對象
         */
        const GPUPrograms getGPUPrograms() const { return mGPUPrograms; }

    protected:
        /** 構造函數 */
        Material(const String &name, MaterialType type);

        /** 重寫 Resource::load() */
        virtual TResult load() override;

        /** 重寫 Resource::unlaod() */
        virtual TResult unload() override;

        /** 重寫 Resource::clone() */
        virtual ResourcePtr clone() const override;

    protected:
        MaterialType    mMaterialType;  /**< 材質資源類型 */
        String          mMaterialName;  /**< 材質名稱 */

        Techniques      mTechniques;    /**< 材質下的 Technique 對象 */
        GPUPrograms     mGPUPrograms;   /**< 材質擁有的 GPUProgram 對象*/
    };
}


#endif  /*__T3D_MATERIAL_H__*/
