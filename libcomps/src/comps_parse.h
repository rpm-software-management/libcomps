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

#ifndef COMPS_PARSE_H
#define COMPS_PARSE_H
#include <stdio.h>
#include <signal.h>

#include "comps_hslist.h"
#include "comps_obj.h"
#include "comps_doc.h"
#include "comps_types.h"
#include "comps_log.h"
#include "comps_default.h"

#include <expat.h>
#include <libxml/parser.h>

typedef struct COMPS_Parsed {
    COMPS_HSList *elem_stack;
    COMPS_Doc *comps_doc;
    COMPS_HSList *text_buffer;
    unsigned int text_buffer_len;
    char **text_buffer_pt;
    char *tmp_buffer;
    COMPS_Log *log;
    char fatal_error;
    XML_Parser parser;
    const char *enc;
    COMPS_DefaultsOptions *def_options;

    COMPS_Str *doctype_name;
    COMPS_Str *doctype_sysid;
    COMPS_Str *doctype_pubid;
} COMPS_Parsed;

COMPS_Parsed* comps_parse_parsed_create();
void comps_parse_parsed_reinit(COMPS_Parsed *parsed);
void comps_parse_parsed_destroy(COMPS_Parsed *parsed);
unsigned comps_parse_parsed_init(COMPS_Parsed * parsed, const char * encoding,
                                 char log_stdout);

unsigned __comps_is_whitespace_only(const char * s, int len);

void comps_parse_end_elem_handler(void *userData, const XML_Char *s);
void comps_parse_def_handler(void *userData, const XML_Char *s, int len);
void comps_parse_start_elem_handler(void *userData,
                              const XML_Char *s,
                              const XML_Char **attrs);
void comps_parse_char_data_handler(void *userData,
                            const XML_Char *s,
                            int len);
void comps_parse_start_doctype(void *userData,
                               const XML_Char *doctypeName,
                               const XML_Char *sysid,
                               const XML_Char *pubid,
                               int standalone);

signed char comps_parse_file(COMPS_Parsed *parsed, FILE *f,
                             COMPS_DefaultsOptions *options);
signed char comps_parse_str(COMPS_Parsed *parsed, char *str,
                            COMPS_DefaultsOptions *options);

unsigned comps_parse_init_parser(XML_Parser *p);
void comps_parse_parsed_destroy(COMPS_Parsed *parsed);
int comps_parse_validate_dtd(char *filename, char *dtd_file);

#endif
