--TEST--
xdiff_string_patch()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$a = file_get_contents('tests/file.1');
$b = file_get_contents('tests/file.diff');
$c = xdiff_string_patch($a, $b);
$d = file_get_contents('tests/file.2');
echo strcmp($c, $d);
?>
--EXPECT--
0
