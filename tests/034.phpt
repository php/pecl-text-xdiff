--TEST--
xdiff_file_patch() and context1.patch and reverse patching
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_patch('tests/lorem2.txt', 'tests/context1.patch', 'tests/lorem.tmp', XDIFF_PATCH_REVERSE);
$a = file_get_contents('tests/lorem1.txt');
$b = file_get_contents('tests/lorem.tmp');
echo strcmp($a, $b);
unlink('tests/lorem.tmp');
?>
--EXPECT--
0
