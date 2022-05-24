/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.1 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/3_1.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Marcin Gibula <mg@iceni.pl>                                  |
  +----------------------------------------------------------------------+

  $Id$
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xdiff.h"

#ifndef ZEND_ARG_INFO_WITH_DEFAULT_VALUE
#define ZEND_ARG_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, default_value) \
	ZEND_ARG_INFO(pass_by_ref, name)
#endif
#if PHP_VERSION_ID < 70200
#undef ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX
#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(name, return_reference, required_num_args, class_name, allow_null) \
	static const zend_internal_arg_info name[] = { \
		{ (const char*)(zend_uintptr_t)(required_num_args), ( #class_name ), 0, return_reference, allow_null, 0 },
#endif

#ifndef ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX
#define ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(name, return_reference, required_num_args, class_name, allow_null) \
	ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(name, return_reference, required_num_args, class_name, allow_null)
#endif

#ifndef ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE
#define ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value) \
	ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null)
#endif

#include "xdiff_arginfo.h"

#ifdef PHP_WIN32
/* libxdiff is compiled with /Zp1 */
# pragma pack(push, 1)
#endif
#include <xdiff.h>
#ifdef PHP_WIN32
# pragma pack(pop)
#endif

/* Not exported by header file */
extern char libxdiff_version[];

struct string_buffer {
	char *ptr;
	unsigned long size;
};

static int load_mm_file(const char *filepath, mmfile_t *dest);
static int load_into_mm_file(const char *buffer, unsigned long size, mmfile_t *dest);
static int append_string(void *ptr, mmbuffer_t *buffer, int array_size);
static int append_stream(void *ptr, mmbuffer_t *buffer, int array_size);
static int init_string(struct string_buffer *string);
static void free_string(struct string_buffer *string);

static int make_diff(char *filepath1, char *filepath2, xdemitcb_t *output, int context, int minimal);
static int make_diff_str(char *str1, int size1, char *str2, int size2,  xdemitcb_t *output, int context, int minimal);
static int make_bdiff(char *filepath1, char *filepath2, xdemitcb_t *output);
static int make_bdiff_str(char *str1, int size1, char *str2, int size2, xdemitcb_t *output);
static int make_patch(char *file_path, char *patch_path, xdemitcb_t *output, xdemitcb_t *error, int flags);
static int make_patch_str(char *file, int size1, char *patch, int size2, xdemitcb_t *output, xdemitcb_t *error, int flags);
static int make_bpatch(char *file_path, char *patch_path, xdemitcb_t *output);
static int make_bpatch_str(char *file, int size1, char *patch, int size2, xdemitcb_t *output);
static int make_merge3(char *filepath1, char *filepath2, char *filepath3, xdemitcb_t *output, xdemitcb_t *error);
static int make_merge3_str(char *content1, int size1, char *content2, int size2, char *content3, int size3, xdemitcb_t *output, xdemitcb_t *error);
static int make_rabdiff(char *filepath1, char *filepath2, xdemitcb_t *output);
static int make_rabdiff_str(char *str1, int size1, char *str2, int size2, xdemitcb_t *output);

static void *xdiff_malloc(void *foo, unsigned int size)
{
	return emalloc(size);
}

static void xdiff_free(void *foo, void *ptr)
{
	if (ptr) {
		efree(ptr);
	}
}

static void *xdiff_realloc(void *foo, void *ptr, unsigned int nsize)
{
	return erealloc(ptr, nsize);
}

static memallocator_t allocator = { NULL, xdiff_malloc, xdiff_free, xdiff_realloc };

/* {{{ xdiff_module_entry
 */
zend_module_entry xdiff_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"xdiff",
	ext_functions,
	PHP_MINIT(xdiff),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(xdiff),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_XDIFF_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XDIFF
ZEND_GET_MODULE(xdiff)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xdiff)
{
	xdl_set_allocator(&allocator);

	REGISTER_LONG_CONSTANT("XDIFF_PATCH_NORMAL", XDL_PATCH_NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XDIFF_PATCH_REVERSE", XDL_PATCH_REVERSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XDIFF_PATCH_IGNORESPACE", XDL_PATCH_IGNOREBSPACE, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xdiff)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "xdiff support", "enabled");
	php_info_print_table_row(2, "extension version", PHP_XDIFF_VERSION);
	php_info_print_table_row(2, "libxdiff version", libxdiff_version);
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto mixed xdiff_string_diff(string str1, string str2, [int context, [bool minimal]])
 */
PHP_FUNCTION(xdiff_string_diff)
{
	zend_string *str1, *str2;
	int retval;
	zend_bool minimal = 0;
	zend_long context = 3;
	xdemitcb_t output;
	struct string_buffer string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|lb", &str1, &str2, &context, &minimal) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	retval = init_string(&string);
	if (!retval)
		goto out;

	output.priv= &string;
	output.outf = append_string;

	make_diff_str(str1->val, str1->len, str2->val, str2->len, &output, context, minimal);
	RETVAL_STRINGL(string.ptr, string.size);
	free_string(&string);
out:
	return;
}
/* }}} */

/* {{{ proto bool xdiff_file_diff(string file1, string file2, string dest, [int context, [bool minimal]])
 */
PHP_FUNCTION(xdiff_file_diff)
{
	zend_string *filepath1, *filepath2, *dest;
	int retval;
	zend_bool minimal = 0;
	zend_long context = 3;
	xdemitcb_t output;
	php_stream *output_stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSS|lb", &filepath1, &filepath2, &dest, &context, &minimal) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	output_stream = php_stream_open_wrapper(dest->val, "wb", REPORT_ERRORS, NULL);
	if (!output_stream)
		goto out;

	output.priv = output_stream;
	output.outf = append_stream;

	retval = make_diff(filepath1->val, filepath2->val, &output, context, minimal);
	if (!retval)
		goto out_stream_close;

	RETVAL_TRUE;

out_stream_close:
	php_stream_close(output_stream);
out:
	return;
}
/* }}} */

/* {{{ proto mixed xdiff_string_diff_binary(string str1, string str2)
 */
PHP_FUNCTION(xdiff_string_bdiff)
{
	zend_string *str1, *str2;
	int retval;
	xdemitcb_t output;
	struct string_buffer string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &str1, &str2) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	retval = init_string(&string);
	if (!retval)
		goto out;

	output.priv= &string;
	output.outf = append_string;

	make_bdiff_str(str1->val, str1->len, str2->val, str2->len, &output);
	RETVAL_STRINGL(string.ptr, string.size);
	free_string(&string);

out:
	return;
}
/* }}} */

