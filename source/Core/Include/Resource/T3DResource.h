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


#ifndef __T3D_RESOURCE_H__
#define __T3D_RESOURCE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Resource/T3DMeta.h"


namespace Tiny3D
{
    typedef TFunction<void(TResult result, Resource *resource)> CompletedCallback;
    
    /**
     * @brief   A 3D engine api.
     */
    class T3D_ENGINE_API Resource : public Object
    {
        friend class ResourceManager;
        
    public:
        /**
         * @enum    Type
         * @brief   資源類型枚舉
         */
        enum class Type : uint32_t
        {
            kUnknown = 0,   /**< 未知類型資源 */
            kDylib,         /**< 動態庫 */
            kMaterial,      /**< 材質 */
            kTexture,       /**< 紋理 */
            kShader,        /**< 著色器 */
            kModel,         /**< 模型数据 */
            kSerializable,  /**< 可直接序列化和反序列化资源 */
        };

        /**
         * @brief   析构函数
         */
        ~Resource() override;

        /**
         * @brief   獲取資源類型
         */
        virtual Type getType() const = 0;

        /**
         * @brief   获取资源源 UUID
         */
        UUID getResourceID() const
        {
            if (mMeta != nullptr)
                return mMeta->uuid;
            return UUID::INVALID;
        }

        Meta *getMeta() const
        {
            return mMeta;
        }

        /**
         * @brief   获取资源名称
         */
        const String &getName() const
        {
            return mName;
        }

        /**
         * @brief   獲取資源是否加載
         */
        bool isLoaded() const
        {
            return mState == State::kLoaded;
        }

        /**
         * @brief   获取资源是否加载中
         */
        bool isLoading() const
        {
            return mState == State::kLoading;
        }

        /**
         * @brief   获取资源是否未加载
         */
        bool isUnloaded() const
        {
            return mState == State::kUnloaded;
        }

    protected:
        /**
         * @brief   构造函数
         * @param [in] name : 资源名称.
         * @remarks 本類不能直接實例化，所以只能隱藏構造函數.
         */
        Resource(const String &name);

        virtual TResult load(ArchivePtr archive);

        virtual TResult loadData(DataStream &stream) = 0;

        virtual TResult unload();

        virtual ResourcePtr clone() const = 0;
        
    protected:
        enum class State : uint32_t
        {
            kUnloaded = 0,  /**< 未加载 */
            kLoading,       /**< 加载中 */
            kLoaded         /**< 已加载 */
        };

        MetaPtr             mMeta;          /**< 资源的元信息对象 */
        State               mState;         /**< 资源状态 */
        String              mName;          /**< 資源名稱 */
        CompletedCallback   mCompletedCB;   /**< 异步加载回调 */
    };
}


#endif  /*__T3D_RESOURCE_H__*/
