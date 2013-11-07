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

#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#include "comps_types.h"
#include "comps_parse.h"
#include "comps_elem.h"

#define BUFF_SIZE 1024

#define XML_DTD

inline unsigned __comps_is_whitespace_only(const char * s, int len)
{
    int i;
    for (i = 0; i<len; i++) {
        if (!isspace(s[i])) return 0;
    }
    return 1;
}
COMPS_Parsed* comps_parse_parsed_create() {
    COMPS_Parsed *ret;
    ret =  malloc(sizeof(COMPS_Parsed));
    if (ret == NULL) return NULL;
    return ret;
}

unsigned comps_parse_parsed_init(COMPS_Parsed * parsed, const char * encoding,
                                 char log_stdout) {
    if (parsed == NULL)
        return 0;
    parsed->parser = XML_ParserCreate((const XML_Char*)encoding);

    XML_SetDefaultHandler(parsed->parser, &comps_parse_def_handler);
    XML_SetElementHandler(parsed->parser, &comps_parse_start_elem_handler,
                              &comps_parse_end_elem_handler);
    XML_SetCharacterDataHandler(parsed->parser, &comps_parse_char_data_handler);

    parsed->enc = encoding;
    parsed->elem_stack = comps_list_create();
    parsed->text_buffer = comps_list_create();
    parsed->text_buffer_len = 0;
    parsed->text_buffer_pt = NULL;
    parsed->tmp_buffer = NULL;
    parsed->log = (COMPS_Log*)comps_object_create(&COMPS_Log_ObjInfo, NULL);
    parsed->log->std_out = log_stdout;
    parsed->comps_doc = NULL;
    parsed->fatal_error = 0;
    if (parsed->elem_stack == NULL || parsed->text_buffer == NULL) {
        if (!parsed->elem_stack)
            comps_list_destroy(&parsed->elem_stack);
        if (!parsed->text_buffer)
            comps_list_destroy(&parsed->text_buffer);
        COMPS_OBJECT_DESTROY(parsed->log);
        free(parsed);
        return 0;
    }
    comps_list_init(parsed->elem_stack);
    comps_list_init(parsed->text_buffer);
    XML_SetUserData(parsed->parser, parsed);
    return 1;
}

void comps_parse_parsed_reinit(COMPS_Parsed *parsed) {
    parsed->fatal_error = 0;
    XML_ParserReset(parsed->parser, parsed->enc);
    XML_SetDefaultHandler(parsed->parser, &comps_parse_def_handler);
    XML_SetElementHandler(parsed->parser, &comps_parse_start_elem_handler,
                                          &comps_parse_end_elem_handler);
    XML_SetCharacterDataHandler(parsed->parser, &comps_parse_char_data_handler);
    XML_SetUserData(parsed->parser, parsed);
    comps_list_clear(parsed->elem_stack);
    comps_list_clear(parsed->text_buffer);
    comps_hslist_clear(parsed->log->entries);
    COMPS_OBJECT_DESTROY(parsed->comps_doc);
    //comps_doc_destroy(&parsed->comps_doc);
}

void comps_parse_parsed_destroy(COMPS_Parsed *parsed) {
    comps_list_destroy(&parsed->elem_stack);
    comps_list_destroy(&parsed->text_buffer);
    COMPS_OBJECT_DESTROY(parsed->log);
    //if (parsed->log)
    //    comps_log_destroy(parsed->log);
    COMPS_OBJECT_DESTROY(parsed->comps_doc);
    //comps_doc_destroy(&parsed->comps_doc);
    XML_ParserFree(parsed->parser);
    free(parsed);
}

void empty_xmlGenericErrorFunc(void * ctx, const char * msg, ...) {
    (void) ctx;
    (void) msg;
}

int comps_parse_validate_dtd(char *filename, char *dtd_file) {
    xmlDocPtr fptr;
    xmlDtdPtr dtd_ptr;
    xmlValidCtxtPtr vctxt;
    int ret;
    xmlErrorPtr err;

    fptr = xmlReadFile(filename, NULL, 0);
    if (fptr == NULL) {
        return -1;
    }

    dtd_ptr = xmlParseDTD(NULL, (const xmlChar*)dtd_file);
    if (dtd_ptr == NULL) {
        xmlFreeDoc(fptr);
        return -2;
    }

    vctxt = xmlNewValidCtxt();
    xmlSetGenericErrorFunc(vctxt, empty_xmlGenericErrorFunc);
    if (vctxt == NULL) {
        xmlFreeDoc(fptr);
        xmlFreeDtd(dtd_ptr);
        return -3;
    }
    ret = xmlValidateDtd(vctxt, fptr, dtd_ptr);
    if (!ret) {
        err = xmlGetLastError();
        printf("%s\n", err->message);
    }
    xmlFreeDoc(fptr);
    xmlFreeDtd(dtd_ptr);
    xmlFreeValidCtxt(vctxt);
    if (!ret)
        return -err->code;
    else
        return ret;
}

