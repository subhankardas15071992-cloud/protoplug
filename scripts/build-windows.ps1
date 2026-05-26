param(
    [string] $Config = "Release",
    [string] $BuildDir = "",
    [string] $DistDir = "",
    [string] $Generator = "Visual Studio 17 2022",
    [string] $Arch = "x64"
)

$ErrorActionPreference = "Stop"

$RootDir = Resolve-Path (Join-Path $PSScriptRoot "..")
$VersionFile = Join-Path $RootDir "DevScripts/resources/version.txt"
$Version = (Get-Content -LiteralPath $VersionFile -Raw).Trim()
if ([string]::IsNullOrWhiteSpace($Version)) {
    throw "Unable to read package version from DevScripts/resources/version.txt"
}

if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $RootDir "build/windows-x64"
}

if ([string]::IsNullOrWhiteSpace($DistDir)) {
    $DistDir = Join-Path $RootDir "dist"
}

$PackageName = "protoplug-$Version-windows-x64"
$PackageDir = Join-Path $DistDir $PackageName
$ZipPath = Join-Path $DistDir "$PackageName.zip"

$cmakeArgs = @(
    "-S", $RootDir,
    "-B", $BuildDir,
    "-DPROTOPLUG_COPY_PLUGIN_AFTER_BUILD=OFF"
)

if (-not [string]::IsNullOrWhiteSpace($Generator)) {
    $cmakeArgs += @("-G", $Generator)
}

if (-not [string]::IsNullOrWhiteSpace($Arch)) {
    $cmakeArgs += @("-A", $Arch)
}

if ($env:PROTOPLUG_JUCE_DIR) {
    $cmakeArgs += "-DPROTOPLUG_JUCE_DIR=$env:PROTOPLUG_JUCE_DIR"
}

if ($env:PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR) {
    $cmakeArgs += "-DPROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR=$env:PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR"
}

& cmake @cmakeArgs
& cmake --build $BuildDir --config $Config --parallel --target `
    protoplug_fx_All protoplug_gen_All protoplug_fx_CLAP protoplug_gen_CLAP

Remove-Item -LiteralPath $PackageDir -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $ZipPath -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path `
    (Join-Path $PackageDir "Plugins/VST3"), `
    (Join-Path $PackageDir "Plugins/CLAP") | Out-Null

function Copy-Artifact {
    param(
        [string] $Source,
        [string] $Destination
    )

    if (-not (Test-Path -LiteralPath $Source)) {
        throw "Missing expected artifact: $Source"
    }

    Copy-Item -LiteralPath $Source -Destination $Destination -Recurse -Force
}

Copy-Artifact (Join-Path $BuildDir "protoplug_fx_artefacts/$Config/VST3/Lua Protoplug Fx.vst3") (Join-Path $PackageDir "Plugins/VST3")
Copy-Artifact (Join-Path $BuildDir "protoplug_fx_artefacts/$Config/CLAP/Lua Protoplug Fx.clap") (Join-Path $PackageDir "Plugins/CLAP")
Copy-Artifact (Join-Path $BuildDir "protoplug_gen_artefacts/$Config/VST3/Lua Protoplug Gen.vst3") (Join-Path $PackageDir "Plugins/VST3")
Copy-Artifact (Join-Path $BuildDir "protoplug_gen_artefacts/$Config/CLAP/Lua Protoplug Gen.clap") (Join-Path $PackageDir "Plugins/CLAP")

Copy-Item -LiteralPath (Join-Path $RootDir "ProtoplugFiles") -Destination $PackageDir -Recurse -Force
Copy-Item -LiteralPath (Join-Path $RootDir "readme.md") -Destination $PackageDir -Force
Copy-Item -LiteralPath (Join-Path $RootDir "license.txt") -Destination $PackageDir -Force

Compress-Archive -LiteralPath $PackageDir -DestinationPath $ZipPath -Force

Write-Host "Created $ZipPath"
