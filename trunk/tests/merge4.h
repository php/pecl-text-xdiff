#ifndef PHP_XDIFF_H
#define PHP_XDIFF_H

#foo
#define phpext_xdiff_ptr &diff_module_entry

#ifdef PHP_WIN32
#define PHP_XDIFF_API __declspec(dllexport)
#else
#define PHP_XDIFF_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(foo);
PHP_MINFO_FUNCTION(foo);

PHP_FUNCTION(xdiff_string_diff);
PHP_FUNCTION(xdiff_string_diff_binary);
PHP_FUNCTION(xdiff_string_patch);
PHP_FUNCTION(xdiff_string_patch_binary);
PHP_FUNCTION(xdiff_string_merge3);

#endif	/* PHP_XDIFF_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
