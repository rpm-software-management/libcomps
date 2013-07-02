#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#include "comps_types.h"
#include "comps_parse.h"

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
    parsed->log = comps_log_create(log_stdout?1:0);
    parsed->comps_doc = NULL;
    if (parsed->elem_stack == NULL || parsed->text_buffer == NULL) {
        if (!parsed->elem_stack)
            comps_list_destroy(&parsed->elem_stack);
        if (!parsed->text_buffer)
            comps_list_destroy(&parsed->text_buffer);
        comps_log_destroy(parsed->log);
        free(parsed);
        return 0;
    }
    comps_list_init(parsed->elem_stack);
    comps_list_init(parsed->text_buffer);
    XML_SetUserData(parsed->parser, parsed);
    return 1;
}

void comps_parse_parsed_reinit(COMPS_Parsed *parsed) {
    XML_ParserReset(parsed->parser, parsed->enc);
    XML_SetDefaultHandler(parsed->parser, &comps_parse_def_handler);
    XML_SetElementHandler(parsed->parser, &comps_parse_start_elem_handler,
                                          &comps_parse_end_elem_handler);
    XML_SetCharacterDataHandler(parsed->parser, &comps_parse_char_data_handler);
    XML_SetUserData(parsed->parser, parsed);
    comps_list_clear(parsed->elem_stack);
    comps_list_clear(parsed->text_buffer);
    comps_list_clear(parsed->log->logger_data);
    comps_doc_destroy(&parsed->comps_doc);
}

void comps_parse_parsed_destroy(COMPS_Parsed *parsed) {
    comps_list_destroy(&parsed->elem_stack);
    comps_list_destroy(&parsed->text_buffer);
    comps_log_destroy(parsed->log);
    comps_doc_destroy(&parsed->comps_doc);
    XML_ParserFree(parsed->parser);
    free(parsed);
}

void empty_xmlGenericErrorFunc(void * ctx, const char * msg, ...) {
    (void) ctx;
    (void) msg;
}

char comps_parse_validate_dtd(char *filename, char *dtd_file) {
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
    if (!ret)
        err = xmlGetLastError();
    xmlFreeDoc(fptr);
    xmlFreeDtd(dtd_ptr);
    xmlFreeValidCtxt(vctxt);
    if (!ret)
        return -err->code;
    else
        return ret;
}

void comps_parse_file(COMPS_Parsed *parsed, FILE *f) {
    void *buff;
    int bytes_read;

    if (!f) {
        comps_log_error(parsed->log, NULL, COMPS_ERR_READFD, 0, 0, 0);
        return;
    }
    comps_parse_parsed_reinit(parsed);

    for (;;) {
        buff = XML_GetBuffer(parsed->parser, BUFF_SIZE);
        if (buff == NULL) {
            comps_log_error(parsed->log, NULL, COMPS_ERR_MALLOC, 0, 0, 0);
            raise(SIGABRT);
            return;
        }
        bytes_read = fread(buff, sizeof(char), BUFF_SIZE, f);
        if (bytes_read < 0)
            comps_log_error(parsed->log, NULL, COMPS_ERR_READFD, 0, 0, 0);
        if (!XML_ParseBuffer(parsed->parser, bytes_read, bytes_read == 0)) {
            comps_log_error(parsed->log,
                            XML_ErrorString(XML_GetErrorCode(parsed->parser)),
                            COMPS_ERR_PARSER, XML_GetErrorCode(parsed->parser),
                            0, 0);
        }
        if (bytes_read == 0) break;
    }
    fclose(f);
}

void comps_parse_str(COMPS_Parsed *parsed, char *str) {
    if (!XML_Parse(parsed->parser, str, strlen(str), 1)) {
        comps_log_error(parsed->log,
                        XML_ErrorString(XML_GetErrorCode(parsed->parser)),
                        COMPS_ERR_PARSER, XML_GetErrorCode(parsed->parser),
                        0, 0);
    }
}

