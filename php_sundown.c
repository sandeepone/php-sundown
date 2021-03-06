/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2011 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Shuhei Tanuma <chobieeee@php.net>                           |
   +----------------------------------------------------------------------+
 */


#include "php_sundown.h"
ZEND_DECLARE_MODULE_GLOBALS(sundown)

#include "ext/standard/info.h"

extern void php_sundown_render_base_init(TSRMLS_D);
extern void php_sundown_render_html_init(TSRMLS_D);
extern void php_sundown_render_html_toc_init(TSRMLS_D);
extern void php_sundown_render_xhtml_init(TSRMLS_D);
extern void php_sundown_markdown_init(TSRMLS_D);

zend_class_entry *sundown_class_entry;

void php_sundown_init(TSRMLS_D);

ZEND_BEGIN_ARG_INFO_EX(arginfo_sundown__construct, 0, 0, 2)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_ARRAY_INFO(0, extensions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sundown_has_extension, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sundown_has_render_flag, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* {{{ proto string Sundown::__construct(string $string [, array $extensions])
	setup Sundown extension */
PHP_METHOD(sundown, __construct)
{
	zval *extensions = NULL, *c_extensions = NULL;
	char *buffer;
	int buffer_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|a", &buffer, &buffer_len, &extensions) == FAILURE) {
		return;
	}

	add_property_string_ex(getThis(), ZEND_STRS("data"), buffer, 1 TSRMLS_CC);

	if (extensions == NULL) {
		MAKE_STD_ZVAL(c_extensions);
		array_init(c_extensions);
	} else {
		ALLOC_INIT_ZVAL(c_extensions);
		ZVAL_ZVAL(c_extensions, extensions, 1, 0);
	}
	add_property_zval_ex(getThis(), ZEND_STRS("extensions"), c_extensions TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Sundown::__destruct()
	cleanup variables */
PHP_METHOD(sundown, __destruct)
{
	zval *extensions;
	
	extensions = zend_read_property(sundown_class_entry, getThis(), ZEND_STRS("extensions")-1, 0 TSRMLS_CC);
	zval_ptr_dtor(&extensions);
}
/* }}} */

/* {{{ proto string Sundown::toHtml()
	Returns converted HTML string */
PHP_METHOD(sundown, toHtml)
{
	php_sundown_markdon_render(SUNDOWN_RENDER_HTML, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string Sundown::__toString()
	Returns converted HTML string */
PHP_METHOD(sundown, __toString)
{
	php_sundown_markdon_render(SUNDOWN_RENDER_HTML, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string Sundown::toToc()
	Returns table of contents*/
PHP_METHOD(sundown, toToc)
{
	php_sundown_markdon_render(SUNDOWN_RENDER_TOC, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}


/* {{{ proto string Sundown::hasExtension(string $ext_name)
*/
PHP_METHOD(sundown, hasExtension)
{
	char *name;
	int name_len = 0;
	HashTable *table;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s", &name, &name_len) == FAILURE) {
		return;
	}
	
	if (Z_TYPE_P(zend_read_property(sundown_class_entry, getThis(), ZEND_STRS("extensions")-1, 0 TSRMLS_CC)) != IS_NULL) {
		table = Z_ARRVAL_P(zend_read_property(sundown_class_entry, getThis(), ZEND_STRS("extensions")-1, 0 TSRMLS_CC));
		RETVAL_BOOL(php_sundown_has_ext(table, name));
	}
}

/* {{{ proto string Sundown::hasRenderFlag(string $ext_name)
*/
PHP_METHOD(sundown, hasRenderFlag)
{
	char *name;
	int name_len = 0;
	HashTable *table;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s", &name, &name_len) == FAILURE) {
		return;
	}
	
	if (Z_TYPE_P(zend_read_property(sundown_class_entry, getThis(), ZEND_STRS("extensions")-1, 0 TSRMLS_CC)) != IS_NULL) {
		table = Z_ARRVAL_P(zend_read_property(sundown_class_entry, getThis(), ZEND_STRS("extensions")-1, 0 TSRMLS_CC));
		RETVAL_BOOL(php_sundown_has_ext(table, name));
	}
}

static zend_function_entry php_sundown_methods[] = {
	PHP_ME(sundown, __construct, arginfo_sundown__construct, ZEND_ACC_PUBLIC)
	PHP_ME(sundown, __destruct,  NULL,                       ZEND_ACC_PUBLIC)
	PHP_ME(sundown, toHtml,      NULL,                       ZEND_ACC_PUBLIC)
	PHP_ME(sundown, toToc,       NULL,                       ZEND_ACC_PUBLIC)
	PHP_ME(sundown, __toString,  NULL,                       ZEND_ACC_PUBLIC)
	PHP_ME(sundown, hasExtension,arginfo_sundown_has_extension,ZEND_ACC_PUBLIC)
	PHP_ME(sundown, hasRenderFlag,arginfo_sundown_has_render_flag,ZEND_ACC_PUBLIC)
	/* to_html and to_toc methods are compatible with Redcarpet */
	PHP_MALIAS(sundown, to_html, toHtml, NULL, ZEND_ACC_PUBLIC)
	PHP_MALIAS(sundown, to_toc,  toToc,  NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

PHP_MINIT_FUNCTION(sundown) {

#ifdef ZTS
	ts_allocate_id(&sundown_globals_id, sizeof(zend_sundown_globals), NULL, NULL); 
#endif

	php_sundown_init(TSRMLS_C);
	php_sundown_render_base_init(TSRMLS_C);
	php_sundown_render_html_init(TSRMLS_C);
	php_sundown_render_xhtml_init(TSRMLS_C);
	php_sundown_render_html_toc_init(TSRMLS_C);
	php_sundown_markdown_init(TSRMLS_C);

	REGISTER_NS_STRING_CONSTANT(ZEND_NS_NAME("Sundown", "Render"), "HTML", "Sundown\\Render\\HTML", CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(sundown) {
	return SUCCESS;
}



PHP_MINFO_FUNCTION(sundown)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Sundown Support",  "enabled");
	php_info_print_table_row(2, "Version", PHP_SUNDOWN_VERSION);
	php_info_print_table_row(2, "Sundown Version", SUNDOWN_VERSION);
	php_info_print_table_end();
}

zend_module_entry sundown_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"sundown",
	NULL,					/* Functions */
	PHP_MINIT(sundown),	/* MINIT */
	PHP_MSHUTDOWN(sundown),	/* MSHUTDOWN */
	NULL,					/* RINIT */
	NULL,					/* RSHUTDOWN */
	PHP_MINFO(sundown),	/* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
	PHP_SUNDOWN_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};


void php_sundown_init(TSRMLS_D)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, "Sundown", php_sundown_methods);
	sundown_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null(sundown_class_entry, ZEND_STRS("extensions")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(sundown_class_entry, ZEND_STRS("enable_pants")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
}


#ifdef COMPILE_DL_SUNDOWN
ZEND_GET_MODULE(sundown)
#endif
