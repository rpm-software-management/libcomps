/* libcomps - C alternative to yum.comps library
 * Copyright (C) 2013 Jindrich Luza
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to  Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA
 */

#ifndef COMPS_OBJECT_H
#define COMPS_OBJECT_H

#include "comps_mm.h"

/** \file comps_obj.h
 * \brief COMPS_Object header file
 *
 */

/** \def COMPS_OBJECT_CREATE(obj_type, args)
 * \brief macro for create object by choosen type without explicit needs
 * of typecast. Macro returns concrete type of object not
 * COMPS_Object type. If you want use this macro, you have to declare
 * COMPS_ObjectInfo object exactly as <YourObject>_ObjInfo
 * @see COMPS_Object_TAIL
 */

/** \def COMPS_OBJECT_CMP(obj1, obj2)
 * \brief macro for compare two COMPS_Object derivates without typecasting to
 *  COMPS_Object pointer
 */

/** \def COMPS_OBJECT_DESTROY(obj1)
 * \brief macro for call comps_object_destroy without typecasting to
 *  COMPS_Object pointer
 */

/** \def COMPS_OBJECT_COPY(obj)
 * \brief macro for call comps_object_copy without typecasting to
 *  COMPS_Object pointer
 */

/** \def COMPS_Object_TAIL(obj)
 * \brief insert "extern COMPS_ObjectInfo <obj>_ObjInfo" statement. Use this
 * macro in combination with COMPS_OBJECT_CREATE
 * @see COMPS_OBJECT_CREATE
 */