signed char comps_parse_file(COMPS_Parsed *parsed, FILE *f) {
    void *buff;
    int bytes_read;

    if (!f) {
        comps_log_error(parsed->log, COMPS_ERR_READFD, 0);
        parsed->fatal_error = 1;
        return -1;
    }
    comps_parse_parsed_reinit(parsed);

    for (;;) {
        buff = XML_GetBuffer(parsed->parser, BUFF_SIZE);
        if (buff == NULL) {
            comps_log_error(parsed->log, COMPS_ERR_MALLOC, 0);
            raise(SIGABRT);
            return -1;
        }
        bytes_read = fread(buff, sizeof(char), BUFF_SIZE, f);
        if (bytes_read < 0)
            comps_log_error(parsed->log, COMPS_ERR_READFD, 0);
        if (!XML_ParseBuffer(parsed->parser, bytes_read, bytes_read == 0)) {
            comps_log_error_x(parsed->log, COMPS_ERR_PARSER, 3,
                          comps_num(XML_GetCurrentLineNumber(parsed->parser)),
                          comps_num(XML_GetCurrentColumnNumber(parsed->parser)),
                          comps_str(XML_ErrorString(
                                    XML_GetErrorCode(parsed->parser))));
            parsed->fatal_error = 1;
        }
        if (bytes_read == 0) break;
    }
    fclose(f);
    if (parsed->fatal_error == 0 && parsed->log->entries->first == NULL)
        return 0;
    else if (parsed->fatal_error != 1)
        return 1;
    else
        return -1;
}

signed char comps_parse_str(COMPS_Parsed *parsed, char *str) {
    if (!XML_Parse(parsed->parser, str, strlen(str), 1)) {
        comps_log_error_x(parsed->log, COMPS_ERR_PARSER, 3,
                          comps_num(XML_GetCurrentLineNumber(parsed->parser)),
                          comps_num(XML_GetCurrentColumnNumber(parsed->parser)),
                          comps_str(XML_ErrorString(
                                            XML_GetErrorCode(parsed->parser))));
        parsed->fatal_error = 1;
    }
    if (parsed->fatal_error == 0 && parsed->log->entries->first == NULL)
        return 0;
    else if (parsed->fatal_error != 1)
        return 1;
    else
        return -1;
}

void comps_parse_end_elem_handler(void *userData, const XML_Char *s) {
    COMPS_ListItem * item;
    char * alltext = NULL;
    size_t item_len;
    int index=0;
    #define parser_line XML_GetCurrentLineNumber(((COMPS_Parsed*)userData)->parser)
    #define parser_col XML_GetCurrentColumnNumber(((COMPS_Parsed*)userData)->parser)
    #define parsed ((COMPS_Parsed*)userData)
    #define last_elem ((COMPS_Elem*)parsed->elem_stack->last->data)

    /* check if there's some text in recent element - are we interested in?*/
    if (parsed->text_buffer_len) {
        alltext = malloc(sizeof(char)*(parsed->text_buffer_len + 1));
        if (alltext == NULL) {
            comps_log_error(parsed->log, COMPS_ERR_MALLOC, 0);
            raise(SIGABRT);
        }
    }
    while ((item = comps_list_shift(parsed->text_buffer)) != NULL) {
        item_len = strlen((char*)item->data);
        memcpy(alltext + index, item->data, item_len * sizeof(char));
        comps_list_item_destroy(item);
        index += item_len;
    }
    /* set zero char at the end of string */
    if (alltext)
        memset(alltext+parsed->text_buffer_len, 0, sizeof(char));
    parsed->tmp_buffer = alltext;

    /* start postprocess for currently processed elements */
    if (comps_elem_get_type(s) == last_elem->type) {
        if (last_elem->valid && COMPS_ElemInfos[last_elem->type]->postproc) {
            COMPS_ElemInfos[last_elem->type]->postproc((COMPS_Parsed*)userData,
                                                       last_elem);
        }
        if (last_elem->valid && parsed->tmp_buffer) {
            comps_log_error_x(parsed->log, COMPS_ERR_TEXT_BETWEEN, 3,
                              comps_str(parsed->tmp_buffer), comps_num(parser_line),
                              comps_num(parser_col));
        }
        /* finaly, remove element from element stack */
        item = comps_list_pop(parsed->elem_stack);
        comps_list_item_destroy(item);
    }
    free(alltext);
    parsed->text_buffer_len = 0;
    #undef parsed
    #undef parser_line
    #undef parser_col
}

void comps_parse_def_handler(void *userData, const XML_Char *s, int len) {
    (void) userData;
    (void) s;
    (void) len;
}

