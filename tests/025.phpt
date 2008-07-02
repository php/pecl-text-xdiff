--TEST--
xdiff_string_diff() with context = 5
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$a = file_get_contents('tests/lorem1.txt');
$b = file_get_contents('tests/lorem2.txt');
$patch = xdiff_string_diff($a, $b, 5);
echo strcmp(file_get_contents('tests/context5.patch'), $patch);
?>
--EXPECT--
0
