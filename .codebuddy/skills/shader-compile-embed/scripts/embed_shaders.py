"""
Embed compiled HLSL/GLSL shader files into C++ string constants in a .cpp file.

Usage:
    python embed_shaders.py --cpp <cpp_path> --hlsl-dir <hlsl_dir> --glsl-dir <glsl_dir> --mapping <mapping_file>

Or with inline mappings:
    python embed_shaders.py --cpp <cpp_path> --hlsl-dir <hlsl_dir> --glsl-dir <glsl_dir> \
        --map "GeometryApp_vertex.hlsl:SAMPLE_VERTEX_SHADER" \
        --map "GeometryApp_fragment.hlsl:SAMPLE_PIXEL_SHADER" \
        ...

The mapping format is: <shader_filename>:<cpp_variable_name>
For HLSL files, the variable name is used as-is.
For GLSL files, the corresponding .glsl file is looked up and the variable name has _GL appended.
"""

import argparse
import json
import os
import sys


def read_shader(path):
    """Read shader file content."""
    with open(path, 'r', encoding='utf-8') as f:
        return f.read()


def to_cpp_string(content):
    """Convert shader content to C++ string literal lines."""
    lines = content.split('\n')
    result = []
    for i, line in enumerate(lines):
        escaped = line.replace('\\', '\\\\').replace('"', '\\"')
        if i == len(lines) - 1 and escaped == '':
            continue
        result.append('    "' + escaped + '\\n"')
    return '\n'.join(result)


def replace_var(content, var_name, shader_dir, shader_file):
    """Replace a C++ string constant with new shader content."""
    shader_content = read_shader(os.path.join(shader_dir, shader_file))
    cpp_str = to_cpp_string(shader_content)

    start_marker = 'const char *' + var_name + ' =\n'
    start_idx = content.find(start_marker)
    if start_idx == -1:
        print(f'  WARNING: Cannot find variable {var_name}, skipping')
        return content, False

    body_start = start_idx + len(start_marker)

    pos = body_start
    end_idx = None
    while pos < len(content):
        nl = content.find('\n', pos)
        if nl == -1:
            nl = len(content)
        line = content[pos:nl].rstrip()

        if line.lstrip().startswith('"'):
            if line.rstrip().endswith(';'):
                end_idx = nl + 1
                break
            pos = nl + 1
            continue
        else:
            end_idx = pos
            break

    if end_idx is None:
        print(f'  WARNING: Cannot find end of {var_name}, skipping')
        return content, False

    new_body = cpp_str + ';\n'
    content = content[:body_start] + new_body + content[end_idx:]
    print(f'  Replaced {var_name}')
    return content, True


def main():
    parser = argparse.ArgumentParser(description='Embed shader files into C++ string constants')
    parser.add_argument('--cpp', required=True, help='Path to the C++ source file')
    parser.add_argument('--hlsl-dir', required=True, help='Directory containing .hlsl files')
    parser.add_argument('--glsl-dir', required=True, help='Directory containing .glsl files')
    parser.add_argument('--map', action='append', required=True,
                        help='Mapping in format "shader_base:CPP_VAR". '
                             'shader_base is the HLSL filename (e.g. GeometryApp_vertex.hlsl). '
                             'CPP_VAR is the HLSL variable name (GLSL variable = CPP_VAR + _GL).')
    args = parser.parse_args()

    # Parse mappings
    mappings = []
    for m in args.map:
        parts = m.split(':')
        if len(parts) != 2:
            print(f'Invalid mapping format: {m}')
            sys.exit(1)
        hlsl_file = parts[0].strip()
        var_name = parts[1].strip()
        glsl_file = hlsl_file.replace('.hlsl', '.glsl')
        mappings.append((hlsl_file, glsl_file, var_name, var_name + '_GL'))

    # Read C++ file
    with open(args.cpp, 'r', encoding='utf-8') as f:
        content = f.read()

    success = 0
    fail = 0

    for hlsl_file, glsl_file, var_hlsl, var_glsl in mappings:
        # Replace HLSL
        content, ok = replace_var(content, var_hlsl, args.hlsl_dir, hlsl_file)
        if ok:
            success += 1
        else:
            fail += 1

        # Replace GLSL
        content, ok = replace_var(content, var_glsl, args.glsl_dir, glsl_file)
        if ok:
            success += 1
        else:
            fail += 1

    # Write back
    with open(args.cpp, 'w', encoding='utf-8') as f:
        f.write(content)

    print(f'\nDone: {success} replaced, {fail} failed out of {len(mappings) * 2} total.')
    if fail > 0:
        sys.exit(1)


if __name__ == '__main__':
    main()
