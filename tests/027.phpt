--TEST--
xdiff_file_diff() with context = 5
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_diff('tests/lorem1.txt', 'tests/lorem2.txt', 'tests/lorem.tmp', 5);
echo strcmp(file_get_contents('tests/context5.patch'), file_get_contents('tests/lorem.tmp'));
unlink('tests/lorem.tmp');
?>
--EXPECT--
0
