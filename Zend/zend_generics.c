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
#include "zend_list.h"
#include "zend_portability.h"
#include "zend_string.h"
#include "zend_type_info.h"
#include "zend_types.h"

typedef struct _builtin_type_info {
	const char* name;
	const size_t name_len;
	const uint8_t type;
} builtin_type_info;

ZEND_API zend_generic_list *zend_create_generic_list(size_t nmemb, bool persistant)
{
	zend_generic_list *list = pemalloc(sizeof(zend_generic_list) + (sizeof(zend_generic) * nmemb - 1), persistant);

	list->children = nmemb;
	list->is_persistant = persistant;

	for (int i = 0; i < list->children; i ++)
	{
		list->child[i].initialized = false;
	}

	return list;
}

ZEND_API void zend_initialize_generic_type(zend_generic* generic, const zval *zval)
{
	ZEND_ASSERT(!generic->initialized && "Generic type cannot be initialized twice");

	uint8_t type_code = Z_TYPE_P(zval);
	zend_type type = ZEND_TYPE_INIT_CODE(type_code, 0, 0);

	if (type_code == IS_OBJECT) {
		zend_class_entry *ce = Z_OBJCE_P(zval);
		zend_string* name = ce->name;

		zend_string_addref(ce->name);

		type = (zend_type) ZEND_TYPE_INIT_CLASS(name, false, 0);
	} else if (type_code == IS_RESOURCE) {
		const char *name = zend_rsrc_list_get_rsrc_type(Z_RES_P(zval));

		zend_error(E_ERROR, "Resource type '%s' cannot be used with generic types", name);
		return;
	}

	generic->type = type;
	generic->initialized = true;
}

