--TEST--
xdiff_string_merge3()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$a = file_get_contents('tests/merge1.h');
$b = file_get_contents('tests/merge2.h');
$c = file_get_contents('tests/merge3.h');
$d = file_get_contents('tests/merge4.h');
$e = xdiff_string_merge3($a, $b, $c);
echo strcmp($d, $e);
?>
--EXPECT--
0