void comps_parse_el_postprocess(const char *s, COMPS_Parsed *parsed)
{
    COMPS_ObjList *list = NULL;
    COMPS_ObjDict *objdict, *prop_dict;
    char *lang;
    COMPS_Object *prop;

    /* check if there's all important data for currently processed element.
       This is also done by dtd checking, but if dtd rules missing, validation
       ends here (however isn't so strict as dtd)*/

    #define parent ((COMPS_Elem*)parsed->elem_stack->last->prev->data)->type
    #define grandparent ((COMPS_Elem*)parsed->elem_stack->last->prev->prev->data)->type
    #define list_last_cat ((COMPS_DocCategory*)list->last->comps_obj)
    #define list_last_env ((COMPS_DocEnv*)list->last->comps_obj)
    #define list_last_group ((COMPS_DocGroup*)list->last->comps_obj)
    #define list_last_group_package list_last_group->packages->last->comps_obj
    #define lastelem ((COMPS_Elem*)parsed->elem_stack->last->data)
    #define parser_line XML_GetCurrentLineNumber(parsed->parser)
    #define parser_col XML_GetCurrentColumnNumber(parsed->parser)

    if (lastelem->type != COMPS_ELEM_DOC){
        if (!parent) {
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
            return;
        }
    }
    if (lastelem->type == COMPS_ELEM_GROUPID
        || lastelem->type == COMPS_ELEM_PACKAGEREQ){
        if (!parent || !grandparent) {
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
            return;
        }
    }
    if (parsed->text_buffer_pt && !*parsed->text_buffer_pt) {
        comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3, comps_str(s),
                        comps_num(parser_line), comps_num(parser_col));
    }
    switch (comps_elem_get_type(s)) {
        case COMPS_ELEM_SENTINEL:
        case COMPS_ELEM_UNKNOWN:
        case COMPS_ELEM_NONE:
            parsed->tmp_buffer = NULL;
            free(parsed->tmp_buffer);
        break;
        case COMPS_ELEM_DOC:
        break;
        case COMPS_ELEM_ENV:
            list = comps_doc_environments(parsed->comps_doc);
            if (!comps_objdict_get_x(list_last_env->properties, "id"))
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("id"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!comps_objdict_get_x(list_last_env->properties, "name"))
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("name"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!comps_objdict_get_x(list_last_env->properties, "desc"))
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("description"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!list_last_env->group_list)
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("grouplist"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!list_last_env->option_list)
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("optionlist"),
                                  comps_num(parser_line), comps_num(parser_col));
            COMPS_OBJECT_DESTROY(list);
        break;
        case COMPS_ELEM_GROUP:
            list = comps_doc_groups(parsed->comps_doc);
            if (!comps_objdict_get_x(list_last_group->properties, "id"))
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("id"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (comps_objdict_get_x(list_last_group->properties, "name") == NULL) {
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("name"),
                                  comps_num(parser_line), comps_num(parser_col));
            }
            if (!comps_objdict_get_x(list_last_group->properties, "desc"))
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("description"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!list_last_group->packages)
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("packagelist"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!comps_objdict_get_x(list_last_group->properties,
                                     "uservisible")) {
                comps_objdict_set_x(list_last_group->properties,
                                    "uservisible",
                                    (COMPS_Object*) comps_num(1));
            }
            COMPS_OBJECT_DESTROY(list);
        break;
        case COMPS_ELEM_CATEGORY:
            list = comps_doc_categories(parsed->comps_doc);
            if (!comps_objdict_get_x(list_last_cat->properties, "id"))
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("id"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!comps_objdict_get_x(list_last_cat->properties, "name"))
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("name"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!comps_objdict_get_x(list_last_cat->properties, "desc"))
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("description"),
                                  comps_num(parser_line), comps_num(parser_col));
            if (!list_last_cat->group_ids)
                comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                                  comps_str("grouplist"),
                                  comps_num(parser_line), comps_num(parser_col));
            COMPS_OBJECT_DESTROY(list);
        case COMPS_ELEM_WHITEOUT:
        case COMPS_ELEM_BLACKLIST:
        case COMPS_ELEM_LANGPACKS:
        case COMPS_ELEM_IGNOREDEP:
        case COMPS_ELEM_PACKAGE:
        case COMPS_ELEM_MATCH:
        break;
        case COMPS_ELEM_ID:
            if (parent == COMPS_ELEM_GROUP) {
                list = comps_doc_groups(parsed->comps_doc);
                objdict = list_last_group->properties;
                prop = comps_objdict_get_x(list_last_group->properties, "id");
            } else if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_doc_categories(parsed->comps_doc);
                objdict = list_last_cat->properties;
                prop = comps_objdict_get_x(list_last_cat->properties, "id");
            } else if (parent == COMPS_ELEM_ENV) {
                list = comps_doc_environments(parsed->comps_doc);
                objdict = list_last_env->properties;
                prop = comps_objdict_get_x(list_last_env->properties, "id");
            } else {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            COMPS_OBJECT_DESTROY(list);
            if (prop != NULL) {
                comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET, 3,
                                    comps_str(s), comps_num(parser_line),
                                    comps_num(parser_col));
                comps_str_set((COMPS_Str*)prop, parsed->tmp_buffer);
            } else {
                prop = (COMPS_Object*)comps_str_x(parsed->tmp_buffer);
                comps_objdict_set_x(objdict, "id", prop);
            }
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_NAME:
            lang = comps_dict_get(lastelem->attrs, "xml:lang");
            if (parent == COMPS_ELEM_GROUP) {
                list = comps_doc_groups(parsed->comps_doc);
                objdict = list_last_group->name_by_lang;
                prop_dict = list_last_group->properties;
                prop = comps_objdict_get_x(list_last_group->properties, "name");
            } else if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_doc_categories(parsed->comps_doc);
                objdict = list_last_cat->name_by_lang;
                prop_dict = list_last_cat->properties;
                prop = comps_objdict_get_x(list_last_cat->properties, "name");
            } else if (parent == COMPS_ELEM_ENV) {
                list = comps_doc_environments(parsed->comps_doc);
                objdict = list_last_env->name_by_lang;
                prop_dict = list_last_env->properties;
                prop = comps_objdict_get_x(list_last_env->properties, "name");
            } else {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            COMPS_OBJECT_DESTROY(list);
            if (lang) {
                comps_objdict_set_x(objdict, lang,
                                  (COMPS_Object*)comps_str_x(parsed->tmp_buffer));
            } else {
                if (prop != NULL) {
                    comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET,
                                        3, comps_str(s), comps_num(parser_line),
                                        comps_num(parser_col));
                    comps_str_set((COMPS_Str*)prop, parsed->tmp_buffer);
                } else {
                    prop = (COMPS_Object*)comps_str_x(parsed->tmp_buffer);
                    comps_objdict_set_x(prop_dict, "name", prop);
                }
            }
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_DESC:
            lang = comps_dict_get(lastelem->attrs, "xml:lang");
            if (parent == COMPS_ELEM_GROUP) {
                list = comps_doc_groups(parsed->comps_doc);
                objdict = list_last_group->desc_by_lang;
                prop_dict = list_last_group->properties;
                prop = comps_objdict_get_x(list_last_group->properties, "desc");
            } else if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_doc_categories(parsed->comps_doc);
                objdict = list_last_cat->desc_by_lang;
                prop_dict = list_last_cat->properties;
                prop = comps_objdict_get_x(list_last_cat->properties, "desc");
            } else if (parent == COMPS_ELEM_ENV) {
                list = comps_doc_environments(parsed->comps_doc);
                objdict = list_last_env->desc_by_lang;
                prop_dict = list_last_env->properties;
                prop = comps_objdict_get_x(list_last_env->properties, "desc");
            } else {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            COMPS_OBJECT_DESTROY(list);
            if (lang) {
                comps_objdict_set_x(objdict, lang,
                                (COMPS_Object*)comps_str_x(parsed->tmp_buffer));
            } else {
                if (prop != NULL) {
                    comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET,
                                        3, comps_str(s), comps_num(parser_line),
                                        comps_num(parser_col));
                    comps_str_set((COMPS_Str*)prop, parsed->tmp_buffer);
                } else {
                    //printf("elem desc:%s\n", parsed->tmp_buffer);
                    //printf("elem desc len:%d\n", strlen(parsed->tmp_buffer));
                    //printf("elem desc p:%p\n", parsed->tmp_buffer);
                    prop = (COMPS_Object*)comps_str_x(parsed->tmp_buffer);
                    comps_objdict_set_x(prop_dict, "desc", prop);
                }
            }
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_PACKAGELIST:
            if (parent != COMPS_ELEM_GROUP) {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_doc_groups(parsed->comps_doc);
            if (!list_last_group->packages->first)
                comps_log_error_x(parsed->log, COMPS_ERR_LIST_EMPTY, 3,
                                  comps_str("packagelist"),
                                  comps_num(parser_line), comps_num(parser_col));
            COMPS_OBJECT_DESTROY(list);
        break;
        case COMPS_ELEM_GROUPLIST:
            if (parent != COMPS_ELEM_CATEGORY &&
                parent != COMPS_ELEM_ENV) {
                parsed->tmp_buffer = NULL;
                break;
            }
            if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_doc_categories(parsed->comps_doc);
                if (!list_last_cat->group_ids->first)
                    comps_log_error_x(parsed->log, COMPS_ERR_LIST_EMPTY, 3,
                                      comps_str("grouplist"),
                                      comps_num(parser_line),
                                      comps_num(parser_col));
            } else {
                list = comps_doc_environments(parsed->comps_doc);
                if (!list_last_env->group_list->first)
                    comps_log_error_x(parsed->log, COMPS_ERR_LIST_EMPTY, 3,
                                      comps_str("grouplist"),
                                      comps_num(parser_line),
                                      comps_num(parser_col));
            }
            COMPS_OBJECT_DESTROY(list);
        break;
        case COMPS_ELEM_OPTLIST:
            if (parent != COMPS_ELEM_ENV) {
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_doc_environments(parsed->comps_doc);
            if (!list_last_env->option_list->first)
                comps_log_error_x(parsed->log, COMPS_ERR_LIST_EMPTY, 3,
                                  comps_str("optionlist"),
                                  comps_num(parser_line),
                                  comps_num(parser_col));
            COMPS_OBJECT_DESTROY(list);
        break;
        case COMPS_ELEM_PACKAGEREQ:
            if (parent != COMPS_ELEM_PACKAGELIST &&
                grandparent != COMPS_ELEM_GROUP) {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_doc_groups(parsed->comps_doc);
            //printf("list last group:%p\n", list_last_group);
            if (parsed->tmp_buffer) {
                comps_docpackage_set_name((COMPS_DocGroupPackage*)list_last_group_package,
                                          parsed->tmp_buffer, 1);
                free(parsed->tmp_buffer);
            }
            COMPS_OBJECT_DESTROY(list);
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_DEFAULT:
            if (parent != COMPS_ELEM_GROUP) {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_doc_groups(parsed->comps_doc);
            prop = comps_objdict_get_x(list_last_group->properties, "def");
            if (prop) {
                comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET, 3,
                                    comps_str(s), comps_num(parser_line),
                                    comps_num(parser_col));
            } else {
                prop = (COMPS_Object*)comps_num(0);
                comps_objdict_set_x(list_last_group->properties, "def", prop);
            }
            if (strcmp(parsed->tmp_buffer,"false") == 0)
                ((COMPS_Num*)prop)->val = 0;
            else if (strcmp(parsed->tmp_buffer,"true") == 0)
                ((COMPS_Num*)prop)->val = 1;
            else {
                comps_log_warning_x(parsed->log, COMPS_ERR_DEFAULT_PARAM, 3,
                                    comps_str(parsed->tmp_buffer),
                                    comps_num(parser_line),
                                    comps_num(parser_col));
            }
            COMPS_OBJECT_DESTROY(list);
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_USERVISIBLE:
            if (parent != COMPS_ELEM_GROUP) {
                //comps_log_error(parsed->log, s, COMPS_ERR_NOPARENT,
                //                parser_line, parser_col, 0);
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_doc_groups(parsed->comps_doc);
            prop = comps_objdict_get_x(list_last_group->properties, "uservisible");
            if (prop) {
                comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET, 3,
                                    comps_str(s), comps_num(parser_line),
                                    comps_num(parser_col));
            } else {
                prop = (COMPS_Object*)comps_num(0);
                comps_objdict_set_x(list_last_group->properties, "uservisible", prop);
            }
            if (strcmp(parsed->tmp_buffer,"false") == 0)
                ((COMPS_Num*)prop)->val = 0;
            else if (strcmp(parsed->tmp_buffer,"true") == 0)
                ((COMPS_Num*)prop)->val = 1;
            else {
                comps_log_warning_x(parsed->log, COMPS_ERR_USERVISIBLE_PARAM, 3,
                                    comps_str(parsed->tmp_buffer),
                                    comps_num(parser_line), comps_num(parser_col));
            }
            COMPS_OBJECT_DESTROY(list);
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_GROUPID:
            if (parent != COMPS_ELEM_GROUPLIST &&
                parent != COMPS_ELEM_OPTLIST) {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            if (grandparent == COMPS_ELEM_CATEGORY) {
                list = comps_doc_categories(parsed->comps_doc);
                if (parent == COMPS_ELEM_GROUPLIST &&
                    list_last_cat->group_ids == NULL) {
                    comps_log_warning_x(parsed->log, COMPS_ERR_GROUPLIST_NOTSET,
                                       3, comps_str(parsed->tmp_buffer),
                                      comps_num(parser_line),
                                      comps_num(parser_col));
                } else if (parent == COMPS_ELEM_GROUPLIST &&
                          list_last_cat->group_ids){
                    comps_docgroupid_set_name(
                        (COMPS_DocGroupId*)list_last_cat->group_ids->last->comps_obj,
                        parsed->tmp_buffer, 1);
                }
            } else if (grandparent == COMPS_ELEM_ENV) {
                list = comps_doc_environments(parsed->comps_doc);
                if (parent == COMPS_ELEM_GROUPLIST &&
                    list_last_env->group_list != NULL) {
                    comps_docgroupid_set_name(
                     (COMPS_DocGroupId*)list_last_env->group_list->last->comps_obj,
                            parsed->tmp_buffer, 1);
                } else if (parent == COMPS_ELEM_OPTLIST &&
                           list_last_env->option_list != NULL) {
                    comps_docgroupid_set_name(
                     (COMPS_DocGroupId*)list_last_env->option_list->last->comps_obj,
                            parsed->tmp_buffer, 2);
                } else if (parent == COMPS_ELEM_GROUPLIST) {
                    comps_log_warning_x(parsed->log, COMPS_ERR_GROUPLIST_NOTSET,
                                        3, comps_str(parsed->tmp_buffer),
                                        comps_num(parser_line),
                                        comps_num(parser_col));
                } else if (parent == COMPS_ELEM_OPTLIST) {
                    comps_log_warning_x(parsed->log, COMPS_ERR_OPTIONLIST_NOTSET,
                                        3, comps_str(parsed->tmp_buffer),
                                        comps_num(parser_line),
                                        comps_num(parser_col));
                }
            }
            COMPS_OBJECT_DESTROY(list);
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_DISPLAYORDER:
            prop = NULL;
            objdict = NULL;
            if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_doc_categories(parsed->comps_doc);
                prop = comps_objdict_get_x(list_last_cat->properties, "display_order");
                objdict = list_last_cat->properties;
            } else if (parent == COMPS_ELEM_ENV) {
                list = comps_doc_environments(parsed->comps_doc);
                prop = comps_objdict_get_x(list_last_env->properties, "display_order");
                objdict = list_last_env->properties;
            } else if (parent == COMPS_ELEM_GROUP) {
                list = comps_doc_groups(parsed->comps_doc);
                prop = comps_objdict_get_x(list_last_group->properties, "display_order");
                objdict = list_last_group->properties;
            }
            COMPS_OBJECT_DESTROY(list);
            if (prop) {
                comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET, 3,
                                    comps_str(s), comps_num(parser_line),
                                    comps_num(parser_col));
            } else if (objdict) {
                prop = (COMPS_Object*)comps_num(0);
                comps_objdict_set_x(objdict, "display_order", prop);
            }
            if (prop)
                sscanf(parsed->tmp_buffer, "%d", &((COMPS_Num*)prop)->val);

            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_LANGONLY:
            if (parent != COMPS_ELEM_GROUP) {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_doc_groups(parsed->comps_doc);
            prop = comps_objdict_get_x(list_last_cat->properties, "langonly");
            if (prop) {
                comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET, 3,
                                    comps_str(s), comps_num(parser_line),
                                    comps_num(parser_col));
                comps_str_set((COMPS_Str*)prop, parsed->tmp_buffer);
                free(parsed->tmp_buffer);
            } else {
                prop = (COMPS_Object*)comps_str_x(parsed->tmp_buffer);
                comps_objdict_set_x(list_last_group->properties, "langonly", prop);
            }
            parsed->tmp_buffer = NULL;
            COMPS_OBJECT_DESTROY(list);
        break;
    }
    if (parsed->tmp_buffer) {
        free(parsed->tmp_buffer);
        parsed->tmp_buffer = NULL;
        printf("buffer not NULL:%s|\n", parsed->tmp_buffer);
    }
    #undef parent
    #undef grandparent
    #undef list_last_cat
    #undef list_last_group
    #undef list_last_group_package
    #undef list_last_env
    #undef lastelem
    #undef parser_line
    #undef parser_col
}

