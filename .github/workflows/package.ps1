param (
    [Parameter(Mandatory)] $ts,
    [Parameter(Mandatory)] $arch
)

$ErrorActionPreference = "Stop"

if ($arch -eq "x86") {
    $builddir = "."
} else {
    $builddir = "x64"
}
$builddir += "\Release"
if ($ts -eq "ts") {
    $builddir += "_TS"
}

$dir = New-Item "windows_package" -ItemType "directory"
Copy-Item "..\deps\COPYING.LIBXDIFF" -Destination $dir
Copy-Item "CREDITS" -Destination $dir
Copy-Item "LICENSE" -Destination $dir
Copy-Item "README.API" -Destination $dir
Copy-Item "$builddir\php_xdiff.dll" -Destination $dir
Copy-Item "$builddir\php_xdiff.pdb" -Destination $dir
