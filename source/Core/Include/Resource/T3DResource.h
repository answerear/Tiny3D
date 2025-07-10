/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#ifndef __T3D_RESOURCE_H__
#define __T3D_RESOURCE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


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
        static const char *EXT_MATERIAL;
        static const char *EXT_TEXTURE;
        static const char *EXT_IMAGE;
        static const char *EXT_SHADER;
        static const char *EXT_MESH;
        static const char *EXT_PREFAB;
        static const char *EXT_SCENE;
        static const char *EXT_TXT;
        static const char *EXT_BIN;
        static const char *EXT_SHADERLAB;
        static const char *EXT_DYLIB;
        static const char *EXT_ANIMATION;
        
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
            /// 图片
            kImage,
            /// 著色器
            kShader,
            /// 网格数据
            kMesh,
            /// 蒙皮网格数据
            kSkinnedMesh,
            /// 骨骼动画数据
            kSkeletalAnimation,
            /// 预制件
            kPrefab,
            /// 场景
            kScene,
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
        const UUID &getUUID() const { return mUUID; }

        /**
         * \brief 获取资源名称
         * \return 返回资源名称
         */
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        const String &getFilename() const { return mFilename; }

        /**
         * \brief 获取资源加载状态
         * \return 返回资源加载状态
         */
        State getState() const { return mState; }

        bool isCloned() const { return mIsCloned; }
        
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
         * \brief 创建回调
         * \return 返回 T3D_OK 以示成功，否则创建会失败
         */
        virtual TResult onCreate();

        /**
         * \brief 保存回调
         * \return 返回 T3D_OK 以示成功，否则保存会失败
         */
        virtual TResult onSave(Archive *archive);

        /**
         * \brief 加载回调
         * \return 返回 T3D_OK 以示成功，否则加载会失败
         */
        virtual TResult onLoad(Archive *archive);

        /**
         * \brief 卸载回调
         * \return 返回 T3D_OK 以示成功，否则卸载会失败
         */
        virtual TResult onUnload();

        /**
         * @brief 需要加载资源的组件的回调
         * @param component : 要加载资源的组件对象
         */
        TFUNCTION()
        virtual void onAddComponentForLoadingResource(Component *component);

    private:
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

        void setFilename(const String &filename) { mFilename = filename; }
        
    protected:
        /// 资源的 UUID
        UUID                mUUID {};
        /// 资源状态
        State               mState {State::kUnloaded};
        /// 资源名称
        String              mName {};
        /// 資源文件名
        String              mFilename {};
        /// 异步加载回调
        CompletedCallback   mCompletedCB {nullptr};
        /// 是否克隆出来的对象
        bool mIsCloned {false};

        /// Component* : 组件对象
        using NeedToLoadResourceComponents = TSet<Component*>;
        
        /// 需要在 onLoad 完去加载资源的 component 列表
        NeedToLoadResourceComponents mNeedToLoadResourceComponents {};
    };
}


#endif  /*__T3D_RESOURCE_H__*/
