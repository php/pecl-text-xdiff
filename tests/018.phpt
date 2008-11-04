--TEST--
xdiff_file_rabdiff() with binary data
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_rabdiff(__DIR__ . '/zend.png', __DIR__ . '/php.png', __DIR__ . '/logo.tmp');
$a = file_get_contents(__DIR__ . '/logo.tmp');
$b = file_get_contents(__DIR__ . '/logo.rabdiff');
echo strcmp($a, $b);
unlink(__DIR__ . '/logo.tmp');
?>
--EXPECT--
0
