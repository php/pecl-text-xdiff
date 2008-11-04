--TEST--
xdiff_string_diff_binary() with binary data
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
$a = file_get_contents(__DIR__ . '/zend.png');
$b = file_get_contents(__DIR__ . '/php.png');
$c = file_get_contents(__DIR__ . '/logo.bdiff');
$d = xdiff_string_diff_binary($a, $b);
echo strcmp($c, $d);
?>
--EXPECT--
0
