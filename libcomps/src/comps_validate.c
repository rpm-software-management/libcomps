#include "comps_validate.h"

static void comps_validate_errres_create(COMPS_ValErrResult *res,
                                         COMPS_Object *args) {
    (void)args;
    res->err_list = (COMPS_ObjList*)
                    comps_object_create(&COMPS_ObjList_ObjInfo, NULL);
}

static void comps_validate_errres_destroy(COMPS_ValErrResult *res) {
    COMPS_OBJECT_DESTROY(res->err_list);
}

static void comps_valerr_destroy(COMPS_ValErr *err) {
    free(err->err_msg);
}

COMPS_ValErr* comps_validate_rule_prop_check(COMPS_ValRuleGeneric *rule,
                                           COMPS_Object *obj) {
    #define _rule_ ((COMPS_ValRuleProp*)rule)
    COMPS_Object *prop;
    COMPS_ValErr *ret;

    prop = _rule_->get_f(obj);
    ret = _rule_->check_f(obj, prop);
    COMPS_OBJECT_DESTROY(prop);
    return ret;

    #undef _rule_
}

COMPS_ValErr* comps_validate_rule_list_check(COMPS_ValRuleGeneric *rule,
                                           COMPS_Object *obj) {
    #define _rule_ ((COMPS_ValRuleList*)rule)
    COMPS_ObjList *prop;
    COMPS_ValErr *ret;

    prop = *(COMPS_ObjList**)((char*)obj + _rule_->offset);
    ret = _rule_->check_f(obj, (COMPS_Object*)prop);
    return ret;
    #undef _rule_
}

COMPS_ValGenResult* comps_validate_execute(COMPS_Object *obj,
                                   COMPS_ValRuleGeneric *rules[]) {
    int i;
    COMPS_Object * valres = NULL;
    COMPS_ValErr *valerr;
    for (i = 0; rules[i]; i++) {
        valerr = rules[i]->rule_check(rules[i], obj);
        if (valerr != &COMPS_ValNoErr) {
            if (!valres)
                valres = comps_object_create(&COMPS_ValErrResult_ObjInfo, NULL);

            char *tmp = malloc(sizeof(char)* (strlen(rules[i]->verbose_msg)
                                              + strlen(valerr->err_msg) + 3));
            tmp[0] = 0;
            strcat(tmp, rules[i]->verbose_msg);
            strcat(tmp, ": ");
            strcat(tmp, valerr->err_msg);
            free(valerr->err_msg);
            valerr->err_msg = tmp;
            comps_objlist_append_x(((COMPS_ValErrResult*)valres)->err_list,
                                   (COMPS_Object*)valerr);
        }
    }
    if (!valres) {
        return (COMPS_ValGenResult*)
               comps_object_create(&COMPS_ValOkResult_ObjInfo, NULL);
    }
    return (COMPS_ValGenResult*)valres;
}

COMPS_ValErr* comps_id_check(COMPS_Object *obj, COMPS_Object *prop) {
    char err = 0;
    COMPS_ValErr *valerr;
    (void)obj;

    if (!prop) {
        err = 1;
    } else if (strcmp("", ((COMPS_Str*)prop)->val) == 0) err=2;
    if (err) {
        valerr = (COMPS_ValErr*)comps_object_create(&COMPS_ValErr_ObjInfo,
                                                    NULL);
        if (err == 1)
            valerr->err_msg = __comps_strcpy("Id not set\n");
        else if (err == 2)
            valerr->err_msg = __comps_strcpy("Id empty\n");
    } else {
        valerr = (COMPS_ValErr*)COMPS_OBJECT_INCREF(&COMPS_ValNoErr);
    }
    return valerr;
}
COMPS_ValErr* comps_objlist_unique_check(COMPS_Object *object,
                                       COMPS_Object *objlist) {
    #define _objlist_ ((COMPS_ObjList*)objlist)
    (void)object;
    COMPS_ValErr *valerr = NULL;

    COMPS_Set *set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_object_cmp_v);
    for (COMPS_ObjListIt *it = _objlist_->first; it != NULL; it = it->next) {
        if (!comps_set_in(set, it->comps_obj)) {
            comps_set_add(set, it->comps_obj);
        } else {
            comps_set_destroy(&set);
            valerr = (COMPS_ValErr*)comps_object_create(&COMPS_ValErr_ObjInfo,
                                                        NULL);
            valerr->err_msg = __comps_strcpy("List contains duplicited items");
            return valerr;
        }
    }
    comps_set_destroy(&set);
    return (COMPS_ValErr*)COMPS_OBJECT_INCREF(&COMPS_ValNoErr);

    #undef _objlist_
}

COMPS_ValErr COMPS_ValNoErr = {
    .refc = NULL
};

COMPS_ObjectInfo COMPS_ValErr_ObjInfo = {
    .obj_size = sizeof(COMPS_ValErr),
    .constructor = NULL,
    .destructor = (COMPS_CAST_DESTR) &comps_valerr_destroy
};

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
