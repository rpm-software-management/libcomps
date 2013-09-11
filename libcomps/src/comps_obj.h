
#ifndef COMPS_OBJECT_H
#define COMPS_OBJECT_H

#include "comps_mm.h"

#define COMPS_Object_HEAD COMPS_RefC *refc;\
                         COMPS_ObjectInfo *obj_info;

typedef struct COMPS_Object COMPS_Object;
typedef struct COMPS_ObjectInfo COMPS_ObjectInfo;
typedef struct COMPS_Packed COMPS_Packed;
typedef struct COMPS_Num COMPS_Num;
typedef struct COMPS_Str COMPS_Str;


struct COMPS_ObjectInfo {
    size_t obj_size;
    void (*constructor)(COMPS_Object*, COMPS_Object **);
    void (*destructor)(COMPS_Object*);
    void (*copy)(COMPS_Object*, COMPS_Object*);
    COMPS_Object* (*deep_copy)(COMPS_Object*, COMPS_Object*);
    signed char (*obj_cmp)(COMPS_Object*, COMPS_Object*);
    char* (*to_str)(COMPS_Object*);
};

struct COMPS_Object {
    COMPS_RefC *refc;
    COMPS_ObjectInfo *obj_info;
};

struct COMPS_Num {
    COMPS_Object_HEAD
    int val;
};

struct COMPS_Str {
    COMPS_Object_HEAD
    char *val;
};


COMPS_Object* comps_object_create(COMPS_ObjectInfo *obj_info, COMPS_Object **args);
void comps_object_destroy(COMPS_Object *comps_obj);
void comps_object_destroy_v(void *comps_obj);
COMPS_Object* comps_object_copy(COMPS_Object *comps_obj);
signed char comps_object_cmp(COMPS_Object *obj1, COMPS_Object *obj2);
char* comps_object_tostr(COMPS_Object *obj1);
COMPS_Object* comps_object_incref(COMPS_Object *obj);

COMPS_Num* comps_num(int n);
COMPS_Str* comps_str(const char *s);

extern COMPS_ObjectInfo COMPS_Num_ObjInfo;
extern COMPS_ObjectInfo COMPS_Str_ObjInfo;

#endif
