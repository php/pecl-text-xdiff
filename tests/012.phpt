--TEST--
xdiff_file_rabdiff()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_rabdiff(__DIR__ . '/file.1', __DIR__ . '/file.2', __DIR__ . '/file.rbd');
$a = file_get_contents(__DIR__ . '/file.rbd');
$b = file_get_contents(__DIR__ . '/file.rabdiff');
echo strcmp($a, $b);
unlink(__DIR__ . '/file.rbd');
?>
--EXPECT--
0
