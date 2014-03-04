#include "comps_validate.h"

static void comps_validate_errres_create(COMPS_ValErrResult *res,
                                         COMPS_Object *args) {
    (void)args;
    res->err_list = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
}

static void comps_validate_errres_destroy(COMPS_ValErrResult *res) {
    COMPS_OBJECT_DESTROY(res->err_list);
}

static void comps_validate_err_create(COMPS_ValErr *res,
                                      COMPS_Object *args) {
    (void)args;
    res->err_object = NULL;
    res->err_msg = NULL;
}

static void comps_validate_err_destroy(COMPS_ValErr *res) {
    COMPS_OBJECT_DESTROY(res->err_object);
    free(res->err_msg);
}

inline void comps_valgenres_concat(COMPS_ValGenResult **res1,
                                     COMPS_ValGenResult *res2) {
    if (!(*res1)) {
        return;
    }
    if ((*res1)->obj_info == &COMPS_ValGenResult_ObjInfo) {
        if (res2->obj_info == &COMPS_ValGenResult_ObjInfo) {

        } else {
            COMPS_OBJECT_DESTROY(*res1);
            *res1 = (COMPS_ValGenResult*)COMPS_OBJECT_INCREF(res2);
        }
    } else {
        if (res2->obj_info == &COMPS_ValGenResult_ObjInfo) {

        } else {
            comps_objlist_concat_in(((COMPS_ValErrResult*)*res1)->err_list,
                                    ((COMPS_ValErrResult*)res2)->err_list);
        }
    }
}

void comps_valgenres_prefix(COMPS_ValGenResult *res, const char *prefix) {
    COMPS_ObjListIt *it;
    size_t oldlen;

    if (res->obj_info != &COMPS_ValErrResult_ObjInfo) return;
    for (it = ((COMPS_ValErrResult*)res)->err_list->first; it != NULL;
         it = it->next) {
        oldlen = strlen(((COMPS_ValErr*)it->comps_obj)->err_msg);
        ((COMPS_ValErr*)it->comps_obj)->err_msg = realloc(
                                      ((COMPS_ValErr*)it->comps_obj)->err_msg,
                                      (oldlen + strlen(prefix) +1) *
                                      sizeof(char));
        memmove(((COMPS_ValErr*)it->comps_obj)->err_msg + (strlen(prefix) *sizeof(char)),
                ((COMPS_ValErr*)it->comps_obj)->err_msg, (oldlen+1) * sizeof(char));
        memcpy(((COMPS_ValErr*)it->comps_obj)->err_msg, prefix,
               strlen(prefix)*sizeof(char));
    }
}

void comps_valgenres_print(COMPS_ValGenResult *res, FILE *stream) {
    COMPS_ObjListIt *it;

    if (res->obj_info != &COMPS_ValErrResult_ObjInfo) return;
    for (it = ((COMPS_ValErrResult*)res)->err_list->first; it != NULL;
         it = it->next) {
        fprintf(stream, "%s\n", ((COMPS_ValErr*)it->comps_obj)->err_msg);
    }
}

COMPS_ValGenResult* comps_validate_rule_prop_check(COMPS_ValRuleGeneric *rule,
                                                   COMPS_Object *obj) {
    #define _rule_ ((COMPS_ValRuleProp*)rule)
    COMPS_Object *prop;
    COMPS_ValGenResult *ret;

    prop = _rule_->get_f(obj);
    ret = _rule_->check_f(obj, prop);
    COMPS_OBJECT_DESTROY(prop);
    return ret;

    #undef _rule_
}

COMPS_ValGenResult* comps_validate_rule_list_check(COMPS_ValRuleGeneric *rule,
                                                   COMPS_Object *obj) {
    #define _rule_ ((COMPS_ValRuleList*)rule)
    COMPS_ObjList *prop;
    COMPS_ValGenResult *ret;

    prop = *(COMPS_ObjList**)((char*)obj + _rule_->offset);
    ret = _rule_->check_f(obj, (COMPS_Object*)prop);
    return ret;
    #undef _rule_
}

COMPS_ValGenResult* comps_validate_rule_list_check2(COMPS_ValRuleGeneric *rule,
                                                    COMPS_Object *obj) {
    #define _rule_ ((COMPS_ValRuleList2*)rule)
    COMPS_ObjList *prop;
    COMPS_ValGenResult *ret;

    prop = (COMPS_ObjList*) _rule_->get_f(obj);
    ret = _rule_->check_f(obj, (COMPS_Object*)prop);
    COMPS_OBJECT_DESTROY(prop);
    return ret;
    #undef _rule_
}

