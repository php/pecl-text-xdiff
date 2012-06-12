--TEST--
Check for xdiff presence
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
echo "xdiff extension is available";
?>
--EXPECT--
xdiff extension is available