/* {{{ proto bool xdiff_file_diff_binary(string file1, string file2, string dest)
 */
PHP_FUNCTION(xdiff_file_bdiff)
{
	zend_string *filepath1, *filepath2, *result;
	int retval;
	xdemitcb_t output;
	php_stream *output_stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSS", &filepath1, &filepath2, &result) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	output_stream = php_stream_open_wrapper(result->val, "wb", REPORT_ERRORS, NULL);
	if (!output_stream)
		goto out;

	output.priv = output_stream;
	output.outf = append_stream;

	retval = make_bdiff(filepath1->val, filepath2->val, &output);
	if (!retval)
		goto out_stream_close;

	RETVAL_TRUE;

out_stream_close:
	php_stream_close(output_stream);
out:
	return;
}
/* }}} */

/* {{{ proto mixed xdiff_string_rabdiff(string str1, string str2)
 */
PHP_FUNCTION(xdiff_string_rabdiff)
{
	zend_string *str1, *str2;
	int retval;
	xdemitcb_t output;
	struct string_buffer string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &str1, &str2) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	retval = init_string(&string);
	if (!retval)
		goto out;

	output.priv= &string;
	output.outf = append_string;

	make_rabdiff_str(str1->val, str1->len, str2->val, str2->len, &output);
	RETVAL_STRINGL(string.ptr, string.size);
	free_string(&string);

