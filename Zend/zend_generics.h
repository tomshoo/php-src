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

#ifndef ZEND_GENERICS_H
#define ZEND_GENERICS_H

#include "zend_types.h"

typedef struct _zend_generic {
	zend_type type;
	bool initialized;
} zend_generic;

typedef struct _zend_generic_list {
	size_t children;
	zend_generic child[1];
} zend_generic_list;

/* Creates a new template generic parameter list */
ZEND_API zend_generic_list *zend_create_generic_list(size_t nmemb, bool persistant);

/* Guess the type of zval and initialize generic type */
ZEND_API void zend_initialize_generic_type(zend_generic *generic, const zval *zval);

ZEND_API void zend_de_initialize_generics_list(zend_generic_list* generics);

#endif
