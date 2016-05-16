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

#ifndef PHP_XDIFF_H
#define PHP_XDIFF_H

extern zend_module_entry xdiff_module_entry;
#define phpext_xdiff_ptr &xdiff_module_entry

#define PHP_XDIFF_VERSION "2.0.1"

#ifdef PHP_WIN32
#define PHP_XDIFF_API __declspec(dllexport)
#else
#define PHP_XDIFF_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(xdiff);
PHP_MINFO_FUNCTION(xdiff);

PHP_FUNCTION(xdiff_file_diff);
PHP_FUNCTION(xdiff_file_bdiff);
PHP_FUNCTION(xdiff_file_patch);
PHP_FUNCTION(xdiff_file_bpatch);
PHP_FUNCTION(xdiff_file_merge3);
PHP_FUNCTION(xdiff_file_rabdiff);
PHP_FUNCTION(xdiff_file_bdiff_size);

PHP_FUNCTION(xdiff_string_diff);
PHP_FUNCTION(xdiff_string_bdiff);
PHP_FUNCTION(xdiff_string_patch);
PHP_FUNCTION(xdiff_string_bpatch);
PHP_FUNCTION(xdiff_string_merge3);
PHP_FUNCTION(xdiff_string_rabdiff);
PHP_FUNCTION(xdiff_string_bdiff_size);

#endif	/* PHP_XDIFF_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
