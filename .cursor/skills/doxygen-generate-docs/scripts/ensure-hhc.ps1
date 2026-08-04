#Requires -Version 5.1
<#
.SYNOPSIS
  Locate hhc.exe (HTML Help Workshop). Install if missing.

.DESCRIPTION
  Search order: PATH, common install dirs, then install via Chocolatey
  or direct download from Microsoft archive (Wayback).

.PARAMETER NoInstall
  Only search for hhc.exe; do not attempt installation.

.OUTPUTS
  Writes the full path to hhc.exe on success; exits non-zero on failure.
#>
[CmdletBinding()]
param(
    [switch]$NoInstall
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Find-Hhc {
    $cmd = Get-Command hhc.exe -ErrorAction SilentlyContinue
    if ($cmd -and $cmd.Source) {
        return $cmd.Source
    }

    $candidates = @(
        "${env:ProgramFiles(x86)}\HTML Help Workshop\hhc.exe"
        "${env:ProgramFiles}\HTML Help Workshop\hhc.exe"
        "C:\Program Files (x86)\HTML Help Workshop\hhc.exe"
        "C:\Program Files\HTML Help Workshop\hhc.exe"
    )
    foreach ($path in $candidates) {
        if ($path -and (Test-Path -LiteralPath $path)) {
            return $path
        }
    }
    return $null
}

function Test-IsAdmin {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($id)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Install-HhcViaChoco {
    $choco = Get-Command choco.exe -ErrorAction SilentlyContinue
    if (-not $choco) {
        return $false
    }
    Write-Host "[ensure-hhc] Installing html-help-workshop via Chocolatey..."
    & choco.exe install html-help-workshop -y
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "[ensure-hhc] choco install returned exit code $LASTEXITCODE"
        return $false
    }
    return $true
}

function Install-HhcViaDownload {
    $url = 'https://web.archive.org/web/20200918004813if_/https://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe'
    $tempRoot = Join-Path $env:TEMP ("htmlhelp-workshop-" + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Force -Path $tempRoot | Out-Null
    $installer = Join-Path $tempRoot 'htmlhelp.exe'
    $extractDir = Join-Path $tempRoot 'extracted'

    try {
        Write-Host "[ensure-hhc] Downloading HTML Help Workshop installer..."
        [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
        Invoke-WebRequest -Uri $url -OutFile $installer -UseBasicParsing

        Write-Host "[ensure-hhc] Extracting installer payload..."
        New-Item -ItemType Directory -Force -Path $extractDir | Out-Null
        # htmlhelp.exe is a self-extracting cabinet; /Q /C /T: extracts quietly.
        $p = Start-Process -FilePath $installer -ArgumentList @('/Q', "/T:$extractDir", '/C') -Wait -PassThru
        if ($p.ExitCode -ne 0) {
            throw "Extractor exited with code $($p.ExitCode)"
        }

        $setup = Get-ChildItem -Path $extractDir -Filter 'setup.exe' -Recurse -ErrorAction SilentlyContinue |
            Select-Object -First 1
        if (-not $setup) {
            # Some payloads ship htmlhelp.msi directly.
            $msi = Get-ChildItem -Path $extractDir -Filter '*.msi' -Recurse -ErrorAction SilentlyContinue |
                Select-Object -First 1
            if ($msi) {
                Write-Host "[ensure-hhc] Running MSI: $($msi.FullName)"
                $msiArgs = @('/i', $msi.FullName, '/qn', '/norestart')
                $p2 = Start-Process -FilePath 'msiexec.exe' -ArgumentList $msiArgs -Wait -PassThru
                if ($p2.ExitCode -ne 0 -and $p2.ExitCode -ne 3010) {
                    throw "msiexec exited with code $($p2.ExitCode)"
                }
                return $true
            }
            throw "Neither setup.exe nor .msi found after extraction"
        }

        Write-Host "[ensure-hhc] Running setup.exe (quiet)..."
        $p3 = Start-Process -FilePath $setup.FullName -ArgumentList @('/q') -Wait -PassThru
        if ($p3.ExitCode -ne 0 -and $p3.ExitCode -ne 3010) {
            # Fallback: interactive-friendly non-fatal if quiet flags differ by version.
            Write-Warning "[ensure-hhc] Quiet setup exit $($p3.ExitCode); retrying default install..."
            $p4 = Start-Process -FilePath $setup.FullName -Wait -PassThru
            if ($p4.ExitCode -ne 0 -and $p4.ExitCode -ne 3010) {
                throw "setup.exe exited with code $($p4.ExitCode)"
            }
        }
        return $true
    }
    finally {
        Remove-Item -LiteralPath $tempRoot -Recurse -Force -ErrorAction SilentlyContinue
    }
}

$found = Find-Hhc
if ($found) {
    Write-Output $found
    exit 0
}

if ($NoInstall) {
    Write-Error "[ensure-hhc] hhc.exe not found and -NoInstall was set"
    exit 1
}

Write-Host "[ensure-hhc] hhc.exe not found; attempting install..."

if (-not (Test-IsAdmin)) {
    Write-Warning "[ensure-hhc] Current shell is not elevated. Install may fail; re-run as Administrator if needed."
}

$installed = $false
try {
    $installed = Install-HhcViaChoco
} catch {
    Write-Warning "[ensure-hhc] Chocolatey path failed: $_"
}

if (-not $installed) {
    try {
        $installed = Install-HhcViaDownload
    } catch {
        Write-Error "[ensure-hhc] Direct download install failed: $_"
        Write-Host @"
[ensure-hhc] Manual install options:
  1) Admin PowerShell: choco install html-help-workshop -y
  2) Download https://web.archive.org/web/*/https://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe
     then run the installer. Typical path: C:\Program Files (x86)\HTML Help Workshop\hhc.exe
"@
        exit 1
    }
}

# Refresh PATH for this process from Machine + User
$env:Path = [System.Environment]::GetEnvironmentVariable('Path', 'Machine') + ';' +
            [System.Environment]::GetEnvironmentVariable('Path', 'User')

$found = Find-Hhc
if (-not $found) {
    Write-Error "[ensure-hhc] Install finished but hhc.exe still not found."
    exit 1
}

Write-Host "[ensure-hhc] Ready: $found"
Write-Output $found
exit 0
