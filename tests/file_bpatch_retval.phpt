--TEST--
xdiff_file_bpatch() return value on success and failure
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php
$result = xdiff_file_bpatch(__DIR__ . '/file.1', __DIR__ . '/file.bdiff', __DIR__ . '/file.bpatch_out');
var_dump($result);
$a = file_get_contents(__DIR__ . '/file.2');
$b = file_get_contents(__DIR__ . '/file.bpatch_out');
echo "content match: " . (strcmp($a, $b) === 0 ? "yes" : "no") . "\n";
unlink(__DIR__ . '/file.bpatch_out');

$result = xdiff_file_bpatch(__DIR__ . '/file.1', __DIR__ . '/file.1', __DIR__ . '/file.bpatch_out2');
var_dump($result);
@unlink(__DIR__ . '/file.bpatch_out2');
?>
--EXPECT--
bool(true)
content match: yes
bool(false)
