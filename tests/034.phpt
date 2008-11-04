--TEST--
xdiff_file_patch() and context1.patch and reverse patching
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_patch(__DIR__ . '/lorem2.txt', __DIR__ . '/context1.patch', __DIR__ . '/lorem.tmp', XDIFF_PATCH_REVERSE);
$a = file_get_contents(__DIR__ . '/lorem1.txt');
$b = file_get_contents(__DIR__ . '/lorem.tmp');
echo strcmp($a, $b);
unlink(__DIR__ . '/lorem.tmp');
?>
--EXPECT--
0
