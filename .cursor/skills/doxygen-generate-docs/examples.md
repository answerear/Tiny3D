# doxygen-generate-docs 示例

## 全量生成（HTML + CHM）

```powershell
cd D:\private\Tiny3D
powershell -NoProfile -ExecutionPolicy Bypass -File .cursor/skills/doxygen-generate-docs/scripts/generate-docs.ps1
```

期望：

- `doc/api/html/index.html`
- `doc/api/Tiny3D-API.chm`

## 仅 HTML

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .cursor/skills/doxygen-generate-docs/scripts/generate-docs.ps1 -SkipChm -OpenHtml
```

## 只生成 Core

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .cursor/skills/doxygen-generate-docs/scripts/generate-docs.ps1 `
  -InputDir source/Core `
  -OutputDir doc/api
```

## 单独确保 hhc

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .cursor/skills/doxygen-generate-docs/scripts/ensure-hhc.ps1
# stdout 最后一行应为 hhc.exe 全路径
```
