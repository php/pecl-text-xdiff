--TEST--
xdiff_file_patch() and context5.patch
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_patch(__DIR__ . '/lorem1.txt', __DIR__ . '/context5.patch', __DIR__ . '/lorem.tmp');
$a = file_get_contents(__DIR__ . '/lorem2.txt');
$b = file_get_contents(__DIR__ . '/lorem.tmp');
echo strcmp($a, $b);
unlink(__DIR__ . '/lorem.tmp');
?>
--EXPECT--
0
