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


#include "Animation/T3DAnimationPlayerMgr.h"
#include "Animation/T3DAnimationPlayer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    AnimationPlayerMgrPtr AnimationPlayerMgr::create()
    {
        return new AnimationPlayerMgr();
    }

    //--------------------------------------------------------------------------

    AnimationPlayerMgr::AnimationPlayerMgr()
    {
        
    }

    //--------------------------------------------------------------------------

    AnimationPlayerMgr::~AnimationPlayerMgr()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayerMgr::addPlayer(AnimationPlayer *player)
    {
        const auto it = mPlayers.find(player);
        if (it != mPlayers.end())
        {
            T3D_LOG_WARNING(LOG_TAG_ANIMATION, "Animation player already exist !");
            return T3D_ERR_DUPLICATED_ITEM;
        }

        const auto ret = mPlayers.emplace(player, player);
        if (ret.second)
        {
            return T3D_OK;
        }
        
        return T3D_ERR_FAIL;
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayerMgr::removePlayer(AnimationPlayer *player)
    {
        TResult ret = T3D_OK;
        
        do
        {
            const auto it = mPlayers.find(player);
            if (it == mPlayers.end())
            {
                T3D_LOG_WARNING(LOG_TAG_ANIMATION, "Animation player already removed !");
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            mWaitingRemovePlayers.emplace_back(player);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult AnimationPlayerMgr::deletePlayer(AnimationPlayer *player)
    {
        TResult ret = T3D_OK;

        do
        {
            const auto it = mPlayers.find(player);
            if (it == mPlayers.end())
            {
                T3D_LOG_WARNING(LOG_TAG_ANIMATION, "Animation player already removed !");
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            mPlayers.erase(it);
        } while (false);
        

        return ret;
    }

    //--------------------------------------------------------------------------

    void AnimationPlayerMgr::update()
    {
        skinning();
        deleteAllPlayers();
    }

    //--------------------------------------------------------------------------

    void AnimationPlayerMgr::skinning()
    {
        for (const auto &player : mPlayers)
        {
            player.second->skinning();
        }
    }

    //--------------------------------------------------------------------------

    void AnimationPlayerMgr::deleteAllPlayers()
    {
        for (const auto &player : mWaitingRemovePlayers)
        {
            deletePlayer(player.get());
        }

        mWaitingRemovePlayers.clear();
    }

    //--------------------------------------------------------------------------

    void AnimationPlayerMgr::removeAllPlayers()
    {
        mPlayers.clear();
    }

    //--------------------------------------------------------------------------
}
