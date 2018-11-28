#include "comps_objlist.h"
#include "comps_utils.h"

inline const COMPS_ObjListIt *comps_objlist_it_next(const COMPS_ObjListIt *it) {
    return (const COMPS_ObjListIt*)it->next;
}

COMPS_ObjListIt* comps_objlist_it_create(COMPS_Object *obj) {
    COMPS_ObjListIt *objit;
    objit = malloc(sizeof(COMPS_ObjListIt));
    if (!objit) return NULL;

    objit->comps_obj = comps_object_incref(obj);
    objit->next = NULL;
    return objit;
}

COMPS_ObjListIt* comps_objlist_it_create_x(COMPS_Object *obj) {
    COMPS_ObjListIt *objit;
    objit = malloc(sizeof(COMPS_ObjListIt));
    if (!objit) return NULL;

    objit->comps_obj = obj;
    objit->next = NULL;
    return objit;
}

void comps_objlist_it_destroy(COMPS_ObjListIt *objit) {
    //objit = malloc(sizeof(COMPS_ObjListIt));
    comps_object_destroy(objit->comps_obj);
    free(objit);
}

void comps_objlist_create(COMPS_ObjList *objlist, COMPS_Object **args) {
    (void)args;
    objlist->first = NULL;
    objlist->last = NULL;
    objlist->len = 0;
}
COMPS_CREATE_u(objlist, COMPS_ObjList)

void comps_objlist_copy(COMPS_ObjList *objlist_dst,
                        COMPS_ObjList *objlist_src) {
    COMPS_ObjListIt *it;

    objlist_dst->first = NULL;
    objlist_dst->last = NULL;
    objlist_dst->len = 0;

    for (it = objlist_src->first; it != NULL; it = it->next) {
        comps_objlist_append_x(objlist_dst, comps_object_copy(it->comps_obj));
    }
}
COMPS_COPY_u(objlist, COMPS_ObjList)


void comps_objlist_destroy(COMPS_ObjList *objlist) {
    COMPS_ObjListIt *oldit, *it = objlist->first;
    COMPS_Object **obj= NULL;

    for (oldit = it; comps_objlist_walk(&it, obj); oldit = it) {
        comps_objlist_it_destroy(oldit);
    }
    if (oldit)
        comps_objlist_it_destroy(oldit);
}
COMPS_DESTROY_u(objlist, COMPS_ObjList)

void comps_objlist_clear(COMPS_ObjList *objlist) {
    comps_objlist_destroy(objlist);
    objlist->first = NULL;
    objlist->last = NULL;
    objlist->len = 0;
}


COMPS_Object* comps_objlist_get(COMPS_ObjList *objlist, unsigned int atpos) {
    unsigned int pos;
    COMPS_ObjListIt *it;
    if (!objlist) return NULL;

    for (it = objlist->first, pos=0;
         it != NULL && pos != atpos;
         it = it->next, pos++);

    if (pos != atpos)
        return NULL;

    return (it)?comps_object_incref(it->comps_obj):NULL;
}

COMPS_Object* comps_objlist_get_x(COMPS_ObjList *objlist, unsigned int atpos) {
    unsigned int pos;
    COMPS_ObjListIt *it;
    if (!objlist) return NULL;

    for (it = objlist->first, pos=0;
         it != NULL && pos != atpos;
         it = it->next, pos++);

    if (pos != atpos)
        return NULL;

    return (it)?it->comps_obj:NULL;
}

COMPS_ObjListIt* comps_objlist_get_it(COMPS_ObjList *objlist,
                              unsigned int atpos) {
    unsigned int pos;
    COMPS_ObjListIt *it;
    if (!objlist) return NULL;

    for (it = objlist->first, pos=0;
         it != NULL && pos != atpos;
         it = it->next, pos++);

    if (pos+1 != atpos)
        return NULL;

    return (it)?it:NULL;
}


int comps_objlist_walk(COMPS_ObjListIt **walker, COMPS_Object **result) {
    if (!walker || !*walker) return 0;
    if (result)
        *result = (*walker)->comps_obj;
    *walker = (*walker)->next;
    return 1;
}

