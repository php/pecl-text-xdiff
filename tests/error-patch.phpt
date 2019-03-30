--TEST--
xdiff_string_patch() error
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php
$str = "123\n456\n";
$patch = "@@ -0,0 +0,0 @@\n 789\n +012\n 345\n";
$result = xdiff_string_patch($str, $patch);
var_dump($result);
?>
--EXPECT--
bool(false)
