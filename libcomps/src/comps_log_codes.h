#ifndef COMPS_LOG_CODES_H
#define COMPS_LOG_CODES_H

#include <stdio.h>
#include <stdarg.h>

#include "comps_obj.h"
#include "comps_utils.h"
#define COMPS_LOG_ENTRY_ERR          0
#define COMPS_LOG_ENTRY_WAR          1

#define COMPS_ERR_NO_ERR                1
#define COMPS_ERR_ELEM_UNKNOWN          2
#define COMPS_ERR_ELEM_ALREADYSET       3
#define COMPS_ERR_PARSER                4
#define COMPS_ERR_DEFAULT_PARAM         5
#define COMPS_ERR_USERVISIBLE_PARAM     6
#define COMPS_ERR_PACKAGE_UNKNOWN       7
#define COMPS_ERR_DEFAULT_MISSING       8
#define COMPS_ERR_USERVISIBLE_MISSING   9
#define COMPS_ERR_NAME_MISSING          10
#define COMPS_ERR_ID_MISSING            11
#define COMPS_ERR_DESC_MISSING          12
#define COMPS_ERR_GROUPLIST_NOTSET      13
#define COMPS_ERR_OPTIONLIST_NOTSET     14
#define COMPS_ERR_GROUPIDS_EMPTY        15
#define COMPS_ERR_NOPARENT              16
#define COMPS_ERR_MALLOC                17
#define COMPS_ERR_READFD                18
#define COMPS_ERR_WRITEF                19
#define COMPS_ERR_XMLGEN                20
#define COMPS_ERR_ELEM_REQUIRED         21
#define COMPS_ERR_LIST_EMPTY            22
#define COMPS_ERR_TEXT_BETWEEN          23
#define COMPS_ERR_NOCONTENT             24
#define COMPS_ERR_PKGLIST_EMPTY         25
#define COMPS_ERR_IDS_EMPTY             26
#define COMPS_ERR_ATTR_UNKNOWN          27

#define LOG_TEST_CODE1              1001
#define LOG_TEST_CODE2              1002
#define LOG_TEST_CODE3              1003
#define LOG_TEST_CODE4              1004
#define LOG_TEST_CODE5              1005
#define LOG_TEST_CODE6              1006



void __expand(char *str, const char *fmt, char out, ...);

void expand0(char *str, const char *fmt, char **args, char out);
void expand1(char *str, const char *fmt, char **args, char out);
void expand2(char *str, const char *fmt, char **args, char out);
void expand3(char *str, const char *fmt, char **args, char out);
void expand4(char *str, const char *fmt, char **args, char out);
void expand5(char *str, const char *fmt, char **args, char out);

void expand(char *str, const char *fmt, char **args, int len, int out);

void expand_out(const char *fmt, char **args, int len);
void expand_s(char *str, const char *fmt, char **args, int len);

#endif
