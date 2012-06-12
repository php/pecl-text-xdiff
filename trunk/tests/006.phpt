--TEST--
xdiff_string_patch()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php
$a = file_get_contents(__DIR__ . '/file.1');
$b = file_get_contents(__DIR__ . '/file_test.diff');
$c = xdiff_string_patch($a, $b);
$d = file_get_contents(__DIR__ . '/file.2');
var_dump(strcmp($c, $d));
?>
--EXPECT--
int(0)
