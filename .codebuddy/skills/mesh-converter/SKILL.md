---
name: mesh-converter
description: This skill runs the Tiny3D MeshConverter tool (mconv.exe) to convert FBX files into Tiny3D engine format resources (.tmesh, .tskel, .tani). It supports specifying material templates, output formats, bounding volume types, and batch conversion of multiple FBX files. This skill should be used when the user wants to convert FBX to engine format, import 3D models, generate mesh/skeleton/animation assets, or run mconv.exe. Trigger phrases include "convert FBX", "convert mesh", "run mconv", "import FBX", "generate tmesh", "FBX to Tiny3D", "mesh converter", "convert model", "generate engine resources from FBX", or any reference to mconv.exe or MeshConverter.
---

# Mesh Converter Skill

## Purpose

Run the Tiny3D `mconv.exe` tool to convert FBX files into Tiny3D engine format resources (`.tmesh`, `.tskel`, `.tani`), with configurable material templates, output formats, and bounding volume types.

## Key Paths

| Resource | Path |
|---|---|
| mconv executable | `source/bin/Windows/Debug/mconv.exe` |
| Default resource root | `assets/editor/builtin` |
| Default material template | `assets/editor/builtin/materials/Tiny3DStandard.tmat` |
| Sample FBX input | `assets/samples/raw/` |
| Default output directory | `assets/samples/meshes/` |
| MeshConverter source code | `source/Tools/MeshConverter/` |

## Usage

### Automatic Conversion (Recommended)

Run the conversion script to convert FBX files with sensible defaults:

```bash
python <skill_dir>/scripts/convert_mesh.py --workspace <workspace_root> --input <fbx_file>
```

### Parameters

| Parameter | Required | Default | Description |
|---|---|---|---|
| `--workspace` | Yes | — | Workspace root path (e.g., `D:\private\Tiny3D`) |
| `--input` | Yes | — | Path to a single `.fbx` file, or a directory when used with `--batch` |
| `--output-dir` | No | Same as input file directory | Output directory for converted files |
| `--output-type` | No | `Tiny3D` | Output format: `Tiny3D`, `tmesh`, `tskin`, `tskel`, `tani` |
| `--bound` | No | `sphere` | Bounding volume type: `sphere` or `AABB` |
| `--material` | No | `materials/Tiny3DStandard.tmat` | Default material file path (relative to resource root) |
| `--resource-root` | No | `assets/editor/builtin` | Resource root directory (relative to workspace) |
| `--mconv` | No | `source/bin/Windows/Debug/mconv.exe` | Path to mconv.exe (relative to workspace) |
| `--text` | No | Enabled | Output as text format; use `--no-text` for binary |
| `--meta` | No | Enabled | Generate .meta files; use `--no-meta` to skip |
| `--batch` | No | Disabled | Scan `--input` directory for all `.fbx` files and convert each |
| `--dry-run` | No | Disabled | Print commands without executing them |
| `--verbose` | No | Disabled | Enable verbose output from mconv.exe |

### Examples

Convert a single FBX file with all defaults:
```powershell
python <skill_dir>/scripts/convert_mesh.py --workspace D:\private\Tiny3D --input D:\private\Tiny3D\assets\samples\raw\tortoise.fbx --output-dir D:\private\Tiny3D\assets\samples\meshes
```

Convert with a custom material template:
```powershell
python <skill_dir>/scripts/convert_mesh.py --workspace D:\private\Tiny3D --input D:\private\Tiny3D\assets\samples\raw\tortoise.fbx --output-dir D:\private\Tiny3D\assets\samples\meshes --material materials/MyCustom.tmat
```

Batch convert all FBX files in a directory:
```powershell
python <skill_dir>/scripts/convert_mesh.py --workspace D:\private\Tiny3D --input D:\private\Tiny3D\assets\samples\raw --output-dir D:\private\Tiny3D\assets\samples\meshes --batch
```

Output only static mesh (no skeleton/animation):
```powershell
python <skill_dir>/scripts/convert_mesh.py --workspace D:\private\Tiny3D --input D:\private\Tiny3D\assets\samples\raw\tortoise.fbx --output-dir D:\private\Tiny3D\assets\samples\meshes --output-type tmesh
```

Preview commands without running (dry-run):
```powershell
python <skill_dir>/scripts/convert_mesh.py --workspace D:\private\Tiny3D --input D:\private\Tiny3D\assets\samples\raw\tortoise.fbx --output-dir D:\private\Tiny3D\assets\samples\meshes --dry-run
```

## Workflow

1. **Pre-check** — Verify that `mconv.exe` exists and is accessible
2. **Validate inputs** — Check that input FBX file(s) and resource root directory exist
3. **Assemble command** — Build the `mconv.exe` command line from parameters with defaults
4. **Execute conversion** — Run `mconv.exe` via subprocess (or print command in dry-run mode)
5. **Report results** — Display success/failure summary with per-file status

### Underlying mconv.exe Command

The script assembles and runs:
```
mconv.exe -o <type> -b <bound> [-t] [-a] [-v] -r <resource_root> -d <material> <input.fbx> [<output_dir>]
```

## When to Run

- When importing new FBX models into the Tiny3D engine
- When re-converting FBX files after modifying the source model
- When changing the material template and needing to regenerate engine resources
- After rebuilding the MeshConverter tool to test with updated conversion logic
- When setting up sample assets for a new scene or demo

## Notes

- The `mconv.exe` must be built first via CMake/VS2019 before this skill can run. Build path: `source/bin/Windows/Debug/mconv.exe`.
- Default parameters (`-o Tiny3D -b sphere -t -a`) match the debug configuration in `main.cpp` and produce all resource types (mesh + skeleton + animation) in text format with meta files.
- The material template path (`-d`) is relative to the resource root (`-r`). For example, `-r assets/editor/builtin -d materials/Tiny3DStandard.tmat` resolves to `assets/editor/builtin/materials/Tiny3DStandard.tmat`.
- Batch mode scans for `*.fbx` files (case-insensitive) in the specified input directory (non-recursive).
- The script exits with code 0 if all conversions succeed, non-zero if any fail.
