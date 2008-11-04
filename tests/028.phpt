--TEST--
xdiff_string_patch() with context1.patch
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
$a = file_get_contents(__DIR__ . '/lorem1.txt');
$b = file_get_contents(__DIR__ . '/context1.patch');
$c = xdiff_string_patch($a, $b);
$d = file_get_contents(__DIR__ . '/lorem2.txt');
echo strcmp($c, $d);
?>
--EXPECT--
0
