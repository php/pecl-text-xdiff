--TEST--
xdiff_file_diff()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
xdiff_file_diff('tests/file.1', 'tests/file.2', 'tests/file.d', 1);
readfile('tests/file.d');
unlink('tests/file.d');
?>
--EXPECT--
@@ -3,3 +3,3 @@
 echo $a;
-$b = 320;
+$b = 420;
 echo $b;

