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
    /// 资源异步加载完成时的回调：result 为加载结果，resource 为对应资源对象
    typedef TFunction<void(TResult result, Resource *resource)> CompletedCallback;

    /**
     * \brief 引擎资源基类，统一管理 UUID、名称、文件名与加载状态，并提供 create/load/save/unload 生命周期钩子
     */
    TCLASS()
    class T3D_ENGINE_API Resource : public Object
    {
        friend class ResourceManager;

        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /// 材质资源文件扩展名（".tmat"）
        static const char *EXT_MATERIAL;
        /// 纹理资源文件扩展名（".ttex"）
        static const char *EXT_TEXTURE;
        /// 图像资源文件扩展名（".timg"）
        static const char *EXT_IMAGE;
        /// 着色器资源文件扩展名（".tshader"）
        static const char *EXT_SHADER;
        /// 网格资源文件扩展名（".tmesh"）
        static const char *EXT_MESH;
        /// 预制体资源文件扩展名（".tprefab"）
        static const char *EXT_PREFAB;
        /// 场景资源文件扩展名（".tscene"）
        static const char *EXT_SCENE;
        /// 文本资源扩展名
        static const char *EXT_TXT;
        /// 二进制资源扩展名
        static const char *EXT_BIN;
        /// ShaderLab 源文件扩展名
        static const char *EXT_SHADERLAB;
        /// 动态库扩展名（平台相关：dll / so / dylib）
        static const char *EXT_DYLIB;
        /// 骨骼动画资源扩展名（".tani"）
        static const char *EXT_ANIMATION;
        /// 骨架资源扩展名（".tskel"）
        static const char *EXT_SKELETON;
        
        /**
         * \brief 资源类型枚举
         */
        TENUM()
        enum class Type : uint32_t
        {
            /// 未知类型
            kUnknown = 0,
            /// 动态库
            kDylib,
            /// 材质
            kMaterial,
            /// 纹理
            kTexture,
            /// 图像
            kImage,
            /// 着色器
            kShader,
            /// 网格
            kMesh,
            /// 蒙皮网格
            kSkinnedMesh,
            /// 骨架
            kSkeleton,
            /// 骨骼动画
            kSkeletalAnimation,
            /// 预制体
            kPrefab,
            /// 场景
            kScene,
        };

        /**
         * \brief 资源加载状态
         */
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
         * \brief 析构资源对象
         * \remarks 析构时断言状态须为 kUnloaded
         */
        ~Resource() override;

        /**
         * \brief 获取资源类型
         * \return 资源类型枚举值
         */
        virtual Type getType() const = 0;

        /**
         * \brief 获取资源 UUID
         * \return 资源 UUID 的常量引用
         */
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="getter")
        const UUID &getUUID() const { return mUUID; }

        /**
         * \brief 获取资源逻辑名称
         * \return 资源名称的常量引用
         */
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /**
         * \brief 获取资源在档案中的文件名
         * \return 文件名的常量引用；未从档案加载或保存过时可能为空
         */
        const String &getFilename() const { return mFilename; }

        /**
         * \brief 获取当前加载状态
         * \return 资源状态枚举值
         */
        State getState() const { return mState; }

        /**
         * \brief 判断是否为 ResourceManager 克隆出的副本
         * \return 克隆资源返回 true；克隆体不进入按文件名索引的 LUT
         */
        bool isCloned() const { return mIsCloned; }
        
    protected:
        /// 默认构造，名称置空并自动生成 UUID
        Resource();

        /**
         * \brief 以指定名称构造资源
         * \param [in] name : 资源逻辑名称
         */
        Resource(const String &name);

        /**
         * \brief 克隆资源对象
         * \return 新资源智能指针；由子类实现具体拷贝逻辑
         */
        virtual ResourcePtr clone() const = 0;

        /**
         * \brief 从源资源拷贝通用属性
         * \param [in] src : 源资源对象，不可为 nullptr
         * \remarks 克隆时会重新生成 UUID，并复制名称、文件名、状态与异步回调
         */
        virtual void cloneProperties(const Resource * const src);

        /**
         * \brief 资源创建完成后的回调
         * \return 调用成功返回 T3D_OK；失败时 ResourceManager 会卸载并移出缓存
         * \remarks 默认实现将状态设为 kLoaded
         */
        virtual TResult onCreate();

        /**
         * \brief 资源写入档案前的回调
         * \param [in] archive : 目标档案，可为 nullptr
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult onSave(Archive *archive);

        /**
         * \brief 资源从档案加载完成后的回调
         * \param [in] archive : 来源档案，可为 nullptr
         * \return 调用成功返回 T3D_OK
         * \remarks 默认将状态设为 kLoaded，并通知 mNeedToLoadResourceComponents 中登记的组件加载依赖资源
         */
        virtual TResult onLoad(Archive *archive);

        /**
         * \brief 资源卸载时的回调
         * \return 调用成功返回 T3D_OK
         * \remarks 默认将状态设为 kUnloaded
         */
        virtual TResult onUnload();

        /**
         * \brief 登记需在 onLoad 完成后加载依赖资源的组件
         * \param [in] component : 待通知的组件对象
         * \remarks 基类实现为空；子类可覆写以维护 mNeedToLoadResourceComponents
         */
        TFUNCTION()
        virtual void onAddComponentForLoadingResource(Component *component);

    private:
        /**
         * \brief 设置资源逻辑名称
         * \param [in] name : 新名称
         */
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        /**
         * \brief 设置资源 UUID
         * \param [in] uuid : 新 UUID
         */
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

        /**
         * \brief 设置资源在档案中的文件名
         * \param [in] filename : 文件名
         */
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

        /// 需要在 onLoad 完成后加载依赖资源的 Component 集合
        NeedToLoadResourceComponents mNeedToLoadResourceComponents {};
    };
}


#endif  /*__T3D_RESOURCE_H__*/
