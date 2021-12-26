--TEST--
Check for xdiff functions
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php
$funcs = get_extension_funcs('xdiff');
sort($funcs, SORT_STRING);
foreach ($funcs as $func) {
    echo "$func\n";
}
?>
--EXPECT--
xdiff_file_bdiff
xdiff_file_bdiff_size
xdiff_file_bpatch
xdiff_file_diff
xdiff_file_diff_binary
xdiff_file_merge3
xdiff_file_patch
xdiff_file_patch_binary
xdiff_file_rabdiff
xdiff_string_bdiff
xdiff_string_bdiff_size
xdiff_string_bpatch
xdiff_string_diff
xdiff_string_diff_binary
xdiff_string_merge3
xdiff_string_patch
xdiff_string_patch_binary
xdiff_string_rabdiff
