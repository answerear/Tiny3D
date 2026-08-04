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


#ifndef __T3D_DYLIB_MANAGER_H__
#define __T3D_DYLIB_MANAGER_H__


#include "T3DPrerequisites.h"
#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    /**
     * \brief 动态库资源管理器，负责 Dylib 的加载与卸载
     */
    class T3D_ENGINE_API DylibManager 
        : public Singleton<DylibManager>
        , public ResourceManager
    {
    public:
        /**
         * \brief 创建 DylibManager 单例对象
         * \return 新建的 DylibManager 智能指针
         */
        static DylibManagerPtr create();

        /// 析构函数
        ~DylibManager() override = default;

        /**
         * \brief 按名称加载动态库
         * \param [in] name : 动态库逻辑名称
         * \return 成功返回 Dylib 智能指针；加载失败时返回 nullptr
         * \remarks 内部 archive 传 nullptr，实际加载由 Dylib::onLoad 完成
         */
        DylibPtr loadDylib(const String &name);

        /**
         * \brief 卸载动态库
         * \param [in] dylib : 要卸载的 Dylib 对象
         * \return 转调 ResourceManager::unload 的返回值
         */
        TResult unloadDylib(DylibPtr dylib);

    protected:
        /// 默认构造
        DylibManager() = default;

        /**
         * \brief 创建 Dylib 资源实例
         * \param [in] name : 资源名称
         * \param [in] argc : 可变参数个数（未使用）
         * \param [in] args : 可变参数列表（未使用）
         * \return Dylib::create(name) 的结果
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * \brief 按名称加载 Dylib（不读档案内容）
         * \param [in] archive : 档案对象（未使用）
         * \param [in] filename : 动态库名称
         * \return Dylib::create(filename) 的结果
         */
        ResourcePtr loadResource(Archive *archive, const String &filename) override;
        
        /**
         * \brief 从数据流加载 Dylib（不解析流内容）
         * \param [in] filename : 动态库名称
         * \param [in,out] stream : 数据流（未使用）
         * \return Dylib::create(filename) 的结果
         */
        ResourcePtr loadResource(const String &filename, DataStream &stream) override;

        /**
         * \brief 保存 Dylib 到数据流（未实现）
         * \param [in,out] stream : 数据流
         * \param [in] res : 资源对象
         * \return 触发断言并返回 T3D_ERR_NOT_IMPLEMENT
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_DYLIB_MGR   (DylibManager::getInstance())

    T3D_EXTERN_SINGLETON(DylibManager)
}


#endif  /*__T3D_DYLIB_MANAGER_H__*/
