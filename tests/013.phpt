--TEST--
xdiff_string_rabdiff()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$a = file_get_contents('tests/file.1');
$b = file_get_contents('tests/file.2');
$c = file_get_contents('tests/file.rabdiff');
$d = xdiff_string_rabdiff($a, $b);
echo strcmp($c, $d);
?>
--EXPECT--
0
