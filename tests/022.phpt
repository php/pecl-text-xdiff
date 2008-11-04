--TEST--
xdiff_file_patch_binary() with binary data
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_patch_binary(__DIR__ . '/zend.png', __DIR__ . '/logo.bdiff', __DIR__ . '/logo.tmp');
$a = file_get_contents(__DIR__ . '/php.png');
$b = file_get_contents(__DIR__ . '/logo.tmp');
echo strcmp($a, $b);
unlink(__DIR__ . '/logo.tmp');
?>
--EXPECT--
0