out:
	return;
}
/* }}} */

/* {{{ proto bool xdiff_file_rabdiff(string file1, string file2, string dest)
 */
PHP_FUNCTION(xdiff_file_rabdiff)
{
	zend_string *filepath1, *filepath2, *result;
	int retval;
	xdemitcb_t output;
	php_stream *output_stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSS", &filepath1, &filepath2, &result) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	output_stream = php_stream_open_wrapper(result->val, "wb", REPORT_ERRORS, NULL);
	if (!output_stream)
		goto out;

	output.priv = output_stream;
	output.outf = append_stream;

	retval = make_rabdiff(filepath1->val, filepath2->val, &output);
	if (!retval)
		goto out_stream_close;

	RETVAL_TRUE;

out_stream_close:
	php_stream_close(output_stream);
out:
	return;
}
/* }}} */

/* {{{ proto bool xdiff_file_bdiff_size(string file1, string file2, string dest)
 */
PHP_FUNCTION(xdiff_file_bdiff_size)
{
	zend_string *filepath;
	int retval;
	long result;
	mmfile_t file;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &filepath) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	retval = load_mm_file(filepath->val, &file);
	if (!retval)
		goto out;

	result = xdl_bdiff_tgsize(&file);
	if (result < 0)
		goto out_free_mmfile;

	RETVAL_LONG(result);

out_free_mmfile:
	xdl_free_mmfile(&file);
out:
	return;
}
/* }}} */

/* {{{ proto bool xdiff_string_bdiff_size(string file1, string file2, string dest)
 */
PHP_FUNCTION(xdiff_string_bdiff_size)
{
	zend_string *patch;
	int retval;
	long result;
	mmfile_t file;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &patch) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	retval = load_into_mm_file(patch->val, patch->len, &file);
	if (!retval)
		goto out;

	result = xdl_bdiff_tgsize(&file);
	if (result < 0)
		goto out_free_mmfile;

	RETVAL_LONG(result);

out_free_mmfile:
	xdl_free_mmfile(&file);
out:
	return;
}
/* }}} */

/* {{{ proto mixed xdiff_file_patch(string file, string patch, string dest [, int flags])
 */
PHP_FUNCTION(xdiff_file_patch)
{
	php_stream *output_stream;
	zend_string *src_path, *patch_path, *dest_path;
	int retval;
	zend_long flags = XDL_PATCH_NORMAL;	/* DIFF_PATCH_NORMAL */
	xdemitcb_t output, error_output;
	struct string_buffer error_string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSS|l", &src_path, &patch_path, &dest_path, &flags) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	output_stream = php_stream_open_wrapper(dest_path->val, "wb", REPORT_ERRORS, NULL);
	if (!output_stream)
		goto out;

	output.outf = append_stream;
	output.priv = output_stream;

	retval = init_string(&error_string);
	if (!retval)
		goto out_stream_close;

	error_output.priv= &error_string;
	error_output.outf = append_string;

	retval = make_patch(src_path->val, patch_path->val, &output, &error_output, flags);
	if (retval < 0)
		goto out_free_string;

	if (error_string.size > 0) {
		RETVAL_STRINGL(error_string.ptr, error_string.size);
	} else {
		RETVAL_TRUE;
	}

out_free_string:
	free_string(&error_string);
out_stream_close:
	php_stream_close(output_stream);
out:
	return;
}
/* }}} */

/* {{{ proto string xdiff_string_patch(string file, string patch [, int flags, [string error]])
 */
