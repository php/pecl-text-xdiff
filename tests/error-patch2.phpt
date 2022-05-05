--TEST--
xdiff_string_patch() rejection
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php
$str = "123\n456\n";
$patch = "@@ -1,2 +1,3 @@\n 789\n+012\n 345\n";
$result = xdiff_string_patch($str, $patch, XDIFF_PATCH_NORMAL, $error);
var_dump($result === $str);
var_dump($error === $patch);
?>
--EXPECT--
bool(true)
bool(true)
