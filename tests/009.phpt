--TEST--
xdiff_file_patch_binary()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_patch_binary('tests/file.1', 'tests/file.bdiff', 'tests/file.p');
$a = file_get_contents('tests/file.2');
$b = file_get_contents('tests/file.p');
echo strcmp($a, $b);
unlink('tests/file.p');
?>
--EXPECT--
0
