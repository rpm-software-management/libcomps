#include "comps_obj.h"

#include <math.h>
#include <stdio.h>
#include <fnmatch.h>

COMPS_Object * comps_object_create(COMPS_ObjectInfo *obj_info, COMPS_Object **args){
    COMPS_Object *obj;
    obj = malloc(obj_info->obj_size);
    obj->obj_info = obj_info;
    obj->refc = comps_refc_create((void*)obj,
                                 (void (*)(void*)) obj_info->destructor);
    if (obj_info->constructor)
        obj_info->constructor(obj, args);
    return obj;
}

void comps_object_destroy(COMPS_Object *comps_obj) {
    if (!comps_obj || !comps_obj->refc) return;
    if (comps_obj->refc->ref_count)
        comps_refc_destroy(comps_obj->refc);
    else {
        comps_refc_destroy(comps_obj->refc);
        free(comps_obj);
    }
}

void comps_object_destroy_v(void *comps_obj) {
    comps_object_destroy((COMPS_Object*) comps_obj);
}

COMPS_Object* comps_object_copy(COMPS_Object *comps_obj) {
    if (!comps_obj) return NULL;
    COMPS_Object *obj;
    obj = malloc(comps_obj->obj_info->obj_size);
    obj->refc = comps_refc_create((void*)obj,
                             (void (*)(void*)) comps_obj->obj_info->destructor);
    obj->obj_info = comps_obj->obj_info;
    comps_obj->obj_info->copy(obj, comps_obj);
    return obj;
}

/*COMPS_Object* comps_object_copy_deep(COMPS_Object *comps_obj) {
   (void) comps_obj;
    return NULL;
}*/

signed char comps_object_cmp(COMPS_Object *obj1, COMPS_Object *obj2) {
    if (!obj1 && !obj2)
        return 1;
    else if (!obj1 || !obj2)
        return 0;
    if (obj1->obj_info->obj_cmp) {
        return obj1->obj_info->obj_cmp(obj1, obj2);
    } else {
        return 0;
    }
}
char comps_object_cmp_v(void *obj1, void *obj2) {
    return (char)comps_object_cmp((COMPS_Object*)obj1, (COMPS_Object*)obj2);
}

char* comps_object_tostr(COMPS_Object *obj1) {
    char *ret;
    if (obj1 && obj1->obj_info->to_str != NULL) {
       return obj1->obj_info->to_str(obj1);
    } else {
        ret = malloc(sizeof(char));
        ret[0]=0;
        return ret;;
    }
}

inline COMPS_Object* comps_object_incref(COMPS_Object *obj) {
    if (obj && obj->refc)
        comps_refc_incref(obj->refc);
    return obj;
}


void comps_num_create_u(COMPS_Object* num, COMPS_Object **args){
    if (args && args[0]->obj_info == &COMPS_Num_ObjInfo) {
        ((COMPS_Num*)num)->val = ((COMPS_Num*)args[0])->val;
    } else
        ((COMPS_Num*)num)->val = 0;
}

void comps_num_copy_u(COMPS_Object *num_dst, COMPS_Object *num_src) {
    ((COMPS_Num*)num_dst)->val = ((COMPS_Num*)num_src)->val;
}

void comps_num_destroy_u(COMPS_Object *num){
    (void)num;
}

char* comps_num_tostr(COMPS_Object *num) {
    char *ret;
    int x;

    if (((COMPS_Num*)num)->val == 0)
        x = 1;
    else
        x = (int)(log10(((COMPS_Num*)num)->val)) + 1;
    ret = malloc(sizeof(char)*(x+1));
    sprintf(ret, "%d", ((COMPS_Num*)num)->val);
    return ret;
}

signed char comps_num_cmp_u(COMPS_Object *num1, COMPS_Object *num2) {
    return ((COMPS_Num*)num1)->val == ((COMPS_Num*)num2)->val;
}

