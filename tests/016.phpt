--TEST--
xdiff_file_diff_binary() with binary data
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_diff_binary('tests/zend.png', 'tests/php.png', 'tests/logo.tmp');
$a = file_get_contents('tests/logo.bdiff');
$b = file_get_contents('tests/logo.tmp');
echo strcmp($a, $b);
unlink('tests/logo.tmp');
?>
--EXPECT--
0
