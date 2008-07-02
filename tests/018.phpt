--TEST--
xdiff_file_rabdiff() with binary data
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_rabdiff('tests/zend.png', 'tests/php.png', 'tests/logo.tmp');
$a = file_get_contents('tests/logo.tmp');
$b = file_get_contents('tests/logo.rabdiff');
echo strcmp($a, $b);
unlink('tests/logo.tmp');
?>
--EXPECT--
0