PHP_FUNCTION(xdiff_string_patch)
{
	zval *error_ref = NULL;
	zend_string *src, *patch;
	int retval;
	zend_long flags = XDL_PATCH_NORMAL;	/* DIFF_PATCH_NORMAL */
	xdemitcb_t output, error_output;
	struct string_buffer output_string, error_string;

	if (zend_parse_parameters_ex(0, ZEND_NUM_ARGS(), "SS|lz", &src, &patch, &flags, &error_ref) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	retval = init_string(&output_string);
	if (!retval)
		goto out;

	output.priv = &output_string;
	output.outf = append_string;

	retval = init_string(&error_string);
	if (!retval)
		goto out_free_output_string;

	error_output.priv= &error_string;
	error_output.outf = append_string;

	retval = make_patch_str(src->val, src->len, patch->val, patch->len, &output, &error_output, flags);
	if (retval < 0)
		goto out_free_error_string;

	if (error_string.size > 0 && error_ref) {
		ZVAL_DEREF(error_ref);
		ZVAL_STRINGL(error_ref, error_string.ptr, error_string.size);
	}

	if (output_string.size > 0) {
		RETVAL_STRINGL(output_string.ptr, output_string.size);
	} else {
		RETVAL_EMPTY_STRING();
	}

out_free_error_string:
	free_string(&error_string);
out_free_output_string:
	free_string(&output_string);
out:
	return;
}
/* }}} */

/* {{{ proto bool xdiff_file_patch_binary(string file, string patch, string dest)
 */
PHP_FUNCTION(xdiff_file_bpatch)
{
	php_stream *output_stream;
	zend_string *src_path, *patch_path, *dest_path;
	int retval;
	xdemitcb_t output;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSS", &src_path, &patch_path, &dest_path) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	output_stream = php_stream_open_wrapper(dest_path->val, "wb", REPORT_ERRORS, NULL);
	if (!output_stream)
		goto out;

	output.outf = append_stream;
	output.priv = output_stream;

	retval = make_bpatch(src_path->val, patch_path->val, &output);
	php_stream_close(output_stream);

	if (retval == 0)
		RETVAL_TRUE;

out:
	return;
}
/* }}} */

/* {{{ proto string xdiff_string_patch_binary(string str, string patch)
 */
PHP_FUNCTION(xdiff_string_bpatch)
{
	zend_string *src, *patch;
	int retval;
	xdemitcb_t output;
	struct string_buffer output_string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &src, &patch) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	retval = init_string(&output_string);
	if (!retval)
		goto out;

	output.priv = &output_string;
	output.outf = append_string;

	retval = make_bpatch_str(src->val, src->len, patch->val, patch->len, &output);
	if (retval < 0)
		goto out_free_string;

	RETVAL_STRINGL(output_string.ptr, output_string.size);

out_free_string:
	free_string(&output_string);
out:
	return;
}
/* }}} */

/* {{{ proto mixed xdiff_file_merge3(string file1, string file2, string file3, string dest)
 */
PHP_FUNCTION(xdiff_file_merge3)
{
	zend_string *file1, *file2, *file3, *dest;
	php_stream *output_stream;
	struct string_buffer string;
	xdemitcb_t output, error_output;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSSS", &file1, &file2, &file3, &dest) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	output_stream = php_stream_open_wrapper(dest->val, "wb", REPORT_ERRORS, NULL);
	if (!output_stream)
		goto out;

	output.priv = output_stream;
	output.outf = append_stream;

	retval = init_string(&string);
	if (!retval)
		goto out_stream_close;

	error_output.priv = &string;
	error_output.outf = append_string;

	retval = make_merge3(file1->val, file2->val, file3->val, &output, &error_output);
	if (!retval)
		goto out_free_string;

	if (string.size > 0) {
		RETVAL_STRINGL(string.ptr, string.size);
	} else {
		RETVAL_TRUE;
	}

out_free_string:
	free_string(&string);
out_stream_close:
	php_stream_close(output_stream);
out:
	return;
}
/* }}} */

/* {{{ proto string xdiff_string_merge3(string str1, string str2, string str3 [, string error])
 */
