/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Shubhanshu Tomar <57960301+tomshoo@users.noreply.github.com>|
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_generics.h"
#include "zend_alloc.h"
#include "zend_compile.h"
#include "zend_portability.h"
#include "zend_string.h"
#include "zend_type_info.h"
#include "zend_types.h"

typedef struct _builtin_type_info {
	const char* name;
	const size_t name_len;
	const uint8_t type;
} builtin_type_info;

static const builtin_type_info builtin_types[] = {
	{ZEND_STRL("null"), IS_NULL},
	{ZEND_STRL("true"), IS_TRUE},
	{ZEND_STRL("false"), IS_FALSE},
	{ZEND_STRL("int"), IS_LONG},
	{ZEND_STRL("float"), IS_DOUBLE},
	{ZEND_STRL("string"), IS_STRING},
	{ZEND_STRL("bool"), _IS_BOOL},
	{ZEND_STRL("void"), IS_VOID},
	{ZEND_STRL("never"), IS_NEVER},
	{ZEND_STRL("iterable"), IS_ITERABLE},
	{ZEND_STRL("object"), IS_OBJECT},
	{ZEND_STRL("mixed"), IS_MIXED},
	{NULL, 0, IS_UNDEF}
};

static zend_always_inline uint8_t zend_lookup_builtin_type_by_name(const zend_string *name) /* {{{ */
{
	const builtin_type_info *info = &builtin_types[0];

	for (; info->name; ++info) {
		if (ZSTR_LEN(name) == info->name_len && zend_binary_strcasecmp(ZSTR_VAL(name), ZSTR_LEN(name), info->name, info->name_len) == 0) {
			return info->type;
		}
	}

	return 0;
}

ZEND_API zend_generic_list *zend_create_generic_list(size_t nmemb, bool persistant)
{
	zend_generic_list *list = pemalloc(sizeof(zend_generic_list) + (sizeof(zend_generic) * nmemb - 1), persistant);

	list->children = nmemb;

	for (int i = 0; i < list->children; i ++)
	{
		list->child[i].initialized = false;
	}

	return list;
}

ZEND_API void zend_initialize_generic_type(zend_generic* generic, const zval *zval)
{
	ZEND_ASSERT(!generic->initialized && "Generic type cannot be initialized twice");

	zend_type type = ZEND_TYPE_INIT_CODE(Z_TYPE_P(zval), 0, 0);
	zend_string *typename = zend_type_to_string(type);

	uint8_t typecode = zend_lookup_builtin_type_by_name(typename);

	if (typecode != 0) {
		type = (zend_type) ZEND_TYPE_INIT_CODE(typecode, 0, 0);
	} else if (EXPECTED(Z_TYPE_P(zval) == IS_OBJECT)) {
		zend_class_entry *ce = Z_OBJCE_P(zval);
		zend_string* name = ce->name;

		zend_string_addref(ce->name);

		type = (zend_type) ZEND_TYPE_INIT_CLASS(name, false, 0);
	}

	zend_string_release(typename);

	generic->type = type;
	generic->initialized = true;
}

ZEND_API zend_always_inline void zend_de_initialize_generics_list(zend_generic_list *generics)
{
	for (int i = 0; i < generics->children; i++) {
		generics->child[i].initialized = false;
	}
}