void comps_str_create_u(COMPS_Object* str, COMPS_Object **args){
    if (args && args[0]->obj_info == &COMPS_Str_ObjInfo) {
        ((COMPS_Str*)str)->val = malloc(sizeof(char) *
                                        (strlen(((COMPS_Str*)args[0])->val)+1));
        strcpy(((COMPS_Str*)str)->val, ((COMPS_Str*)args[0])->val);
    }
}

void comps_str_copy_u(COMPS_Object *str_dst, COMPS_Object *str_src) {
    if (((COMPS_Str*)str_src)->val) {
        ((COMPS_Str*)str_dst)->val =
                      malloc(sizeof(char) *
                             (strlen(((COMPS_Str*)str_src)->val) + 1));
        strcpy(((COMPS_Str*)str_dst)->val, ((COMPS_Str*)str_src)->val);
    } else
        ((COMPS_Str*)str_dst)->val = NULL;
}

void comps_str_destroy_u(COMPS_Object *str){
    free(((COMPS_Str*)str)->val);
}

char* comps_str_tostr(COMPS_Object *str) {
    char *ret;
    if (((COMPS_Str*)str)->val) {
        ret = malloc(sizeof(char)*(strlen(((COMPS_Str*)str)->val)+1));
        strcpy(ret, ((COMPS_Str*)str)->val);
    } else {
        ret = malloc(sizeof(char));
        ret[0] = 0;
    }
    return ret;
}

signed char comps_str_cmp_u(COMPS_Object *str1, COMPS_Object *str2) {
    if (!((COMPS_Str*)str1)->val && !((COMPS_Str*)str2)->val) {
        return 1;
    } else if (!((COMPS_Str*)str1)->val || !((COMPS_Str*)str2)->val) {
        return 0;
    } else return strcmp(((COMPS_Str*)str1)->val,
                         ((COMPS_Str*)str2)->val) == 0;
}


COMPS_Num* comps_num(int n) {
    COMPS_Num *ret = COMPS_OBJECT_CREATE(COMPS_Num, NULL);
    ret->val = n;
    return ret;
}

COMPS_Str* comps_str(const char *s) {
    COMPS_Str *ret = COMPS_OBJECT_CREATE(COMPS_Str, NULL);
    if (s) {
        ret->val = malloc(sizeof(char) * ((strlen(s)+1)));
        strcpy(ret->val, s);
    } else 
        ret->val = NULL;
    return ret;
}
COMPS_Str* comps_str_x(char *s) {
    COMPS_Str *ret = COMPS_OBJECT_CREATE(COMPS_Str, NULL);
    ret->val = s;
    return ret;
}
void comps_str_set(COMPS_Str *str, char *s) {
    free(str->val);
    str->val = malloc(sizeof(char) * ((strlen(s)+1)));
    strcpy(str->val, s);
}
signed char comps_str_fnmatch(COMPS_Str *str, char *pattern, int flags) {
    return fnmatch(pattern, str->val, flags) == 0;
}
signed char comps_str_fnmatch_o(COMPS_Str *str, COMPS_Str *pattern, int flags) {
    return fnmatch(pattern->val, str->val, flags) == 0;
}

COMPS_ObjectInfo COMPS_Num_ObjInfo = {
    .obj_size = sizeof(COMPS_Num),
    .constructor = &comps_num_create_u,
    .destructor = &comps_num_destroy_u,
    .copy = &comps_num_copy_u,
    .to_str = &comps_num_tostr,
    .obj_cmp = &comps_num_cmp_u
};

COMPS_ObjectInfo COMPS_Str_ObjInfo = {
    .obj_size = sizeof(COMPS_Str),
    .constructor = &comps_str_create_u,
    .destructor = &comps_str_destroy_u,
    .copy = &comps_str_copy_u,
    .to_str = &comps_str_tostr,
    .obj_cmp = &comps_str_cmp_u
};