PHP_FUNCTION(xdiff_string_merge3)
{
	zval *error_ref = NULL;
	zend_string *file1, *file2, *file3;
	struct string_buffer output_string, error_string;
	xdemitcb_t output, error_output;
	int retval;

	if (zend_parse_parameters_ex(0, ZEND_NUM_ARGS(), "SSS|z", &file1,  &file2, &file3, &error_ref) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	retval = init_string(&output_string);
	if (!retval)
		goto out;

	output.priv = &output_string;
	output.outf = append_string;

	retval = init_string(&error_string);
	if (!retval)
		goto out_free_output_string;

	error_output.priv = &error_string;
	error_output.outf = append_string;

	retval = make_merge3_str(file1->val, file1->len, file2->val, file2->len, file3->val, file3->len, &output, &error_output);
	if (!retval)
		goto out_free_error_string;

	if (error_string.size > 0 && error_ref) {
		ZVAL_DEREF(error_ref);
		ZVAL_STRINGL(error_ref, error_string.ptr, error_string.size);
	}

	if (output_string.size > 0) {
		RETVAL_STRINGL(output_string.ptr, output_string.size);
	} else {
		RETVAL_TRUE;
	}

out_free_error_string:
	free_string(&error_string);
out_free_output_string:
	free_string(&output_string);
out:
	return;
}
/* }}} */

static int load_mm_file(const char *filepath, mmfile_t *dest)
{
	int retval;
	off_t filesize;
	void *ptr;
	php_stream *src;
	php_stream_statbuf stat;

	src = php_stream_open_wrapper((char *) filepath, "rb", REPORT_ERRORS, NULL);
	if (!src)
		goto out;

	retval = php_stream_stat(src, &stat);
	if (retval < 0)
		goto out_stream_close;

	filesize = stat.sb.st_size;

	retval = xdl_init_mmfile(dest, filesize, XDL_MMF_ATOMIC);
	if (retval < 0)
		goto out_stream_close;

	ptr = xdl_mmfile_writeallocate(dest, (long) filesize);
	if (!ptr)
		goto out_free_mmfile;

	php_stream_read(src, ptr, filesize);
	php_stream_close(src);

	return 1;

out_free_mmfile:
	xdl_free_mmfile(dest);
out_stream_close:
	php_stream_close(src);
out:
	return 0;
}

static int load_into_mm_file(const char *buffer, unsigned long size, mmfile_t *dest)
{
	int retval;
	void *ptr;

	retval = xdl_init_mmfile(dest, size, XDL_MMF_ATOMIC);
	if (retval < 0)
		goto out;

	ptr = xdl_mmfile_writeallocate(dest, (long) size);
	if (!ptr)
		goto out_free_mmfile;

	memcpy(ptr, buffer, size);
	return 1;

out_free_mmfile:
	xdl_free_mmfile(dest);
out:
	return 0;
}

static int append_string(void *ptr, mmbuffer_t *buffer, int array_size)
{
	struct string_buffer *string = ptr;
	void *new_ptr;
	unsigned int i;

	for (i = 0; i < array_size; i++) {
		new_ptr = erealloc(string->ptr, string->size + buffer[i].size + 1);
		if (!new_ptr) {
			efree(string->ptr);
			return -1;
		}

		string->ptr = new_ptr;
		memcpy(string->ptr + string->size, buffer[i].ptr, buffer[i].size);
		string->size += buffer[i].size;
	}
	if (array_size) {
		string->ptr[string->size] = '\0';
	}

	return 0;
}

static int append_stream(void *ptr, mmbuffer_t *buffer, int array_size)
{
	php_stream *stream = ptr;
	unsigned int i;

	for (i = 0; i < array_size; i++) {
		php_stream_write(stream, buffer[i].ptr, buffer[i].size);
	}

	return 1;
}

static int init_string(struct string_buffer *string)
{
	string->ptr = emalloc(1);
	if (!string->ptr)
		return 0;

	string->size = 0;
	memset(string->ptr, 0, 1);

	return 1;
}

static void free_string(struct string_buffer *string)
{
	if (string->ptr)
		efree(string->ptr);
}

static int make_diff(char *filepath1, char *filepath2, xdemitcb_t *output, int context, int minimal)
{
	mmfile_t file1, file2;
	xpparam_t params;
	xdemitconf_t conf;
	int retval, result = 0;

	retval = load_mm_file(filepath1, &file1);
	if (!retval)
		goto out;

	retval = load_mm_file(filepath2, &file2);
	if (!retval)
		goto out_free_mmfile;

	params.flags = (minimal) ? XDF_NEED_MINIMAL : 0;
	conf.ctxlen = abs(context);

	retval = xdl_diff(&file1, &file2, &params, &conf, output);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&file2);
out_free_mmfile:
	xdl_free_mmfile(&file1);
out:
	return result;
}

