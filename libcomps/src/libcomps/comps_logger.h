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
 * along with this program; if not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 */

/*! \file comps_logger.h
 * \brief A Documented file.
 * Details.
 * */

#ifndef COMPS_LOGGER_H
#define COMPS_LOGGER_H
#include "comps_list.h"
#include "comps_elem.h"
#include "comps_types.h"

typedef enum {COMPS_LOG_INFO, COMPS_LOG_ERROR,
              COMPS_LOG_WARNING} COMPS_LogEntryType;

/**
 * COMPS_Logger entry
 * @see comps_log_entry_create
 * @see comps_log_entry_str
 */
typedef struct {
    /** log entry message*/
    char * log_message;
    /** log entry errno @see COMPS_LogErrCode*/
    unsigned int code;
    /** log entry optional code1*/
    unsigned int opt_code1;
    /** log entry optional code2*/
    unsigned int opt_code2;
    /** log entry optional code3*/
    unsigned int opt_code3;
    /** log entry type*/
    unsigned type;
} COMPS_LoggerEntry;

//typedef struct COMPS_Logger COMPS_Logger;

typedef struct __COMPS_LoggerMsg {
    unsigned usedm : 2;
    unsigned used1 : 2;
    unsigned used2 : 2;
    unsigned used3 : 2;
    char * format;
} __COMPS_LoggerMsg;

/**
 * COMPS_Logger
 * @see comps_log_create
 * @see comps_log_destroy
 * @see comps_log_warning
 * @see comps_log_error
 * @see comps_log_info
 * @see comps_log_str
 */
struct COMPS_Logger {
    /** COMPS_List object of COMPS_LoggerEntry @see COMPS_LoggerEntry*/
    COMPS_List * logger_data;
    /** redirection to output flag*/
    unsigned redirect2output;
};

typedef enum {COMPS_ERR_NO_ERR,
              COMPS_ERR_ELEM_UNKNOWN,
              COMPS_ERR_ELEM_ALREADYSET,
              COMPS_ERR_PARSER,
              COMPS_ERR_DEFAULT_PARAM,
              COMPS_ERR_USERVISIBLE_PARAM,
              COMPS_ERR_PACKAGE_UNKNOWN,
              COMPS_ERR_DEFAULT_MISSING,
              COMPS_ERR_USERVISIBLE_MISSING,
              COMPS_ERR_NAME_MISSING,
              COMPS_ERR_ID_MISSING,
              COMPS_ERR_DESC_MISSING,
              COMPS_ERR_GROUPIDS_EMPTY,
              COMPS_ERR_DTD_ELEM,
              COMPS_ERR_DTD_SEQUENCE,
              COMPS_ERR_DTD_ATTRS,
              COMPS_ERR_DTD_NOCHILD,
              COMPS_ERR_MALLOC,
              COMPS_ERR_READFD,
              COMPS_ERR_WRITEF,
              COMPS_ERR_XMLGEN,
              COMPS_ERR_NOPARENT,
              COMPS_ERR_GROUPLIST_NOTSET,
              COMPS_ERR_OPTIONLIST_NOTSET,
              COMPS_ERR_ELEM_REQUIRED,
              COMPS_ERR_LIST_EMPTY,
              COMPS_ERR_TEXT_BETWEEN,
              COMPS_ERR_NOCONTENT} COMPS_LogErrCode;

COMPS_Logger * comps_log_create(unsigned redirect2out);
void comps_log_destroy(COMPS_Logger * log);

COMPS_LoggerEntry * comps_log_entry_create(const char *msg, unsigned type,
                     COMPS_LogErrCode code, unsigned int optcode1,
                     unsigned int optcode2, unsigned int optcode3);
char * comps_log_entry_str(COMPS_LoggerEntry *entry);
void comps_log_error(COMPS_Logger *logger, const char *msg,
                     COMPS_LogErrCode code, unsigned int optcode1,
                     unsigned int optcode2, unsigned int optcode3);
void comps_log_warning(COMPS_Logger *logger, const char *msg,
                       COMPS_LogErrCode code, unsigned int optcode1,
                       unsigned int optcode2, unsigned int optcode3);
void comps_log_info(COMPS_Logger *logger, const char *msg,
                    COMPS_LogErrCode code, unsigned int optcode1,
                    unsigned int optcode2, unsigned int optcode3);
void comps_log_entry_destroy(COMPS_LoggerEntry *entry);
void comps_log_entry_destroy_v(void *entry);
char* comps_log_str(COMPS_Logger *logger);

#endif

