--TEST--
xdiff_file_patch_binary() with binary data and rabdiff patch
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_patch_binary('tests/zend.png', 'tests/logo.rabdiff', 'tests/logo.tmp');
$a = file_get_contents('tests/php.png');
$b = file_get_contents('tests/logo.tmp');
echo strcmp($a, $b);
unlink('tests/logo.tmp');
?>
--EXPECT--
0
