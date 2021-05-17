$ErrorActionPreference = "Stop"

Set-Location ..

# SDK

$file = "php-sdk-2.2.0.zip"
Invoke-WebRequest "https://github.com/microsoft/php-sdk-binary-tools/archive/$file" -OutFile $file
Expand-Archive $file -DestinationPath .
Move-Item "php-sdk-binary-tools-php-sdk-2.2.0" -Destination "php-sdk"

# PHP binaries

$file = "php-7.4.19-nts-Win32-vc15-x64.zip"
Invoke-WebRequest "https://windows.php.net/downloads/releases/$file" -OutFile $file
Expand-Archive $file -DestinationPath "php"

# PHP devel pack

$file = "php-devel-pack-7.4.19-nts-Win32-vc15-x64.zip"
Invoke-WebRequest "https://windows.php.net/downloads/releases/$file" -OutFile $file
Expand-Archive $file -DestinationPath .
Move-Item "php-7.4.19-devel-vc15-x64" -Destination "php-devel"

# libxdiff

$file = "libxdiff-0.23-vs16-x64.zip"
Invoke-WebRequest "https://windows.php.net/downloads/pecl/deps/$file" -OutFile $file
Expand-Archive $file -DestinationPath "deps"
Move-Item "deps\COPYING" "deps\COPYING.LIBXDIFF"
