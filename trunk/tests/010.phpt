--TEST--
xdiff_string_merge3()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
$a = file_get_contents(__DIR__ . '/merge1.h');
$b = file_get_contents(__DIR__ . '/merge2.h');
$c = file_get_contents(__DIR__ . '/merge3.h');
$d = file_get_contents(__DIR__ . '/merge4.h');
$e = xdiff_string_merge3($a, $b, $c);
echo strcmp($d, $e);
?>
--EXPECT--
0
