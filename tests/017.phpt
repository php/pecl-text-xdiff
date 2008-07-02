--TEST--
xdiff_string_diff_binary() with binary data
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$a = file_get_contents('tests/zend.png');
$b = file_get_contents('tests/php.png');
$c = file_get_contents('tests/logo.bdiff');
$d = xdiff_string_diff_binary($a, $b);
echo strcmp($c, $d);
?>
--EXPECT--
0
