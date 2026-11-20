<#
.SYNOPSIS
    Embeds scc-generated sample shaders into the SampleShaders_*.h C++ constants.

.DESCRIPTION
    The samples under source/Samples do not load assets/samples/shaders/*.pshader at
    runtime. Those sources are cross-compiled by scc into assets/samples/shaders/output
    (which is gitignored), and the results are embedded into
    source/Samples/Common/SampleShaders_{hlsl,gl4,gles3,vk}.h as C++ constants. That
    last step used to be a manual copy/paste, which is how edits to the .pshader
    sources could silently fail to reach the binaries. This script automates it.

    Handled: the lit pixel shaders, the ComputeApp linear write kernel, and every
    GL / ES pixel shader that samples a texture. The GL / ES ones must come from scc,
    because the runtime reads the HLSL t# / s# registers out of the combined sampler
    names that HLSLCross generates. Extend $jobs if more constants need to be kept in sync.

    Regenerate the inputs first, once per backend (scc picks its renderer plugin from
    a single -t value, so the four backends cannot be batched into one invocation):

        scc <shader>.pshader -t hlsl  -o <output>\HLSL      -N -O0
        scc <shader>.pshader -t glsl  -o <output>\OpenGL4   -N -O0
        scc <shader>.pshader -t essl  -o <output>\OpenGLES3 -N -O0
        scc <shader>.pshader -t spirv -o <output>\Vulkan    -N -O0

.PARAMETER Verify
    Compare only, do not write. Running -Verify against the generated output that the
    headers were last built from must report MATCH for every header; that is the
    regression test for this script's formatting.

.PARAMETER Headers
    Only process these headers (e.g. SampleShaders_gl4.h). Useful when only some
    backends were regenerated; the others keep their current contents.

.EXAMPLE
    .\embed-sample-shaders.ps1 -Verify
    .\embed-sample-shaders.ps1
    .\embed-sample-shaders.ps1 -Headers SampleShaders_gl4.h,SampleShaders_gles3.h
#>
param(
    [string]$Repo,
    [string]$OutputRoot,
    [switch]$Verify,
    [string[]]$Headers
)

$ErrorActionPreference = "Stop"

if (-not $Repo) {
    $Repo = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
}
if (-not $OutputRoot) {
    $OutputRoot = Join-Path $Repo "assets\samples\shaders\output"
}

$common = Join-Path $Repo "source\Samples\Common"

# Constant name -> generated file. Text backends become "...\n" string literals,
# Vulkan becomes a SPIR-V byte array.
$jobs = @(
    [PSCustomObject]@{ Header = "SampleShaders_hlsl.h";  Const = "SAMPLE_LIT_PIXEL_SHADER";      Src = "HLSL\LitGeometryApp_fragment.hlsl";      Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_hlsl.h";  Const = "FORWARD_PIXEL_SHADER";         Src = "HLSL\ForwardPass_fragment.hlsl";         Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gl4.h";   Const = "SAMPLE_LIT_PIXEL_SHADER_GL";   Src = "OpenGL4\LitGeometryApp_fragment.glsl";   Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gl4.h";   Const = "FORWARD_PIXEL_SHADER_GL";      Src = "OpenGL4\ForwardPass_fragment.glsl";      Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gles3.h"; Const = "SAMPLE_LIT_PIXEL_SHADER_GLES"; Src = "OpenGLES3\LitGeometryApp_fragment.essl"; Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gles3.h"; Const = "FORWARD_PIXEL_SHADER_GLES";    Src = "OpenGLES3\ForwardPass_fragment.essl";    Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_vk.h";    Const = "SAMPLE_LIT_PIXEL_SHADER_VK";   Src = "Vulkan\LitGeometryApp_fragment.spirv";   Kind = "spirv" }
    [PSCustomObject]@{ Header = "SampleShaders_vk.h";    Const = "FORWARD_PIXEL_SHADER_VK";      Src = "Vulkan\ForwardPass_fragment.spirv";      Kind = "spirv" }

    # ComputeApp. The compute stage is named "compute" by scc, hence the _compute suffix.
    [PSCustomObject]@{ Header = "SampleShaders_hlsl.h";  Const = "COMPUTE_LINEAR_WRITE";         Src = "HLSL\ComputeLinearWrite_compute.hlsl";       Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gl4.h";   Const = "COMPUTE_LINEAR_WRITE_GL";      Src = "OpenGL4\ComputeLinearWrite_compute.glsl";    Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gles3.h"; Const = "COMPUTE_LINEAR_WRITE_GLES";    Src = "OpenGLES3\ComputeLinearWrite_compute.essl";  Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_vk.h";    Const = "COMPUTE_LINEAR_WRITE_VK";      Src = "Vulkan\ComputeLinearWrite_compute.spirv";    Kind = "spirv" }

    # GL / ES pixel shaders with combined samplers (texture + sampler register numbers in the name)
    [PSCustomObject]@{ Header = "SampleShaders_gl4.h";   Const = "SAMPLE_PIXEL_SHADER_GL";       Src = "OpenGL4\GeometryApp_fragment.glsl";          Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gl4.h";   Const = "INVERT_PIXEL_SHADER_GL";       Src = "OpenGL4\InvertEffect_fragment.glsl";         Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gl4.h";   Const = "TINT_PIXEL_SHADER_GL";         Src = "OpenGL4\TintEffect_fragment.glsl";           Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gl4.h";   Const = "GRAYSCALE_PIXEL_SHADER_GL";    Src = "OpenGL4\GrayscaleEffect_fragment.glsl";      Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gles3.h"; Const = "SAMPLE_PIXEL_SHADER_GLES";     Src = "OpenGLES3\GeometryApp_fragment.essl";        Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gles3.h"; Const = "INVERT_PIXEL_SHADER_GLES";     Src = "OpenGLES3\InvertEffect_fragment.essl";       Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gles3.h"; Const = "TINT_PIXEL_SHADER_GLES";       Src = "OpenGLES3\TintEffect_fragment.essl";         Kind = "text"  }
    [PSCustomObject]@{ Header = "SampleShaders_gles3.h"; Const = "GRAYSCALE_PIXEL_SHADER_GLES";  Src = "OpenGLES3\GrayscaleEffect_fragment.essl";    Kind = "text"  }
)

if ($Headers) {
    $jobs = @($jobs | Where-Object { $Headers -contains $_.Header })
    if ($jobs.Count -eq 0) { throw "no jobs match -Headers $($Headers -join ', ')" }
}

function Build-TextBlock([string]$name, [string]$path) {
    $lines = [System.IO.File]::ReadAllText($path) -split "`r?`n"
    # Drop only the empty element produced by the file's final newline. hlsl output
    # ends with "}\n" while glsl/essl end with "}\n\n"; that extra blank line is part
    # of the embedded literal and must survive.
    if ($lines.Count -gt 0 -and $lines[-1] -eq "") {
        $lines = $lines[0..($lines.Count - 2)]
    }

    $sb = [System.Text.StringBuilder]::new()
    [void]$sb.Append("const char *$name =")
    for ($i = 0; $i -lt $lines.Count; $i++) {
        $esc = $lines[$i].Replace('\', '\\').Replace('"', '\"')
        $tail = if ($i -eq $lines.Count - 1) { '\n";' } else { '\n"' }
        [void]$sb.Append("`n    `"$esc$tail")
    }
    $sb.ToString()
}

function Build-SpirvBlock([string]$name, [string]$path) {
    $bytes = [System.IO.File]::ReadAllBytes($path)
    $sb = [System.Text.StringBuilder]::new()
    [void]$sb.Append("alignas(4) extern const unsigned char $name[] = {")
    for ($i = 0; $i -lt $bytes.Length; $i += 16) {
        $end = [Math]::Min($i + 15, $bytes.Length - 1)
        $cells = foreach ($b in $bytes[$i..$end]) { "0x{0:x2}" -f $b }
        $sep = if ($end -eq $bytes.Length - 1) { "" } else { "," }
        [void]$sb.Append("`n    " + ($cells -join ", ") + $sep)
    }
    [void]$sb.Append("`n};")
    $sb.ToString()
}

# Locate a constant by line: from its declaration to the first terminator line.
function Replace-Block([string[]]$lines, [string]$startPattern, [string]$endPattern, [string]$block) {
    $start = -1
    for ($i = 0; $i -lt $lines.Count; $i++) {
        if ($lines[$i] -match $startPattern) { $start = $i; break }
    }
    if ($start -lt 0) { throw "start not found: $startPattern" }

    $end = -1
    for ($i = $start; $i -lt $lines.Count; $i++) {
        if ($lines[$i] -match $endPattern) { $end = $i; break }
    }
    if ($end -lt 0) { throw "end not found after line $start" }

    $head = if ($start -eq 0) { @() } else { $lines[0..($start - 1)] }
    $tail = if ($end -eq $lines.Count - 1) { @() } else { $lines[($end + 1)..($lines.Count - 1)] }
    @($head) + ($block -split "`n") + @($tail)
}

$failed = $false

foreach ($g in ($jobs | Group-Object Header)) {
    $headerPath = Join-Path $common $g.Name
    $original = [System.IO.File]::ReadAllText($headerPath)
    $nl = if ($original -match "`r`n") { "`r`n" } else { "`n" }
    $lines = $original -split "`r?`n"

    foreach ($job in $g.Group) {
        $srcPath = Join-Path $OutputRoot $job.Src
        if (-not (Test-Path $srcPath)) { throw "missing generated file: $srcPath" }

        if ($job.Kind -eq "text") {
            $block = Build-TextBlock $job.Const $srcPath
            $startPat = '^const char \*' + [regex]::Escape($job.Const) + '\s*=\s*$'
            $endPat = '";\s*$'
        }
        else {
            $block = Build-SpirvBlock $job.Const $srcPath
            $startPat = '^alignas\(4\) extern const unsigned char ' + [regex]::Escape($job.Const) + '\[\]'
            $endPat = '^\};\s*$'
        }

        $lines = Replace-Block $lines $startPat $endPat $block
    }

    $updated = ($lines -join $nl)

    if ($Verify) {
        if ($updated -eq $original) {
            "MATCH   $($g.Name)"
        }
        else {
            "DIFFER  $($g.Name)  (current $($original.Length) bytes, rebuilt $($updated.Length) bytes)"
            $failed = $true
        }
    }
    else {
        [System.IO.File]::WriteAllText($headerPath, $updated)
        "updated $($g.Name)"
    }
}

if ($Verify -and $failed) { exit 1 }
