<?php
/** @generate-function-entries */

/** @return string|false */
function xdiff_string_diff(string $str1, string $str2, int $context = 3, bool $minimal = false) {}

function xdiff_file_diff(string $file1, string $file2, string $dest, int $context = 3, bool $minimal = false): bool {}

/** @return string|false */
function xdiff_string_bdiff(string $str1, string $str2) {}

/**
 * @return string|false
 * @alias xdiff_string_bdiff
 */
function xdiff_string_diff_binary(string $str1, string $str2) {}

function xdiff_file_bdiff(string $file1, string $file2, string $dest): bool {}

/** @alias xdiff_file_bdiff */
function xdiff_file_diff_binary(string $file1, string $file2, string $dest): bool {}

/** @return string|false */
function xdiff_string_rabdiff(string $str1, string $str2) {}

function xdiff_file_rabdiff(string $file1, string $file2, string $dest): bool {}

function xdiff_file_bdiff_size(string $file1, string $file2, string $dest): bool {}

function xdiff_string_bdiff_size(string $file1, string $file2, string $dest): bool {}

/** @return string|bool */
function xdiff_file_patch(string $file, string $patch, string $dest, int $flags=XDIFF_PATCH_NORMAL) {}

/** @return string|bool */
function xdiff_string_patch(string $file, string $patch, int $flags=XDIFF_PATCH_NORMAL, string &$error=null) {}

function xdiff_file_bpatch(string $file, string $patch, string $dest): bool {}

/** @alias xdiff_file_bpatch */
function xdiff_file_patch_binary(string $file, string $patch, string $dest): bool {}

/** @return string|false */
function xdiff_string_bpatch(string $str, string $patch) {}

/**
 * @return string|false
 * @alias xdiff_string_bpatch
 */
function xdiff_string_patch_binary(string $str, string $patch) {}

/** @return string|bool */
function xdiff_file_merge3(string $file1, string $file2, string $file3, string $dest) {}

/** @return string|bool */
function xdiff_string_merge3(string $str1, string $str2, string $str3, string &$error=null) {}
