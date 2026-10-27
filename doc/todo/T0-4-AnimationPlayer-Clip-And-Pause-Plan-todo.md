# T0-4 实现计划：AnimationPlayer 按名称查找 Clip 与暂停/恢复闭环

> **所属批次**：T0（引擎核心层小切口收尾）  
> **预估工作量**：1.0 人日  
> **涉及文件**：
> - `source/Core/Include/Animation/T3DAnimationPlayer.h`
> - `source/Core/Source/Animation/T3DAnimationPlayer.cpp`
> - `source/Samples/SkeletalAnimationApp/`（验证）

---

## 1. 现状分析（代码依据）

核查 `source/Core/Include/Animation/T3DAnimationPlayer.h` 与 `source/Core/Source/Animation/T3DAnimationPlayer.cpp`，当前单 Clip 播放存在 4 个明显断点：

1. **`playClip` 不保存 `clipName`，`updateBones` 永远只取首个 Clip**：
   - `T3DAnimationPlayer.cpp:158` 校验了 `if (clipName.empty())`，但完全没有保存 `clipName`，也没有校验该名称是否存在于 `SkeletalAnimation` 中；
   - `T3DAnimationPlayer.cpp:277` 的 `updateBones()` 直接写死 `AnimationClip *clip = clips.begin()->second;`，导致包含多个动作片段的模型永远只能播放第一个动作。
2. **`playClip` 恒返回 `INVALID_ID`**：
   - `T3DAnimationPlayer.h:124` 已经写好了 `ID generateID() const { return ++msGeneratedID; }` 和成员 `ID mCurrentPlaybackID {INVALID_ID};`，但 `playClip()` 局部变量写成了 `TResult ret = INVALID_ID;`，成功路径既未调用 `generateID()` 赋值给 `mCurrentPlaybackID`，也未更新 `ret`，导致调用方拿到的永远是 `INVALID_ID`。
3. **`pausePlayback` 与 `resumePlayback` 为空操作**：
   - `T3DAnimationPlayer.cpp:236-247` 两个函数直接 `return T3D_OK;`，没有任何暂停逻辑。
4. **动画时间直接使用墙钟 `DateTime::currentMSecsSinceEpoch()`，不受暂停或 `Time::timeScale` 控制，且回绕时关键帧游标未重置**：
   - `T3DAnimationPlayer.cpp:282` 用 `currentTS - mStartTimestamp` 计算 `elapsed`，一旦暂停再恢复，时间会直接跳变到墙钟差值。

---

## 2. 实现方案（建议实现）

### 2.1 数据成员扩展（`T3DAnimationPlayer.h`）

在 `AnimationPlayer` 的 `protected` 区补充：

```cpp
        /// 当前正在播放的动画片段名称
        String mCurrentClipName {};
        /// 当前绑定的 AnimationClip 缓存指针
        AnimationClip *mCurrentClip {nullptr};
        /// 是否处于暂停状态
        bool mIsPaused {false};
        /// 已累积播放时长（毫秒），替代单纯的 DateTime 差值以支持暂停/恢复与 Time::deltaTime
        uint32_t mElapsedMS {0};
        /// 上一帧更新时的墙钟时间戳（毫秒）
        int64_t mLastUpdateTimestamp {0};
```

### 2.2 修复 `playClip`：按名查找 + 生成有效 Playback ID

在 `T3DAnimationPlayer.cpp:146` 中：

1. 从 `mSkinnedGeometry->getMeshObject()` 取出 `SkinnedMesh *` 与 `SkeletalAnimation *`（判空保护）；
2. 在 `skeletalAni->getAnimationClips()` 中按 `clipName` 查找：
   - 若找不到且 `clipName == "default"`（兼容旧测试代码），可回退到 `clips.begin()->second` 并打一条 `T3D_LOG_WARNING`；
   - 若找不到则返回 `INVALID_ID` 并打印 `T3D_LOG_ERROR`。
3. 找到后：
   ```cpp
   mCurrentClipName = clipName;
   mCurrentClip = clip;
   mElapsedMS = 0;
   mLastUpdateTimestamp = DateTime::currentMSecsSinceEpoch();
   mIsPlaying = true;
   mIsPaused = false;
   mIsLoop = isLoop;
   mCurrentPlaybackID = generateID();
   ret = mCurrentPlaybackID;
   ```

### 2.3 实现 `pausePlayback` / `resumePlayback` / `stopPlayback`

```cpp
// 建议实现
TResult AnimationPlayer::pausePlayback(ID playbackID)
{
    if (!mIsPlaying || (playbackID != INVALID_ID && playbackID != mCurrentPlaybackID))
    {
        return T3D_ERR_INVALID_PARAM;
    }

    if (!mIsPaused)
    {
        int64_t now = DateTime::currentMSecsSinceEpoch();
        if (now > mLastUpdateTimestamp)
        {
            mElapsedMS += static_cast<uint32_t>(now - mLastUpdateTimestamp);
        }
        mIsPaused = true;
    }
    return T3D_OK;
}

TResult AnimationPlayer::resumePlayback(ID playbackID)
{
    if (!mIsPlaying || (playbackID != INVALID_ID && playbackID != mCurrentPlaybackID))
    {
        return T3D_ERR_INVALID_PARAM;
    }

    if (mIsPaused)
    {
        mLastUpdateTimestamp = DateTime::currentMSecsSinceEpoch();
        mIsPaused = false;
    }
    return T3D_OK;
}
```

`stopPlayback` 中同步清空 `mIsPlaying = false; mIsPaused = false; mCurrentPlaybackID = INVALID_ID; mCurrentClip = nullptr;`。

### 2.4 修复 `updateAnimation` 与 `updateBones`

1. `updateAnimation()`：若 `!isPlaying() || mIsPaused` 则直接返回，不推进骨骼插值；
2. `updateBones()`：
   - 直接使用 `mCurrentClip`（若为空则按 `mCurrentClipName` 查找）；
   - 每帧累加 `now - mLastUpdateTimestamp` 到 `mElapsedMS`，更新 `mLastUpdateTimestamp = now`；
   - 当 `mElapsedMS >= clip->getDuration()` 且 `mIsLoop == true` 时，`mElapsedMS %= std::max(1u, clip->getDuration())`，并清零 `mCurrentFrameT/O/S` 游标数组，避免循环播放时帧时间跳变。

---

## 3. 验收标准

1. 调用 `playClip("...", ...)` 返回非 `INVALID_ID` 的递增正整数；
2. 传入不存在的 `clipName` 时准确报错并返回 `INVALID_ID`；
3. 调用 `pausePlayback(id)` 后骨骼动作立即定格在当前姿态，等待数秒后调用 `resumePlayback(id)`，动作从定格帧平滑继续，无时间跳变。