#define COMPS_OBJECT_CREATE(objtype, args)\
    (objtype*)comps_object_create(&objtype##_ObjInfo, args)


#define COMPS_OBJECT_CMP(obj1,obj2)\
    comps_object_cmp((COMPS_Object*)obj1, (COMPS_Object*)obj2)

#define COMPS_OBJECT_DESTROY(obj1)\
    comps_object_destroy((COMPS_Object*)obj1)

#define COMPS_OBJECT_COPY(obj)\
    comps_object_copy(((COMPS_Object*)obj))

#define COMPS_OBJECT_INCREF(obj)\
    comps_object_incref(((COMPS_Object*)obj))

#define COMPS_OBJECT_REPLACE(oldobj, TYPE, new_obj)\
    COMPS_OBJECT_DESTROY(oldobj);\
    oldobj = (TYPE*)COMPS_OBJECT_INCREF(new_obj);


#define COMPS_CAST_CONSTR void (*)(COMPS_Object*, COMPS_Object**)
#define COMPS_CAST_DESTR void (*)(COMPS_Object*)

/** ensure that COMPS_Object derivate has need struct members for properly
 * behaviour
 */
#define COMPS_Object_HEAD COMPS_RefC *refc;\
                         COMPS_ObjectInfo *obj_info

#define COMPS_Object_TAIL(obj) extern COMPS_ObjectInfo obj##_ObjInfo

typedef struct COMPS_Object COMPS_Object;
typedef struct COMPS_ObjectInfo COMPS_ObjectInfo;
typedef struct COMPS_Packed COMPS_Packed;
typedef struct COMPS_Num COMPS_Num;
typedef struct COMPS_Str COMPS_Str;


/** Structure holding all importating callback functions supporting
 * COMPS_Object derivate proper behavior. All callbacks except constructor
 * and destructor are optional @see comps_object_create
 */
struct COMPS_ObjectInfo {
    size_t obj_size; /**< size of derivate object which is sizeof(obj) */
    void (*constructor)(COMPS_Object*, COMPS_Object **);
    /**< pointer to derivate object constructor @see comps_object_create*/
    void (*destructor)(COMPS_Object*);
    /**< pointer to derivate objects destructor @see comps_object_destroy*/
    void (*copy)(COMPS_Object*, COMPS_Object*);
    /**< pointer to derivate object copy function @see comps_object_copy*/
    COMPS_Object* (*deep_copy)(COMPS_Object*, COMPS_Object*);
    /**< currently unused*/
    signed char (*obj_cmp)(COMPS_Object*, COMPS_Object*);
    /**< pointer to comparator function*/
    char* (*to_str)(COMPS_Object*);
    /**< pointer to string representation convert function */
};

/** COMPS Object structure
 * COMPS_Object is basic structure from which are derived concrete COMPS
 * structure. Using COMPS Object as bootstrap of concrete object ensure
 * eventuality of creating and destroying with reference counting, copying,
 * comparing with other object, string representation
*/
struct COMPS_Object {
    COMPS_RefC *refc; /**< reference counter pointer for COMPS_Object*/
    COMPS_ObjectInfo *obj_info; /**< pointer to COMPS_ObjectInfo struct*/
};

/** COMPS Object derivate representing number
 *
 * COMPS_Num represents integer (signer or unsigned) number as COMPS Object
*/
struct COMPS_Num {
    COMPS_Object_HEAD; /** \n */
    int val; /**< value of represented number*/
};
COMPS_Object_TAIL(COMPS_Num);

/** COMPS Object derivate representing string
 *
 * COMPS_Str represents string as COMPS Object
*/
struct COMPS_Str {
    COMPS_Object_HEAD; /** \n */
    char *val; /**< holds reprezented string, freed at destruction time*/
};
COMPS_Object_TAIL(COMPS_Str);


/** Create COMPS_Object derivate and pass \a args arguments to its constructor
 * @param obj_info pointer to COMPS_ObjectInfo structure
 * @param args array of arguments passed to derivate constructor. Array doesn't
 * have to end with NULL sentinel.
 * Processing args attribute passed to contructor
 * is completely in programmer's care
 * @return COMPS_Object derivate typecasted as general COMPS_Object
 */
COMPS_Object* comps_object_create(COMPS_ObjectInfo *obj_info, COMPS_Object **args);

/** Destroy passed COMPS_Object derivate if its reference counter is zero
 * if not, only decrement reference counter
 */
void comps_object_destroy(COMPS_Object *comps_obj);
void comps_object_destroy_v(void *comps_obj);
/** Return whole new copy of COMPS_Object derivate.
 *
 * Function create new allocation of derivate and call obj_copy callback with
 * old instance and new instance of derivate. Copying inner structure members
 * are in programmers care
 * @param comps_obj derivate object want to be copied
 * @return new copy of derivate object
 * @see COMPS_ObjectInfo
 */
COMPS_Object* comps_object_copy(COMPS_Object *comps_obj);

/** Compare two COMPS_Object derivates and return non-zero value if equals
 *
 * \warning Function doen't check equality of derivate types (COMPS_ObjectInfo)!!
 *
 * @param obj1 first derivate
 * @param obj2 second derivate
 * @return non-zero value if equals, zero otherwise
*/
signed char comps_object_cmp(COMPS_Object *obj1, COMPS_Object *obj2);
char comps_object_cmp_v(void *obj1, void *obj2);

/** Return string representation of COMPS_Object derivate
 *
 * \warning
 * Returned string is new allocation which needs to be freed manualy
 *
 * @param obj1 COMPS_Object derivate
 * @return new alllocation of string representation of concrete object
 */
char* comps_object_tostr(COMPS_Object *obj1);

/** Increment COMPS_Object derivate reference counter
 */
COMPS_Object* comps_object_incref(COMPS_Object *obj);

/** Directly construct COMPS_Num derivate from passed argument
 * @param n value of COMPS_Num
 */
COMPS_Num* comps_num(int n);

/** Directly construct COMPS_Str derivate from passed argument
 *
 * passed argument is copied as new allocation
 * @param s string value of derivate
 */
COMPS_Str* comps_str(const char *s);

/** Directly construct COMPS_Str derivate from passed argument
 *
 * \warning
 * passed argument is not copied. COMPS_Str derivate use same memory place as
 * \a s argument and during destruction of derivate this memory place is freed
 * @param s string value of derivate
 */
COMPS_Str* comps_str_x(char *s);

/** Set memory copy of passed argument as COMPS_Str value
 *
 * @param str COMPS_Str object
 * @param s desired new COMPS_Str object value
 */
void comps_str_set(COMPS_Str *str, char *s);

//extern COMPS_ObjectInfo COMPS_Num_ObjInfo;
//extern COMPS_ObjectInfo COMPS_Str_ObjInfo;

/** Return non-zero if str match the pattern by fnmatch
 *
 * @param str source string. COMPS_Str object
 * @param pattern match pattern
 */
signed char comps_str_fnmatch(COMPS_Str *str, char *pattern, int flags);

/** Return non-zero if str match the pattern by fnmatch
 *
 * @param str source string. COMPS_Str object
 * @param pattern COMPS_Str match pattern
 */
signed char comps_str_fnmatch_o(COMPS_Str *str, COMPS_Str *pattern, int flags);

#endif
