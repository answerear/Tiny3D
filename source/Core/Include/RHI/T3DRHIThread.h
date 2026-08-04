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


#ifndef __T3D_RHI_THREAD_H__
#define __T3D_RHI_THREAD_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "T3DRHICommand.h"
#include <functional>
#include <tuple>


namespace Tiny3D
{
    /**
     * \brief 专用 RHI 线程：双缓冲命令队列，将 GPU 操作从引擎主线程异步投递执行
     */
    class T3D_ENGINE_API RHIThread : public Runnable, public Object, public Singleton<RHIThread>
    {
    public:
        /**
         * \brief 工厂方法，创建 RHIThread 实例
         * \return 堆分配的 RHIThreadPtr
         */
        static RHIThreadPtr create();

        /// 析构时清空两条命令队列中的命令
        ~RHIThread() override;

        /**
         * \brief 初始化双缓冲命令队列并标记线程为运行态
         * \return 始终返回 true
         */
        bool init() override;

        /**
         * \brief RHI 线程主循环：等待 resume 信号，执行当前处理队列中的命令，然后唤醒引擎线程
         * \return 线程退出时返回 T3D_OK
         */
        TResult run() override;

        /**
         * \brief 请求停止 RHI 线程并唤醒等待
         */
        void stop() final;

        /**
         * \brief 线程退出钩子（当前为空实现）
         */
        void exit() override;

        /**
         * \brief 交换命令队列索引并触发 RHI 线程执行
         * \remarks 由引擎主线程在每帧 RHI 工作提交完成后调用
         */
        void resume();

        /// 返回 mIsRunning，表示 RHI 线程是否处于运行循环中
        bool isRunning() const { return mIsRunning; }

        /**
         * \brief 向当前入队缓冲追加一条 RHI 命令
         * \param [in] command : 待执行的命令对象所有权转移给队列
         * \return 始终返回 T3D_OK
         */
        TResult addCommand(RHICommand * command);

        /// RHI 线程首次执行命令前的初始化回调类型（如绑定 GL context）
        using ThreadInitCallback = std::function<void()>;

        /**
         * \brief 设置 RHI 线程一次性初始化回调
         * \param [in] cb : 在线程 run 循环中首次处理命令前调用，调用后清空
         */
        void setThreadInitCallback(ThreadInitCallback cb) { mThreadInitCallback = std::move(cb); }

        /**
         * \brief 提取可调用对象的参数类型元组（内部 trait，供 enqueue_unique_command 使用）
         * \tparam T : 待分析的类型
         */
        template <typename T>
        struct function_traits;

        template <typename R, typename... Args>
        struct function_traits<R(Args...)>
        {
            using arg_types = std::tuple<Args...>;
        };

        template <typename R, typename... Args>
        struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
        {};

        template <typename R, typename... Args>
        struct function_traits<R(&)(Args...)> : public function_traits<R(Args...)>
        {};

        template <typename R, typename C, typename... Args>
        struct function_traits<R(C::*)(Args...)> : public function_traits<R(Args...)>
        {};

        template <typename R, typename C, typename... Args>
        struct function_traits<R(C::*)(Args...) const> : public function_traits<R(Args...)>
        {};

        template <typename Lambda>
        struct function_traits : public function_traits<decltype(&Lambda::operator())>
        {};

        /**
         * \brief 将 std::tuple 参数类型映射到模板类（内部 trait）
         * \tparam Action : 可调用对象类型
         * \tparam Tuple : 参数元组类型
         * \tparam Template : 目标模板模板参数
         */
        template <typename Action, typename Tuple, template <typename...> class Template>
        struct tuple_to_template;

        template <typename Action, typename... Args, template <typename...> class Template>
        struct tuple_to_template<Action, std::tuple<Args...>, Template>
        {
            using type = Template<Action, Args...>;
        };

        /**
         * \brief 将可调用对象封装为 RHICommand 并入队；若 RHI 线程未运行则在当前线程同步执行
         * \tparam Action : 可调用对象类型，须返回 TResult
         * \tparam Args : 参数类型包
         * \param [in] action : 待执行的 RHI 操作
         * \param [in] args : 转发给 action 的参数
         * \return action 的返回值，或 addCommand 的返回值
         */
        template<typename Action, typename... Args>
        TResult enqueue_unique_command(Action action, Args... args)
        {
            TResult ret = T3D_OK;

            using arg_types = typename function_traits<Action>::arg_types;
            using command_type = typename tuple_to_template<Action, arg_types, RHICommandT>::type;

            if (isRunning())
            {
                command_type *cmd = T3D_NEW command_type(std::move(args)..., action);
                ret = addCommand(cmd);
            }
            else
            {
                ret = action(args...);
                //ret = std::apply(action, std::make_tuple(args...));
            }

            return ret;
        }

    protected:
        /// 受保护默认构造
        RHIThread();

        /**
         * \brief 切换处理/入队队列索引，并清空即将被处理的队列
         */
        void exchange();

        enum
        {
            kMaxCommandLists = 2,   ///< 双缓冲命令队列数量
        };

        /// 唤醒 RHI 线程的事件对象
        Event                           mEvent;

        /// 双缓冲命令队列（mHanldeCommandListIdx 处理，mEnqueueCommandListIdx 入队）
        TArray<TList<RHICommandPtr>>    mCommandLists {};
        /// 当前 RHI 线程正在执行的队列索引
        int32_t                         mHanldeCommandListIdx {0};
        /// 当前主线程写入的队列索引
        int32_t                         mEnqueueCommandListIdx {0};
        /// RHI 线程是否在 run 循环中
        bool                            mIsRunning {false};
        /// 线程首次执行命令前的一次性初始化回调
        ThreadInitCallback              mThreadInitCallback;
    };

    #define T3D_RHI_THREAD      (RHIThread::getInstance())

    #define ENQUEUE_UNIQUE_COMMAND(ACTION, ...) RHIThread::getInstance().enqueue_unique_command(ACTION, ##__VA_ARGS__)

    // #define ENQUEUE_UNIQUE_COMMAND_RET(RET, ACTION, ...) \
    //     { \
    //         auto lambda = ACTION; \
    //         RET = RHIThread::getInstance().enqueue_unique_command(lambda, ##__VA_ARGS__); \
    //     }
    //
    // #define ENQUEUE_UNIQUE_COMMAND_RETURN(ACTION, ...) \
    //     auto lambda = ACTION; \
    //     return RHIThread::getInstance().enqueue_unique_command(lambda, ##__VA_ARGS__)
    
    T3D_EXTERN_SINGLETON(RHIThread)
}


#endif  /*__T3D_RHI_THREAD_H__*/
