#!/usr/bin/env python3
"""
Tiny3D Build Pipeline - Orchestrates the full resource generation workflow.

Steps:
  1. builtin-generator       - Run BuiltinGenerator.exe to generate builtin assets
  2. compile-builtin-shaders - Compile .shader -> .tshader via scc.exe
  3. sync-builtin-to-samples - Sync builtin assets to samples/meshes
  4. mesh-converter          - Convert FBX files to engine format (.tmesh/.tskel/.tani)
  5. shader-compile-embed    - Compile .vshader/.pshader and embed into SampleShaders.cpp
  6. deploy-assets-to-debug  - Copy assets to Debug build directory (optional)

Usage:
  python build_pipeline.py --workspace <path> [--deploy] [--skip step1,step2]
"""

import argparse
import os
import subprocess
import sys
import time


STEP_NAMES = [
    "builtin-generator",
    "compile-builtin-shaders",
    "sync-builtin-to-samples",
    "mesh-converter",
    "shader-compile-embed",
    "deploy-assets-to-debug",
]

# Timeout in seconds per step
TIMEOUTS = {
    "builtin-generator": 600,
    "compile-builtin-shaders": 300,
    "sync-builtin-to-samples": 120,
    "mesh-converter": 600,
    "shader-compile-embed": 300,
    "deploy-assets-to-debug": 600,  # includes BundleBuilder for samples bundle
}


def print_header(step_name, index, total):
    print(f"\n{'='*60}")
    print(f"  Step {index}/{total}: {step_name}")
    print(f"{'='*60}")


def print_summary(results):
    print(f"\n{'='*60}")
    print("  Pipeline Summary")
    print(f"{'='*60}")
    for name, status, elapsed in results:
        icon = "[OK]" if status == "success" else ("[SKIP]" if status == "skipped" else "[FAIL]")
        time_str = f" ({elapsed:.1f}s)" if elapsed > 0 else ""
        print(f"  {icon} {name}: {status}{time_str}")
    print(f"{'='*60}")


