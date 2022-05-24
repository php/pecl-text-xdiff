/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ec3a63aa4462f084433c3576ad532de87da7f867 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_xdiff_string_diff, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, context, IS_LONG, 0, "3")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, minimal, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xdiff_file_diff, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, file1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, file2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dest, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, context, IS_LONG, 0, "3")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, minimal, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xdiff_string_diff_binary, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xdiff_file_diff_binary, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, file1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, file2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dest, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xdiff_string_rabdiff arginfo_xdiff_string_diff_binary

#define arginfo_xdiff_file_rabdiff arginfo_xdiff_file_diff_binary

#define arginfo_xdiff_file_bdiff_size arginfo_xdiff_file_diff_binary

#define arginfo_xdiff_string_bdiff_size arginfo_xdiff_file_diff_binary

ZEND_BEGIN_ARG_INFO_EX(arginfo_xdiff_file_patch, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, patch, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dest, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "XDIFF_PATCH_NORMAL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xdiff_string_patch, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, patch, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "XDIFF_PATCH_NORMAL")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(1, error, IS_STRING, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xdiff_file_patch_binary, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, patch, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dest, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xdiff_string_patch_binary, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, patch, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xdiff_file_merge3, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, file1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, file2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, file3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dest, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xdiff_string_merge3, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(1, error, IS_STRING, 0, "null")
ZEND_END_ARG_INFO()


ZEND_FUNCTION(xdiff_string_diff);
ZEND_FUNCTION(xdiff_file_diff);
ZEND_FUNCTION(xdiff_string_bdiff);
ZEND_FUNCTION(xdiff_file_bdiff);
ZEND_FUNCTION(xdiff_string_rabdiff);
ZEND_FUNCTION(xdiff_file_rabdiff);
ZEND_FUNCTION(xdiff_file_bdiff_size);
ZEND_FUNCTION(xdiff_string_bdiff_size);
ZEND_FUNCTION(xdiff_file_patch);
ZEND_FUNCTION(xdiff_string_patch);
ZEND_FUNCTION(xdiff_file_bpatch);
ZEND_FUNCTION(xdiff_string_bpatch);
ZEND_FUNCTION(xdiff_file_merge3);
ZEND_FUNCTION(xdiff_string_merge3);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(xdiff_string_diff, arginfo_xdiff_string_diff)
	ZEND_FE(xdiff_file_diff, arginfo_xdiff_file_diff)
	ZEND_FALIAS(xdiff_string_diff_binary, xdiff_string_bdiff, arginfo_xdiff_string_diff_binary)
	ZEND_FALIAS(xdiff_file_diff_binary, xdiff_file_bdiff, arginfo_xdiff_file_diff_binary)
	ZEND_FE(xdiff_string_rabdiff, arginfo_xdiff_string_rabdiff)
	ZEND_FE(xdiff_file_rabdiff, arginfo_xdiff_file_rabdiff)
	ZEND_FE(xdiff_file_bdiff_size, arginfo_xdiff_file_bdiff_size)
	ZEND_FE(xdiff_string_bdiff_size, arginfo_xdiff_string_bdiff_size)
	ZEND_FE(xdiff_file_patch, arginfo_xdiff_file_patch)
	ZEND_FE(xdiff_string_patch, arginfo_xdiff_string_patch)
	ZEND_FALIAS(xdiff_file_patch_binary, xdiff_file_bpatch, arginfo_xdiff_file_patch_binary)
	ZEND_FALIAS(xdiff_string_patch_binary, xdiff_string_bpatch, arginfo_xdiff_string_patch_binary)
	ZEND_FE(xdiff_file_merge3, arginfo_xdiff_file_merge3)
	ZEND_FE(xdiff_string_merge3, arginfo_xdiff_string_merge3)
	ZEND_FE_END
};