int comps_objlist_walk_r(COMPS_ObjListIt *start,
                            COMPS_ObjListIt *mantinel,
                            COMPS_Object **result) {
    if (!start->next) return 0;
    if (start == mantinel)
        return 0;
    start = start->next;
    *result = start->comps_obj;
    return 1;
}

static int __comps_objlist_append(COMPS_ObjList *objlist, COMPS_ObjListIt *objit) {
    if (!objlist || !objit) return 0;
    if (objlist->first == NULL) {
        objlist->first = objit;
        objlist->last = objit;
    } else {
        objlist->last->next = objit;
        objlist->last = objlist->last->next;
    }
    objlist->len++;
    return 1;
}

int comps_objlist_append_x(COMPS_ObjList *objlist, COMPS_Object *obj) {
    COMPS_ObjListIt *new_it = comps_objlist_it_create_x(obj);
    return __comps_objlist_append(objlist, new_it);
}
int comps_objlist_append(COMPS_ObjList *objlist, COMPS_Object *obj) {
    COMPS_ObjListIt *new_it = comps_objlist_it_create(obj);
    return __comps_objlist_append(objlist, new_it);
}

int comps_objlist_insert_after(COMPS_ObjList *objlist,
                              COMPS_ObjListIt *it,
                              COMPS_Object *obj) {
    if (!objlist) return -1;
    if (!it) return -1;

    COMPS_ObjListIt *new_it = comps_objlist_it_create(obj);

    new_it->next = it->next;
    it->next = new_it;
    if (it == objlist->last) {
        objlist->last = new_it;
    }
    objlist->len++;
    return 1;
}

int comps_objlist_insert_before(COMPS_ObjList *objlist,
                               COMPS_ObjListIt *it,
                               COMPS_Object *obj) {
    if (!objlist) return -1;
    if (!it) return -1;

    COMPS_ObjListIt *new_it = comps_objlist_it_create(obj);
    COMPS_ObjListIt *tmpit;
    for (tmpit = objlist->first; tmpit->next != it; tmpit = tmpit->next);

    if (tmpit == objlist->first) {
        new_it->next = objlist->first;
        objlist->first = new_it;
        if (objlist->last == NULL)
            objlist->last = new_it;
    } else {
        new_it->next = tmpit->next;
        tmpit->next = new_it;
    }
    objlist->len++;
    return 1;
}

int __comps_objlist_insert_at(COMPS_ObjList *objlist,
                           unsigned int pos,
                           COMPS_ObjListIt *newit) {
    COMPS_ObjListIt *tmpit, *oldit;
    unsigned int i;
    if (pos == 0) {
        newit->next = objlist->first;
        objlist->first = newit;
        if (!objlist->last) {
            objlist->last = newit;
        }
    } else if (pos == objlist->len){
        newit->next = NULL;
        objlist->last->next = newit;
        objlist->last = newit;
    } else {
        i = 0;
        oldit = NULL;
        for (tmpit = objlist->first; tmpit != NULL && i != pos;
             i++, tmpit = tmpit->next) {
            oldit = tmpit;
        }
        newit->next = oldit->next;
        oldit->next = newit; 
    }
    objlist->len++;
    return 1;
}
int comps_objlist_insert_at_x(COMPS_ObjList *objlist,
                           unsigned int pos,
                           COMPS_Object *obj) {
    if (!objlist) return -1;
    if (pos > objlist->len) return -1;
    COMPS_ObjListIt *newit = comps_objlist_it_create_x(obj);
    return __comps_objlist_insert_at(objlist, pos, newit);
}
int comps_objlist_insert_at(COMPS_ObjList *objlist,
                           unsigned int pos,
                           COMPS_Object *obj) {
    if (!objlist) return -1;
    if (pos > objlist->len) return -1;
    COMPS_ObjListIt *newit = comps_objlist_it_create(obj);
    return __comps_objlist_insert_at(objlist, pos, newit);
}

