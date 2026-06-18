"""
Embed compiled SPIR-V binary shader files into C++ byte array constants in a .h file.

Usage:
    python embed_spirv.py --cpp <h_path> --spirv-dir <spirv_dir> \
        --map "GeometryApp_vertex.spirv:SAMPLE_VERTEX_SHADER_VK" \
        --map "GeometryApp_fragment.spirv:SAMPLE_PIXEL_SHADER_VK" \
        ...

The target file is typically `SampleShaders_vk.h`.

The mapping format is: <spirv_filename>:<cpp_variable_name>
For each mapping, two C++ symbols are generated:
  - alignas(4) const unsigned char <VAR>[] = { ... };
  - const size_t <VAR>_SIZE = sizeof(<VAR>);
"""

import argparse
import os
import sys


def read_spirv(path):
    """Read SPIR-V binary file."""
    with open(path, 'rb') as f:
        return f.read()


def to_cpp_byte_array(data, indent='    '):
    """Convert binary data to C++ byte array initializer lines.

    Output format:
        0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00,
        ...
    """
    lines = []
    bytes_per_line = 16
    for i in range(0, len(data), bytes_per_line):
        chunk = data[i:i + bytes_per_line]
        hex_vals = ', '.join(f'0x{b:02x}' for b in chunk)
        lines.append(indent + hex_vals + ',')
    # Remove trailing comma from last line
    if lines:
        lines[-1] = lines[-1].rstrip(',')
    return '\n'.join(lines)


def generate_var_block(var_name, data):
    """Generate the full C++ variable declaration block for one SPIR-V shader."""
    byte_array = to_cpp_byte_array(data)
    block = (
        f'extern alignas(4) const unsigned char {var_name}[] = {{\n'
        f'{byte_array}\n'
        f'}};\n'
        f'extern const size_t {var_name}_SIZE = sizeof({var_name});\n'
    )
    return block


def replace_var(content, var_name, spirv_dir, spirv_file):
    """Replace or insert a SPIR-V byte array variable in the C++ file."""
    spirv_path = os.path.join(spirv_dir, spirv_file)
    if not os.path.exists(spirv_path):
        print(f'  WARNING: SPIR-V file not found: {spirv_path}, skipping')
        return content, False

    data = read_spirv(spirv_path)
    new_block = generate_var_block(var_name, data)

    # Try to find existing variable and replace it (support both with and without extern)
    start_marker = f'extern alignas(4) const unsigned char {var_name}[]'
    start_idx = content.find(start_marker)
    if start_idx == -1:
        # Fallback: match without extern (old format)
        start_marker = f'alignas(4) const unsigned char {var_name}[]'
        start_idx = content.find(start_marker)

    if start_idx != -1:
        # Find the end of the SIZE declaration (support both with and without extern)
        size_marker = f'extern const size_t {var_name}_SIZE = sizeof({var_name});'
        size_idx = content.find(size_marker, start_idx)
        if size_idx == -1:
            # Fallback: match without extern (old format)
            size_marker = f'const size_t {var_name}_SIZE = sizeof({var_name});'
            size_idx = content.find(size_marker, start_idx)
        if size_idx != -1:
            end_idx = size_idx + len(size_marker)
            # Include trailing newline
            if end_idx < len(content) and content[end_idx] == '\n':
                end_idx += 1
            content = content[:start_idx] + new_block + content[end_idx:]
            print(f'  Replaced {var_name} ({len(data)} bytes)')
            return content, True

    # Variable not found - append at the end of the file
    content = content.rstrip('\n') + '\n\n' + new_block
    print(f'  Inserted {var_name} ({len(data)} bytes) at end of file')
    return content, True


def main():
    parser = argparse.ArgumentParser(description='Embed SPIR-V binary files into C++ byte array constants')
    parser.add_argument('--cpp', required=True, help='Path to the C++ source file')
    parser.add_argument('--spirv-dir', required=True, help='Directory containing .spirv files')
    parser.add_argument('--map', action='append', required=True,
                        help='Mapping in format "spirv_file:CPP_VAR". '
                             'spirv_file is the SPIR-V filename (e.g. GeometryApp_vertex.spirv). '
                             'CPP_VAR is the variable name (e.g. SAMPLE_VERTEX_SHADER_VK).')
    args = parser.parse_args()

    # Parse mappings
    mappings = []
    for m in args.map:
        parts = m.split(':')
        if len(parts) != 2:
            print(f'Invalid mapping format: {m}')
            sys.exit(1)
        spirv_file = parts[0].strip()
        var_name = parts[1].strip()
        mappings.append((spirv_file, var_name))

    # Read C++ file
    with open(args.cpp, 'r', encoding='utf-8') as f:
        content = f.read()

    success = 0
    fail = 0

    for spirv_file, var_name in mappings:
        content, ok = replace_var(content, var_name, args.spirv_dir, spirv_file)
        if ok:
            success += 1
        else:
            fail += 1

    # Write back
    with open(args.cpp, 'w', encoding='utf-8') as f:
        f.write(content)

    print(f'\nDone: {success} replaced/inserted, {fail} failed out of {len(mappings)} total.')
    if fail > 0:
        sys.exit(1)


if __name__ == '__main__':
    main()
