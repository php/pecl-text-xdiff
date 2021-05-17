param (
    [Parameter(Mandatory)] $version,
    [Parameter(Mandatory)] $ts,
    [Parameter(Mandatory)] $arch,
    [Parameter(Mandatory)] $libxdiff
)

$ErrorActionPreference = "Stop"

if ($ts -eq "ts") {
    $tspart = ""
} else {
    $tspart = "-nts";
}

$releases = Invoke-WebRequest "https://windows.php.net/downloads/releases/releases.json" | ConvertFrom-Json
$phpversion = $releases.$version.version

Set-Location ..

# SDK

$file = "php-sdk-2.2.0.zip"
Invoke-WebRequest "https://github.com/microsoft/php-sdk-binary-tools/archive/$file" -OutFile $file
Expand-Archive $file -DestinationPath .
Move-Item "php-sdk-binary-tools-php-sdk-2.2.0" -Destination "php-sdk"

# PHP binaries

$file = "php-$phpversion$tspart-Win32-vc15-$arch.zip"
Invoke-WebRequest "https://windows.php.net/downloads/releases/$file" -OutFile $file
Expand-Archive $file -DestinationPath "php"

# PHP devel pack

$file = "php-devel-pack-$phpversion$tspart-Win32-vc15-$arch.zip"
Invoke-WebRequest "https://windows.php.net/downloads/releases/$file" -OutFile $file
Expand-Archive $file -DestinationPath .
Move-Item "php-$phpversion-devel-vc15-$arch" -Destination "php-devel"

# libxdiff

$file = "libxdiff-$libxdiff-vc15-$arch.zip"
Invoke-WebRequest "https://windows.php.net/downloads/pecl/deps/$file" -OutFile $file
Expand-Archive $file -DestinationPath "deps"
Move-Item "deps\COPYING" "deps\COPYING.LIBXDIFF"
