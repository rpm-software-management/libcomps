#include "comps_log.h"

const char* COMPS_LogCodeFormat[] = {
      [COMPS_ERR_NO_ERR] = "",
      [COMPS_ERR_ELEM_UNKNOWN] = "WARNING: Unknown elemenet '%s' at line:%s\n",
      [COMPS_ERR_ATTR_UNKNOWN] = "WARNING: Unknown attribute '%s' in element "
                                 "'%s' at line:%s col: %s\n",
      [COMPS_ERR_ELEM_ALREADYSET] = "WARNING: Element <%s> at line:%s "
                                     "col:%s has been already found\n",
      [COMPS_ERR_PARSER] = "ERROR: Parser error at line:%s col:%s\n    %s\n",
      [COMPS_ERR_DEFAULT_PARAM] = "WARNING: Wrong default param value '%s' "
                                   "at line:%s col:%s\n",
      [COMPS_ERR_USERVISIBLE_PARAM] = "WARNING: Wrong uservisible param value"
                                        "'%s' at line:%s col:%s\n",
      [COMPS_ERR_PACKAGE_UNKNOWN] = "WARNING: Unkonwn package type '%s' "
                                     "at line:%s col:%s\n",
      [COMPS_ERR_DEFAULT_MISSING] = "WARNING: <default> subelement required"
                                     " at line:%s col:%s\n",
      [COMPS_ERR_USERVISIBLE_MISSING] = "WARNING: <uservisible> subelement"
                                         "required at line:%s col%s\n",
      [COMPS_ERR_NAME_MISSING] = "ERROR: '<name>' subelement required at "
                                  "line:%s col:%s\n",
      [COMPS_ERR_ID_MISSING] = "WARNING: '<id>' subelement required at "
                                "line:%s col:%s\n",
      [COMPS_ERR_DESC_MISSING] = "WARNING: '<description>' subelement required"
                                  " at line:%s col:%s\n",
      [COMPS_ERR_GROUPLIST_NOTSET] = "WARNING: '<grouplist>' subelement"
                                  " not parsed and groupid found at line:%s col:%s\n",
      [COMPS_ERR_OPTIONLIST_NOTSET] = "WARNING: '<optionlist>' subelement"
                                  " not parsed and groupid found at line:%s col:%s\n",
      [COMPS_ERR_NOPARENT] = "ERROR: <%s> found with no suitable parent"
                                      " at line:%s col:%s\n",
      [COMPS_ERR_MALLOC] = "Malloc failed. Cannot continue\n",
      [COMPS_ERR_READFD] = "Can't read file\n",
      [COMPS_ERR_WRITEF] = "Can't write file %s\n",
      [COMPS_ERR_XMLGEN] = "Can't generate xml\n",
      [COMPS_ERR_ELEM_REQUIRED] = "Element %s is required at line:%s "
                                    "column :%s but missing\n",
      [COMPS_ERR_LIST_EMPTY] = "%s is empty at line:%s column :%s\n",
      [COMPS_ERR_TEXT_BETWEEN] = "'%s' found between elements at line:"
                                  "%s column :%s\n",
      [COMPS_ERR_NOCONTENT] = "<%s> content missing at line:%s "
                               "column:%s\n",
      [COMPS_ERR_PKGLIST_EMPTY] = "Group with id %s has no packages."
                                  "skipping xml output\n",
      [COMPS_ERR_GROUPIDS_EMPTY] = "Category with id %s has no group ids."
                                  "skipping xml output\n",
      [COMPS_ERR_IDS_EMPTY] = "Environment with id %s has no group ids and no ."
                                  "option ids. Skipping xml output\n"
};

void comps_log_create(COMPS_Log *log, COMPS_Object **args){
    log->std_out = 0;
    if (args != NULL && args[0]->obj_info == &COMPS_Num_ObjInfo) {
        if (((COMPS_Num*)args[0])->val) {
            log->std_out = 1;
        }
    }
    log->entries = comps_hslist_create();
    comps_hslist_init(log->entries, NULL, NULL, comps_log_entry_destroy);
}
void comps_log_create_u(COMPS_Object *log, COMPS_Object **args) {
    comps_log_create((COMPS_Log*)log, args);
}

void comps_log_destroy(COMPS_Log *log) {
    comps_hslist_destroy(&log->entries);
}
void comps_log_destroy_u(COMPS_Object *log) {
    comps_log_destroy((COMPS_Log*)log);
}

COMPS_LogEntry* comps_log_entry_create() {
    COMPS_LogEntry *ret;
    ret = malloc(sizeof(COMPS_LogEntry));
    if (!ret) return NULL;
    ret->args = NULL;
    return ret;
}

