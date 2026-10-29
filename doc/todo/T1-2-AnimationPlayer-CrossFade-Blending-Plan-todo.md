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

## 2. 设计方案（建议实现，面向未来 BlendTree 演进就绪）

为了在不增加当前实现负担的前提下，让底层架构能够零返工平滑演进到未来的**动画混合树（Blend Tree，含 1D/2D 混合、相位同步、分层遮罩）**，本方案采用 **「Clip 状态对象化 + 整姿缓冲（SkeletonPose）+ 独立混合算子」** 的三层解耦设计。

### 2.1 引入姿势缓冲结构 `BonePose` / `SkeletonPose` 与轨道状态 `ClipState`

1. **显式姿势缓冲（Pose Buffer）**——将“姿势采样（Evaluate）”与“场景节点写入（Apply to Transform3D）”彻底解耦，作为未来混合树节点的标准输入/输出载体：

```cpp
// 单根骨骼的局部空间姿势
struct BonePose
{
    Vector3     translation {Vector3::ZERO};
    Quaternion  orientation {Quaternion::IDENTITY};
    Vector3     scaling     {Vector3::UNIT_SCALE};
    uint8_t     validMask   {0}; // bit0: T, bit1: O, bit2: S（标记该骨骼在 Clip 中是否有对应有效轨道）
};

// 整套骨骼的局部姿势缓冲（混合树节点求值的标准 I/O）
struct SkeletonPose
{
    BonePose    bones[T3D_MAX_SKIN_BONES];
    size_t      boneCount {0};
};
```

2. **播放轨道状态结构 `ClipState`**——将原本散落在 `AnimationPlayer` 成员里的单片段状态封装为 `ClipState`（即未来混合树中叶子节点 `ClipNode` 的运行时状态），并引入浮点时间与速率支持未来的**步频/相位同步（Phase Sync）**：

```cpp
// 建议实现（位于 AnimationPlayer protected 区）
struct ClipState
{
    String          clipName {};
    AnimationClip  *clip {nullptr};
    Real            elapsedMS {REAL_ZERO};   // 使用 Real 支持非整数倍速率缩放与归一化时间同步
    Real            speed {REAL_ONE};        // 播放速率倍率，供未来 BlendTree 同步不同周期的 Walk/Run 步频
    Real            weight {REAL_ONE};       // 混合权重
    bool            isLoop {false};
    uint32_t        frameT[T3D_MAX_SKIN_BONES] {0};
    uint32_t        frameO[T3D_MAX_SKIN_BONES] {0};
    uint32_t        frameS[T3D_MAX_SKIN_BONES] {0};

    // 获取/设置归一化进度 [0, 1]，便于未来混合树按统一相位驱动子片段
    Real getNormalizedTime() const;
    void setNormalizedTime(Real normalizedTime);
};
```

> **关键帧游标回退保护**：当 `speed` 动态变化或通过 `setNormalizedTime` 回退时间时，若 `getKeyframe` 检测到当前查询时间 `time < keyframes[startFrame]->getTime()`，需自动将 `startFrame` 重置为 `0` 重新检索。

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

### 2.3 整姿采样、无状态混合算子与骨骼写回（Pose Evaluation Pipeline）

将 `updateBones()` 拆分为 **「采样 → 混合 → 写回」** 三步标准管线，所有算子均围绕 `SkeletonPose` 运作：

1. **单片段整姿采样函数**（未来可直接作为混合树叶子节点 `ClipNode::evaluate` 的实现）：
   ```cpp
   void sampleClipPose(ClipState &state, const BoneGameObjects &bones, SkeletonPose &outPose);
   ```
2. **独立的无状态姿势混合算子**（未来可直接作为 1D/2D 混合节点与分层遮罩节点的内核）：
   ```cpp
   // 双姿态插值（用于 CrossFade 过渡与未来 1D BlendTree 相邻区间插值）
   static void blendPoses(const SkeletonPose &poseA, const SkeletonPose &poseB,
       Real alpha, SkeletonPose &outPose);

   // 多姿态加权混合（用于 playClips 同骨骼多轨道混合与未来 2D BlendTree）
   // 注：四元数采用半球符号对齐（dot < 0 时取反）后的加权累加并归一化（Weighted Nlerp），保证顺序无关性
   static void blendPosesWeighted(const SkeletonPose *poses, const Real *weights,
       size_t count, SkeletonPose &outPose);

   // 分层/局部轨道覆盖混合（基于 BonePose::validMask 或未来的 BoneMask 实现上下半身分层叠加）
   static void blendPosesLayered(const SkeletonPose &basePose, const SkeletonPose &layerPose,
       Real layerWeight, SkeletonPose &outPose);
   ```
3. **最终姿势写回场景节点**：
   ```cpp
   void applySkeletonPose(const SkeletonPose &pose, const BoneGameObjects &bones);
   ```

4. **`updateBones()` 管线执行流程**：
   - **双片段 CrossFade 过渡态**（`mTargetClip.clip != nullptr`）：
     - 计算混合因子 $\alpha = \text{clamp}(\text{float}(mBlendElapsedMS) / mBlendDurationMS, 0.0f, 1.0f)$；
     - 分别调用 `sampleClipPose(mPrimaryClip, ..., poseA)` 与 `sampleClipPose(mTargetClip, ..., poseB)`；
     - 调用 `blendPoses(poseA, poseB, alpha, finalPose)`（内部对平移/缩放执行 `Vector3::lerp`，对旋转执行带最短路径半球修正的 `Quaternion::slerp`）；
     - 调用 `applySkeletonPose(finalPose, boneGameObjects)` 统一写回 `Transform3D`；
     - 当 $\alpha \ge 1.0f$ 时，过渡完成：执行 `mPrimaryClip = mTargetClip; mTargetClip = ClipState{};`，无缝切入目标片段独立播放。
   - **`playClips` 多片段模式**：
     - 对 `mLayeredClips` 中各片段采样得到 `SkeletonPose`，根据 `BonePose::validMask` 判断：若某骨骼仅在单个片段中存在有效轨道，则直接采用该片段姿势（天然支持上下半身分离动画）；若多个片段包含同一骨骼轨道，则按归一化权重调用 `blendPosesWeighted` 或 `blendPosesLayered` 汇总后写回。

### 2.4 未来向完整「动画混合树（Blend Tree）」演进的路径

基于上述结构，未来引入混合树时无需修改采样、混合与蒙皮底层代码，仅需：
1. 抽象基类 `AnimNode { virtual void evaluate(Real dt, const BoneGameObjects &bones, SkeletonPose &outPose) = 0; }`；
2. 将 `ClipState + sampleClipPose` 包装为叶子节点 `AnimClipNode`；
3. 将 `blendPoses` / `blendPosesWeighted` / `blendPosesLayered` 包装为容器节点 `AnimBlend1DNode`、`AnimBlend2DNode`、`AnimLayerNode`；
4. `AnimationPlayer` 只需持有一棵根 `AnimNodePtr mRootNode`，每帧调用 `mRootNode->evaluate(dt, bones, finalPose)` 后执行 `applySkeletonPose(finalPose, bones)` 即可。

---

## 3. 验收标准

1. 在 `SkeletalAnimationApp` 中构造两个关键帧姿态不同的 `AnimationClip`（或同一 Clip 的不同速率/偏移副本）；
2. 调用 `playClip("ClipA", "ClipB", ...)` 或在播放中调用 `playClip("ClipB", /*isBlending=*/true)`，观察骨骼在过渡期内平滑插值，无瞬间跳变；
3. 过渡结束后自动稳定在 `ClipB` 循环播放。
