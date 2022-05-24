<?php
/** @generate-function-entries */

/**
 * @param string $str1
 * @param string $str2
 * @param int $context
 * @param bool $minimal
 * @return string|false
 */
function xdiff_string_diff(string $str1, string $str2, int $context = 3, bool $minimal = false) {}

function xdiff_file_diff(string $file1, string $file2, string $dest, int $context = 3, bool $minimal = false): bool {}

/**
 * @param string $str1
 * @param string $str2
 * @return string|false
 * @alias xdiff_string_bdiff
 */
function xdiff_string_diff_binary(string $str1, string $str2) {}

/** @alias xdiff_file_bdiff */
function xdiff_file_diff_binary(string $file1, string $file2, string $dest): bool {}

/**
 * @param string $str1
 * @param string $str2
 * @return string|false
 */
function xdiff_string_rabdiff(string $str1, string $str2) {}

function xdiff_file_rabdiff(string $file1, string $file2, string $dest): bool {}

function xdiff_file_bdiff_size(string $file1, string $file2, string $dest): bool {}

function xdiff_string_bdiff_size(string $file1, string $file2, string $dest): bool {}

/**
 * @param string $file
 * @param string $patch
 * @param string $dest
 * @param int $flags
 * @return string|bool
 */
function xdiff_file_patch(string $file, string $patch, string $dest, int $flags=XDIFF_PATCH_NORMAL) {}

/**
 * @param string $file
 * @param string $patch
 * @param int $flags
 * @param string $error
 * @return string|bool
 */
function xdiff_string_patch(string $file, string $patch, int $flags=XDIFF_PATCH_NORMAL, ?string &$error=null) {}

/** @alias xdiff_file_bpatch */
function xdiff_file_patch_binary(string $file, string $patch, string $dest): bool {}

/**
 * @param string $file
 * @param string $patch
 * @return string|false
 * @alias xdiff_string_bpatch
 */
function xdiff_string_patch_binary(string $str, string $patch) {}

/**
 * @param string $file1
 * @param string $file2
 * @param string $file3
 * @param string $dest
 * @return string|bool
 */
function xdiff_file_merge3(string $file1, string $file2, string $file3, string $dest) {}

/**
 * @param string $str1
 * @param string $str2
 * @param string $str3
 * @param string $error
 * @return string|bool
 */
function xdiff_string_merge3(string $str1, string $str2, string $str3, ?string &$error=null) {}
