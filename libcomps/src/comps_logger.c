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

#include "comps_logger.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

__COMPS_LoggerMsg COMPS_LogMessages[] = {
      [COMPS_ERR_NO_ERR] = {.format ="",
                            .usedm=0,.used1=0,.used2=0,.used3=0},
      [COMPS_ERR_ELEM_UNKNOWN] = {.format="WARNING: Unknown elemenet '%s' at line:%d\n",
                                  .usedm=1,.used1=2,.used2=0,.used3=0},
      [COMPS_ERR_ELEM_ALREADYSET] = {.format="WARNING: Element <%s> at line:%d "
                                     "col:%d has been already found\n",
                                  .usedm=1,.used1=2,.used2=3,.used3=0},
      [COMPS_ERR_PARSER] = {.format="ERROR: Parser error at line:%d col:%d\n    %s\n",
                            .usedm=3,.used1=1,.used2=2,.used3=0},
      [COMPS_ERR_DEFAULT_PARAM] = {.format="WARNING: Wrong default param value '%s' "
                                   "at line:%d col:%d\n",
                                   .usedm=1,.used1=2,.used2=3,.used3=0},
      [COMPS_ERR_USERVISIBLE_PARAM] = {.format="WARNING: Wrong uservisible param value"
                                        "'%s' at line:%d col:%d\n",
                                        .usedm=1,.used1=2,.used2=3,.used3=0},
      [COMPS_ERR_PACKAGE_UNKNOWN] = {.format="WARNING: Unkonwn package type '%s' "
                                     "at line:%d col:%d\n",
                                     .usedm=1,.used1=2,.used2=3,.used3=0},
      [COMPS_ERR_DEFAULT_MISSING] = {.format="WARNING: <default> subelement required"
                                     " at line:%d col:%d\n",
                                        .usedm=0,.used1=1,.used2=2,.used3=0},
      [COMPS_ERR_USERVISIBLE_MISSING] = {.format="WARNING: <uservisible> subelement"
                                         "required at line:%d col%d\n",
                                         .usedm=0,.used1=1,.used2=2,.used3=0},
      [COMPS_ERR_NAME_MISSING] = {.format="ERROR: '<name>' subelement required at "
                                  "line:%d col:%d\n",
                                  .usedm=0,.used1=1,.used2=2,.used3=0},
      [COMPS_ERR_ID_MISSING] = {.format="WARNING: '<id>' subelement required at "
                                "line:%d col:%d\n",
                                .usedm=0,.used1=1,.used2=2,.used3=0},
      [COMPS_ERR_DESC_MISSING] = {.format="WARNING: '<description>' subelement required"
                                  " at line:%d col:%d\n",
                                  .usedm=0,.used1=1,.used2=2,.used3=0},
      [COMPS_ERR_GROUPLIST_NOTSET] = {.format="WARNING: '<grouplist>' subelement"
                                  " not parsed and groupid found at line:%d col:%d\n",
                                  .usedm=0,.used1=1,.used2=2,.used3=0},
      [COMPS_ERR_OPTIONLIST_NOTSET] = {.format="WARNING: '<optionlist>' subelement"
                                  " not parsed and groupid found at line:%d col:%d\n",
                                  .usedm=0,.used1=1,.used2=2,.used3=0},
      [COMPS_ERR_GROUPIDS_EMPTY] = {.format="ERROR: Missing <groupid> elements\n",
                                     .usedm=0,.used1=0,.used2=0,.used3=0},
      [COMPS_ERR_NOPARENT] = {.format="ERROR: <%s> found with no suitable parent"
                                      " at line:%d col:%d\n",
                                     .usedm=1,.used1=2,.used2=3,.used3=0},
      [COMPS_ERR_MALLOC] = {.format="Malloc failed. Cannot continue\n",
                            .usedm=0,.used1=0,.used2=0,.used3=0},
      [COMPS_ERR_READFD] = {.format="Can't read file\n",
                            .usedm=0,.used1=0,.used2=0,.used3=0},
      [COMPS_ERR_WRITEF] = {.format="Can't write file %s\n",
                            .usedm=1,.used1=0,.used2=0,.used3=0},
      [COMPS_ERR_XMLGEN] = {.format="Can't generate xml\n",
                            .usedm=0,.used1=0,.used2=0,.used3=0},
      [COMPS_ERR_ELEM_REQUIRED] = {.format="Element %s is required at line:%d "
                                    "column :%d but missing\n", .usedm=1,
                                    .used1=2, .used2=3, .used3=0},
      [COMPS_ERR_LIST_EMPTY] = {.format="%s is empty at line:%d column :%d\n",
                                .usedm=1, .used1=2, .used2=3, .used3=0},
      [COMPS_ERR_TEXT_BETWEEN] = {.format="'%s' found between elements at line:"
                                  "%d column :%d\n",
                                .usedm=1, .used1=2, .used2=3, .used3=0},
      [COMPS_ERR_NOCONTENT] = {.format="<%s> content missing at line:%d "
                               "column:%d\n", .usedm=1, .used1=2, .used2=3,
                               .used3=0}
};