def run_step_builtin_generator(workspace):
    """Step 1: Run BuiltinGenerator.exe."""
    exe = os.path.join(workspace, "source", "bin", "Windows", "Debug", "BuiltinGenerator.exe")
    output_dir = os.path.join(workspace, "assets", "editor", "builtin")

    if not os.path.isfile(exe):
        print(f"  ERROR: BuiltinGenerator.exe not found at: {exe}")
        print(f"  Please build the project first.")
        return 1

    cmd = [exe, "-t", output_dir]
    print(f"  Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=workspace, timeout=TIMEOUTS["builtin-generator"])
    return result.returncode


def run_step_compile_builtin_shaders(workspace):
    """Step 2: Compile .shader to .tshader."""
    script = os.path.join(workspace, ".codebuddy", "skills", "compile-builtin-shaders",
                          "scripts", "compile_builtin_shaders.py")

    if not os.path.isfile(script):
        print(f"  ERROR: Script not found: {script}")
        return 1

    cmd = [sys.executable, script]
    print(f"  Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=workspace, timeout=TIMEOUTS["compile-builtin-shaders"])
    return result.returncode


def run_step_sync_builtin_to_samples(workspace):
    """Step 3: Sync builtin assets to samples/meshes."""
    script = os.path.join(workspace, ".codebuddy", "skills", "sync-builtin-to-samples",
                          "scripts", "sync_builtin.py")

    if not os.path.isfile(script):
        print(f"  ERROR: Script not found: {script}")
        return 1

    cmd = [sys.executable, script, "--workspace", workspace]
    print(f"  Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=workspace, timeout=TIMEOUTS["sync-builtin-to-samples"])
    return result.returncode


def run_step_mesh_converter(workspace):
    """Step 4: Convert FBX files to engine format (.tmesh/.tskel/.tani)."""
    script = os.path.join(workspace, ".codebuddy", "skills", "mesh-converter",
                          "scripts", "convert_mesh.py")

    if not os.path.isfile(script):
        print(f"  ERROR: Script not found: {script}")
        return 1

    input_dir = os.path.join(workspace, "assets", "samples", "raw")
    output_dir = os.path.join(workspace, "assets", "samples", "meshes")

    if not os.path.isdir(input_dir):
        print(f"  WARNING: FBX input directory not found: {input_dir}")
        print(f"  Skipping mesh conversion (no raw FBX files).")
        return 0

    # Check if there are any FBX files to convert
    fbx_files = [f for f in os.listdir(input_dir)
                 if f.lower().endswith(".fbx")]
    if not fbx_files:
        print(f"  No FBX files found in: {input_dir}")
        print(f"  Skipping mesh conversion.")
        return 0

    cmd = [
        sys.executable, script,
        "--workspace", workspace,
        "--input", input_dir,
        "--output-dir", output_dir,
        "--batch",
    ]
    print(f"  Running: {' '.join(cmd)}")
    print(f"  Converting {len(fbx_files)} FBX file(s)...")
    result = subprocess.run(cmd, cwd=workspace, timeout=TIMEOUTS["mesh-converter"])
    return result.returncode


def run_step_shader_compile_embed(workspace):
    """Step 4: Compile shaders and embed into C++."""
    skill_dir = os.path.join(workspace, ".codebuddy", "skills", "shader-compile-embed", "scripts")
    compile_script = os.path.join(skill_dir, "compile_shaders.py")
    embed_script = os.path.join(skill_dir, "embed_shaders.py")

    if not os.path.isfile(compile_script):
        print(f"  ERROR: Script not found: {compile_script}")
        return 1
    if not os.path.isfile(embed_script):
        print(f"  ERROR: Script not found: {embed_script}")
        return 1

    scc = os.path.join(workspace, "source", "bin", "Windows", "Debug", "scc.exe")
    src_dir = os.path.join(workspace, "assets", "samples", "shaders")
    hlsl_out = os.path.join(workspace, "assets", "samples", "shaders", "output", "HLSL")
    glsl_out = os.path.join(workspace, "assets", "samples", "shaders", "output", "OpenGL4")
    cpp_file = os.path.join(workspace, "source", "Samples", "Common", "SampleShaders.cpp")

    # Step 4.1: Compile shaders
    cmd_compile = [
        sys.executable, compile_script,
        "--scc", scc,
        "--src", src_dir,
        "--hlsl-out", hlsl_out,
        "--glsl-out", glsl_out,
        "--extra-args", "-N -O0",
    ]
    print(f"  Running (compile): {' '.join(cmd_compile)}")
    result = subprocess.run(cmd_compile, cwd=workspace, timeout=TIMEOUTS["shader-compile-embed"])
    if result.returncode != 0:
        print(f"  ERROR: Shader compilation failed (exit code {result.returncode})")
        return result.returncode

    # Step 4.2: Embed into C++
    shader_maps = [
        "GeometryApp_vertex.hlsl:SAMPLE_VERTEX_SHADER",
        "GeometryApp_fragment.hlsl:SAMPLE_PIXEL_SHADER",
        "LitGeometryApp_vertex.hlsl:SAMPLE_LIT_VERTEX_SHADER",
        "LitGeometryApp_fragment.hlsl:SAMPLE_LIT_PIXEL_SHADER",
        "ShadowPass_vertex.hlsl:SHADOW_VERTEX_SHADER",
        "ForwardPass_vertex.hlsl:FORWARD_VERTEX_SHADER",
        "ForwardPass_fragment.hlsl:FORWARD_PIXEL_SHADER",
        "SkinShadowPass_vertex.hlsl:SKIN_SHADOW_VERTEX_SHADER",
        "SkinForwardPass_vertex.hlsl:SKIN_FORWARD_VERTEX_SHADER",
        "GPUSkinShadowPass_vertex.hlsl:GPU_SKIN_SHADOW_VERTEX_SHADER",
        "GPUSkinForwardPass_vertex.hlsl:GPU_SKIN_FORWARD_VERTEX_SHADER",
    ]

    cmd_embed = [
        sys.executable, embed_script,
        "--cpp", cpp_file,
        "--hlsl-dir", hlsl_out,
        "--glsl-dir", glsl_out,
    ]
    for m in shader_maps:
        cmd_embed.extend(["--map", m])

    print(f"  Running (embed): {' '.join(cmd_embed[:6])} ... ({len(shader_maps)} mappings)")
    result = subprocess.run(cmd_embed, cwd=workspace, timeout=TIMEOUTS["shader-compile-embed"])
    return result.returncode


def run_step_deploy_assets(workspace):
    """Step 5: Deploy assets to Debug build directory."""
    script = os.path.join(workspace, ".codebuddy", "skills", "deploy-assets-to-debug",
                          "scripts", "deploy_assets.py")

    if not os.path.isfile(script):
        print(f"  ERROR: Script not found: {script}")
        return 1

    cmd = [sys.executable, script, "--workspace", workspace]
    print(f"  Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=workspace, timeout=TIMEOUTS["deploy-assets-to-debug"])
    return result.returncode


STEP_FUNCTIONS = {
    "builtin-generator": run_step_builtin_generator,
    "compile-builtin-shaders": run_step_compile_builtin_shaders,
    "sync-builtin-to-samples": run_step_sync_builtin_to_samples,
    "mesh-converter": run_step_mesh_converter,
    "shader-compile-embed": run_step_shader_compile_embed,
    "deploy-assets-to-debug": run_step_deploy_assets,
}


def run_pipeline(workspace, skip_steps, deploy):
    """Execute the pipeline steps in order."""
    steps_to_run = STEP_NAMES[:5] if not deploy else STEP_NAMES
    total = len(steps_to_run)
    results = []
    failed = False

    print(f"\nTiny3D Build Pipeline")
    print(f"  Workspace: {workspace}")
    print(f"  Deploy:    {'Yes' if deploy else 'No'}")
    if skip_steps:
        print(f"  Skipping:  {', '.join(skip_steps)}")
    print(f"  Steps:     {total}")

    for i, step_name in enumerate(steps_to_run, 1):
        if step_name in skip_steps:
            print_header(step_name, i, total)
            print("  SKIPPED (user request)")
            results.append((step_name, "skipped", 0))
            continue

        print_header(step_name, i, total)
        start = time.time()

        try:
            ret = STEP_FUNCTIONS[step_name](workspace)
        except subprocess.TimeoutExpired:
            elapsed = time.time() - start
            print(f"  ERROR: Step timed out after {elapsed:.1f}s")
            results.append((step_name, "TIMEOUT", elapsed))
            failed = True
            break
        except Exception as e:
            elapsed = time.time() - start
            print(f"  ERROR: {e}")
            results.append((step_name, "FAILED", elapsed))
            failed = True
            break

        elapsed = time.time() - start

        if ret != 0:
            print(f"  FAILED (exit code {ret}) in {elapsed:.1f}s")
            results.append((step_name, "FAILED", elapsed))
            failed = True
            break
        else:
            print(f"  SUCCESS in {elapsed:.1f}s")
            results.append((step_name, "success", elapsed))

    # Mark remaining steps as not-run if we failed early
    if failed:
        executed_names = {r[0] for r in results}
        for step_name in steps_to_run:
            if step_name not in executed_names:
                results.append((step_name, "not-run", 0))

    print_summary(results)
    return 0 if not failed else 1


def main():
    parser = argparse.ArgumentParser(description="Tiny3D Build Pipeline")
    parser.add_argument("--workspace", required=True, help="Workspace root directory")
    parser.add_argument("--deploy", action="store_true",
                        help="Include deploy-assets-to-debug step (Step 5)")
    parser.add_argument("--no-deploy", action="store_true",
                        help="Exclude deploy-assets-to-debug step (default)")
    parser.add_argument("--skip", type=str, default="",
                        help="Comma-separated step names to skip")

    args = parser.parse_args()

    workspace = os.path.abspath(args.workspace)
    if not os.path.isdir(workspace):
        print(f"ERROR: Workspace directory not found: {workspace}")
        sys.exit(1)

    deploy = args.deploy and not args.no_deploy
    skip_steps = [s.strip() for s in args.skip.split(",") if s.strip()]

    # Validate skip step names
    for s in skip_steps:
        if s not in STEP_NAMES:
            print(f"WARNING: Unknown step '{s}' in --skip. Valid steps: {', '.join(STEP_NAMES)}")

    sys.exit(run_pipeline(workspace, skip_steps, deploy))


if __name__ == "__main__":
    main()
