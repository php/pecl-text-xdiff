--TEST--
xdiff_string_rabdiff() with binary data
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
$a = file_get_contents(__DIR__ . '/zend.png');
$b = file_get_contents(__DIR__ . '/php.png');
$c = file_get_contents(__DIR__ . '/logo.rabdiff');
$d = xdiff_string_rabdiff($a, $b);
echo strcmp($c, $d);
?>
--EXPECT--
0
