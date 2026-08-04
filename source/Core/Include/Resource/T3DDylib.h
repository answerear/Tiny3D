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


#ifndef __T3D_DYLIB_H__
#define __T3D_DYLIB_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    /**
     * \brief 动态库资源，封装平台 dlopen/LoadLibrary 加载的插件模块
     */
    class T3D_ENGINE_API Dylib : public Resource
    {
    public:
        /**
         * \brief 创建 Dylib 对象
         * \param [in] name : 动态库逻辑名称（不含扩展名；加载时会按平台拼接 lib 前缀与后缀）
         * \return 新建的 Dylib 智能指针
         */
        static DylibPtr create(const String &name);

        /// 析构函数
        ~Dylib() override;

        /**
         * \brief 获取资源类型
         * \return 固定返回 Type::kDylib
         */
        Type getType() const override;

        /**
         * \brief 按符号名获取动态库导出地址
         * \param [in] name : 符号名称
         * \return 找到返回符号地址；未加载或符号不存在时返回 nullptr
         */
        virtual void *getSymbol(const String &name) const;

    protected:
        /**
         * \brief 构造动态库资源
         * \param [in] name : 动态库逻辑名称
         */
        Dylib(const String &name);

        /**
         * \brief 克隆动态库资源
         * \return 新 Dylib 智能指针（仅拷贝属性，不重复加载库文件）
         */
        ResourcePtr clone() const override;

        /**
         * \brief 加载动态库文件
         * \param [in] archive : 档案对象（本实现未使用）
         * \return 加载失败返回 T3D_ERR_PLG_LOAD_FAILED；成功则调用基类 onLoad 并返回其结果
         * \remarks 非 Android 从 Agent::getPluginsPath() 下加载；Android 直接按拼接后的库名加载
         */
        TResult onLoad(Archive *archive) override;

        /**
         * \brief 卸载动态库
         * \return 状态为 kLoaded 时调用平台卸载 API，再执行基类 onUnload
         */
        TResult onUnload() override;
     
    protected:
        /// 平台动态库句柄
        THandle mHandle;
    };
}


#endif  /*__T3D_DYLIB_H__*/