void comps_log_entry_destroy(COMPS_LogEntry *log_entry) {
    for (int i=0; i<log_entry->arg_count; i++) {
        comps_object_destroy(log_entry->args[i]);
    }
    free(log_entry->args);
    free(log_entry);
}

inline static COMPS_LogEntry * __comps_log_entry_prep(int code, int type, int n){
    COMPS_LogEntry *entry;

    entry = comps_log_entry_create();

    entry->args = malloc(sizeof(COMPS_Object*) * n);
    entry->arg_count = n;
    entry->code = code;
    entry->type = type;
    return entry;
}

static void __comps_log_entry(COMPS_Log *log, int code, int type, int n,
                             va_list va){
    COMPS_LogEntry *entry;
    COMPS_Object *val;
    char *str;

    entry = __comps_log_entry_prep(code, type, n);
    for (int i=0; i<n; i++) {
        val = va_arg(va, COMPS_Object*);
        entry->args[i] = comps_object_copy(val);
    }
    if (log->std_out) {
        str = comps_log_entry_str(entry);
        fprintf(stderr, "%s", str);
        free(str);
    }
    comps_hslist_append(log->entries, entry, 0);
}

static void __comps_log_entry_x(COMPS_Log *log, int code, int type, int n,
                             va_list va){
    COMPS_LogEntry *entry;
    COMPS_Object *val;
    char *str;
    entry = __comps_log_entry_prep(code, type, n);
    for (int i=0; i<n; i++) {
        val = va_arg(va, COMPS_Object*);
        entry->args[i] = val;
    }
    if (log->std_out) {
        str = comps_log_entry_str(entry);
        fprintf(stderr, "%s", str);
        free(str);
    }
    comps_hslist_append(log->entries, entry, 0);
}

void comps_log_error(COMPS_Log *log, int code, int n, ...) {
    va_list list;
    va_start(list, n);
    __comps_log_entry(log, code, COMPS_LOG_ENTRY_ERR, n, list);
    va_end(list);
}
void comps_log_error_x(COMPS_Log *log, int code, int n, ...) {
    va_list list;
    va_start(list, n);
    __comps_log_entry_x(log, code, COMPS_LOG_ENTRY_ERR, n, list);
    va_end(list);
}

void comps_log_warning(COMPS_Log *log, int code, int n, ...) {
    va_list list;
    va_start(list, n);
    __comps_log_entry(log, code, COMPS_LOG_ENTRY_WAR, n, list);
    va_end(list);
}
void comps_log_warning_x(COMPS_Log *log, int code, int n, ...) {
    va_list list;
    va_start(list, n);
    __comps_log_entry_x(log, code, COMPS_LOG_ENTRY_WAR, n, list);
    va_end(list);
}


static char ** __comps_log_entry_out(COMPS_LogEntry *log_entry, int *total_len) {
    char **args;
    args = malloc(sizeof(char*) * log_entry->arg_count);
    *total_len = 0;

    for (int i = 0; i < log_entry->arg_count; i++) {
        args[i] = comps_object_tostr((COMPS_Object*)log_entry->args[i]);
        *total_len += strlen(args[i]);
    }
    return args;
}

char* comps_log_entry_str(COMPS_LogEntry *log_entry) {
    char **args;
    char *ret;
    int total_len;

    args = __comps_log_entry_out(log_entry, &total_len);
    ret = malloc(sizeof(char) * (strlen(COMPS_LogCodeFormat[log_entry->code]) +
                                 total_len + 1 - (2*log_entry->arg_count)));
    expand_s(ret, COMPS_LogCodeFormat[log_entry->code],
           args,
           log_entry->arg_count);
    for (int i = 0; i < log_entry->arg_count; i++) {
        free(args[i]);
    }
    free(args);
    return ret;
}

void comps_log_entry_print(COMPS_LogEntry *log_entry) {
    int total_len;
    char **args;

    args = __comps_log_entry_out(log_entry, &total_len);
    expand_out(COMPS_LogCodeFormat[log_entry->code],
           args,
           log_entry->arg_count);
    printf("\n");
    for (int i = 0; i < log_entry->arg_count; i++) {
        free(args[i]);
    }
    free(args);
}

void comps_log_print(COMPS_Log *log) {
    COMPS_HSListItem *hsit;
    for (hsit = log->entries->first; hsit != NULL; hsit = hsit->next) {
        comps_log_entry_print(hsit->data);
    }
}

COMPS_ObjectInfo COMPS_Log_ObjInfo = {
    .obj_size = sizeof(COMPS_Log),
    .constructor = &comps_log_create_u,
    .destructor = &comps_log_destroy_u,
    //.deep_copy = &comps_log_copy_u
};
