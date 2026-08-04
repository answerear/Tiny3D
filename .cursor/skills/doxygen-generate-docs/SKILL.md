---
name: doxygen-generate-docs
description: >-
  用 Doxygen 从头文件注释生成 Tiny3D API 的 HTML 与 CHM。扫描 source/
  下自研模块头文件，排除 External、dependencies 及构建产物，输出到
  doc/api。缺失 hhc.exe 时自动安装 HTML Help Workshop。在用户要求生成
  API 文档、Doxygen HTML/CHM、doc/api，或点名本 skill 时使用。
disable-model-invocation: true
---

# Tiny3D Doxygen → HTML / CHM

## 目标

根据头文件中的 Doxygen 注释，生成：

| 产物 | 路径 |
|------|------|
| HTML | `doc/api/html/index.html` |
| CHM  | `doc/api/Tiny3D-API.chm`（同时保留 `doc/api/html/Tiny3D-API.chm`） |

与 [doxygen-core-api](../doxygen-core-api/SKILL.md) 分工：后者写注释，本 skill **只负责生成文档**。

## 硬性约定

1. **输入**：自动收集 `source/` 下各模块的 `Include/`（`*.h` / `*.hpp` / `*.hxx` / `*.hh`）
2. **排除**：路径中含 `External`、`dependencies`、以及构建/IDE 树（`vs2019-x64`、`nmake`、`bin`、`lib`、`.cxx`、`Android`、`Projects`、`CMakeFiles`）。**不要**用 Windows 下大小写不敏感的 `*/Source/*` 排除规则（会误伤整个 `source/`）
3. **输出**：默认 `doc/api/`
4. **CHM**：有 `hhc.exe` 直接编；没有则运行 `ensure-hhc.ps1` 安装后再编
5. **不改头文件**：本 skill 不增删改 Doxygen 注释

## 前置依赖

| 工具 | 用途 | 缺失时 |
|------|------|--------|
| `doxygen` | 生成 HTML / `.hhp` | 提示安装（choco/scoop/官网），**不要静默装系统级 doxygen** 除非用户同意 |
| `hhc.exe` | 编译 CHM | **自动安装** HTML Help Workshop（见脚本） |

## Agent 工作流

```
Generate Docs Progress:
- [ ] 1. 确认范围（默认全量 source/ → doc/api）
- [ ] 2. 执行 scripts/generate-docs.ps1
- [ ] 3. 核对 HTML / CHM 是否存在
- [ ] 4. 若失败：读警告日志，按「故障排查」处理
```

### 1. 默认一键生成（推荐）

在仓库根目录执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .cursor/skills/doxygen-generate-docs/scripts/generate-docs.ps1
```

常用开关：

```powershell
# 只要 HTML（跳过 CHM / 不装 hhc）
...\generate-docs.ps1 -SkipChm

# 生成后打开 HTML
...\generate-docs.ps1 -OpenHtml

# 自定义输出目录
...\generate-docs.ps1 -OutputDir doc/api-core
```

### 2. 仅安装 / 定位 hhc

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .cursor/skills/doxygen-generate-docs/scripts/ensure-hhc.ps1
```

安装顺序：`PATH` / 常见目录 → Chocolatey `html-help-workshop` → Wayback 下载官方 `htmlhelp.exe`。  
若无管理员权限导致安装失败：提示用户用**管理员 PowerShell**重跑，或改用 `-SkipChm`。

### 3. 成功标准

- 存在 `doc/api/html/index.html`
- 未使用 `-SkipChm` 时存在 `doc/api/Tiny3D-API.chm`
- 向用户回报上述绝对路径；可选打开 HTML

### 4. 用户缩窄范围时

若用户只要某模块（如 Core）：

```powershell
...\generate-docs.ps1 -InputDir source/Core -OutputDir doc/api
```

仍须排除第三方：不要把 `source/External` 或仓库根 `dependencies` 设为 Input。

## 脚本职责

| 脚本 | 作用 | Agent 用法 |
|------|------|------------|
| `scripts/generate-docs.ps1` | 填 Doxyfile → 调 doxygen →（可选）hhc → 复制 CHM | **执行** |
| `scripts/ensure-hhc.ps1` | 查找/安装 `hhc.exe`，stdout 打印路径 | 由 generate 调用；也可单独执行 |
| `templates/Doxyfile.in` | 配置模板（占位符由脚本替换） | 勿手改生成结果；改模板 |

生成时会写入 `doc/api/Doxyfile` 与 `doc/api/doxygen-warnings.log`（可本地忽略，不必提交）。

## Doxygen 要点（已写入模板）

- `EXTRACT_PRIVATE` / `EXTRACT_STATIC` = YES（与注释 skill 覆盖面一致；protected 默认收录）
- `EXTRACT_ALL` = NO（无注释符号不硬抽，减少噪声）
- 输入为各模块 `Include/`（由脚本枚举，避开实现目录与第三方）
- CHM 兼容：`GENERATE_TREEVIEW` / `SEARCHENGINE` = NO，`HTML_COLORSTYLE` = LIGHT
- **`CHM_INDEX_ENCODING = GBK`**（必填）：hhc 不支持 UTF-8 索引，否则目录中文乱码
- `OUTPUT_LANGUAGE` = Chinese
- `GENERATE_HTML` = YES，`GENERATE_HTMLHELP` = YES（非 SkipChm 时）
- `GENERATE_LATEX` = NO
- 预定义清空常见 `T3D_*_API` 导出宏，避免解析干扰

## 故障排查

| 现象 | 处理 |
|------|------|
| `doxygen.exe not found` | 请用户安装 doxygen 后重试；征得同意后再用包管理器安装 |
| `hhc` 安装失败 / 非管理员 | 提示提权重跑 `ensure-hhc.ps1`，或 `-SkipChm` 先出 HTML |
| 有 HTML 无 CHM | 检查 `doc/api/html/index.hhp`；手动：`& "C:\Program Files (x86)\HTML Help Workshop\hhc.exe" doc\api\html\index.hhp`（hhc 常返回 1 仍可能已生成 chm，以文件是否存在为准） |
| **CHM 目录/索引中文乱码** | 确认 `CHM_INDEX_ENCODING = GBK` 后重新生成；勿把索引改回 UTF-8 |
| 警告过多 | 查看 `doc/api/doxygen-warnings.log`；注释质量交给 `doxygen-core-api` |

## 附加资源

- 命令与验收示例：[examples.md](examples.md)