/**
 * COMPS_Logger constructor
 * @param redirect2output if set to non-zero, log messages will be printed
 * to stdout/stderr
 * @return COMPS_Logger object
 */
COMPS_Logger * comps_log_create(unsigned redirect2output)
{
    COMPS_Logger * log;
    if ((log = malloc(sizeof(COMPS_Logger))) == NULL) {
        return NULL;
    }
    log->logger_data = comps_list_create();
    comps_list_init(log->logger_data);
    log->redirect2output = redirect2output;
    return log;
}

/**
 * COMPS_Logger destructor
 * @param log COMPS_Logger object
 */
void comps_log_destroy(COMPS_Logger * log) {
    if (log == NULL) return;
    comps_list_destroy((COMPS_List**)&log->logger_data);
    free(log);
}

void comps_log_print(COMPS_LoggerEntry *entry) {
    char *str  = comps_log_entry_str(entry);
    printf("%s", str);
    free(str);
}


/**
 * Logger entry constructor
 * @param msg optional log entry message
 * @param type log type(info,warning,error)
 * @param code errno of log entry
 * @param optcode1 optional log entry code 1
 * @param optcode2 optional log entry code 2
 * @param optcode3 optional log entry code 3
 * @return COMPS_LoggerEntry object
 */
COMPS_LoggerEntry * comps_log_entry_create(const char * msg, unsigned type,
                     COMPS_LogErrCode code, unsigned int optcode1,
                     unsigned int optcode2, unsigned int optcode3) {
    COMPS_LoggerEntry * entry;

    if ((entry = malloc(sizeof(COMPS_LoggerEntry))) == NULL)
        return NULL;
    if (msg) {
        if ((entry->log_message =
             malloc((strlen(msg)+1)*sizeof(char))) == NULL) {
            free(entry);
            return NULL;
        }
        memcpy((void*)entry->log_message,
               (void*)msg, sizeof(char)*(strlen((char*)msg)+1));
    } else
        entry->log_message = NULL;
    entry->type = type;
    entry->code = code;
    entry->opt_code1 = optcode1;
    entry->opt_code2 = optcode2;
    entry->opt_code3 = optcode3;
    return entry;
}

void comps_log_entry_destroy(COMPS_LoggerEntry *entry) {
    free(entry->log_message);
    free(entry);
}

inline void comps_log_entry_destroy_v(void *entry) {
    comps_log_entry_destroy((COMPS_LoggerEntry*)entry);
}


/**
 * Return string representation of logger entry message. Returned string
 * need to be freed
 * @param entry COMPS_LoggerEntry object
 * @return string representation of log entry
 */
char * comps_log_entry_str(COMPS_LoggerEntry *entry) {
    unsigned int len;
    char * ret;
    union {
        char *s;
        int i;
    } pitem[4];

    len = strlen(COMPS_LogMessages[entry->code].format);
    if (COMPS_LogMessages[entry->code].usedm != 0) {
        if (entry->log_message)
            len += strlen(entry->log_message) -2;
        else
            len += -2; // empty message
    }
    if (COMPS_LogMessages[entry->code].used1 != 0) {
        if (entry->opt_code1)
            len += (int)ceil(log10(entry->opt_code1));
        else
            len++;
        len -= 2;
    }
    if (COMPS_LogMessages[entry->code].used2 != 0) {
        if (entry->opt_code2) {
            len += (int)ceil(log10(entry->opt_code2));
        } else
            len++;
        len -= 2;
    }
    if (COMPS_LogMessages[entry->code].used3 != 0) {
        if (entry->opt_code3)
            len += (int)ceil(log10(entry->opt_code3));
        else
            len++;
        len -= 2;
    }
    ret = malloc(sizeof(char)*(len+1));
    if (ret == NULL) return NULL;
    if (COMPS_LogMessages[entry->code].usedm)
        pitem[COMPS_LogMessages[entry->code].usedm-1].s = entry->log_message?
                                                          entry->log_message:
                                                          "";
    if (COMPS_LogMessages[entry->code].used1)
        pitem[COMPS_LogMessages[entry->code].used1-1].i = entry->opt_code1;
    if (COMPS_LogMessages[entry->code].used2)
        pitem[COMPS_LogMessages[entry->code].used2-1].i = entry->opt_code2;
    if (COMPS_LogMessages[entry->code].used3)
        pitem[COMPS_LogMessages[entry->code].used3-1].i = entry->opt_code3;
    sprintf(ret, COMPS_LogMessages[entry->code].format,
            pitem[0], pitem[1], pitem[2], pitem[3]);
    return ret;
}

