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

/* {{{ xdiff_functions[]
 *
 * Every user visible function must have an entry in xdiff_functions[].
 */
function_entry xdiff_functions[] = {
	PHP_FE(xdiff_compare,	NULL)
	PHP_FE(xdiff_patch,		NULL)
	PHP_FE(xdiff_merge3,	NULL)
	PHP_FE(xdiff_bcompare,	NULL)
	PHP_FE(xdiff_bpatch,	NULL)
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
	"0.1", /* Replace with version number for your extension */
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

/* {{{ proto string xdiff_compare(string file1, string file2, [int context, [bool minimal]])
 */
PHP_FUNCTION(xdiff_compare)
{
	mmfile_t file1, file2;
	char *filepath1, *filepath2;
	int size, retval, context = 3, minimal = 0;
	xpparam_t params;
	xdemitconf_t conf;
	xdemitcb_t output;
	struct string_buffer string;
	
	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 4 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|lb", &filepath1, &size, &filepath2, &size, &context, &minimal) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	retval = load_mm_file(filepath1, &file1);
	if (!retval) {
		RETURN_FALSE;
	}
	
	retval = load_mm_file(filepath2, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		RETURN_FALSE;
	}
	
	params.flags = (minimal) ? XDF_NEED_MINIMAL : 0;
	conf.ctxlen = abs(context);
	
	output.priv= &string;
	output.outf = append_string;
	
	retval = init_string(&string);	
	if (!retval) {
		xdl_free_mmfile(&file1);
		xdl_free_mmfile(&file2);
		RETURN_FALSE;
	}

	retval = xdl_diff(&file1, &file2, &params, &conf, &output);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	
	if (retval < 0) {
		free_string(&string);
		RETURN_FALSE;
	}
	
	RETVAL_STRINGL(string.ptr, string.size, 1);
	free_string(&string);
}
/* }}} */
 
/* {{{ proto string xdiff_patch(string file, string patch, string dest [, int flags])
 */
PHP_FUNCTION(xdiff_patch)
{
	mmfile_t file, patch;
	php_stream *output_stream;
	char *src_path, *patch_content, *dest_path;
	int retval, size, patch_size, flags = XDL_PATCH_NORMAL;	/* DIFF_PATCH_NORMAL */
	xdemitcb_t output, error_output;
	struct string_buffer string;
	
	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 4 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|l", &src_path, &size, &patch_content, &patch_size, &dest_path, &size, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	retval = load_mm_file(src_path, &file);
	if (!retval) {
		RETURN_FALSE;
	}
	
	retval = load_into_mm_file(patch_content, patch_size, &patch);
	if (!retval) {
		xdl_free_mmfile(&file);
		RETURN_FALSE;
	}
	
	output_stream = php_stream_open_wrapper(dest_path, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		xdl_free_mmfile(&file);
		xdl_free_mmfile(&patch);
		RETURN_FALSE;
	}
	
	error_output.priv= &string;
	error_output.outf = append_string;

	retval = init_string(&string);	
	if (!retval) {
		php_stream_close(output_stream);
		xdl_free_mmfile(&file);
		xdl_free_mmfile(&patch);
		RETURN_FALSE;
	}
	
	output.priv = output_stream;
	output.outf = append_stream;
	
	retval = xdl_patch(&file, &patch, flags, &output, &error_output); 
	xdl_free_mmfile(&file);
	xdl_free_mmfile(&patch);
	php_stream_close(output_stream);
	
	if (retval < 0) {
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

/* {{{ proto string xdiff_merge3(string file, string file2, string file2, string dest)
 */
PHP_FUNCTION(xdiff_merge3)
{
	mmfile_t file1, file2, file3;
	int retval, size;
	char *file_path1, *file_path2, *file_path3, *dest;
	php_stream *output_stream;
	struct string_buffer string;
	xdemitcb_t output, error_output;
	
	if (ZEND_NUM_ARGS() != 4 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss", &file_path1, &size, &file_path2, &size, &file_path3, &size, &dest, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	retval = load_mm_file(file_path1, &file1);
	if (!retval) {
		RETURN_FALSE;
	}
	
	retval = load_mm_file(file_path2, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		RETURN_FALSE;
	}
	
	retval = load_mm_file(file_path3, &file3);
	if (!retval) {
		xdl_free_mmfile(&file1);
		xdl_free_mmfile(&file2);
		RETURN_FALSE;
	}
	
	output_stream = php_stream_open_wrapper(dest, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		xdl_free_mmfile(&file1);
		xdl_free_mmfile(&file2);
		xdl_free_mmfile(&file3);
		RETURN_FALSE;
	}
	
	error_output.priv= &string;
	error_output.outf = append_string;
	
	retval = init_string(&string);	
	if (!retval) {
		php_stream_close(output_stream);
		xdl_free_mmfile(&file1);
		xdl_free_mmfile(&file2);
		xdl_free_mmfile(&file3);
		RETURN_FALSE;
	}
	
	output.priv = output_stream;
	output.outf = append_stream;
	
	retval = xdl_merge3(&file1, &file2, &file3, &output, &error_output);
	
	php_stream_close(output_stream);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	xdl_free_mmfile(&file3);
	
	if (retval < 0) {
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

/* {{{ proto string xdiff_bcompare(string file, string file2)
 */
PHP_FUNCTION(xdiff_bcompare)
{
	mmfile_t file1, file2;
	int retval, size;
	char *file1_path, *patch_path, *file2_path;
	php_stream *output_stream;
	xdemitcb_t output;
	bdiffparam_t params;
	
	if (ZEND_NUM_ARGS() != 3 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &file1_path, &size, &file2_path, &size, &patch_path, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	retval = load_mm_file(file1_path, &file1);
	if (!retval) {
		RETURN_FALSE;
	}
	
	retval = load_mm_file(file2_path, &file2);
	if (!retval) {
		xdl_free_mmfile(&file1);
		RETURN_FALSE;
	}
	
	output_stream = php_stream_open_wrapper(patch_path, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		xdl_free_mmfile(&file1);
		xdl_free_mmfile(&file2);
	}
	
	output.priv = output_stream;
	output.outf = append_stream;
	params.bsize = 16;
	
	retval = xdl_bdiff(&file1, &file2, &params, &output);
	php_stream_close(output_stream);
	xdl_free_mmfile(&file1);
	xdl_free_mmfile(&file2);
	
	if (retval < 0) {
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool xdiff_bpatch(string file, string patch, string dest)
 */
PHP_FUNCTION(xdiff_bpatch)
{
	mmfile_t file, patch;
	int retval, size;
	char *file_path, *patch_path, *dest_path;
	php_stream *output_stream;
	xdemitcb_t output;
	
	if (ZEND_NUM_ARGS() != 3 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &file_path, &size, &patch_path, &size, &dest_path, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	retval = load_mm_file(file_path, &file);
	if (!retval) {
		RETURN_FALSE;
	}
	
	retval = load_mm_file(patch_path, &patch);
	if (!retval) {
		xdl_free_mmfile(&file);
		RETURN_FALSE;
	}
	
	output_stream = php_stream_open_wrapper(dest_path, "wb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL);
	if (!output_stream) {
		xdl_free_mmfile(&file);
		xdl_free_mmfile(&patch);
	}
	
	output.priv = output_stream;
	output.outf = append_stream;
	
	retval = xdl_bpatch(&file, &patch, &output);
	php_stream_close(output_stream);
	xdl_free_mmfile(&file);
	xdl_free_mmfile(&patch);
	
	if (retval < 0) {
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
