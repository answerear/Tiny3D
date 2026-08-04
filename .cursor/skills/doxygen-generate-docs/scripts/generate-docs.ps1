#Requires -Version 5.1
<#
.SYNOPSIS
  Generate Tiny3D API HTML (+ CHM) from header Doxygen comments.

.DESCRIPTION
  Scans first-party headers under source/ (excludes External / dependencies /
  build trees), writes docs to doc/api by default.

.PARAMETER RepoRoot
  Repository root. Default: three levels up from this script
  (.cursor/skills/doxygen-generate-docs/scripts).

.PARAMETER OutputDir
  Output directory relative to RepoRoot or absolute. Default: doc/api

.PARAMETER InputDir
  Input directory relative to RepoRoot or absolute. Default: source

.PARAMETER SkipChm
  Generate HTML only; do not require or invoke hhc.

.PARAMETER OpenHtml
  Open doc/api/html/index.html after success.

.EXAMPLE
  pwsh -File .cursor/skills/doxygen-generate-docs/scripts/generate-docs.ps1
#>
[CmdletBinding()]
param(
    [string]$RepoRoot = '',
    [string]$OutputDir = 'doc/api',
    [string]$InputDir = 'source',
    [switch]$SkipChm,
    [switch]$OpenHtml
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Resolve-RepoPath {
    param([string]$Root, [string]$Path)
    if ([System.IO.Path]::IsPathRooted($Path)) {
        return [System.IO.Path]::GetFullPath($Path)
    }
    return [System.IO.Path]::GetFullPath((Join-Path $Root $Path))
}

function Test-ExcludedPath([string]$Path) {
    $norm = $Path -replace '/', '\'
    return $norm -match '\\External\\' -or
           $norm -match '\\dependencies\\' -or
           $norm -match '\\Android\\' -or
           $norm -match '\\\.cxx\\' -or
           $norm -match '\\vs2019-x64\\' -or
           $norm -match '\\nmake\\' -or
           $norm -match '\\bin\\' -or
           $norm -match '\\lib\\' -or
           $norm -match '\\Projects\\' -or
           $norm -match '\\CMakeFiles\\'
}

function Get-DoxygenInputDirs {
    param([string]$Root)
    # Prefer module Include/ trees. Do NOT use EXCLUDE_PATTERNS */Source/*
    # on Windows — case-insensitive match would wipe out the whole source/ tree.
    if ((Split-Path -Leaf $Root) -ieq 'Include') {
        return @($Root)
    }
    $dirs = Get-ChildItem -Path $Root -Recurse -Directory -Filter 'Include' -ErrorAction SilentlyContinue |
        Where-Object { -not (Test-ExcludedPath $_.FullName) } |
        ForEach-Object { $_.FullName }
    if ($dirs -and @($dirs).Count -gt 0) {
        return @($dirs)
    }
    return @($Root)
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$skillDir = Split-Path -Parent $scriptDir

if (-not $RepoRoot) {
    # scripts -> skill -> skills -> .cursor -> repo
    $RepoRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $skillDir))
}
$RepoRoot = [System.IO.Path]::GetFullPath($RepoRoot)

$inputRoot = Resolve-RepoPath -Root $RepoRoot -Path $InputDir
$outputAbs = Resolve-RepoPath -Root $RepoRoot -Path $OutputDir
$templatePath = Join-Path $skillDir 'templates\Doxyfile.in'
$generatedDoxyfile = Join-Path $outputAbs 'Doxyfile'
$warnLog = Join-Path $outputAbs 'doxygen-warnings.log'
$chmName = 'Tiny3D-API.chm'
$ensureHhc = Join-Path $scriptDir 'ensure-hhc.ps1'

if (-not (Test-Path -LiteralPath $inputRoot)) {
    throw "INPUT directory not found: $inputRoot"
}
$inputDirs = Get-DoxygenInputDirs -Root $inputRoot
if (-not $inputDirs -or @($inputDirs).Count -eq 0) {
    throw "No Include/ directories found under: $inputRoot"
}

Write-Host "[generate-docs] RepoRoot   = $RepoRoot"
Write-Host "[generate-docs] InputRoot  = $inputRoot"
Write-Host "[generate-docs] Include#   = $(@($inputDirs).Count)"
Write-Host "[generate-docs] Output     = $outputAbs"

$doxygen = Get-Command doxygen.exe -ErrorAction SilentlyContinue
if (-not $doxygen) {
    throw @"
doxygen.exe not found in PATH.
Install options:
  choco install doxygen.install -y
  scoop install doxygen
  https://www.doxygen.nl/download.html
"@
}
Write-Host "[generate-docs] Doxygen    = $($doxygen.Source) ($(& doxygen.exe -v))"

