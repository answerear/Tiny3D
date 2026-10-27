# T2-3 实现计划：Material / Pass 层接入 Compute Shader Stage

> **所属批次**：T2（RHI 多后端存量 Stub 填平与材质 Compute 接入）  
> **预估工作量**：2.0 人日  
> **涉及文件**：
> - `source/Core/Include/Material/T3DPass.h`
> - `source/Core/Source/Material/T3DPass.cpp`
> - `source/Core/Include/Material/T3DPassInstance.h`
> - `source/Core/Source/Material/T3DPassInstance.cpp`
> - `source/Tools/ShaderCrossCompiler/Source/T3DShaderCross.cpp`

---

## 1. 现状分析（代码依据）

目前 Tiny3D 的离线编译链（`scc` + `HLSLCross`）和 RHI 层（`RHIContext::createComputeShader` / `setComputeShader` / `dispatch`）均已完整支持 `SHADER_STAGE::kCompute`，唯独中间的 **Material / Pass 层将 Compute 挡在了门外**：

1. **`Pass::addShaderVariant` 直接拒绝 `kCompute`**：
   ```cpp
   // source/Core/Source/Material/T3DPass.cpp:359-362
   case SHADER_STAGE::kCompute:
       {
           return T3D_ERR_NOT_IMPLEMENT;
       }
       break;
   ```
2. **`Pass` 缺少 `mComputeShaders` 成员与 RTTR 属性**：
   - `source/Core/Include/Material/T3DPass.h:318-328` 只有 `mVertexShaders`、`mPixelShaders`、`mGeometryShaders`、`mHullShaders`、`mDomainShaders` 五个集合，没有 `mComputeShaders`；
   - `Pass::compile()`、`Pass::reflect()`、`Pass::removeShaderVariant()`、`Pass::onPostLoad()` 均只遍历上述 5 个图形阶段。
3. **`PassInstance` 缺少 `mCurrentCS` 与绑定/参数转发**：
   - `source/Core/Include/Material/T3DPassInstance.h:287-297` 只有 `mCurrentVS/PS/GS/HS/DS`，没有 `mCurrentCS`；
   - `PassInstance::setFloat` / `setVector` / `setMatrix` / `setTexture` 等模板转发以及 `PassInstance::bind(RHIContext *ctx, Material *material)` 均未处理 Compute 阶段。

这导致在 `ComputeApp` 中无法通过 `.tshader` / `Material` 管理 Compute Shader，被迫在业务层手造 dummy `Shader` / `Technique` / `Pass` 绕过 `Pass::addShaderVariant`（见 `ComputeApp-Sample-Design-todo.md §9.1 P2`）。

---

## 2. 实现方案（建议实现）

### 2.1 扩展 `Pass`（`T3DPass.h` / `T3DPass.cpp`）

1. **新增成员与 RTTR 属性**：
   - 在 `T3DPass.h` 增加 `ShaderVariantSets mComputeShaders {};`；
   - 增加 `TPROPERTY(RTTRFuncName="ComputeShaders", RTTRFuncType="getter")` `getComputeShaders()` 与对应的 private setter `setComputeShaders(const ShaderVariantSets &variants)`。
2. **打通 `addShaderVariant`**：
   - 将 `T3DPass.cpp:359` 的 `case SHADER_STAGE::kCompute:` 改为 `vars = &mComputeShaders; break;`。
3. **打通 `compile` / `reflect` / `removeShaderVariant` / `onPostLoad`**：
   - 在这四个函数中与 `mVertexShaders` 等并列追加对 `mComputeShaders` 的处理：
     - `compile()`：调用 `compileShader(mComputeShaders)`；
     - `reflect()`：调用 `reflectShader(mComputeShaders)`；
     - `removeShaderVariant()`：调用 `eraseKeyword(mComputeShaders)`；
     - `onPostLoad()`：调用 `setPass(mComputeShaders)`。

### 2.2 扩展 `PassInstance`（`T3DPassInstance.h` / `T3DPassInstance.cpp`）

1. **新增成员与 Getter**：
   - 新增 `ShaderVariantInstancePtr mCurrentCS {nullptr};`；
   - 新增 `ShaderVariantInstance *getCurrentComputeShader() const { return mCurrentCS; }`。
2. **打通关键字切换与克隆**：
   - `switchKeyword(const ShaderKeyword &keyword)`：增加 `switchShaderVariants(keyword, mPass->getComputeShaders(), cs)`，成功后 `mCurrentCS = cs`；
   - `cloneProperties`：若 `src->mCurrentCS != nullptr`，克隆 `mCurrentCS`。
3. **打通 `setBool` / `setFloat` / `setVector` / `setMatrix` / `setData` / `setTexture` 内联转发**：
   - 在 `T3DPassInstance.h:301-452` 的所有 `set*` 函数末尾追加一行：
     ```cpp
     setValue(mCurrentCS, name, value, &ShaderVariantInstance::setXxx);
     ```
4. **打通 `PassInstance::bind` 与新增 `PassInstance::dispatch`**：
   - 在 `PassInstance::bind(RHIContext *ctx, Material *material)` 中：
     - 若 `mCurrentCS != nullptr`，调用 `ctx->setComputeShader(mCurrentCS->getShaderVariant())`，并更新绑定 CS 的 ConstantBuffers、Samplers 与 PixelBuffers；
   - 新增便捷派发方法：
     ```cpp
     TResult dispatch(RHIContext *ctx, Material *material,
         uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1);
     ```

### 2.3 验证 `ShaderCrossCompiler (scc)` 的 `.tshader` 输出

- 检查 `source/Tools/ShaderCrossCompiler/Source/T3DShaderCross.cpp` 中 `#pragma compute` 编译产物写入 `Pass::addShaderVariant` 的路径：此前因 `addShaderVariant` 返回 `T3D_ERR_NOT_IMPLEMENT`，`.shader` 文件中写 `#pragma compute` 无法生成 `.tshader`。放开后，`scc` 可直接将含 `#pragma compute` 的 ShaderLab 文件序列化为标准 `.tshader` 资产。

---

## 3. 验收标准

1. 重新运行构建（触发 `T3DCore` / `T3DCoreEditor` 的反射生成 `rpp`，确认 `Pass::ComputeShaders` 属性注册进 RTTR）；
2. 用 `scc` 编译一个含 `#pragma compute CSMain` 的 `.shader` 文件生成 `.tshader` 成功，反序列化后 `pass->getComputeShaders()` 非空；
3. 通过 `PassInstance::bind` + `ctx->dispatch` 正常派发计算着色器并产出正确结果。