static int make_diff_str(char *str1, int size1, char *str2, int size2, xdemitcb_t *output, int context, int minimal)
{
	mmfile_t file1, file2;
	xpparam_t params;
	xdemitconf_t conf;
	int retval, result = 0;

	retval = load_into_mm_file(str1, size1, &file1);
	if (!retval)
		goto out;

	retval = load_into_mm_file(str2, size2, &file2);
	if (!retval)
		goto out_free_mmfile;

	params.flags = (minimal) ? XDF_NEED_MINIMAL : 0;
	conf.ctxlen = abs(context);

	retval = xdl_diff(&file1, &file2, &params, &conf, output);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&file2);
out_free_mmfile:
	xdl_free_mmfile(&file1);
out:
	return result;
}

static int make_bdiff(char *filepath1, char *filepath2, xdemitcb_t *output)
{
	mmfile_t file1, file2;
	bdiffparam_t params;
	int retval, result = 0;

	retval = load_mm_file(filepath1, &file1);
	if (!retval)
		goto out;

	retval = load_mm_file(filepath2, &file2);
	if (!retval)
		goto out_free_mmfile;

	params.bsize = 16;

	retval = xdl_bdiff(&file1, &file2, &params, output);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&file2);
out_free_mmfile:
	xdl_free_mmfile(&file1);
out:
	return result;
}

static int make_bdiff_str(char *str1, int size1, char *str2, int size2, xdemitcb_t *output)
{
	mmfile_t file1, file2;
	bdiffparam_t params;
	int retval, result = 0;

	retval = load_into_mm_file(str1, size1, &file1);
	if (!retval)
		goto out;

	retval = load_into_mm_file(str2, size2, &file2);
	if (!retval)
		goto out_free_mmfile;

	params.bsize = 16;

	retval = xdl_bdiff(&file1, &file2, &params, output);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&file2);
out_free_mmfile:
	xdl_free_mmfile(&file1);
out:
	return result;
}

static int make_rabdiff(char *filepath1, char *filepath2, xdemitcb_t *output)
{
	mmfile_t file1, file2;
	int retval, result = 0;

	retval = load_mm_file(filepath1, &file1);
	if (!retval)
		goto out;

	retval = load_mm_file(filepath2, &file2);
	if (!retval)
		goto out_free_mmfile;

	retval = xdl_rabdiff(&file1, &file2, output);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&file2);
out_free_mmfile:
	xdl_free_mmfile(&file1);
out:
	return result;
}

static int make_rabdiff_str(char *str1, int size1, char *str2, int size2, xdemitcb_t *output)
{
	mmfile_t file1, file2;
	int retval, result = 0;

	retval = load_into_mm_file(str1, size1, &file1);
	if (!retval)
		goto out;

	retval = load_into_mm_file(str2, size2, &file2);
	if (!retval)
		goto out_free_mmfile;

	retval = xdl_rabdiff(&file1, &file2, output);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&file2);
out_free_mmfile:
	xdl_free_mmfile(&file1);
out:
	return result;
}

static int make_patch(char *file_path, char *patch_path, xdemitcb_t *output, xdemitcb_t *error, int flags)
{
	mmfile_t file, patch;
	int retval, result = 0;

	retval = load_mm_file(file_path, &file);
	if (!retval)
		goto out;

	retval = load_mm_file(patch_path, &patch);
	if (!retval)
		goto out_free_mmfile;

	retval = xdl_patch(&file, &patch, flags, output, error);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&patch);
