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

#include "stdio.h"
#include "zend.h"
#include "zend_generics.h"
#include "zend_alloc.h"
#include "zend_portability.h"
#include "zend_string.h"
#include "zend_type_info.h"
#include "zend_types.h"

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

	if (EXPECTED(Z_TYPE_P(zval) == IS_OBJECT))
	{
		zend_class_entry *ce = Z_OBJCE_P(zval);
		zend_string* name = ce->name;

		zend_string_addref(ce->name);

		type = (zend_type) ZEND_TYPE_INIT_CLASS(name, false, 0);
	}

	generic->type = type;
	generic->initialized = true;
}

ZEND_API zend_always_inline void zend_de_initialize_generics_list(zend_generic_list *generics)
{
	for (int i = 0; i < generics->children; i++) {
		generics->child[i].initialized = false;
	}
}
