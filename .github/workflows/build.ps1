param (
    [Parameter(Mandatory)] $arch
)

$ErrorActionPreference = "Stop"

Set-Location ..

$develdir = (Get-Item "php-devel").FullName
$phpdir = (Get-Item "php").FullName
$Env:PATH = "$develdir;$phpdir;$Env:PATH"

Set-Location "pecl-text-xdiff"

$task = New-Item "..\task.bat" -Force
Add-Content $task "call phpize 2>&1"
Add-Content $task "call configure --with-prefix=$phpdir --with-extra-includes=deps\include --with-extra-libs=deps\lib --enable-debug-pack --with-xdiff"
Add-Content $task "nmake 2>&1"
Add-Content $task "exit %errorlevel%"
& "..\php-sdk\phpsdk-vc15-$arch.bat" -t $task
if (-not $?) {
    throw "build failed with errorlevel $LastExitCode"
}
