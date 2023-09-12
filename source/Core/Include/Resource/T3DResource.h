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
     * \brief 资源基类
     */
    TCLASS()
    class T3D_ENGINE_API Resource : public Object
    {
        friend class ResourceManager;

        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 资源类型
         */
        TENUM()
        enum class Type : uint32_t
        {
            /// 未知類型資源
            kUnknown = 0,
            /// 動態庫
            kDylib,
            /// 材質
            kMaterial,
            /// 紋理
            kTexture,
            /// 著色器
            kShader,
            /// 模型数据
            kModel,
            /// 预制件
            kPrefab,
        };

        TENUM()
        enum class State : uint32_t
        {
            /// 未加载
            kUnloaded = 0,
            /// 加载中
            kLoading,
            /// 已加载
            kLoaded
        };
        
        /**
         * \brief 析构函数
         */
        ~Resource() override;

        /**
         * \brief 获取资源类型
         * \return 返回资源类型 
         */
        virtual Type getType() const = 0;

        /**
         * \brief 获取资源 GUID
         * \return 返回资源 GUID
         */
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="getter")
        const UUID &getUUID() const
        {
            return mUUID;
        }

        /**
         * \brief 获取资源名称
         * \return 返回资源名称
         */
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }

        /**
         * \brief 获取资源加载状态
         * \return 返回资源加载状态
         */
        State getState() const
        {
            return mState;
        }

    protected:
        Resource();

        /**
         * \brief 构造函数
         * \param [in] name : 资源名称
         */
        Resource(const String &name);

        /**
         * \brief 克隆资源对象
         * \return 返回新克隆的对象
         */
        virtual ResourcePtr clone() const = 0;

        /**
         * \brief 克隆属性
         * \param [in] src : 源资源对象 
         */
        virtual void cloneProperties(const Resource * const src);

        /**
         * \brief 加载回调
         * \return 返回 T3D_OK 以示成功，否则加载会失败
         */
        virtual TResult onLoad();

        /**
         * \brief 卸载回调
         * \return 返回 T3D_OK 以示成功，否则卸载会失败
         */
        virtual TResult onUnload();

    private:
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter")
        void setUUID(const UUID &uuid)
        {
            mUUID = uuid;
        }
        
    protected:
        /// 资源的 UUID
        UUID                mUUID {};
        /// 资源状态
        State               mState = State::kUnloaded;
        /// 资源名称
        String              mName {};
        /// 异步加载回调
        CompletedCallback   mCompletedCB = nullptr;
    };
}


#endif  /*__T3D_RESOURCE_H__*/
