/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
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

#include <xdiff.h>

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

/* {{{ xdiff_functions[]
 *
 * Every user visible function must have an entry in xdiff_functions[].
 */
function_entry xdiff_functions[] = {
	PHP_FE(xdiff_file_diff,			NULL)
	PHP_FE(xdiff_file_diff_binary,		NULL)
	PHP_FE(xdiff_file_patch,		NULL)
	PHP_FE(xdiff_file_patch_binary,		NULL)
	PHP_FE(xdiff_file_merge3,		NULL)
	PHP_FE(xdiff_string_diff,		NULL)
	PHP_FE(xdiff_string_diff_binary,	NULL)
	PHP_FE(xdiff_string_patch,		NULL)
	PHP_FE(xdiff_string_patch_binary,	NULL)
	PHP_FE(xdiff_string_merge3,		NULL)	
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ xdiff_module_entry
 */
zend_module_entry xdiff_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"xdiff",
	xdiff_functions,
	PHP_MINIT(xdiff),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(xdiff),
#if ZEND_MODULE_API_NO >= 20010901
	"0.2", /* Replace with version number for your extension */
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
	REGISTER_LONG_CONSTANT("XDIFF_PATCH_NORMAL", XDL_PATCH_NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XDIFF_PATCH_REVERSE", XDL_PATCH_REVERSE, CONST_CS | CONST_PERSISTENT);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xdiff)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "xdiff support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto mixed xdiff_string_diff(string str1, string str2, [int context, [bool minimal]])
 */
PHP_FUNCTION(xdiff_string_diff)
{
	char *str1, *str2;
	int size1, size2, retval, context = 3, minimal = 0;
	xdemitcb_t output;
	struct string_buffer string;
	
	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 4 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|lb", &str1, &size1, &str2, &size2, &context, &minimal) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	retval = init_string(&string);
	if (!retval) {
		RETURN_FALSE;
	}

	output.priv= &string;
	output.outf = append_string;
	
	retval = make_diff_str(str1, size1, str2, size2, &output, context, minimal);
	if (!retval) {
		free_string(&string);
		RETURN_FALSE;
	}
	
	RETVAL_STRINGL(string.ptr, string.size, 1);
	free_string(&string);
}
/* }}} */

/* {{{ proto bool xdiff_file_diff(string file1, string file2, string dest, [int context, [bool minimal]])
 */
PHP_FUNCTION(xdiff_file_diff)
{
	char *filepath1, *filepath2, *dest;
	int size, retval, context = 3, minimal = 0;
	xdemitcb_t output;
	php_stream *output_stream;
	
	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 5 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|lb", &filepath1, &size, &filepath2, &size, &dest, &size, &context, &minimal) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	output_stream = php_stream_open_wrapper(dest, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		RETURN_FALSE;
	}
	
	output.priv = output_stream;
	output.outf = append_stream;
	
	retval = make_diff(filepath1, filepath2, &output, context, minimal);
	if (!retval) {
		php_close_stream(output_stream);
		RETURN_FALSE;
	}

	RETURN_TRUE;	
}
/* }}} */
 
/* {{{ proto mixed xdiff_string_diff_binary(string str1, string str2)
 */
PHP_FUNCTION(xdiff_string_diff_binary)
{
	char *str1, *str2;
	int size1, size2, retval;
	xdemitcb_t output;
	struct string_buffer string;
	
	if (ZEND_NUM_ARGS() != 2 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &str1, &size1, &str2, &size2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
		
	retval = init_string(&string);
	if (!retval) {
		RETURN_FALSE;
	}
	
	output.priv= &string;
	output.outf = append_string;
	
	retval = make_bdiff_str(str1, size1, str2, size2, &output);
	if (!retval) {
		free_string(&string);
		RETURN_FALSE;
	}
	
	RETVAL_STRINGL(string.ptr, string.size, 1);
	free_string(&string);
}
/* }}} */

/* {{{ proto bool xdiff_file_diff_binary(string file1, string file2, string dest)
 */
PHP_FUNCTION(xdiff_file_diff_binary)
{
	char *filepath1, *filepath2, *result;
	int size, retval;
	xdemitcb_t output;
	php_stream *output_stream;
	
	if (ZEND_NUM_ARGS() != 3 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &filepath1, &size, &filepath2, &size, &result, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	output_stream = php_stream_open_wrapper(result, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		RETURN_FALSE;
	}
	
	output.priv = output_stream;
	output.outf = append_stream;
	
	retval = make_bdiff(filepath1, filepath2, &output);
	if (!retval) {
		php_close_stream(output_stream);
		RETURN_FALSE;
	}

	RETURN_TRUE;	
}
/* }}} */

/* {{{ proto mixed xdiff_file_patch(string file, string patch, string dest [, int flags])
 */
PHP_FUNCTION(xdiff_file_patch)
{
	php_stream *output_stream;
	char *src_path, *patch_path, *dest_path;
	int retval, size, flags = XDL_PATCH_NORMAL;	/* DIFF_PATCH_NORMAL */
	xdemitcb_t output, error_output;
	struct string_buffer error_string;
	
	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 4 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|l", &src_path, &size, &patch_path, &size, &dest_path, &size, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	output_stream = php_stream_open_wrapper(dest_path, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		RETURN_FALSE;
	}

	output.outf = append_stream;
	output.priv = output_stream;
	
	retval = init_string(&error_string);	
	if (!retval) {
		php_stream_close(output_stream);
		RETURN_FALSE;
	}

	error_output.priv= &error_string;
	error_output.outf = append_string;
	
	retval = make_patch(src_path, patch_path, &output, &error_output, flags);
	php_stream_close(output_stream);
	
	if (retval < 0) {
		free_string(&error_string);
		RETURN_FALSE;
	}
	
	if (error_string.size > 0) {
		RETVAL_STRINGL(error_string.ptr, error_string.size, 1);
		free_string(&error_string);
	} else {
		free_string(&error_string);
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string xdiff_string_patch(string file, string patch [, int flags, [string error]])
 */
PHP_FUNCTION(xdiff_string_patch)
{
	zval *error_ref = NULL;
	php_stream *output_stream;
	char *src, *patch;
	int retval, size1, size2, flags = XDL_PATCH_NORMAL;	/* DIFF_PATCH_NORMAL */
	xdemitcb_t output, error_output;
	struct string_buffer output_string, error_string;
	
	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 4 || zend_parse_parameters_ex(0, ZEND_NUM_ARGS() TSRMLS_CC, "ss|lz", &src, &size1, &patch, &size2, &flags, &error_ref) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	retval = init_string(&output_string);
	if (!retval) {
		RETURN_FALSE;
	}

	output.priv = &output_string;
	output.outf = append_string;

	retval = init_string(&error_string);
	if (!retval) {
		free_string(&output_string);
		RETURN_FALSE;
	}
	
	error_output.priv= &error_string;
	error_output.outf = append_string;

	retval = make_patch_str(src, size1, patch, size2, &output, &error_output, flags);	
	if (retval < 0) {
		free_string(&error_string);
		free_string(&output_string);
		RETURN_FALSE;
	}
	
	if (error_string.size > 0 && error_ref) {
		ZVAL_STRINGL(error_ref, error_string.ptr, error_string.size, 1);	
	}
	
	if (output_string.size > 0) {
		RETVAL_STRINGL(output_string.ptr, output_string.size, 1);
		free_string(&output_string);
		free_string(&error_string);
	} else {
		free_string(&output_string);
		free_string(&error_string);
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto bool xdiff_file_patch_binary(string file, string patch, string dest)
 */
PHP_FUNCTION(xdiff_file_patch_binary)
{
	php_stream *output_stream;
	char *src_path, *patch_path, *dest_path;
	int retval, size;
	xdemitcb_t output;
	
	if (ZEND_NUM_ARGS() != 3 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &src_path, &size, &patch_path, &size, &dest_path, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	output_stream = php_stream_open_wrapper(dest_path, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		RETURN_FALSE;
	}

	output.outf = append_stream;
	output.priv = output_stream;
	
	retval = make_bpatch(src_path, patch_path, &output);
	php_stream_close(output_stream);
	
	if (retval < 0) {
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string xdiff_string_patch_binary(string file, string patch)
 */
PHP_FUNCTION(xdiff_string_patch_binary)
{
	char *src, *patch;
	int retval, size1, size2;
	xdemitcb_t output;
	struct string_buffer output_string;
	
	if (ZEND_NUM_ARGS() != 2 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &src, &size1, &patch, &size2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	retval = init_string(&output_string);
	if (!retval) {
		RETURN_FALSE;
	}

	output.priv = &output_string;
	output.outf = append_string;

	retval = make_bpatch_str(src, size1, patch, size2, &output);
	if (retval < 0) {
		free_string(&output_string);
		RETURN_FALSE;
	}
	
	RETVAL_STRINGL(output_string.ptr, output_string.size, 1);
	free_string(&output_string);
}
/* }}} */

/* {{{ proto string xdiff_file_merge3(string file1, string file2, string file3, string dest)
 */
PHP_FUNCTION(xdiff_file_merge3)
{
	char *file1, *file2, *file3, *dest;
	php_stream *output_stream;
	struct string_buffer string;
	xdemitcb_t output, error_output;
	int retval, size;
	
	if (ZEND_NUM_ARGS() != 4 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss", &file1, &size, &file2, &size, &file3, &size, &dest, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	output_stream = php_stream_open_wrapper(dest, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		RETURN_FALSE;	
	}
	
	output.priv = output_stream;
	output.outf = append_stream;
	
	retval = init_string(&string);	
	if (!retval) {
		php_stream_close(output_stream);
		RETURN_FALSE;
	}
	
	error_output.priv = &string;
	error_output.outf = append_string;
	
	retval = make_merge3(file1, file2, file3, &output, &error_output);
	php_stream_close(output_stream);
	
	if (!retval) {
		free_string(&string);
		RETURN_FALSE;
	}
	
	if (string.size > 0) {
		RETVAL_STRINGL(string.ptr, string.size, 1);
		free_string(&string);
	} else {
		free_string(&string);
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string xdiff_string_merge3(string file1, string file2, string file3 [, string error])
 */
PHP_FUNCTION(xdiff_string_merge3)
{
	zval *error_ref = NULL;
	char *file1, *file2, *file3;
	struct string_buffer output_string, error_string;
	xdemitcb_t output, error_output;
	int retval, size1, size2, size3;
	
	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 4 || zend_parse_parameters_ex(0, ZEND_NUM_ARGS() TSRMLS_CC, "sss|z", &file1, &size1, &file2, &size2, &file3, &size3, &error_ref) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	retval = init_string(&output_string);
	if (!retval) {
		RETURN_FALSE;	
	}
	
	output.priv = &output_string;
	output.outf = append_string;
	
	retval = init_string(&error_string);	
	if (!retval) {
		free_string(&output_string);
		RETURN_FALSE;
	}
	
	error_output.priv = &error_string;
	error_output.outf = append_string;
	
	retval = make_merge3_str(file1, size1, file2, size2, file3, size3, &output, &error_output);

	if (!retval) {
		free_string(&output_string);
		free_string(&error_string);
		RETURN_FALSE;
	}
	
	if (error_string.size > 0 && error_ref) {
		ZVAL_STRINGL(error_ref, error_string.ptr, error_string.size, 1);	
	}
	
	if (output_string.size > 0) {
		RETVAL_STRINGL(output_string.ptr, output_string.size, 1);
		free_string(&output_string);
		free_string(&error_string);
	} else {
		free_string(&output_string);
		free_string(&error_string);
		RETURN_TRUE;
	}
}
/* }}} */

static int load_mm_file(const char *filepath, mmfile_t *dest)
{
	int retval;
	off_t filesize;
	void *ptr;
	php_stream *src;
	php_stream_statbuf stat;
		
	src = php_stream_open_wrapper((char *) filepath, "rb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!src) {
		return 0;
	}
	
	retval = php_stream_stat(src, &stat);
	if (retval < 0) {
		php_stream_close(src);
		return 0;
	}
	
	filesize = stat.sb.st_size;
	
	retval = xdl_init_mmfile(dest, filesize, XDL_MMF_ATOMIC);
	if (retval < 0) {
		php_stream_close(src);
		return 0;
	}
	
	ptr = xdl_mmfile_writeallocate(dest, (long) filesize);
	if (!ptr) {
		xdl_free_mmfile(dest);
		php_stream_close(src);
		return 0;
	}
	
	php_stream_read(src, ptr, filesize);
	php_stream_close(src);
	
	return 1;
}

static int load_into_mm_file(const char *buffer, unsigned long size, mmfile_t *dest)
{
	int retval;
	void *ptr;
	
	retval = xdl_init_mmfile(dest, size, XDL_MMF_ATOMIC);
	if (retval < 0) {
		return 0;
	}
	
	ptr = xdl_mmfile_writeallocate(dest, (long) size);
	if (!ptr) {
		xdl_free_mmfile(dest);
		return 0;
	}
	
	memcpy(ptr, buffer, size);
	return 1;
}

static int append_string(void *ptr, mmbuffer_t *buffer, int array_size)
{
	struct string_buffer *string = ptr;
	unsigned int i;
	
	for (i = 0; i < array_size; i++) {
		string->ptr = erealloc(string->ptr, string->size + buffer[i].size + 1);
		memcpy(string->ptr + string->size, buffer[i].ptr, buffer[i].size);
		string->size += buffer[i].size;
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
}

static int init_string(struct string_buffer *string)
{
	string->size = 0;
	string->ptr = emalloc(1);
	if (!string->ptr) {
		return 0;
	}
	
	memset(string->ptr, 0, 1);
	return 1;
}

static void free_string(struct string_buffer *string)
{
	efree(string->ptr);
}

static int make_diff(char *filepath1, char *filepath2, xdemitcb_t *output, int context, int minimal)
{
	mmfile_t file1, file2;
	xpparam_t params;
	xdemitconf_t conf;
	int retval;
	
	retval = load_mm_file(filepath1, &file1);
	if (!retval) {
		return 0;
	}
	
	retval = load_mm_file(filepath2, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		return 0;
	}

	params.flags = (minimal) ? XDF_NEED_MINIMAL : 0;
	conf.ctxlen = abs(context);
		
	retval = xdl_diff(&file1, &file2, &params, &conf, output);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	
	if (retval < 0) {
		return 0;
	}
	
	return 1;
}

static int make_diff_str(char *str1, int size1, char *str2, int size2, xdemitcb_t *output, int context, int minimal)
{
	mmfile_t file1, file2;
	xpparam_t params;
	xdemitconf_t conf;
	int retval;
	
	retval = load_into_mm_file(str1, size1, &file1);
	if (!retval) {
		return 0;
	}
	
	retval = load_into_mm_file(str2, size2, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		return 0;
	}

	params.flags = (minimal) ? XDF_NEED_MINIMAL : 0;
	conf.ctxlen = abs(context);
		
	retval = xdl_diff(&file1, &file2, &params, &conf, output);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	
	if (retval < 0) {
		return 0;
	}
	
	return 1;
}

static int make_bdiff(char *filepath1, char *filepath2, xdemitcb_t *output)
{
	mmfile_t file1, file2;
	bdiffparam_t params;
	int retval;
	
	retval = load_mm_file(filepath1, &file1);
	if (!retval) {
		return 0;
	}
	
	retval = load_mm_file(filepath2, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		return 0;
	}

	params.bsize = 16;

	retval = xdl_bdiff(&file1, &file2, &params, output);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	
	if (retval < 0) {
		return 0;
	}
	
	return 1;
}

static int make_bdiff_str(char *str1, int size1, char *str2, int size2, xdemitcb_t *output)
{
	mmfile_t file1, file2;
	bdiffparam_t params;
	int retval;
	
	retval = load_into_mm_file(str1, size1, &file1);
	if (!retval) {
		return 0;
	}
	
	retval = load_into_mm_file(str2, size2, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		return 0;
	}

	params.bsize = 16;

	retval = xdl_bdiff(&file1, &file2, &params, output);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	
	if (retval < 0) {
		return 0;
	}
	
	return 1;
}

static int make_patch(char *file_path, char *patch_path, xdemitcb_t *output, xdemitcb_t *error, int flags)
{
	int retval;
	mmfile_t file, patch;
	
	retval = load_mm_file(file_path, &file);
	if (!retval) {
		return 0;
	}
	
	retval = load_mm_file(patch_path, &patch);
	if (!retval) {
		xdl_free_mmfile(&file);
		return 0;
	}
		
	retval = xdl_patch(&file, &patch, flags, output, error); 
	xdl_free_mmfile(&file);
	xdl_free_mmfile(&patch);
	
	if (retval < 0) {
		return 0;
	}
	
	return 1;
}

static int make_patch_str(char *file, int size1, char *patch, int size2, xdemitcb_t *output, xdemitcb_t *error, int flags)
{
	int retval;
	mmfile_t file_mm, patch_mm;
	
	retval = load_into_mm_file(file, size1, &file_mm);
	if (!retval) {
		return 0;
	}
	
	retval = load_into_mm_file(patch, size2, &patch_mm);
	if (!retval) {
		xdl_free_mmfile(&file_mm);
		return 0;
	}
		
	retval = xdl_patch(&file_mm, &patch_mm, flags, output, error); 
	xdl_free_mmfile(&file_mm);
	xdl_free_mmfile(&patch_mm);
	
	if (retval < 0) {
		return 0;
	}
	
	return 1;
}

static int make_bpatch(char *file_path, char *patch_path, xdemitcb_t *output)
{
	int retval;
	mmfile_t file_mm, patch_mm;
	
	retval = load_mm_file(file_path, &file_mm);
	if (!retval) {
		return 0;
	}
	
	retval = load_mm_file(patch_path, &patch_mm);
	if (!retval) {
		xdl_free_mmfile(&file_mm);
		return 0;
	}
		
	retval = xdl_bpatch(&file_mm, &patch_mm, output); 
	xdl_free_mmfile(&file_mm);
	xdl_free_mmfile(&patch_mm);
	
	if (retval < 0) {
		return 0;
	}
	
	return 1;
}

static int make_bpatch_str(char *file, int size1, char *patch, int size2, xdemitcb_t *output)
{
	int retval;
	mmfile_t file_mm, patch_mm;
	
	retval = load_into_mm_file(file, size1, &file_mm);
	if (!retval) {
		return 0;
	}
	
	retval = load_into_mm_file(patch, size2, &patch_mm);
	if (!retval) {
		xdl_free_mmfile(&file_mm);
		return 0;
	}
		
	retval = xdl_bpatch(&file_mm, &patch_mm, output); 
	xdl_free_mmfile(&file_mm);
	xdl_free_mmfile(&patch_mm);
	
	if (retval < 0) {
		return 0;
	}
	
	return 1;
}

static int make_merge3(char *filepath1, char *filepath2, char *filepath3, xdemitcb_t *output, xdemitcb_t *error)
{
	int retval;
	mmfile_t file1, file2, file3;
	
	retval = load_mm_file(filepath1, &file1);
	if (!retval) {
		return 0;	
	}
	
	retval = load_mm_file(filepath2, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		return 0;
	}
	
	retval = load_mm_file(filepath3, &file3);
	if (!retval) {
		xdl_free_mmfile(&file1);
		xdl_free_mmfile(&file2);
		return 0;
	}
	
	retval = xdl_merge3(&file1, &file2, &file3, output, error);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	xdl_free_mmfile(&file3);

	if (retval < 0) {
		return 0;	
	}
	
	return 1;
}

static int make_merge3_str(char *content1, int size1, char *content2, int size2, char *content3, int size3, xdemitcb_t *output, xdemitcb_t *error)
{
	int retval;
	mmfile_t file1, file2, file3;
	
	retval = load_into_mm_file(content1, size1, &file1);
	if (!retval) {
		return 0;	
	}
	
	retval = load_into_mm_file(content2, size2, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		return 0;
	}
	
	retval = load_into_mm_file(content3, size3, &file3);
	if (!retval) {
		xdl_free_mmfile(&file1);
		xdl_free_mmfile(&file2);
		return 0;
	}
	
	retval = xdl_merge3(&file1, &file2, &file3, output, error);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	xdl_free_mmfile(&file3);

	if (retval < 0) {
		return 0;	
	}
	
	return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
