#ifndef COMPS_LOG_H
#define COMPS_LOG_H

#include <stdarg.h>

#include "comps_log_codes.h"
#include "comps_hslist.h"
#include "comps_types.h"

struct COMPS_LogEntry {
    COMPS_Object **args;
    int arg_count;
    int code;
    int type;
};

struct COMPS_Log {
    COMPS_Object_HEAD;
    COMPS_HSList *entries;
    char std_out;
};
COMPS_Object_TAIL(COMPS_Log);

void comps_log_create(COMPS_Log *log, COMPS_Object **args);
void comps_log_create_u(COMPS_Object *log, COMPS_Object **args);

void comps_log_destroy(COMPS_Log *log);
void comps_log_destroy_u(COMPS_Object *log);

COMPS_LogEntry *comps_log_entry_create();
void comps_log_entry_destroy();
char* comps_log_entry_str(COMPS_LogEntry *log_entry);


void comps_log_error(COMPS_Log *log, int code, int n, ...);
void comps_log_error_x(COMPS_Log *log, int code, int n, ...);
void comps_log_warning(COMPS_Log *log, int code, int n, ...);
void comps_log_warning_x(COMPS_Log *log, int code, int n, ...);
void comps_log_print(COMPS_Log *log);

extern const char * COMPS_LogCodeFormat[];
//extern COMPS_ObjectInfo COMPS_Log_ObjInfo;

#endif