/**
 * Log new error message
 * @param log COMPS_Logger object
 * @param msg optional log entry message
 * @param code errno of log entry
 * @param optcode1 optional log entry code 1
 * @param optcode2 optional log entry code 2
 * @param optcode3 optional log entry code 3
 */
void comps_log_error(COMPS_Logger * log, const char * msg,
                     COMPS_LogErrCode code, unsigned int optcode1,
                     unsigned int optcode2, unsigned int optcode3) {
    COMPS_ListItem *it;
    COMPS_LoggerEntry *lentry;
    if (log == NULL) return;

    lentry = comps_log_entry_create(msg, COMPS_LOG_ERROR, code,
                                    optcode1, optcode2, optcode3);
    it = comps_list_item_create(lentry, NULL, &comps_log_entry_destroy_v);

    comps_list_append(log->logger_data, it);
    if (log->redirect2output)
        comps_log_print(lentry);
    //if (code == COMPS_ERR_MALLOC) {
    //    assert(1==0);
    //}
}

/**
 * Log new warning message
 * @param log COMPS_Logger object
 * @param msg optional log entry message
 * @param code errno of log entry
 * @param optcode1 optional log entry code 1
 * @param optcode2 optional log entry code 2
 * @param optcode3 optional log entry code 3
 */
void comps_log_warning(COMPS_Logger * log, const char * msg,
                     COMPS_LogErrCode code, unsigned int optcode1,
                     unsigned int optcode2, unsigned int optcode3) {
    COMPS_ListItem * it;
    COMPS_LoggerEntry *lentry;
    if (log == NULL) return;

    lentry = comps_log_entry_create(msg, COMPS_LOG_WARNING, code,
                                    optcode1, optcode2, optcode3);
    it = comps_list_item_create(lentry, NULL, &comps_log_entry_destroy_v);
    comps_list_append(log->logger_data, it);
    if (log->redirect2output)
        comps_log_print(lentry);
}

/**
 * Log new info message
 * @param log COMPS_Logger object
 * @param msg optional log entry message
 * @param code errno of log entry
 * @param optcode1 optional log entry code 1
 * @param optcode2 optional log entry code 2
 * @param optcode3 optional log entry code 3
 */
void comps_log_info(COMPS_Logger * log, const char * msg, COMPS_LogErrCode code,
                     unsigned int optcode1, unsigned int optcode2,
                     unsigned int optcode3) {
    if (log == NULL) return;
    COMPS_ListItem * it;
    it = comps_list_item_create(comps_log_entry_create(msg, COMPS_LOG_INFO,
                                                       code, optcode1,
                                                       optcode2, optcode3),
                                NULL, &comps_log_entry_destroy_v);
    comps_list_append(log->logger_data, it);
}

/**
 * Return string representaiton of all log entries. Returned string need
 * to be freed
 * @param logger COMPS_Logger object
 * @return string representation of log entries
 */
char* comps_log_str(COMPS_Logger *logger) {

    unsigned int total_len = 1;
    unsigned int pos = 0;
    unsigned int len;
    COMPS_ListItem *it;
    char *ret, *entry_str;
    for (it = logger->logger_data->first; it != NULL; it = it->next) {
        total_len += strlen(COMPS_LogMessages[((COMPS_LoggerEntry*)it->data)
                                              ->code].format);
        if (COMPS_LogMessages[((COMPS_LoggerEntry*)it->data)->code].usedm) {
            total_len += strlen(((COMPS_LoggerEntry*)it->data)->log_message);
            total_len -=2;
        }
        if (COMPS_LogMessages[((COMPS_LoggerEntry*)it->data)->code].used1) {
            total_len += ceil(log10(((COMPS_LoggerEntry*)it->data)->opt_code1));
            total_len -=2;
        }
        if (COMPS_LogMessages[((COMPS_LoggerEntry*)it->data)->code].used2) {
            total_len += ceil(log10(((COMPS_LoggerEntry*)it->data)->opt_code2));
            total_len -=2;
        }
        if (COMPS_LogMessages[((COMPS_LoggerEntry*)it->data)->code].used3) {
            total_len += ceil(log10(((COMPS_LoggerEntry*)it->data)->opt_code3));
            total_len -=2;
        }
    }
    ret = malloc(sizeof(char)*total_len);
    if (ret == NULL) return NULL;
    for (it = logger->logger_data->first; it != NULL; it = it->next) {
        entry_str = comps_log_entry_str((COMPS_LoggerEntry*)it->data);
        len = strlen(entry_str);
        memcpy(ret+pos, entry_str, len*sizeof(char));
        free(entry_str);
        pos += len;
    }
    ret[total_len-1]=0;
    return ret;
}
