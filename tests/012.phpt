--TEST--
xdiff_file_rabdiff()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_rabdiff('tests/file.1', 'tests/file.2', 'tests/file.rbd');
$a = file_get_contents('tests/file.rbd');
$b = file_get_contents('tests/file.rabdiff');
echo strcmp($a, $b);
unlink('tests/file.rbd');
?>
--EXPECT--
0
