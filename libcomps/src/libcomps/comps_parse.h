#ifndef COMPS_PARSE_H
#define COMPS_PARSE_H
#include <stdio.h>
#include <signal.h>

//#include "comps_dict.h"
#include "comps_list.h"
#include "comps.h"
//#include "comps_dtd.h"
#include "comps_types.h"
#include "comps_logger.h"
#include "comps_elem.h"

#include <expat.h>
#include <libxml/parser.h>

typedef struct {
    COMPS_List *elem_stack;
    COMPS_Doc *comps_doc;
    COMPS_List *text_buffer;
    unsigned int text_buffer_len;
    char **text_buffer_pt;
    char *tmp_buffer;
    COMPS_Logger *log;
    XML_Parser parser;
    const char *enc;
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

void comps_parse_el_preprocess(COMPS_Elem *elem, COMPS_Parsed *parsed);
void comps_parse_el_postprocess(const char *s, COMPS_Parsed *parsed);

void comps_parse_file(COMPS_Parsed *parsed, FILE *f);
void comps_parse_str(COMPS_Parsed *parsed, char *str);

unsigned comps_parse_init_parser(XML_Parser *p);//, COMPS_DTDRuleList * rules);
void comps_parse_parsed_destroy(COMPS_Parsed *parsed);
COMPS_PackageType comps_package_get_type(const XML_Char *s);
//char comps_parse_validate_attrs(COMPS_DTDRule * elem_rule,
//                                COMPS_DTDRuleList * rules, char **attrs);
COMPS_ElemType comps_elem_get_type(const XML_Char *s);
char * comps_elem_get_name(const COMPS_ElemType type);
char comps_parse_validate_dtd(char *filename, char *dtd_file);

#endif
