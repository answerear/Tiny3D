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

#ifndef __T3D_SKELETON_MANAGER_H__
#define __T3D_SKELETON_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 骨架资源单例管理器，负责 Skeleton 的创建、加载与保存
     */
    class T3D_ENGINE_API SkeletonManager
        : public Singleton<SkeletonManager>
        , public ResourceManager
    {
    public:
        /**
         * \brief 创建 SkeletonManager 实例
         * \return 新 SkeletonManager 智能指针
         */
        static SkeletonManagerPtr create();

        /**
         * \brief 创建并绑定根骨骼 GameObject 的 Skeleton 资源
         * \param [in] name : 骨架名称
         * \param [in] rootBoneGameObject : 根骨骼 GameObject
         * \param [in] uuid : 资源 UUID；T3D_EDITOR 下可指定以保留已有 guid，默认 UUID::INVALID
         * \return 新 Skeleton 智能指针
         */
        SkeletonPtr createSkeleton(const String &name, GameObject *rootBoneGameObject, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 按文件名从 Archive 加载 Skeleton
         * \param [in] archive : 资源档案
         * \param [in] filename : 骨架文件名
         * \return 成功返回 Skeleton 智能指针，失败返回 nullptr
         */
        SkeletonPtr loadSkeleton(Archive *archive, const String &filename);

        /**
         * \brief 按 UUID 从 Archive 加载 Skeleton
         * \param [in] archive : 资源档案
         * \param [in] uuid : 资源 UUID
         * \return 成功返回 Skeleton 智能指针，失败返回 nullptr
         */
        SkeletonPtr loadSkeleton(Archive *archive, const UUID &uuid);

        /**
         * \brief 将 Skeleton 保存到 Archive 指定文件名
         * \param [in] archive : 目标档案
         * \param [in] filename : 保存文件名
         * \param [in] skeleton : 待保存 Skeleton，不可为 nullptr
         * \return 成功返回 T3D_OK
         */
        TResult saveSkeleton(Archive *archive, const String &filename, Skeleton *skeleton);

        /**
         * \brief 将 Skeleton 按自身 UUID 保存到 Archive
         * \param [in] archive : 目标档案
         * \param [in] skeleton : 待保存 Skeleton，不可为 nullptr
         * \return 成功返回 T3D_OK
         */
        TResult saveSkeleton(Archive *archive, Skeleton *skeleton);

    protected:
        /**
         * \brief 创建 Skeleton 并绑定 rootBoneGameObject；T3D_EDITOR 且 argc==2 时设置 UUID
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数（1 或 2）
         * \param [in] args : rootBoneGameObject；T3D_EDITOR 下可为 UUID*
         * \return 新 Skeleton 智能指针
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 从数据流反序列化 Skeleton
         * \param [in] name : 资源名称（未使用）
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Skeleton 智能指针
         */
        ResourcePtr loadResource(const String &name, DataStream &stream) override;

        /**
         * \brief 从数据流反序列化 Skeleton
         * \param [in,out] stream : 输入数据流
         * \return 反序列化得到的 Skeleton 智能指针
         */
        ResourcePtr loadResource(DataStream &stream) override;

        /**
         * \brief 将 Skeleton 序列化写入数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] res : 待保存的 Skeleton 资源（须为 kSkeleton 类型）
         * \return 序列化结果
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_SKELETON_MGR    (SkeletonManager::getInstance())

    T3D_EXTERN_SINGLETON(SkeletonManager)
}


#endif    /*__T3D_SKELETON_MANAGER_H__*/
