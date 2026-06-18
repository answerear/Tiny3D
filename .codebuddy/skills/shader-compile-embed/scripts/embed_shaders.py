"""
Embed compiled HLSL/GLSL/ESSL shader files into C++ string constants in separate .h files.

Usage:
    python embed_shaders.py --hlsl-cpp <hlsl_h_path> --glsl-cpp <glsl_h_path> --essl-cpp <essl_h_path> \
        --hlsl-dir <hlsl_dir> --glsl-dir <glsl_dir> --essl-dir <essl_dir> \
        --map "GeometryApp_vertex.hlsl:SAMPLE_VERTEX_SHADER" \
        --map "GeometryApp_fragment.hlsl:SAMPLE_PIXEL_SHADER" \
        ...

Legacy single-file mode (deprecated, kept for backward compatibility):
    python embed_shaders.py --cpp <cpp_path> --hlsl-dir <hlsl_dir> --glsl-dir <glsl_dir> --essl-dir <essl_dir> \
        --map "GeometryApp_vertex.hlsl:SAMPLE_VERTEX_SHADER" ...

The mapping format is: <shader_filename>:<cpp_variable_name>
For HLSL files, the variable name is used as-is (written to --hlsl-cpp).
For GLSL files, the corresponding .glsl file is looked up and the variable name has _GL appended (written to --glsl-cpp).
For ESSL files, the corresponding .essl file is looked up and the variable name has _GLES appended (written to --essl-cpp).
"""

import argparse
import os
import re
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
    """Replace a C++ string constant with new shader content.

    Handles both multi-line format:
        const char *VAR =
            "line1\\n"
            "line2\\n";

    And single-line format (with optional extra spaces):
        const char *VAR = "";
        const char *VAR          = "";
    """
    shader_path = os.path.join(shader_dir, shader_file)
    if not os.path.isfile(shader_path):
        print(f'  WARNING: Shader file {shader_path} not found, skipping {var_name}')
        return content, False

    shader_content = read_shader(shader_path)
    cpp_str = to_cpp_string(shader_content)

    # Try multi-line format first: "const char *VAR =\n"
    start_marker = 'const char *' + var_name + ' =\n'
    start_idx = content.find(start_marker)

    if start_idx != -1:
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

    # Try single-line format: "const char *VAR<spaces>= <value>;"
    pattern = re.compile(
        r'(const\s+char\s+\*' + re.escape(var_name) + r'\s*=\s*)([^;]*;)[ \t]*\n'
    )
    match = pattern.search(content)
    if match:
        prefix = match.group(1).rstrip()
        new_content = prefix + ' =\n' + cpp_str + ';\n'
        content = content[:match.start()] + new_content + content[match.end():]
        print(f'  Replaced {var_name}')
        return content, True

    print(f'  WARNING: Cannot find variable {var_name}, skipping')
    return content, False


def main():
    parser = argparse.ArgumentParser(description='Embed shader files into C++ string constants')
    parser.add_argument('--cpp', default='', help='(Deprecated) Path to a single C++ source file for all backends')
    parser.add_argument('--hlsl-cpp', default='', help='Path to the HLSL C++ header file (SampleShaders_hlsl.h)')
    parser.add_argument('--glsl-cpp', default='', help='Path to the GL4 C++ header file (SampleShaders_gl4.h)')
    parser.add_argument('--essl-cpp', default='', help='Path to the GLES3 C++ header file (SampleShaders_gles3.h)')
    parser.add_argument('--hlsl-dir', required=True, help='Directory containing .hlsl files')
    parser.add_argument('--glsl-dir', required=True, help='Directory containing .glsl files')
    parser.add_argument('--essl-dir', default='', help='Directory containing .essl files (optional, for GLES3)')
    parser.add_argument('--map', action='append', required=True,
                        help='Mapping in format "shader_base:CPP_VAR". '
                             'shader_base is the HLSL filename (e.g. GeometryApp_vertex.hlsl). '
                             'CPP_VAR is the HLSL variable name (GLSL = CPP_VAR + _GL, ESSL = CPP_VAR + _GLES).')
    args = parser.parse_args()

    # Determine target files (new split mode vs legacy single-file mode)
    if args.hlsl_cpp or args.glsl_cpp or args.essl_cpp:
        hlsl_target = args.hlsl_cpp
        glsl_target = args.glsl_cpp
        essl_target = args.essl_cpp
    elif args.cpp:
        hlsl_target = args.cpp
        glsl_target = args.cpp
        essl_target = args.cpp
    else:
        print('ERROR: Must provide either --hlsl-cpp/--glsl-cpp/--essl-cpp or --cpp')
        sys.exit(1)

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
        essl_file = hlsl_file.replace('.hlsl', '.essl')
        mappings.append((hlsl_file, glsl_file, essl_file, var_name, var_name + '_GL', var_name + '_GLES'))

    # Read target files
    file_contents = {}
    for path in set(filter(None, [hlsl_target, glsl_target, essl_target])):
        with open(path, 'r', encoding='utf-8') as f:
            file_contents[path] = f.read()

    success = 0
    fail = 0

    for hlsl_file, glsl_file, essl_file, var_hlsl, var_glsl, var_gles in mappings:
        # Replace HLSL
        if hlsl_target:
            content, ok = replace_var(file_contents[hlsl_target], var_hlsl, args.hlsl_dir, hlsl_file)
            file_contents[hlsl_target] = content
            if ok:
                success += 1
            else:
                fail += 1

        # Replace GLSL
        if glsl_target:
            content, ok = replace_var(file_contents[glsl_target], var_glsl, args.glsl_dir, glsl_file)
            file_contents[glsl_target] = content
            if ok:
                success += 1
            else:
                fail += 1

        # Replace ESSL (GLES3) if essl-dir is provided
        if args.essl_dir and essl_target:
            content, ok = replace_var(file_contents[essl_target], var_gles, args.essl_dir, essl_file)
            file_contents[essl_target] = content
            if ok:
                success += 1
            else:
                fail += 1

    # Write back all modified files
    for path, content in file_contents.items():
        with open(path, 'w', encoding='utf-8') as f:
            f.write(content)

    total = len(mappings) * 2
    if args.essl_dir:
        total += len(mappings)
    print(f'\nDone: {success} replaced, {fail} failed out of {total} total.')
    if fail > 0:
        sys.exit(1)


if __name__ == '__main__':
    main()
