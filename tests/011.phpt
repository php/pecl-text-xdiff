--TEST--
xdiff_file_merge3()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_merge3('tests/merge1.h', 'tests/merge2.h', 'tests/merge3.h', 'tests/merge.h');
$a = file_get_contents('tests/merge.h');
$b = file_get_contents('tests/merge4.h');
echo strcmp($a, $b);
unlink('tests/merge.h');
?>
--EXPECT--
0
