#include "comps_obj.h"

#include <math.h>
#include <stdio.h>

COMPS_Object * comps_object_create(COMPS_ObjectInfo *obj_info, COMPS_Object **args){
    COMPS_Object *obj;
    obj = malloc(obj_info->obj_size);
     

    obj->obj_info = obj_info;
    obj->refc = comps_refc_create((void*)obj,
                                 (void (*)(void*)) obj_info->destructor);
    obj_info->constructor(obj, args);
    return obj;
}

void comps_object_destroy(COMPS_Object *comps_obj) {
    if (!comps_obj) return;
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
    COMPS_Object *ret = comps_object_create(comps_obj->obj_info, NULL);
    comps_obj->obj_info->copy(ret, comps_obj);
    return ret;
}

COMPS_Object* comps_object_copy_deep(COMPS_Object *comps_obj) {
   (void) comps_obj;
    return NULL;
}

signed char comps_object_cmp(COMPS_Object *obj1, COMPS_Object *obj2) {
    if (obj1->obj_info->obj_cmp) {
        return obj1->obj_info->obj_cmp(obj1, obj2);
    } else {
        return -1;
    }
}

char* comps_object_tostr(COMPS_Object *obj1) {
    char *ret;
    if (obj1->obj_info->to_str != NULL) {
       return obj1->obj_info->to_str(obj1);
    } else {
        ret = malloc(sizeof(char));
        ret[0]=0;
        return ret;;
    }
}

inline COMPS_Object* comps_object_incref(COMPS_Object *obj) {
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

void comps_str_create_u(COMPS_Object* str, COMPS_Object **args){
    if (args && args[0]->obj_info == &COMPS_Str_ObjInfo) {
        ((COMPS_Str*)str)->val = malloc(sizeof(char) * (strlen(((COMPS_Str*)args[0])->val)+1));
        strcpy(((COMPS_Str*)str)->val, ((COMPS_Str*)args[0])->val);
    }
}

void comps_str_copy_u(COMPS_Object *str_dst, COMPS_Object *str_src) {
    ((COMPS_Str*)str_dst)->val = malloc(sizeof(char) * strlen(((COMPS_Str*)str_src)->val));
    strcpy(((COMPS_Str*)str_dst)->val, ((COMPS_Str*)str_dst)->val);
}

void comps_str_destroy_u(COMPS_Object *str){
    free(((COMPS_Str*)str)->val);
}

char* comps_str_tostr(COMPS_Object *str) {
    char *ret;
    ret = malloc(sizeof(char)*(strlen(((COMPS_Str*)str)->val)+1));
    strcpy(ret, ((COMPS_Str*)str)->val);
    return ret;
}


COMPS_Num* comps_num(int n) {
    COMPS_Num *ret = (COMPS_Num*)comps_object_create(&COMPS_Num_ObjInfo, NULL);
    ret->val = n;
    return ret;
}

COMPS_Str* comps_str(const char *s) {
    COMPS_Str *ret = (COMPS_Str*)comps_object_create(&COMPS_Str_ObjInfo, NULL);
    ret->val = malloc(sizeof(char) * ((strlen(s)+1)));
    strcpy(ret->val, s);
    return ret;
}
COMPS_Str* comps_str_x(char *s) {
    COMPS_Str *ret = (COMPS_Str*)comps_object_create(&COMPS_Str_ObjInfo, NULL);
    ret->val = s;
    return ret;
}
void comps_str_set(COMPS_Str *str, char *s) {
    free(str->val);
    str->val = malloc(sizeof(char) * ((strlen(s)+1)));
    strcpy(str->val, s);
}

COMPS_ObjectInfo COMPS_Num_ObjInfo = {
    .obj_size = sizeof(COMPS_Num),
    .constructor = &comps_num_create_u,
    .destructor = &comps_num_destroy_u,
    .copy = &comps_num_copy_u,
    .to_str = &comps_num_tostr
};

COMPS_ObjectInfo COMPS_Str_ObjInfo = {
    .obj_size = sizeof(COMPS_Str),
    .constructor = &comps_str_create_u,
    .destructor = &comps_str_destroy_u,
    .copy = &comps_str_copy_u,
    .to_str = &comps_str_tostr
};

