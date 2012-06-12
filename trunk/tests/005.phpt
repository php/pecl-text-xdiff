--TEST--
xdiff_file_diff_binary()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_diff_binary(__DIR__ . '/file.1', __DIR__ . '/file.2', __DIR__ . '/file.bd');
$a = file_get_contents(__DIR__ . '/file.bd');
$b = file_get_contents(__DIR__ . '/file.bdiff');
echo strcmp($a, $b);
unlink(__DIR__ . '/file.bd');
?>
--EXPECT--
0
