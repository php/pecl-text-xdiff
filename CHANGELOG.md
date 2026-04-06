# Version 2.2.0 - 2026-04-06

- Fixed memory leak when $error parameter is refcounted in patch/merge functions (ranvis)
- Fixed inverted return value in xdiff_file_bpatch() (returned true on failure)
- Fixed missing error checks in xdiff_string_diff/bdiff/rabdiff()
- Fixed potential double-free and size overflow in append_string()
- Fixed unchecked php_stream_read() return in load_mm_file()
- Fixed size truncation: use size_t consistently, add LONG_MAX bounds checks
- Fixed signed/unsigned mismatches in callback functions
- Added PIE support (composer.json)

# Version 2.1.1 - 2022-05-24

- Fixed bug #81552 xdiff_string_bpatch() and others missing (Kevin Israel)
- Fixed bug #77826 xdiff_string_bpatch wrong return value on error (SATO Kentaro)

# Version 2.1.0 - 2021-05-18

- PHP 8 support
- Drop PHP 5 support

# Version 2.0.1 - 2016-05-17

- Update License to PHP 3.1

# Version 2.0.0 - 2015-11-23

- PHP 7 Release

# Version 1.5.1 - 2009-10-18

- Fix for #16566

# Version 1.5.0 - 2009-10-18

- New functions added
- Memory optimizations
- Better handling of out of memory situations

# Version 1.4.1 - 2008-07-01

- Fixes for #7215, #7422 and #8227

# Version 1.4 - 2005-11-14

- Fixed bug #5064

# Version 1.3 - 2005-05-09

- Fix compile error

# Version 1.2 - 2004-11-09

- Fix libxdiff 0.9 crash
- Force last parameters of xdiff_string_patch and xdiff_string_merge3 to be passed by reference

# Version 1.1 - 2004-03-21

- Fix FreeBSD build

# Version 1.0 - 2004-02-09

- Add support to memory_limit (libxdiff >= 0.6 required)

# Version 0.3 - 2004-01-10

- Win32 build fixes

# Version 0.2 - 2004-01-08

- Initial release
