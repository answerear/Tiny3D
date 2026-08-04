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

#ifndef __T3D_ANIMATION_PLAYER_MGR_H__
#define __T3D_ANIMATION_PLAYER_MGR_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 全局 AnimationPlayer 单例管理器，集中调度蒙皮与播放器生命周期
     */
    class T3D_ENGINE_API AnimationPlayerMgr final
        : public Singleton<AnimationPlayerMgr>
        , public Object 
    {
    public:
        /**
         * \brief 创建管理器实例
         * \return AnimationPlayerMgr 智能指针
         */
        static AnimationPlayerMgrPtr create();

        /// 析构函数
        ~AnimationPlayerMgr() override;

        /**
         * \brief 注册播放器
         * \param [in] player : 待加入的 AnimationPlayer 指针
         * \return 成功返回 T3D_OK；已存在返回 T3D_ERR_DUPLICATED_ITEM；插入失败返回 T3D_ERR_FAIL
         */
        TResult addPlayer(AnimationPlayer *player);

        /**
         * \brief 标记播放器待移除（延迟到 update 时从映射表删除）
         * \param [in] player : 待移除的 AnimationPlayer 指针
         * \return 成功返回 T3D_OK；不在表中返回 T3D_ERR_NOT_FOUND
         */
        TResult removePlayer(AnimationPlayer *player);

        /**
         * \brief 立即从映射表删除播放器
         * \param [in] player : 待删除的 AnimationPlayer 指针
         * \return 成功返回 T3D_OK；不在表中返回 T3D_ERR_NOT_FOUND
         */
        TResult deletePlayer(AnimationPlayer *player);

        /// 清空全部播放器映射，不处理待移除队列
        void removeAllPlayers();
        
        /**
         * \brief 帧更新：对所有已注册播放器执行蒙皮，并处理待移除队列
         */
        void update();
        
    protected:
        /// 受保护构造，供 create 与单例使用
        AnimationPlayerMgr();

        /// 遍历 mPlayers 调用各播放器的 skinning()
        void skinning();

        /// 将 mWaitingRemovePlayers 中的播放器从 mPlayers 移除
        void deleteAllPlayers();

    protected:
        /// 已注册的 AnimationPlayer 映射（指针 → 智能指针）
        AnimationPlayers    mPlayers {};

        using WaitingForRemoveAnimationPlayers = TArray<AnimationPlayerPtr>;
        /// 待延迟删除的播放器队列
        WaitingForRemoveAnimationPlayers mWaitingRemovePlayers {};
    };

    #define T3D_ANIMATION_PLAYER_MGR    (AnimationPlayerMgr::getInstance())

    T3D_EXTERN_SINGLETON(AnimationPlayerMgr)
}


#endif    /*__T3D_ANIMATION_PLAYER_MGR_H__*/