void comps_parse_end_elem_handler(void *userData, const XML_Char *s) {
    COMPS_ListItem * item;
    char * all = NULL;
    int item_len, index=0;
    #define parser_line XML_GetCurrentLineNumber(((COMPS_Parsed*)userData)->parser)
    #define parser_col XML_GetCurrentColumnNumber(((COMPS_Parsed*)userData)->parser)
    /* check if there's some text in recent element - are we interested in?*/
    if (((COMPS_Parsed*)userData)->text_buffer_pt) {
        all = malloc(sizeof(char)*(((COMPS_Parsed*)userData)->text_buffer_len+1));
        if (all == NULL)
            comps_log_error(((COMPS_Parsed*)userData)->log, NULL,
                            COMPS_ERR_MALLOC, 0, 0, 0);
    }
    /* remove all items of "text_buffer" list and append it into one string,
       but only if we're interested in text data in current element */
    if (all == NULL && ((COMPS_Parsed*)userData)->text_buffer->first) {
            comps_log_error(((COMPS_Parsed*)userData)->log,
                     (char*)((COMPS_Parsed*)userData)->text_buffer->first->data,
                            COMPS_ERR_TEXT_BETWEEN, parser_line, parser_col, 0);
    }

    while ((item = comps_list_shift(((COMPS_Parsed*)userData)->text_buffer)) != NULL) {
        item_len = strlen((char*)item->data);

        if (all) memcpy(all + index, item->data, item_len * sizeof(char));
        comps_list_item_destroy(item);
        index += item_len;
    }
    /* set zero char at the end of string */
    if (all) {
        memset(all+((COMPS_Parsed*)userData)->text_buffer_len, 0, sizeof(char));
        *((COMPS_Parsed*)userData)->text_buffer_pt = all;
    }
    ((COMPS_Parsed*)userData)->text_buffer_len = 0;
    ((COMPS_Parsed*)userData)->text_buffer_pt = NULL;

    /* start postprocess for currently processed elements */
    if (comps_elem_get_type(s) ==
        ((COMPS_Elem*)((COMPS_Parsed*)userData)->elem_stack->last->data)->type){
        comps_parse_el_postprocess(s, userData);

        /* finaly, remove element from element stack */
        item = comps_list_pop(((COMPS_Parsed*)userData)->elem_stack);
        comps_list_item_destroy(item);
    }

    #undef parser_line
    #undef parser_col
}

void comps_parse_def_handler(void *userData, const XML_Char *s, int len) {
    (void) userData;
    (void) s;
    (void) len;
}

COMPS_PackageType comps_package_get_type(const XML_Char *s)
{
    COMPS_ElemType type;
    if (strcmp(s, "default") == 0) type = COMPS_PACKAGE_DEFAULT;
    else if (strcmp(s, "optional") == 0) type = COMPS_PACKAGE_OPTIONAL;
    else if (strcmp(s, "mandatory") == 0) type = COMPS_PACKAGE_MANDATORY;
    else if (strcmp(s, "conditional") == 0) type = COMPS_PACKAGE_CONDITIONAL;
    else type = COMPS_PACKAGE_UNKNOWN;
    return type;
}

