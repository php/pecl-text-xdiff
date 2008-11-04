--TEST--
xdiff_string_rabdiff()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
$a = file_get_contents(__DIR__ . '/file.1');
$b = file_get_contents(__DIR__ . '/file.2');
$c = file_get_contents(__DIR__ . '/file.rabdiff');
$d = xdiff_string_rabdiff($a, $b);
echo strcmp($c, $d);
?>
--EXPECT--
0
