--TEST--
xdiff_file_diff() with context = 1
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_diff(__DIR__ . '/lorem1.txt', __DIR__ . '/lorem2.txt', __DIR__ . '/lorem.tmp', 1);
echo strcmp(file_get_contents(__DIR__ . '/context1.patch'), file_get_contents(__DIR__ . '/lorem.tmp'));
unlink(__DIR__ . '/lorem.tmp');
?>
--EXPECT--
0