void comps_parse_el_postprocess(const char *s, COMPS_Parsed *parsed)
{
    COMPS_List *list;
    COMPS_Dict *dict, *prop_dict;
    char *lang;
    COMPS_Prop *prop;

    /* check if there's all important data for currently processed element.
       This is also done by dtd checking, but if dtd rules missing, validation
       ends here (however isn't so strict as dtd)*/

    #define parent ((COMPS_Elem*)parsed->elem_stack->last->prev->data)->type
    #define grandparent ((COMPS_Elem*)parsed->elem_stack->last->prev->prev->data)->type
    #define list_last_cat ((COMPS_DocCategory*)list->last->data)
    #define list_last_env ((COMPS_DocEnv*)list->last->data)
    #define list_last_group ((COMPS_DocGroup*)list->last->data)
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
    switch (comps_elem_get_type(s)) {
        case COMPS_ELEM_UNKNOWN:
        case COMPS_ELEM_NONE:
            parsed->tmp_buffer = NULL;
            free(parsed->tmp_buffer);
        break;
        case COMPS_ELEM_DOC:
        break;
        case COMPS_ELEM_ENV:
            list = comps_dict_get(parsed->comps_doc->lobjects, "envs");
            if (!comps_dict_get(list_last_env->properties, "id"))
                comps_log_error(parsed->log, "id", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (!comps_dict_get(list_last_env->properties, "name"))
                comps_log_error(parsed->log, "name", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (!comps_dict_get(list_last_env->properties, "desc"))
                comps_log_error(parsed->log, "description", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (!list_last_env->group_list)
                comps_log_error(parsed->log, "grouplist", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (!list_last_env->option_list)
                comps_log_error(parsed->log, "optionlist", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
        break;
        case COMPS_ELEM_GROUP:
            list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
            if (!comps_dict_get(list_last_group->properties, "id"))
                comps_log_error(parsed->log, "id", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (comps_dict_get(list_last_group->properties, "name") == NULL) {
                printf("name is missing\n");
                comps_log_error(parsed->log, "name", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            }
            if (!comps_dict_get(list_last_group->properties, "desc"))
                comps_log_error(parsed->log, "description", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (!list_last_group->packages)
                comps_log_error(parsed->log, "packagelist", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
        break;
        case COMPS_ELEM_CATEGORY:
            list = comps_dict_get(parsed->comps_doc->lobjects, "categories");
            if (!comps_dict_get(list_last_cat->properties, "id"))
                comps_log_error(parsed->log, "id", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (!comps_dict_get(list_last_cat->properties, "name"))
                comps_log_error(parsed->log, "name", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (!comps_dict_get(list_last_cat->properties, "desc"))
                comps_log_error(parsed->log, "description", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
            if (!list_last_cat->group_ids)
                comps_log_error(parsed->log, "grouplist", COMPS_ERR_ELEM_REQUIRED,
                                parser_line, parser_col, 0);
        case COMPS_ELEM_WHITEOUT:
        case COMPS_ELEM_BLACKLIST:
        case COMPS_ELEM_LANGPACKS:
        case COMPS_ELEM_IGNOREDEP:
        case COMPS_ELEM_PACKAGE:
        case COMPS_ELEM_MATCH:
        break;
        case COMPS_ELEM_ID:
            if (parent == COMPS_ELEM_GROUP) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
                dict = list_last_group->properties;
                prop = comps_dict_get(list_last_group->properties, "id");
            } else if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "categories");
                dict = list_last_cat->properties;
                prop = comps_dict_get(list_last_cat->properties, "id");
            } else if (parent == COMPS_ELEM_ENV) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "envs");
                dict = list_last_env->properties;
                prop = comps_dict_get(list_last_env->properties, "id");
            } else {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            if (prop != NULL) {
                comps_log_warning(parsed->log, s, COMPS_ERR_ELEM_ALREADYSET,
                                  parser_line, parser_col, 0);
                __comps_doc_char_setter((void**)&prop->prop.str,
                                        parsed->tmp_buffer, 0);
            } else {
                prop = comps_doc_prop_str_create(parsed->tmp_buffer, 0);
                comps_dict_set(dict, "id", prop);
            }
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_NAME:
            lang = comps_dict_get(lastelem->attrs, "xml:lang");
            if (parent == COMPS_ELEM_GROUP) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
                dict = list_last_group->name_by_lang;
                prop_dict = list_last_group->properties;
                prop = comps_dict_get(list_last_group->properties, "name");
            } else if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "categories");
                dict = list_last_cat->name_by_lang;
                prop_dict = list_last_cat->properties;
                prop = comps_dict_get(list_last_cat->properties, "name");
            } else if (parent == COMPS_ELEM_ENV) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "envs");
                dict = list_last_env->name_by_lang;
                prop_dict = list_last_env->properties;
                prop = comps_dict_get(list_last_env->properties, "name");
            } else {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            if (lang) {
                comps_dict_set(dict, lang, parsed->tmp_buffer);
            } else {
                if (prop != NULL) {
                comps_log_warning(parsed->log, s, COMPS_ERR_ELEM_ALREADYSET,
                                  parser_line, parser_col, 0);
                __comps_doc_char_setter((void**)&prop->prop.str,
                                        parsed->tmp_buffer, 0);
                } else {
                    prop = comps_doc_prop_str_create(parsed->tmp_buffer, 0);
                    comps_dict_set(prop_dict, "name", prop);
                }
            }
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_DESC:
            lang = comps_dict_get(lastelem->attrs, "xml:lang");
            if (parent == COMPS_ELEM_GROUP) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
                dict = list_last_group->desc_by_lang;
                prop_dict = list_last_group->properties;
                prop = comps_dict_get(list_last_group->properties, "desc");
            } else if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "categories");
                dict = list_last_cat->desc_by_lang;
                prop_dict = list_last_cat->properties;
                prop = comps_dict_get(list_last_cat->properties, "desc");
            } else if (parent == COMPS_ELEM_ENV) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "envs");
                dict = list_last_env->desc_by_lang;
                prop_dict = list_last_env->properties;
                prop = comps_dict_get(list_last_env->properties, "desc");
            } else {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            if (lang) {
                comps_dict_set(dict, lang, parsed->tmp_buffer);
            } else {
                if (prop != NULL) {
                    comps_log_warning(parsed->log, s, COMPS_ERR_ELEM_ALREADYSET,
                                      parser_line, parser_col, 0);
                    __comps_doc_char_setter((void**)&prop->prop.str,
                                            parsed->tmp_buffer, 0);
                } else {
                    prop = comps_doc_prop_str_create(parsed->tmp_buffer, 0);
                    comps_dict_set(prop_dict, "desc", prop);
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
            list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
            if (!list_last_group->packages->first)
                comps_log_error(parsed->log, "packagelist", COMPS_ERR_LIST_EMPTY,
                                parser_line, parser_col, 0);
        break;
        case COMPS_ELEM_GROUPLIST:
            if (parent != COMPS_ELEM_CATEGORY &&
                parent != COMPS_ELEM_ENV) {
                parsed->tmp_buffer = NULL;
                break;
            }
            if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "categories");
                if (!list_last_cat->group_ids->first)
                    comps_log_error(parsed->log, "grouplist",
                                    COMPS_ERR_LIST_EMPTY,
                                    parser_line, parser_col, 0);
            } else {
                list = comps_dict_get(parsed->comps_doc->lobjects, "envs");
                if (!list_last_env->group_list->first)
                    comps_log_error(parsed->log, "grouplist",
                                    COMPS_ERR_LIST_EMPTY,
                                    parser_line, parser_col, 0);
            }
        break;
        case COMPS_ELEM_OPTLIST:
            if (parent != COMPS_ELEM_ENV) {
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_dict_get(parsed->comps_doc->lobjects, "envs");
            if (!list_last_env->group_list->first)
                comps_log_error(parsed->log, "grouplist", COMPS_ERR_LIST_EMPTY,
                                parser_line, parser_col, 0);
        break;
        case COMPS_ELEM_PACKAGEREQ:
            if (parent != COMPS_ELEM_PACKAGELIST &&
                grandparent != COMPS_ELEM_GROUP) {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
            }
        break;
        case COMPS_ELEM_DEFAULT:
            if (parent != COMPS_ELEM_GROUP) {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
            prop = comps_dict_get(list_last_group->properties, "def");
            if (prop) {
                comps_log_warning(parsed->log, s, COMPS_ERR_ELEM_ALREADYSET,
                                  parser_line, parser_col, 0);
            } else {
                prop = comps_doc_prop_num_create(0);
                comps_dict_set(list_last_group->properties, "def", prop);
            }
            if (strcmp(parsed->tmp_buffer,"false") == 0)
                prop->prop.num = 0;
            else if (strcmp(parsed->tmp_buffer,"true") == 0)
                prop->prop.num = 1;
            else {
                comps_log_warning(parsed->log, parsed->tmp_buffer,
                                  COMPS_ERR_DEFAULT_PARAM,
                                  parser_line, parser_col, 0);
            }
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
            list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
            prop = comps_dict_get(list_last_group->properties, "uservisible");
            if (prop) {
                comps_log_warning(parsed->log, s, COMPS_ERR_ELEM_ALREADYSET,
                                  parser_line, parser_col, 0);
            } else {
                prop = comps_doc_prop_num_create(0);
                comps_dict_set(list_last_group->properties, "uservisible", prop);
            }
            if (strcmp(parsed->tmp_buffer,"false") == 0)
                prop->prop.num = 0;
            else if (strcmp(parsed->tmp_buffer,"true") == 0)
                prop->prop.num = 1;
            else {
                comps_log_warning(parsed->log, parsed->tmp_buffer,
                                  COMPS_ERR_USERVISIBLE_PARAM,
                                  parser_line, parser_col, 0);
            }
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
                list = comps_dict_get(parsed->comps_doc->lobjects, "categories");
                comps_list_append(list_last_cat->group_ids,
                                  comps_list_item_create(parsed->tmp_buffer,
                                                         NULL, &free));
            } else if (grandparent == COMPS_ELEM_ENV) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "envs");
                if (list_last_env->option_list == NULL) {
                    if (list_last_env->group_list == NULL) {
                        comps_log_warning(parsed->log, parsed->tmp_buffer,
                                          COMPS_ERR_GROUPLIST_NOTSET,
                                          parser_line, parser_col, 0);
                    } else {
                        comps_list_append(list_last_env->group_list,
                                      comps_list_item_create(parsed->tmp_buffer,
                                                         NULL, &free));
                    }
                } else {
                    if (list_last_env->option_list == NULL) {
                        comps_log_warning(parsed->log, parsed->tmp_buffer,
                                          COMPS_ERR_OPTIONLIST_NOTSET,
                                          parser_line, parser_col, 0);
                    } else {
                        comps_list_append(list_last_env->option_list,
                                      comps_list_item_create(parsed->tmp_buffer,
                                                             NULL, &free));
                    }
                }
            } else {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_DISPLAYORDER:
            if (parent == COMPS_ELEM_CATEGORY) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "categories");
                prop = comps_dict_get(list_last_cat->properties, "display_order");
                dict = list_last_cat->properties;
            } else if (parent == COMPS_ELEM_ENV) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "envs");
                prop = comps_dict_get(list_last_env->properties, "display_order");
                dict = list_last_env->properties;
            } else if (parent == COMPS_ELEM_GROUP) {
                list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
                prop = comps_dict_get(list_last_group->properties, "display_order");
                dict = list_last_group->properties;
            } 
            if (prop) {
                comps_log_warning(parsed->log, s, COMPS_ERR_ELEM_ALREADYSET,
                                  parser_line, parser_col, 0);
            } else {
                prop = comps_doc_prop_num_create(0);
                comps_dict_set(dict, "display_order", prop);
            }
            sscanf(parsed->tmp_buffer, "%d", &prop->prop.num);

            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
        break;
        case COMPS_ELEM_LANGONLY:
            if (parent != COMPS_ELEM_GROUP) {
                free(parsed->tmp_buffer);
                parsed->tmp_buffer = NULL;
                break;
            }
            list = comps_dict_get(parsed->comps_doc->lobjects, "groups");
            prop = comps_dict_get(list_last_cat->properties, "lang_only");
            if (prop) {
                comps_log_warning(parsed->log, s, COMPS_ERR_ELEM_ALREADYSET,
                                  parser_line, parser_col, 0);
                __comps_doc_char_setter((void**)&prop->prop.str,
                                        parsed->tmp_buffer, 0);
            } else {
                prop = comps_doc_prop_str_create(parsed->tmp_buffer, 0);
                comps_dict_set(list_last_group->properties, "lang_only", prop);
            }
            parsed->tmp_buffer = NULL;
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
    COMPS_ListItem * it;
    COMPS_DocGroupPackage * package;
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
            comps_log_error(parsed->log, elem->name, COMPS_ERR_NOPARENT,
                            parser_line, parser_col, 0);
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
            return;
        }
    }
    if (elem->type == COMPS_ELEM_GROUPID
        || elem->type == COMPS_ELEM_PACKAGEREQ){
        if (!parent_o || !grandparent_o) {
            comps_log_error(parsed->log, elem->name, COMPS_ERR_NOPARENT,
                            parser_line, parser_col, 0);
            free(parsed->tmp_buffer);
            parsed->tmp_buffer = NULL;
            return;
        }
    }

    switch (elem->type)
    {
        case COMPS_ELEM_WHITEOUT:
        case COMPS_ELEM_BLACKLIST:
        case COMPS_ELEM_LANGPACKS:
        case COMPS_ELEM_NONE:
        break;
        case COMPS_ELEM_DOC:
            parsed->comps_doc = comps_doc_create(parsed->enc);
            comps_doc_init(parsed->comps_doc);
        break;
        case COMPS_ELEM_GROUP:
            if (parent != COMPS_ELEM_DOC) {
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col, 0);
                break;
            }
            group = comps_docgroup_create();
            category = NULL;
            env = NULL;
            comps_doc_add_group(parsed->comps_doc, group);
        break;
        case COMPS_ELEM_GROUPLIST:
            if (parent != COMPS_ELEM_CATEGORY && parent != COMPS_ELEM_ENV) {
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col, 0);
                break;
            }
            if (category) {
                if (category->group_ids != NULL) {
                    comps_log_warning(parsed->log,
                                      comps_elem_get_name(elem->type),
                                      COMPS_ERR_ELEM_ALREADYSET, parser_line,
                                      parser_col, 0);
                    break;
               }
                category->group_ids = comps_list_create();
                comps_list_init(category->group_ids);
            } else if (env) {
                if (env->group_list != NULL) {
                    comps_log_warning(parsed->log,
                                      comps_elem_get_name(elem->type),
                                      COMPS_ERR_ELEM_ALREADYSET, parser_line,
                                      parser_col, 0);
                    break;
                }
                env->group_list = comps_list_create();
                comps_list_init(env->group_list);
            }
        break;
        case COMPS_ELEM_OPTLIST:
            if (parent != COMPS_ELEM_ENV) {
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
            if (env->option_list != NULL) {
                comps_log_warning(parsed->log,
                                  comps_elem_get_name(elem->type),
                                  COMPS_ERR_ELEM_ALREADYSET, parser_line,
                                  parser_col, 0);
                break;
            }
            env->option_list = comps_list_create();
            comps_list_init(env->option_list);
        break;
        case COMPS_ELEM_CATEGORY:
            if (parent != COMPS_ELEM_DOC) {
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
            category = comps_doccategory_create();
            group = NULL;
            env = NULL;
            comps_doc_add_category(parsed->comps_doc, category);
        break;
        case COMPS_ELEM_ENV:
            if (parent != COMPS_ELEM_DOC) {
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
            env = comps_docenv_create();
            comps_doc_add_env(parsed->comps_doc, env);
            group = NULL;
            category = NULL;
        break;
        case COMPS_ELEM_ID:
        case COMPS_ELEM_NAME:
        case COMPS_ELEM_DESC:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            if (parent != COMPS_ELEM_CATEGORY && parent != COMPS_ELEM_ENV
                && parent != COMPS_ELEM_GROUP) {
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
        break;
        case COMPS_ELEM_LANGONLY:
        case COMPS_ELEM_DEFAULT:
        case COMPS_ELEM_USERVISIBLE:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            if (parent != COMPS_ELEM_GROUP) {
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
        break;
        case COMPS_ELEM_PACKAGELIST:
            if (parent != COMPS_ELEM_GROUP) {
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
            group->packages = comps_list_create();
            comps_list_init(group->packages);
        break;
        case COMPS_ELEM_PACKAGEREQ:
            if (grandparent != COMPS_ELEM_GROUP || parent != COMPS_ELEM_PACKAGELIST){
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                parsed->text_buffer_pt = &parsed->tmp_buffer;
                break;
            }
            package = comps_docpackage_create();
            it = comps_list_item_create(package,
                                   NULL, &comps_docpackage_destroy);
            comps_list_append(group->packages, it);

            parsed->text_buffer_pt = &package->name;
            package->type = comps_package_get_type(comps_dict_get(elem->attrs,
                                                                  "type"));

            if (package->type == COMPS_PACKAGE_UNKNOWN)
                comps_log_warning(parsed->log,
                          comps_dict_get(elem->attrs, "type"),
                          COMPS_ERR_PACKAGE_UNKNOWN, parser_line, parser_col, 0);
        break;
        case COMPS_ELEM_DISPLAYORDER:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            if (parent != COMPS_ELEM_CATEGORY && parent != COMPS_ELEM_ENV &&
                parent != COMPS_ELEM_GROUP){
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
        break;
        case COMPS_ELEM_GROUPID:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            if ((grandparent != COMPS_ELEM_CATEGORY
                && grandparent != COMPS_ELEM_ENV) ||
                (parent != COMPS_ELEM_GROUPLIST
                && parent != COMPS_ELEM_OPTLIST)){
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
        break;
        case COMPS_ELEM_MATCH:
            if (parent != COMPS_ELEM_LANGPACKS){
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
            comps_doc_add_langpack(parsed->comps_doc,
                     comps_dict_get(elem->attrs, "name"),
                     __comps_str_clone(comps_dict_get(elem->attrs, "install")));
        break;
        case COMPS_ELEM_PACKAGE:
            if (parent != COMPS_ELEM_BLACKLIST){
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
            comps_doc_add_blacklist(parsed->comps_doc,
                                    comps_dict_get(elem->attrs, "name"),
                     __comps_str_clone(comps_dict_get(elem->attrs, "arch")));
        break;
        case COMPS_ELEM_IGNOREDEP:
            if (parent != COMPS_ELEM_WHITEOUT){
                comps_log_error(parsed->log, comps_elem_get_name(elem->type),
                                COMPS_ERR_NOPARENT, parser_line, parser_col,
                                0);
                break;
            }
            comps_doc_add_whiteout(parsed->comps_doc,
                                      comps_dict_get(elem->attrs, "requires"),
                    __comps_str_clone(comps_dict_get(elem->attrs, "package")));
            
        break;
        case COMPS_ELEM_UNKNOWN:
            parsed->text_buffer_pt = &parsed->tmp_buffer;
            comps_log_warning(parsed->log, elem->name, COMPS_ERR_ELEM_UNKNOWN,
                           XML_GetCurrentLineNumber(parsed->parser),
                           XML_GetCurrentColumnNumber(parsed->parser), 0);
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

    COMPS_Elem * elem = NULL;
    COMPS_ElemType type;
    COMPS_ListItem * item;

    /* create new element */
    type = comps_elem_get_type(s);
    elem = comps_elem_create(s, attrs, type);
    if (elem == NULL) {
        comps_log_error(((COMPS_Parsed*)userData)->log, NULL,
                        COMPS_ERR_MALLOC, 0, 0, 0);
        raise(SIGABRT);
        return;
    }
    item = comps_list_item_create(elem, NULL, &comps_elem_destroy);
    if (((COMPS_Parsed*)userData)->text_buffer->first) {
            comps_log_error(((COMPS_Parsed*)userData)->log,
                     (char*)((COMPS_Parsed*)userData)->text_buffer->first->data,
                            COMPS_ERR_TEXT_BETWEEN, parser_line, parser_col, 0);
        comps_list_clear(((COMPS_Parsed*)userData)->text_buffer);
        ((COMPS_Parsed*)userData)->text_buffer_len = 0;
    }

    /* end append it to element stack */
    comps_list_append(((COMPS_Parsed*)userData)->elem_stack, item);
    /* preprocess new element */
    comps_parse_el_preprocess(elem, (COMPS_Parsed*)userData);

    #undef parser_line
    #undef parser_col
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
        comps_log_error(((COMPS_Parsed*)userData)->log, NULL,
                        COMPS_ERR_MALLOC, 0, 0, 0);
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