int comps_objlist_remove_at(COMPS_ObjList *objlist, unsigned int atpos) {
    int pos;
    COMPS_ObjListIt *it, *itprev = NULL;
    if (!objlist) return 0;

    for (it = objlist->first, pos=0;
         it != NULL && pos != (int)atpos;
         it = it->next, pos++) {
        itprev = it;
    }
    if (!it)
        return 0;
    if (itprev)
        itprev->next = it->next;
    else
        objlist->first = it->next;
    if (it == objlist->last)
        objlist->last = itprev;
    comps_objlist_it_destroy(it);
    objlist->len--;
    return 1;
}

int comps_objlist_remove(COMPS_ObjList *objlist, COMPS_Object *obj) {
    COMPS_ObjListIt *it, *itprev = NULL;
    if (!objlist) return 0;

    for (it = objlist->first; it != NULL && it->comps_obj != obj;
         it = it->next) {
        itprev = it;
    }
    if (!it)
        return 0;
    if (itprev)
        itprev->next = it->next;
    else
        objlist->first = it->next;
    if (it == objlist->last)
        objlist->last = itprev;
    comps_objlist_it_destroy(it);
    objlist->len--;
    return 1;
}

int comps_objlist_index(COMPS_ObjList *objlist, COMPS_Object *obj) {
    int x;
    COMPS_ObjListIt *it;

    for (it = objlist->first, x=0; it != NULL && it->comps_obj != obj;
         it = it->next, x++) {
    };
    if (it == NULL) return -1;
    return x;
}

COMPS_ObjList* comps_objlist_sublist_it(COMPS_ObjListIt *startit,
                                      COMPS_ObjListIt *end) {
    COMPS_ObjList *ret;
    COMPS_ObjListIt *it;
    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    for (it = startit; it != end; it = it->next) {
        comps_objlist_append(ret, it->comps_obj);
    }
    return ret;
}

COMPS_ObjList* comps_objlist_sublist_it_step(COMPS_ObjListIt *startit,
                                           COMPS_ObjListIt *end,
                                           unsigned int step) {
    unsigned int stepc;
    COMPS_ObjList *ret;
    COMPS_ObjListIt *it;

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
    stepc = step;
    for (it = startit; it->next != end; it = it->next, stepc++) {
        if (step == stepc) {
            stepc = 0;
            comps_objlist_append(ret, it->comps_obj);
        }
    }
    return ret;
}

COMPS_ObjList* comps_objlist_sublist_indexed(COMPS_ObjList *objlist,
                                           unsigned int start,
                                           unsigned int end) {
    unsigned int pos;
    COMPS_ObjList *ret;
    COMPS_ObjListIt *it;
    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    for (it = objlist->first, pos=0;
         it != NULL && pos != start;
         it = it->next, pos++);
    if (!it)
        return ret;
    for (; it->next != NULL && pos != end; it = it->next, pos++) {
        comps_objlist_append(ret, it->comps_obj);
    }
    return ret;
}

COMPS_ObjList* comps_objlist_sublist_indexed_step(COMPS_ObjList *objlist,
                                               unsigned int start,
                                               unsigned int end,
                                               unsigned int step) {
    unsigned int pos;
    unsigned int stepc;
    COMPS_ObjList *ret;
    COMPS_ObjListIt *it;
    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    pos = 0;
    stepc = 0;
    for (it = objlist->first;
         it != NULL && pos != start;
         it = it->next, pos++);
    if (!it)
        return ret;
    for (; it->next != NULL && pos != end; it = it->next, pos++, stepc++) {
        if (stepc == step) {
            step = 0;
            comps_objlist_append(ret, it->comps_obj);
        }
    }
    return ret;
}

COMPS_ObjList* comps_objlist_filter(COMPS_ObjList *list,
                                  char (*filter_func)(COMPS_Object*)) {
    COMPS_ObjList *ret;
    unsigned int i = 0;
    COMPS_ObjListIt *it;

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    for (i=0, it = list->first; i < list->len; it = it->next, i++) {
        if (filter_func(it->comps_obj))
            comps_objlist_append(ret, it->comps_obj);
    }
    return ret;
}

