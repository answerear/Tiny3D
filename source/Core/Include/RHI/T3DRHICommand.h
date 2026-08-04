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


#ifndef __T3D_RHI_COMMAND_H__
#define __T3D_RHI_COMMAND_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief RHI 命令抽象基类，封装可在 RHI 线程上执行的 deferred 操作
     */
    class T3D_ENGINE_API RHICommand : public Object
    {
    public:
        /// 默认虚析构
        virtual ~RHICommand() = default;

        /**
         * \brief 执行本命令绑定的 RHI 操作
         * \return 操作返回值，由具体命令实现决定
         */
        virtual TResult execute() = 0;
    };

    /**
     * \brief 将可调用对象及其参数打包为 RHICommand 的模板实现
     * \tparam ACTION : 可调用对象类型，返回 TResult
     * \tparam Args : 绑定到可调用对象的参数类型包
     */
    template<typename ACTION, typename... Args>
    class RHICommandT : public RHICommand
    {
        using TAction = ACTION;

    public:
        /**
         * \brief 构造命令，保存参数元组与可调用对象
         * \param [in] args : 转发给 action 的参数
         * \param [in] action : 在 execute 时通过 std::apply 调用的可调用对象
         */
        RHICommandT(Args&&... args, TAction action)
            : mArgs(std::forward<typename std::remove_reference<Args>::type>(args)...)
            , mAction(action)
        {
        }

        /**
         * \brief 以保存的参数调用 mAction
         * \return std::apply(mAction, mArgs) 的返回值
         */
        TResult execute() override
        { 
            return std::apply(mAction, mArgs);
        }
        
    private:
        /// 绑定参数元组
        TTuple<typename std::remove_reference<Args>::type...> mArgs;
        /// 待执行的可调用对象
        TAction         mAction;
    };
}


#endif  /*__T3D_RHI_COMMAND_H__*/