$hhcPath = ''
$generateHtmlHelp = 'NO'
if (-not $SkipChm) {
    $hhcOut = & $ensureHhc 2>&1
    $hhcExit = $LASTEXITCODE
    if ($hhcExit -ne 0) {
        throw "ensure-hhc.ps1 failed (exit $hhcExit). Re-run elevated or pass -SkipChm.`n$hhcOut"
    }
    # ensure-hhc writes the path on stdout (last non-empty line).
    $hhcPath = ($hhcOut | Where-Object { $_ -and "$_" -notmatch '^\[' } | Select-Object -Last 1)
    if (-not $hhcPath) {
        $hhcPath = ($hhcOut | Select-Object -Last 1)
    }
    $hhcPath = "$hhcPath".Trim()
    if (-not (Test-Path -LiteralPath $hhcPath)) {
        throw "hhc path invalid: $hhcPath"
    }
    $generateHtmlHelp = 'YES'
    Write-Host "[generate-docs] HHC        = $hhcPath"
} else {
    Write-Host "[generate-docs] CHM skipped (-SkipChm)"
}

New-Item -ItemType Directory -Force -Path $outputAbs | Out-Null

# Doxygen prefers forward slashes; quote paths that contain spaces.
function Format-DoxyPath([string]$Path) {
    $p = $Path -replace '\\', '/'
    if ($p -match '\s') { return "`"$p`"" }
    return $p
}

if (-not (Test-Path -LiteralPath $templatePath)) {
    throw "Doxyfile template not found: $templatePath"
}

$inputForDoxy = (@($inputDirs) | ForEach-Object { Format-DoxyPath $_ }) -join " \`n                         "

$replacements = @{
    '@OUTPUT_DIRECTORY@'   = (Format-DoxyPath $outputAbs)
    '@INPUT_DIR@'          = $inputForDoxy
    '@STRIP_FROM_PATH@'    = (Format-DoxyPath $RepoRoot)
    '@WARN_LOGFILE@'       = (Format-DoxyPath $warnLog)
    '@GENERATE_HTMLHELP@'  = $generateHtmlHelp
    '@CHM_FILE@'           = $chmName
    '@HHC_LOCATION@'       = $(if ($hhcPath) { Format-DoxyPath $hhcPath } else { '' })
}

$content = Get-Content -LiteralPath $templatePath -Raw -Encoding UTF8
foreach ($key in $replacements.Keys) {
    $content = $content.Replace($key, [string]$replacements[$key])
}
# Write without BOM for doxygen compatibility.
$utf8NoBom = New-Object System.Text.UTF8Encoding $false
[System.IO.File]::WriteAllText($generatedDoxyfile, $content, $utf8NoBom)
Write-Host "[generate-docs] Wrote      = $generatedDoxyfile"

Push-Location $RepoRoot
try {
    Write-Host "[generate-docs] Running doxygen..."
    & doxygen.exe $generatedDoxyfile
    if ($LASTEXITCODE -ne 0) {
        throw "doxygen exited with code $LASTEXITCODE (see $warnLog)"
    }
}
finally {
    Pop-Location
}

$htmlIndex = Join-Path $outputAbs 'html\index.html'
$chmPath = Join-Path $outputAbs "html\$chmName"

Write-Host "[generate-docs] Done."
Write-Host "[generate-docs] HTML index = $htmlIndex"
if (Test-Path -LiteralPath $htmlIndex) {
    Write-Host "[generate-docs] HTML OK"
} else {
    Write-Warning "[generate-docs] HTML index missing: $htmlIndex"
}

if (-not $SkipChm) {
    if (Test-Path -LiteralPath $chmPath) {
        # Also copy CHM next to html/ for easier discovery.
        $chmCopy = Join-Path $outputAbs $chmName
        Copy-Item -LiteralPath $chmPath -Destination $chmCopy -Force
        Write-Host "[generate-docs] CHM OK     = $chmCopy"
    } else {
        Write-Warning @"
[generate-docs] CHM not found at $chmPath
Doxygen may have written .hhp under html/ but hhc failed.
Check console output and $warnLog. You can compile manually:
  & `"$hhcPath`" `"$(Join-Path $outputAbs 'html\index.hhp')`"
"@
    }
}

if (Test-Path -LiteralPath $warnLog) {
    $warnSize = (Get-Item -LiteralPath $warnLog).Length
    if ($warnSize -gt 0) {
        Write-Host "[generate-docs] Warnings log: $warnLog ($warnSize bytes)"
    }
}

if ($OpenHtml -and (Test-Path -LiteralPath $htmlIndex)) {
    Start-Process $htmlIndex
}

exit 0
