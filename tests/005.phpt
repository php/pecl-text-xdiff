--TEST--
xdiff_file_diff_binary()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_diff_binary('tests/file.1', 'tests/file.2', 'tests/file.bd');
$a = file_get_contents('tests/file.bd');
$b = file_get_contents('tests/file.bdiff');
echo strcmp($a, $b);
unlink('tests/file.bd');
?>
--EXPECT--
0