void comps_parse_el_preprocess(COMPS_Elem *elem, COMPS_Parsed *parsed)
{
    static COMPS_DocGroup * group;
    static COMPS_DocCategory * category;
    static COMPS_DocEnv * env;
    //COMPS_ListItem * it;
    COMPS_DocGroupPackage * package;
    COMPS_DocGroupId *groupid;
    COMPS_Object *prop;
    //COMPS_ObjList *list;
    char *tmp;

    /* prepare currently processed element. Create it, set text_buffer pointer
       to text data destination, if needed*/
    #define parent_o parsed->elem_stack->last->prev
    #define grandparent_o parsed->elem_stack->last->prev->prev

    #define parent ((COMPS_Elem*)parsed->elem_stack->last->prev->data)->type
    #define grandparent ((COMPS_Elem*)parsed->elem_stack->last->prev->prev->data)->type
    #define parser_line XML_GetCurrentLineNumber(parsed->parser)
    #define parser_col XML_GetCurrentColumnNumber(parsed->parser)

    if (elem->type != COMPS_ELEM_DOC){
        if (!parent_o) {
            comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                            comps_str(elem->name), comps_num(parser_line),
                            comps_num(parser_col));
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
            return;
        }
    }
    if (elem->type == COMPS_ELEM_GROUPID
        || elem->type == COMPS_ELEM_PACKAGEREQ){
        if (!parent_o || !grandparent_o) {
            comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                            comps_str(elem->name), comps_num(parser_line),
                            comps_num(parser_col));
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
            return;
        }
    }

    switch (elem->type)
    {
        case COMPS_ELEM_SENTINEL:
        case COMPS_ELEM_WHITEOUT:
        case COMPS_ELEM_BLACKLIST:
        case COMPS_ELEM_LANGPACKS:
        case COMPS_ELEM_NONE:
        break;
        case COMPS_ELEM_DOC:
            prop = (COMPS_Object*)comps_str(parsed->enc);
            parsed->comps_doc = (COMPS_Doc*)comps_object_create(&COMPS_Doc_ObjInfo,
                                                    (COMPS_Object*[]){prop});
            comps_object_destroy(prop);
        break;
        case COMPS_ELEM_GROUP:
            if (parent != COMPS_ELEM_DOC) {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
            group = (COMPS_DocGroup*)
                    comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
            category = NULL;
            env = NULL;
            comps_doc_add_group(parsed->comps_doc, group);
        break;
        case COMPS_ELEM_GROUPLIST:
            if (parent != COMPS_ELEM_CATEGORY && parent != COMPS_ELEM_ENV) {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
            /*if (category) {
                if (category->group_ids != NULL) {
                    comps_log_warning(parsed->log,
                                      comps_elem_get_name(elem->type),
                                      COMPS_ERR_ELEM_ALREADYSET, parser_line,
                                      parser_col, 0);
                    break;
               }
                //category->group_ids = (COMPS_ObjList*)
                //                     comps_object_create(&COMPS_ObjList_ObjInfo,
                //                                         NULL);
            } else if (env) {
                if (env->group_list != NULL) {
                    comps_log_warning(parsed->log,
                                      comps_elem_get_name(elem->type),
                                      COMPS_ERR_ELEM_ALREADYSET, parser_line,
                                      parser_col, 0);
                    break;
                }
                //env->group_list = (COMPS_ObjList*)
                //                  comps_object_create(&COMPS_ObjList_ObjInfo,
                //                                      NULL);
            }*/
        break;
        case COMPS_ELEM_OPTLIST:
            if (parent != COMPS_ELEM_ENV) {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
            /*if (env->option_list != NULL) {
                comps_log_warning(parsed->log,
                                  comps_elem_get_name(elem->type),
                                  COMPS_ERR_ELEM_ALREADYSET, parser_line,
                                  parser_col, 0);
                break;
            }
            env->option_list = (COMPS_ObjList*)
                               comps_object_create(&COMPS_ObjList_ObjInfo,
                                                   NULL);
            */
        break;
        case COMPS_ELEM_CATEGORY:
            if (parent != COMPS_ELEM_DOC) {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
            category = (COMPS_DocCategory*)
                       comps_object_create(&COMPS_DocCategory_ObjInfo, NULL);
            group = NULL;
            env = NULL;
            comps_doc_add_category(parsed->comps_doc, category);
        break;
        case COMPS_ELEM_ENV:
            if (parent != COMPS_ELEM_DOC) {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
            env = (COMPS_DocEnv*)
                       comps_object_create(&COMPS_DocEnv_ObjInfo, NULL);
            comps_doc_add_environment(parsed->comps_doc, env);
            //list = comps_doc_environments(parsed->comps_doc);
            //printf("envs: %d\n", list->len);
            group = NULL;
            category = NULL;
        break;
        case COMPS_ELEM_ID:
        case COMPS_ELEM_NAME:
        case COMPS_ELEM_DESC:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            if (parent != COMPS_ELEM_CATEGORY && parent != COMPS_ELEM_ENV
                && parent != COMPS_ELEM_GROUP) {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
        break;
        case COMPS_ELEM_LANGONLY:
        case COMPS_ELEM_DEFAULT:
        case COMPS_ELEM_USERVISIBLE:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            if (parent != COMPS_ELEM_GROUP) {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
        break;
        case COMPS_ELEM_PACKAGELIST:
            if (parent != COMPS_ELEM_GROUP) {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
        break;
        case COMPS_ELEM_PACKAGEREQ:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            if (grandparent != COMPS_ELEM_GROUP || parent != COMPS_ELEM_PACKAGELIST){
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                comps_str(comps_elem_get_name(elem->type)),
                                comps_num(parser_line), comps_num(parser_col));
                break;
            }
            package = (COMPS_DocGroupPackage*)
                      comps_object_create(&COMPS_DocGroupPackage_ObjInfo,
                                          NULL);
            comps_docgroup_add_package(group, package);

            package->type = comps_package_get_type(comps_dict_get(elem->attrs,
                                                                  "type"));
            tmp = comps_dict_get(elem->attrs, "requires");
            if (tmp)
                package->requires = comps_str(tmp);

            if (package->type == COMPS_PACKAGE_UNKNOWN)
                comps_log_warning_x(parsed->log, COMPS_ERR_PACKAGE_UNKNOWN, 3,
                                    comps_str(comps_dict_get(elem->attrs, "type")),
                                    comps_num(parser_line),
                                    comps_num(parser_col));
        break;
        case COMPS_ELEM_DISPLAYORDER:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            if (parent != COMPS_ELEM_CATEGORY && parent != COMPS_ELEM_ENV &&
                parent != COMPS_ELEM_GROUP){
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                  comps_str(comps_elem_get_name(elem->type)),
                                  comps_num(parser_line),
                                  comps_num(parser_col));
                break;
            }
        break;
        case COMPS_ELEM_GROUPID:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            groupid = (COMPS_DocGroupId*)
                      comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
            tmp = comps_dict_get(elem->attrs, "default");
            if (tmp)
                comps_docgroupid_set_default(groupid,
                                             __comps_strcmp(tmp, "true")?0:1);
            else
                comps_docgroupid_set_default(groupid,0);

            if (parent == COMPS_ELEM_GROUPLIST) {
                if (grandparent == COMPS_ELEM_CATEGORY) {
                    comps_doccategory_add_groupid(category, groupid);
                } else if (grandparent == COMPS_ELEM_ENV) {
                    comps_docenv_add_groupid(env, groupid);
                } else {
                    comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                      comps_str(comps_elem_get_name(elem->type)),
                                      comps_num(parser_line),
                                      comps_num(parser_col));
                    COMPS_OBJECT_DESTROY(groupid);
                }
            } else if (parent == COMPS_ELEM_OPTLIST) {
                if (grandparent == COMPS_ELEM_ENV) {
                    comps_docenv_add_optionid(env, groupid);
                } else {
                    comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                      comps_str(comps_elem_get_name(elem->type)),
                                      comps_num(parser_line),
                                      comps_num(parser_col));
                    COMPS_OBJECT_DESTROY(groupid);
                }
            } else {
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                  comps_str(comps_elem_get_name(elem->type)),
                                  comps_num(parser_line),
                                  comps_num(parser_col));
                COMPS_OBJECT_DESTROY(groupid);
            }
        break;
        case COMPS_ELEM_MATCH:
            if (parent != COMPS_ELEM_LANGPACKS){
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                  comps_str(comps_elem_get_name(elem->type)),
                                  comps_num(parser_line),
                                  comps_num(parser_col));
                break;
            }
            
            comps_doc_add_langpack(parsed->comps_doc,
                     comps_dict_get(elem->attrs, "name"),
                     comps_str(comps_dict_get(elem->attrs, "install")));
        break;
        case COMPS_ELEM_PACKAGE:
            if (parent != COMPS_ELEM_BLACKLIST){
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                  comps_str(comps_elem_get_name(elem->type)),
                                  comps_num(parser_line),
                                  comps_num(parser_col));
                break;
            }
            comps_doc_add_blacklist(parsed->comps_doc,
                                    comps_dict_get(elem->attrs, "name"),
                            comps_str((comps_dict_get(elem->attrs, "arch"))));
        break;
        case COMPS_ELEM_IGNOREDEP:
            if (parent != COMPS_ELEM_WHITEOUT){
                comps_log_error_x(parsed->log, COMPS_ERR_NOPARENT, 3,
                                  comps_str(comps_elem_get_name(elem->type)),
                                  comps_num(parser_line),
                                  comps_num(parser_col));
                break;
            }
            comps_doc_add_whiteout(parsed->comps_doc,
                                      comps_dict_get(elem->attrs, "requires"),
                    comps_str((comps_dict_get(elem->attrs, "package"))));
            
        break;
        case COMPS_ELEM_UNKNOWN:
            //parsed->text_buffer_pt = &parsed->tmp_buffer;
            comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_UNKNOWN, 3,
                                comps_str(elem->name),
                                comps_num(parser_line),
                                comps_num(parser_col));
        break;
    }

    #undef parent_o
    #undef grandparent_o

    #undef parent
    #undef grandparent
    #undef parser_line
    #undef parser_col
}

