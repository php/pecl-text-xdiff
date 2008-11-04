--TEST--
xdiff_file_diff()
--SKIPIF--
<?php if (!extension_loaded("xdiff")) print "skip"; ?>
--FILE--
<?php 
xdiff_file_diff(__DIR__ . '/file.1', __DIR__ . '/file.2', __DIR__ . '/file.d', 1);
readfile(__DIR__ . '/file.d');
unlink(__DIR__ . '/file.d');
?>
--EXPECT--
@@ -3,3 +3,3 @@
 echo $a;
-$b = 320;
+$b = 420;
 echo $b;