COMPS_ObjList* comps_objlist_concat(COMPS_ObjList *list1, COMPS_ObjList *list2) {
    COMPS_ObjList *ret;
    COMPS_ObjListIt *it;

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
    for (it = list1->first; it != NULL; it = it->next) {
        comps_objlist_append(ret, it->comps_obj);
    }
    for (it = list2->first; it != NULL; it = it->next) {
        comps_objlist_append(ret, it->comps_obj);
    }
    return ret;
}

void comps_objlist_concat_in(COMPS_ObjList *list1, COMPS_ObjList *list2) {
    COMPS_ObjListIt *it;
    for (it = list2->first; it != NULL; it = it->next) {
        comps_objlist_append(list1, it->comps_obj);
    }
}

signed char comps_objlist_cmp(COMPS_Object *list1, COMPS_Object *list2) {
    COMPS_ObjListIt *it, *it2;
    if (!list1 || !list2) return -1;
    it =  ((COMPS_ObjList*)list1)->first;
    it2 =  ((COMPS_ObjList*)list2)->first;

    for (; it != NULL && it2 != NULL; it = it->next, it2 = it2->next) {
        if (!comps_object_cmp(it->comps_obj, it2->comps_obj))
            return 0;
    }
    if (!it && !it2)
        return 1;
    else
        return 0;
}

int comps_objlist_set(COMPS_ObjList *objlist, unsigned int atpos,
                      COMPS_Object *obj) {
    COMPS_ObjListIt *it;
    unsigned int i = 0;

    if (!objlist) return -1;
    it =  ((COMPS_ObjList*)objlist)->first;

    for (; it != NULL && i != atpos; it = it->next, i++);
    if (!it)
        return -1;
    COMPS_OBJECT_DESTROY(it->comps_obj);
    it->comps_obj = comps_object_incref(obj);
    return 0;
}

char* comps_objlist_tostr_u(COMPS_Object* list) {
    char *items[((COMPS_ObjList*)list)->len];
    char *tmps, *ret;
    int i=0, total_strlen = 10;//, total2;
    const int sep_len = strlen(", ");

    COMPS_ObjListIt *it = ((COMPS_ObjList*)list)->first;
    if (it) {
        for (; it != ((COMPS_ObjList*)list)->last; it = it->next, i++) {
            tmps = comps_object_tostr(it->comps_obj);
            items[i] = tmps;
            total_strlen += sep_len + strlen(tmps);
        }
        tmps = comps_object_tostr(it->comps_obj);
        items[i] = tmps;
        total_strlen += strlen(tmps);
    }
    //printf("total len:%d", total_strlen);
    //printf("list len:%d\n", ((COMPS_ObjList*)list)->len);
    //total2=0;
    ret = malloc(sizeof(char) * (total_strlen));
    ret[0]=0;
    strcat(ret, "[");
    //total2 += strlen("[");
    if (((COMPS_ObjList*)list)->len) {
        for (i = 0; i < (int)(((COMPS_ObjList*)list)->len-1); i++) {
            strcat(ret, items[i]);
            //total2 += strlen(items[i]);
            strcat(ret, ", ");
            //total2 += strlen(", ");
            free(items[i]);
        }
        strcat(ret, items[i]);
        //total2 += strlen(items[i]);
        free(items[i]);
    }
    strcat(ret, "]");
    //total2 += strlen("]");
    //printf("total len2:%d\n", total2);
    return ret;
}


COMPS_ObjectInfo COMPS_ObjList_ObjInfo = {
    .obj_size = sizeof(COMPS_ObjList),
    .constructor = &comps_objlist_create_u,
    .destructor = &comps_objlist_destroy_u,
    .copy = &comps_objlist_copy_u,
    .obj_cmp = &comps_objlist_cmp,
    .to_str = &comps_objlist_tostr_u
};
