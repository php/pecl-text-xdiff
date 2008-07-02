--TEST--
xdiff_file_patch() and context5.patch
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_patch('tests/lorem1.txt', 'tests/context5.patch', 'tests/lorem.tmp');
$a = file_get_contents('tests/lorem2.txt');
$b = file_get_contents('tests/lorem.tmp');
echo strcmp($a, $b);
unlink('tests/lorem.tmp');
?>
--EXPECT--
0