void comps_parse_start_elem_handler(void *userData,
                              const XML_Char *s,
                              const XML_Char **attrs) {
    #define parser_line XML_GetCurrentLineNumber(((COMPS_Parsed*)userData)->parser)
    #define parser_col XML_GetCurrentColumnNumber(((COMPS_Parsed*)userData)->parser)
    #define ELEMINFO  COMPS_ElemInfos[elem->type]
    #define LAST ((COMPS_Parsed*)userData)->elem_stack->last
    #define LASTELEM  ((COMPS_Elem*)((COMPS_Parsed*)userData)->elem_stack->last->data)

    COMPS_Elem * elem = NULL;
    COMPS_ElemType type;
    COMPS_ListItem * item;

    /* create new element */
    type = comps_elem_get_type(s);
    elem = comps_elem_create(s, attrs, type);
    if (elem == NULL) {
        comps_log_error_x(((COMPS_Parsed*)userData)->log, COMPS_ERR_MALLOC, 0);
        raise(SIGABRT);
        return;
    }
    elem->valid = 0;

    if (ELEMINFO->ancestors[0] != COMPS_ELEM_NONE) {
        if (!LAST) {

        } else {
            elem->ancestor = LASTELEM;
            for (int x = 0; ELEMINFO->ancestors[x] != COMPS_ELEM_SENTINEL; x++){
                if (ELEMINFO->ancestors[x] == LASTELEM->type) {
                    elem->valid = 1;
                    break;
                }
            }
        }
    } else {
        elem->valid = 1;
    }
    if (!elem->valid) {
        comps_log_error_x(((COMPS_Parsed*)userData)->log,
                          COMPS_ERR_NOPARENT, 3, comps_str(s),
                          comps_num(parser_line),
                          comps_num(parser_col));
    }

    item = comps_list_item_create(elem, NULL, &comps_elem_destroy);
    if (((COMPS_Parsed*)userData)->text_buffer->first) {
        comps_log_error_x(((COMPS_Parsed*)userData)->log,
                          COMPS_ERR_TEXT_BETWEEN, 3,
                          comps_str((char*)((COMPS_Parsed*)userData)
                                         ->text_buffer->first->data),
                          comps_num(parser_line), comps_num(parser_col));
        comps_list_clear(((COMPS_Parsed*)userData)->text_buffer);
        ((COMPS_Parsed*)userData)->text_buffer_len = 0;
    }

    /* end append it to element stack */
    comps_list_append(((COMPS_Parsed*)userData)->elem_stack, item);
    /* preprocess new element */
    if (ELEMINFO->preproc && elem->valid) {
        ELEMINFO->preproc((COMPS_Parsed*)userData, elem);
    } else {

    }

    #undef parser_line
    #undef parser_col
    #undef ELEMINFO
    #undef LAST
    #undef LASTELEM
}


void comps_parse_char_data_handler(void *userData,
                            const XML_Char *s,
                            int len) {

    char * c = NULL;
    COMPS_ListItem * it;

    /* skip whitespace data */
    if (__comps_is_whitespace_only(s, len)) {
        return;
    }
    if ((c = malloc(sizeof(char) * (len+1))) == NULL) {
        comps_log_error(((COMPS_Parsed*)userData)->log, COMPS_ERR_MALLOC, 0);
        raise(SIGABRT);
        return;
    }

    /* copy text data of element to temporal string and append it to text_buffer
       list*/
    memcpy(c, s, sizeof(char) * len);
    memset(c+len, 0, sizeof(char));
    /* and increment total lenght of strings in text_buffer */
    ((COMPS_Parsed*)userData)->text_buffer_len += len;

    it = comps_list_item_create((void*)c, NULL, &free);
        comps_list_append(((COMPS_Parsed*)userData)->text_buffer, it);

}
