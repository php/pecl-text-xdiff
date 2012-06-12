--TEST--
xdiff_file_merge3()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_merge3(__DIR__ . '/merge1.h', __DIR__ . '/merge2.h', __DIR__ . '/merge3.h', __DIR__ . '/merge.h');
$a = file_get_contents(__DIR__ . '/merge.h');
$b = file_get_contents(__DIR__ . '/merge4.h');
echo strcmp($a, $b);
unlink(__DIR__ . '/merge.h');
?>
--EXPECT--
0
