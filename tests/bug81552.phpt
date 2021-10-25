--TEST--
Bug #81552: xdiff_string_bpatch() and others missing
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php
$s1 = 'The quick brown fox jumps over the lazy dog.';
$s2 = 'The quick brown fox jumps over the lazy cog.';
var_dump(xdiff_string_bpatch($s1, xdiff_string_bdiff($s1, $s2)));
xdiff_file_bdiff(__DIR__ . '/file.1', __DIR__ . '/file.2', __DIR__ . '/file.bd81552');
xdiff_file_bpatch(__DIR__ . '/file.1', __DIR__ . '/file.bd81552', __DIR__ . '/file.p81552');
$a = file_get_contents(__DIR__ . '/file.2');
$b = file_get_contents(__DIR__ . '/file.p81552');
var_dump(strcmp($a, $b));
unlink(__DIR__ . '/file.bd81552');
unlink(__DIR__ . '/file.p81552');
?>
--EXPECT--
string(44) "The quick brown fox jumps over the lazy cog."
int(0)
