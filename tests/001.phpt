--TEST--
Check for xdiff presence
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
echo "xdiff extension is available";
?>
--EXPECT--
xdiff extension is available
