# xdiff

PHP extension for creating and applying patches to both text and binary files. Wraps the [libxdiff](http://www.xmailserver.org/xdiff-lib.html) library.

## Installation

### PIE (recommended)

```bash
pie install pecl/xdiff
```

### PECL

```bash
pecl install xdiff
```

### From source

Requires libxdiff with `xdl_set_allocator`, `xdl_rabdiff`, and `XDL_PATCH_IGNOREBSPACE` support.

```bash
phpize
./configure --with-xdiff
make
make install
```

## API

### Diff

```php
xdiff_string_diff(string $str1, string $str2, int $context = 3, bool $minimal = false): string|false
```

Creates a unified diff of two strings. `$context` sets the number of surrounding lines. `$minimal` reduces diff size at the cost of speed.

```php
xdiff_file_diff(string $file1, string $file2, string $dest, int $context = 3, bool $minimal = false): bool
```

Creates a unified diff of two files and writes the result to `$dest`.

```php
xdiff_string_diff_binary(string $str1, string $str2): string|false
```

Creates a binary diff of two strings.

```php
xdiff_file_diff_binary(string $file1, string $file2, string $dest): bool
```

Creates a binary diff of two files and writes the result to `$dest`.

```php
xdiff_string_rabdiff(string $str1, string $str2): string|false
```

Creates a binary diff of two strings using the Rabin fingerprint algorithm.

```php
xdiff_file_rabdiff(string $file1, string $file2, string $dest): bool
```

Creates a binary diff of two files using the Rabin fingerprint algorithm and writes the result to `$dest`.

### Patch

```php
xdiff_string_patch(string $file, string $patch, int $flags = XDIFF_PATCH_NORMAL, ?string &$error = null): string|bool
```

Applies a unified patch to a string. Returns the patched string, or `false` on error. Rejected chunks are returned via `$error`.

`$flags`: `XDIFF_PATCH_NORMAL` (default) or `XDIFF_PATCH_REVERSE`.

```php
xdiff_file_patch(string $file, string $patch, string $dest, int $flags = XDIFF_PATCH_NORMAL): string|bool
```

Applies a unified patch from a file. Returns `true` on success, a string of rejected chunks on partial failure, or `false` on error.

```php
xdiff_string_patch_binary(string $str, string $patch): string|false
```

Applies a binary patch to a string. Works with both bdiff and rabdiff patches.

```php
xdiff_file_patch_binary(string $file, string $patch, string $dest): bool
```

Applies a binary patch from a file and writes the result to `$dest`.

### Merge

```php
xdiff_string_merge3(string $str1, string $str2, string $str3, ?string &$error = null): string|bool
```

Three-way merge of strings. Returns the merged string, or `false` on error. Conflicts are returned via `$error`.

```php
xdiff_file_merge3(string $file1, string $file2, string $file3, string $dest): string|bool
```

Three-way merge of files. Returns `true` on success, a string of rejected chunks on conflict, or `false` on error.

### Binary diff size

```php
xdiff_string_bdiff_size(string $patch): int|false
```

Returns the size of the file that would be created by applying the given binary patch.

```php
xdiff_file_bdiff_size(string $patch_file): int|false
```

Same as above, but reads the patch from a file.

## Constants

| Constant | Description |
|---|---|
| `XDIFF_PATCH_NORMAL` | Apply patch normally (default) |
| `XDIFF_PATCH_REVERSE` | Reverse the patch |
| `XDIFF_PATCH_IGNORESPACE` | Ignore whitespace when patching |
