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


#ifndef __T3D_RHI_THREAD_H__
#define __T3D_RHI_THREAD_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RHIThread : public Runnable, public Object, public Singleton<RHIThread>
    {
    public:
        static RHIThreadPtr create();
        
        ~RHIThread() override;
        
        bool init() override;
        TResult run() override;
        void stop() final;
        void exit() override;

        void resume();
        
        bool isRunning() const { return mIsRunning; }

        TResult addCommand(RHICommand * command);

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

        template <typename Action, typename Tuple, template <typename...> class Template>
        struct tuple_to_template;

        template <typename Action, typename... Args, template <typename...> class Template>
        struct tuple_to_template<Action, std::tuple<Args...>, Template>
        {
            using type = Template<Action, Args...>;
        };

        template<typename Action, typename... Args>
        TResult enqueue_unique_command(Action action, Args... args)
        {
            TResult ret = T3D_OK;

            using arg_types = typename function_traits<Action>::arg_types;
            using command_type = typename tuple_to_template<Action, arg_types, RHICommandT>::type;

            if (isRunning())
            {
                command_type *cmd = new command_type(args..., action);
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
        RHIThread();
        
        void exchange();
        
        enum
        {
            kMaxCommandLists = 2,
        };

        Event                           mEvent;

        /// 双缓冲命令队列
        TArray<TList<RHICommandPtr>>    mCommandLists {};
        /// 当前线程执行队列序号
        int32_t                         mHanldeCommandListIdx {0};
        /// 当前入队列序号
        int32_t                         mEnqueueCommandListIdx {0};
        /// 线程是否在运行
        bool                            mIsRunning {false};
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
    
}


#endif  /*__T3D_RHI_THREAD_H__*/