COMPS_ValGenResult* comps_validate_execute(COMPS_Object *obj,
                                           COMPS_ValRuleGeneric *rules[]) {
    int i;
    COMPS_ValGenResult *valres = NULL;
    COMPS_ValGenResult *tmpres;
    for (i = 0; rules[i]; i++) {
        tmpres = rules[i]->rule_check(rules[i], obj);
        if (tmpres->obj_info != &COMPS_ValOkResult_ObjInfo) {
            if (!valres)
                valres = (COMPS_ValGenResult*)
                         COMPS_OBJECT_CREATE(COMPS_ValErrResult, NULL);
            comps_valgenres_prefix(tmpres, rules[i]->verbose_msg);
            comps_valgenres_concat(&valres, tmpres);
        }
        COMPS_OBJECT_DESTROY(tmpres);
    }
    if (!valres) {
        return (COMPS_ValGenResult*)
               COMPS_OBJECT_CREATE(COMPS_ValOkResult, NULL);
    }
    return (COMPS_ValGenResult*)valres;
}

COMPS_ValGenResult* comps_empty_check(COMPS_Object *obj, COMPS_Object *prop) {
    char err = 0;
    COMPS_ValGenResult *valres;
    COMPS_ValErr *v_err;
    (void)obj;

    if (!prop) err = 1;
    else if (__comps_strcmp("", ((COMPS_Str*)prop)->val)) err=2;

    if (err) {
        valres = (COMPS_ValGenResult*)COMPS_OBJECT_CREATE(COMPS_ValErrResult,
                                                          NULL);
        v_err = COMPS_OBJECT_CREATE(COMPS_ValErr, NULL);
        v_err->err_object = COMPS_OBJECT_INCREF(obj);
        if (err == 1)
            v_err->err_msg = __comps_strcpy("attr not set");
        else if (err == 2)
            v_err->err_msg = __comps_strcpy("attr empty");
        comps_objlist_append_x(((COMPS_ValErrResult*)valres)->err_list,
                               (COMPS_Object*)v_err);
    } else {
        valres = (COMPS_ValGenResult*)COMPS_OBJECT_CREATE(COMPS_ValOkResult,
                                                          NULL);
    }
    return valres;
}
COMPS_ValGenResult* comps_objlist_unique_check(COMPS_Object *object,
                                               COMPS_Object *objlist) {
    #define _objlist_ ((COMPS_ObjList*)objlist)
    (void)object;
    COMPS_ValGenResult *valres = NULL;
    COMPS_ValErr *v_err;
    size_t x;
    int index;
    void *data;
    char *msg;
    const char *msg_fmt = "Duplicate items at %d and %d";

    COMPS_Set *set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_object_cmp_v);
    x = 0;
    for (COMPS_ObjListIt *it = _objlist_->first;
         it != NULL;
         it = it->next, x++) {
        if (!(data = comps_set_data_at(set, it->comps_obj))) {
            comps_set_add(set, it->comps_obj);
        } else {
            if (!valres) {
                valres = (COMPS_ValGenResult*)COMPS_OBJECT_CREATE(
                                                    COMPS_ValErrResult,
                                                    NULL);
            }
            v_err = COMPS_OBJECT_CREATE(COMPS_ValErr, NULL);
            v_err->err_object = COMPS_OBJECT_INCREF(object);
            index = comps_objlist_index(_objlist_, (COMPS_Object*)data);
            msg = malloc(sizeof(char) * (strlen(msg_fmt)-3)+digits_count(x)\
                                                           +digits_count(index));
            sprintf(msg, msg_fmt, index, x);
            v_err->err_msg = msg;
            comps_objlist_append_x(((COMPS_ValErrResult*)valres)->err_list,
                                   (COMPS_Object*)v_err);
        }
    }
    comps_set_destroy(&set);
    if (!valres) {
        valres = (COMPS_ValGenResult*)COMPS_OBJECT_CREATE(COMPS_ValOkResult,
                                                          NULL);
    }
    return valres;
    #undef _objlist_
}

COMPS_ObjectInfo COMPS_ValGenResult_ObjInfo = {
    .obj_size = sizeof(COMPS_ValGenResult),
    .constructor = NULL,
    .destructor = NULL
};
COMPS_ObjectInfo COMPS_ValOkResult_ObjInfo = {
    .obj_size = sizeof(COMPS_ValOkResult),
    .constructor = NULL,
    .destructor = NULL
};

COMPS_ObjectInfo COMPS_ValErrResult_ObjInfo = {
    .obj_size = sizeof(COMPS_ValErrResult),
    .constructor = (COMPS_CAST_CONSTR) &comps_validate_errres_create,
    .destructor = (COMPS_CAST_DESTR) &comps_validate_errres_destroy
};

COMPS_ObjectInfo COMPS_ValErr_ObjInfo = {
    .obj_size = sizeof(COMPS_ValErr),
    .constructor = (COMPS_CAST_CONSTR) &comps_validate_err_create,
    .destructor = (COMPS_CAST_DESTR) &comps_validate_err_destroy
};
