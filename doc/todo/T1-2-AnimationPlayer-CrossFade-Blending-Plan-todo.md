# T1-2 实现计划：AnimationPlayer 双/多片段交叉混合过渡（CrossFade Blending）

> **所属批次**：T1（引擎渲染管线与动画半成品补齐）  
> **前置依赖**：T0-4（`AnimationPlayer` 单 Clip 按名查找与 `mElapsedMS` 时间累积）  
> **预估工作量**：2.5 人日  
> **涉及文件**：
> - `source/Core/Include/Animation/T3DAnimationPlayer.h`
> - `source/Core/Source/Animation/T3DAnimationPlayer.cpp`

---

## 1. 现状分析（代码依据）

在 `source/Core/Include/Animation/T3DAnimationPlayer.h:63-81` 与 `source/Core/Source/Animation/T3DAnimationPlayer.cpp:213-224` 中，头文件早已公开声明了三个混合相关的接口，但实现均为空：

1. **单片段接口中的 `isBlending` 参数被忽略**：
   - `ID playClip(const String &clipName, bool isBlending, bool isLoop = false, bool isGPUSkinning = false);`
   - 当正在播放动作 A 时调用 `playClip("B", /*isBlending=*/true)`，当前实现直接把关键帧游标清零硬切到动作 B，无任何过渡。
2. **两片段混合重载直接返回 `INVALID_ID`**：
   ```cpp
   // T3DAnimationPlayer.cpp:213-216
   ID AnimationPlayer::playClip(const String &fromClipName, const String &toClipName,
       uint32_t toClipStartedMS, bool isLoop, bool isGPUSkinning)
   {
       return INVALID_ID;
   }
   ```
3. **多片段混合重载直接返回 `INVALID_ID`**：
   ```cpp
   // T3DAnimationPlayer.cpp:220-223
   ID AnimationPlayer::playClips(const StringArray &clipNames, bool isGPUSkinning)
   {
       return INVALID_ID;
   }
   ```

---

## 2. 设计方案（建议实现）

### 2.1 引入播放轨道状态结构 `ClipState`

将原本散落在 `AnimationPlayer` 成员里的单片段状态封装为 `ClipState`，使播放器能同时维护「主轨道（当前/源片段）」与「过渡轨道（目标片段）」或多条并行混合轨道：

```cpp
// 建议实现（位于 AnimationPlayer protected 区）
struct ClipState
{
    String          clipName {};
    AnimationClip  *clip {nullptr};
    uint32_t        elapsedMS {0};
    bool            isLoop {false};
    Real            weight {REAL_ONE};
    uint32_t        frameT[T3D_MAX_SKIN_BONES] {0};
    uint32_t        frameO[T3D_MAX_SKIN_BONES] {0};
    uint32_t        frameS[T3D_MAX_SKIN_BONES] {0};
};
```

在 `AnimationPlayer` 中持有：
- `ClipState mPrimaryClip {};`（当前主片段或 `fromClip`）
- `ClipState mTargetClip {};`（正在淡入的 `toClip`，`clip == nullptr` 表示非过渡态）
- `uint32_t mBlendDurationMS {200};`（默认交叉淡入淡出时长，如 200ms）
- `uint32_t mBlendElapsedMS {0};`（当前过渡已进行时长）
- `TArray<ClipState> mLayeredClips {};`（供 `playClips` 多片段并行叠加使用）

### 2.2 三个接口的具体行为定义

1. **`playClip(clipName, isBlending, isLoop, isGPUSkinning)`**：
   - 若当前未在播放，或 `!isBlending`：直接初始化 `mPrimaryClip` 为 `clipName`，清空 `mTargetClip`；
   - 若当前正在播放且 `isBlending == true` 且 `clipName != mPrimaryClip.clipName`：
     - 将 `mTargetClip` 初始化为 `clipName`（`elapsedMS = 0`），设置 `mBlendDurationMS = 250`（可配）、`mBlendElapsedMS = 0`，平滑启动 CrossFade。
2. **`playClip(fromClipName, toClipName, toClipStartedMS, isLoop, isGPUSkinning)`**：
   - 校验 `fromClipName` 与 `toClipName` 均存在于 `SkeletalAnimation`；
   - `mPrimaryClip` 设为 `fromClipName`（`elapsedMS = 0`）；
   - `mTargetClip` 设为 `toClipName`（`elapsedMS = 0`）；
   - 过渡触发点与持续时间：当 `mPrimaryClip.elapsedMS >= toClipStartedMS` 时开始进入混合区间，混合时长取 `std::min(fromClip->getDuration() - toClipStartedMS, toClip->getDuration())`（若 `toClipStartedMS == 0` 则立即以默认 250ms 混合时长开始过渡）；
   - 生成并返回有效 `PlaybackID`。
3. **`playClips(clipNames, isGPUSkinning)`**：
   - 校验 `clipNames` 非空；
   - 将每个有效片段放入 `mLayeredClips`，默认均分权重 `weight = 1.0f / N`（或按骨骼轨道独立覆盖：若片段 A 只含下半身骨骼 Track、片段 B 只含上半身骨骼 Track，则各自在有效骨骼上取满权重，实现上下半身分层动画！）。

### 2.3 骨骼姿势采样与插值混合（`updateBones`）

抽出单片段对单根骨骼的采样函数：

```cpp
bool sampleBonePose(ClipState &state, size_t boneIdx, const String &boneName,
    Vector3 &outTrans, Quaternion &outRot, Vector3 &outScale);
```

在 `updateBones()` 中：
1. 若处于双片段 CrossFade 过渡态（`mTargetClip.clip != nullptr`）：
   - 计算混合因子 $\alpha = \text{clamp}(\text{float}(mBlendElapsedMS) / mBlendDurationMS, 0.0f, 1.0f)$；
   - 对每根骨骼分别调用 `sampleBonePose(mPrimaryClip, ...)` 得到 $(T_0, R_0, S_0)$，调用 `sampleBonePose(mTargetClip, ...)` 得到 $(T_1, R_1, S_1)$；
   - 混合：
     - `T = Vector3::lerp(T0, T1, alpha)`
     - `R = Quaternion::slerp(R0, R1, alpha)`（注意处理四元数点积 `< 0` 时的最短路径号反转）
     - `S = Vector3::lerp(S0, S1, alpha)`
   - 当 $\alpha \ge 1.0f$ 时，过渡完成：执行 `mPrimaryClip = mTargetClip; mTargetClip = ClipState{};`，无缝切入目标片段独立播放。
2. 若处于 `playClips` 多片段模式：
   - 对每根骨骼遍历 `mLayeredClips`，仅汇总包含该骨骼 Track 的片段（若只有一个片段含该骨骼，则直接采用其 TRS，天然支持上下半身分离动画；若有多个片段含同一骨骼，则按归一化权重顺序 `lerp` / `slerp`）。

---

## 3. 验收标准

1. 在 `SkeletalAnimationApp` 中构造两个关键帧姿态不同的 `AnimationClip`（或同一 Clip 的不同速率/偏移副本）；
2. 调用 `playClip("ClipA", "ClipB", ...)` 或在播放中调用 `playClip("ClipB", /*isBlending=*/true)`，观察骨骼在过渡期内平滑插值，无瞬间跳变；
3. 过渡结束后自动稳定在 `ClipB` 循环播放。