out_free_mmfile:
	xdl_free_mmfile(&file);
out:
	return result;
}

static int make_patch_str(char *file, int size1, char *patch, int size2, xdemitcb_t *output, xdemitcb_t *error, int flags)
{
	mmfile_t file_mm, patch_mm;
	int retval, result = 0;

	retval = load_into_mm_file(file, size1, &file_mm);
	if (!retval)
		goto out;

	retval = load_into_mm_file(patch, size2, &patch_mm);
	if (!retval)
		goto out_free_mmfile;

	retval = xdl_patch(&file_mm, &patch_mm, flags, output, error);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&patch_mm);
out_free_mmfile:
	xdl_free_mmfile(&file_mm);
out:
	return result;
}

static int make_bpatch(char *file_path, char *patch_path, xdemitcb_t *output)
{
	mmfile_t file_mm, patch_mm;
	int retval, result = 0;

	retval = load_mm_file(file_path, &file_mm);
	if (!retval)
		goto out;

	retval = load_mm_file(patch_path, &patch_mm);
	if (!retval)
		goto out_free_mmfile;

	retval = xdl_bpatch(&file_mm, &patch_mm, output);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&patch_mm);
out_free_mmfile:
	xdl_free_mmfile(&file_mm);
out:
	return result;
}

static int make_bpatch_str(char *file, int size1, char *patch, int size2, xdemitcb_t *output)
{
	mmfile_t file_mm, patch_mm;
	int retval, result = 0;

	retval = load_into_mm_file(file, size1, &file_mm);
	if (!retval)
		goto out;

	retval = load_into_mm_file(patch, size2, &patch_mm);
	if (!retval)
		goto out_free_mmfile;

	retval = xdl_bpatch(&file_mm, &patch_mm, output);
	if (retval < 0)
		goto out_free_mmfile2;

	result = 1;

out_free_mmfile2:
	xdl_free_mmfile(&patch_mm);
out_free_mmfile:
	xdl_free_mmfile(&file_mm);
out:
	return result;
}

static int make_merge3(char *filepath1, char *filepath2, char *filepath3, xdemitcb_t *output, xdemitcb_t *error)
{
	mmfile_t file1, file2, file3;
	int retval, result = 0;

	retval = load_mm_file(filepath1, &file1);
	if (!retval)
		goto out;

	retval = load_mm_file(filepath2, &file2);
	if (!retval)
		goto out_free_mmfile;

	retval = load_mm_file(filepath3, &file3);
	if (!retval)
		goto out_free_mmfile2;

	retval = xdl_merge3(&file1, &file2, &file3, output, error);
	if (retval < 0)
		goto out_free_mmfile3;

	result = 1;

out_free_mmfile3:
	xdl_free_mmfile(&file3);
out_free_mmfile2:
	xdl_free_mmfile(&file2);
out_free_mmfile:
	xdl_free_mmfile(&file1);
out:
	return result;
}

static int make_merge3_str(char *content1, int size1, char *content2, int size2, char *content3, int size3, xdemitcb_t *output, xdemitcb_t *error)
{
	mmfile_t file1, file2, file3;
	int retval, result = 0;

	retval = load_into_mm_file(content1, size1, &file1);
	if (!retval)
		goto out;

	retval = load_into_mm_file(content2, size2, &file2);
	if (!retval)
		goto out_free_mmfile;

	retval = load_into_mm_file(content3, size3, &file3);
	if (!retval)
		goto out_free_mmfile2;

	retval = xdl_merge3(&file1, &file2, &file3, output, error);
	if (retval < 0)
		goto out_free_mmfile3;

	result = 1;

out_free_mmfile3:
	xdl_free_mmfile(&file3);
out_free_mmfile2:
	xdl_free_mmfile(&file2);
out_free_mmfile:
	xdl_free_mmfile(&file1);
out:
	return result;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
