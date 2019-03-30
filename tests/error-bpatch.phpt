--TEST--
xdiff_string_bpatch() error
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php
$str = '0123456789';
$patch = hex2bin('0d02f50a0b000000010a31323334353637383930');
$result = xdiff_string_bpatch($str, $patch);
var_dump($result);
?>
--EXPECT--
bool(false)
