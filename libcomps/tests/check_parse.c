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

#include <check.h>
#include <stdio.h>

#include "../src/comps_doc.h"
#include "../src/comps_parse.h"
#include "../src/comps_docpackage.h"

void print_all_str(COMPS_RTree *rt) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;
    char *x;

    printf("-------- print pairs --------------\n");
    pairlist = comps_rtree_pairs(rt);

    for (it = pairlist->first; it != NULL; it = it->next) {
        x = ((COMPS_RTreePair*)it->data)->data;
        printf("%s = %s \n",  ((COMPS_RTreePair*)it->data)->key,
                              x);

    }
    comps_hslist_destroy(&pairlist);
    printf("-----------------------------------\n");
}

START_TEST(test_comps_parse1)
{
    FILE *fp;
    //char *err_log;
    COMPS_Parsed *parsed;

    const char *groups_ids[] = {"additional-devel", "backup-client", "backup-server"};
    const char *groups_names[] = {"Additional Development", "Backup Client",
                                  "Backup Server"};
    const char *groups_descs[] = {"Additional development headers and libraries"
                                " for developing applications",
                                "Client tools for connecting to a backup server"
                                " and doing backups.",
                                "Software to centralize your infrastructure's"
                                " backups."};

    const int group_packages[][3] = {{40, 62, 0}, {0,1,1}, {0,2,2}};

    const char* cats_ids[] = {"cat 1", "cat 2"};
    const char* cats_names[] = {"category 1", "category 2"};
    const int cats_gids[] = {1, 2};

    int ret;
    COMPS_Object *tmpobj, *tmpobj2;
    char *tmpstr;

    COMPS_ObjList * tmplist;
    fprintf(stderr, "## Running test_parse1\n");

    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);


    fp = fopen("sample-comps.xml", "r");
    comps_parse_file(parsed, fp, NULL);
    //fail_unless(comps_parse_validate_dtd("sample-comps.xml", "comps.dtd"));

    if (parsed->log->entries->first != NULL) {
        //err_log = comps_log_str(parsed->log);
        ck_assert_msg(parsed->log->entries->first != NULL,
                "Some errors have been found (and shouldn't have) during parsing");
        //free(err_log);
    }
    tmplist = comps_doc_groups(parsed->comps_doc);
    ck_assert_msg(tmplist->len == 3, "Should have 3 groups parsed."
            "Have %d", tmplist->len);
    COMPS_OBJECT_DESTROY(tmplist);
    tmplist = comps_doc_categories(parsed->comps_doc);
    ck_assert_msg(tmplist->len == 2, "Should have 2 categories"
            "parsed. Have %d", tmplist->len);
    COMPS_OBJECT_DESTROY(tmplist);
    tmplist = comps_doc_environments(parsed->comps_doc);
    fail_if(tmplist->len != 1,
            "Should have 1 environment parsed. Have %d", tmplist->len);
    COMPS_OBJECT_DESTROY(tmplist);


    for (int i=0; i<3; i++) {
        tmplist = comps_doc_groups(parsed->comps_doc);
        tmpobj2 = comps_objlist_get_x(tmplist, i);
        fail_if(tmpobj2 == NULL, "Group not found");
        tmpobj = comps_docgroup_get_id((COMPS_DocGroup*)tmpobj2);
        tmpstr = comps_object_tostr(tmpobj);
        fail_if(strcmp(tmpstr, groups_ids[i]) != 0,
                       "%d.group should have id:%s not %s",
                        i, groups_ids[i], tmpstr);
        free(tmpstr);
        COMPS_OBJECT_DESTROY(tmpobj);

        tmpobj = comps_docgroup_get_name((COMPS_DocGroup*)tmpobj2);
        tmpstr = comps_object_tostr(tmpobj);
        fail_if(strcmp(tmpstr, groups_names[i]) != 0,
                       "%d.group should have name:%s not %s",
                        i, groups_names[i], tmpstr);
        free(tmpstr);
        COMPS_OBJECT_DESTROY(tmpobj);

        tmpobj = comps_docgroup_get_desc((COMPS_DocGroup*)tmpobj2);
        tmpstr = comps_object_tostr(tmpobj);
        fail_if(strcmp(tmpstr, groups_descs[i]) != 0,
                       "%d.group should have desc:%s not %s",
                        i, groups_descs[i], tmpstr);
        free(tmpstr);
        COMPS_OBJECT_DESTROY(tmpobj);
        COMPS_OBJECT_DESTROY(tmplist);

        tmplist = comps_docgroup_get_packages((COMPS_DocGroup*)tmpobj2, NULL,
                                             COMPS_PACKAGE_DEFAULT);
        fail_if(group_packages[i][0] != tmplist->len, "Group #%d should have"
                " %d default packages, Have %d", i, group_packages[i][0],
                tmplist->len);
        COMPS_OBJECT_DESTROY(tmplist);
        tmplist = comps_docgroup_get_packages((COMPS_DocGroup*)tmpobj2, NULL,
                                             COMPS_PACKAGE_OPTIONAL);
        fail_if(group_packages[i][1] != tmplist->len, "Group #%d should have"
                " %d optional packages, Have %d", i, group_packages[i][1],
                tmplist->len);
        COMPS_OBJECT_DESTROY(tmplist);
    }
    for (int i=0; i<2; i++) {
        tmplist = comps_doc_categories(parsed->comps_doc);
        tmpobj2 = comps_objlist_get(tmplist, i);
        tmpobj = comps_doccategory_get_id((COMPS_DocCategory*)tmpobj2);
        tmpstr = comps_object_tostr(tmpobj);
        fail_if(strcmp(tmpstr, cats_ids[i]) != 0,
                       "%s. category should have id:%s not %s",
                        i, cats_ids[i], tmpstr);
        free(tmpstr);
        COMPS_OBJECT_DESTROY(tmpobj);

        tmpobj = comps_doccategory_get_name((COMPS_DocCategory*)tmpobj2);
        tmpstr = comps_object_tostr(tmpobj);
        fail_if(strcmp(tmpstr, cats_names[i]) != 0,
                       "%s. category should have name:%s not %s",
                        i, cats_names[i], tmpstr);
        free(tmpstr);
        COMPS_OBJECT_DESTROY(tmpobj);

        fail_if(((COMPS_DocCategory*)tmpobj2)->group_ids->len != cats_gids[i],
                "Category #%d should have %d groupids, have %d", i,
                cats_gids[i], ((COMPS_DocCategory*)tmpobj2)->group_ids->len);
        COMPS_OBJECT_DESTROY(tmpobj2);
        COMPS_OBJECT_DESTROY(tmplist);
    }

    fp = fopen("sample-bad-elem.xml", "r");
    comps_parse_parsed_destroy(parsed);

    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    comps_parse_file(parsed, fp, NULL);
    ret = comps_parse_validate_dtd("sample-bad-elem.xml", "comps.dtd");
    fail_if(ret >0, "XML shouldn't be valid. Validation returned: %d", ret);

    if (parsed->log->entries->first != NULL) {
        //err_log = comps_log_str(parsed->log);
        ck_assert_msg(parsed->log->entries->first != NULL,
                      "No errors have found during parsing (and should have)");
        //free(err_log);
    }
    comps_parse_parsed_destroy(parsed);
}
END_TEST

int check_errors(COMPS_Log *log, COMPS_LogEntry ** known_errors,
                  int known_len) {
    COMPS_HSListItem *it;
    int i;

    it = log->entries->first;
    for (i = 0; it != NULL && i != known_len; it = it->next, i++) {
        fail_if(((COMPS_LogEntry*)it->data)->arg_count !=
                known_errors[i]->arg_count,
                "%d err opt_message doesn't match (%d != %d)", i,
                ((COMPS_LogEntry*)it->data)->arg_count,
                known_errors[i]->arg_count);
        fail_if(((COMPS_LogEntry*)it->data)->code !=
                    known_errors[i]->code, "%d. err code different\n (%d != %d)",
                    i, ((COMPS_LogEntry*)it->data)->code,
                    known_errors[i]->code);
        for (int x = 0; x < known_errors[i]->arg_count; x++) {
            char *_x, *_y;
            _x = comps_object_tostr(((COMPS_LogEntry*)it->data)->args[x]);
            _y = comps_object_tostr(known_errors[i]->args[x]);
            fail_if(comps_object_cmp(((COMPS_LogEntry*)it->data)->args[x],
                                     known_errors[i]->args[x]) == 0, "%d. %s != %s",
                                     x, _x, _y);
            free(_x);
            free(_y);
        }
    }
    return i;
}

COMPS_LogEntry* __log_entry_x(int code, int n, ...){
    COMPS_LogEntry *entry;
    COMPS_Object *val;
    va_list arg_list;
    
    va_start(arg_list, n);
    entry = comps_log_entry_create();

    entry->args = malloc(sizeof(COMPS_Object*) * n);
    entry->arg_count = n;
    entry->code = code;
    //entry->type = type;
    for (int i=0; i<n; i++) {
        val = va_arg(arg_list, COMPS_Object*);
        entry->args[i] = val;
    }
    va_end(arg_list);
    return entry;
}

START_TEST(test_comps_parse2)
{
    FILE *fp;
    //char *err_log;
    COMPS_Parsed *parsed;
    //COMPS_ListItem *it;
    int i;

    fprintf(stderr, "## Running test_parse2\n");
    COMPS_LogEntry* known_errors[10];
    known_errors[0] = __log_entry_x(COMPS_ERR_NOCONTENT, 3,
                                    comps_str("description"),
                                    comps_num(265), comps_num(18));
    known_errors[1] = __log_entry_x(COMPS_ERR_LIST_EMPTY, 3,
                                    comps_str("packagelist"),
                                    comps_num(270), comps_num(4));
    known_errors[2] = __log_entry_x(COMPS_ERR_ELEM_REQUIRED, 3,
                                    comps_str("description"),
                                    comps_num(271), comps_num(2));
    known_errors[3] = __log_entry_x(COMPS_ERR_LIST_EMPTY, 3,
                                    comps_str("packagelist"),
                                    comps_num(320), comps_num(4));
    known_errors[4] = __log_entry_x(COMPS_ERR_NOCONTENT, 3,
                                    comps_str("description"),
                                    comps_num(379), comps_num(18));
    known_errors[5] = __log_entry_x(COMPS_ERR_LIST_EMPTY, 3,
                                    comps_str("packagelist"),
                                    comps_num(384), comps_num(4));
    known_errors[6] = __log_entry_x(COMPS_ERR_ELEM_REQUIRED, 3,
                                    comps_str("description"),
                                    comps_num(385), comps_num(2));
    known_errors[7] = __log_entry_x(COMPS_ERR_NOCONTENT, 3,
                                    comps_str("description"),
                                    comps_num(440), comps_num(18));
    known_errors[8] = __log_entry_x(COMPS_ERR_LIST_EMPTY, 3,
                                    comps_str("packagelist"),
                                    comps_num(445), comps_num(4));
    known_errors[9] = __log_entry_x(COMPS_ERR_ELEM_REQUIRED, 3,
                                    comps_str("description"),
                                    comps_num(446), comps_num(2));
    //known_errors[7] = comps_log_entry_create("optionlist", 0,
    //                                         COMPS_ERR_ELEM_REQUIRED, 1201, 2, 0);

    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    fp = fopen("sample_comps.xml", "r");
    comps_parse_file(parsed, fp, NULL);

    fail_if(parsed->log->entries->first == NULL);
    i = check_errors(parsed->log, known_errors, 10);

    fail_if(i != 10);

    comps_parse_parsed_destroy(parsed);
    for (i = 0; i < 10; i++) {
        comps_log_entry_destroy(known_errors[i]);
    }
}
END_TEST

START_TEST(test_comps_parse3)
{
    FILE *fp;
    //char *err_log,
    COMPS_Parsed *parsed;
    COMPS_ObjListIt *it;
    int i;
    COMPS_ObjList *tmplist;
    COMPS_LogEntry* known_errors[3];
    COMPS_Object *tmpobj;

    fprintf(stderr, "## Running test_parse3\n");

    known_errors[0] = __log_entry_x(COMPS_ERR_ELEM_REQUIRED, 3,
                                             comps_str("id"), comps_num(188),
                                             comps_num(2));
    known_errors[1] = __log_entry_x(COMPS_ERR_ELEM_REQUIRED, 3,
                                            comps_str("name"), comps_num(188),
                                            comps_num(2));
    known_errors[2] = __log_entry_x(COMPS_ERR_ELEM_REQUIRED, 3,
                                             comps_str("description"),
                                             comps_num(188), comps_num(2));

    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    fp = fopen("sample_comps_bad1.xml", "r");
    comps_parse_file(parsed, fp, NULL);

    fail_if(parsed->log->entries->first == NULL);
    check_errors(parsed->log, known_errors, 3);

    for (i = 0; i < 3; i++) {
        comps_log_entry_destroy(known_errors[i]);
    }
    tmplist = comps_doc_groups(parsed->comps_doc);
    it = tmplist->first;

    /*pairlist = comps_rtree_pairs(((COMPS_DocCategory*)it->data)->properties);
    for (hsit = pairlist->first; hsit != NULL; hsit = hsit->next){
        printf("%s = %s\n", ((COMPS_RTreePair*)hsit->data)->key,
                          ((COMPS_RTreePair*)hsit->data)->data);
    }*/
    COMPS_OBJECT_DESTROY(tmplist);

    tmpobj = comps_docgroup_get_id((COMPS_DocGroup*)it->comps_obj);
    fail_if(tmpobj, "%d. category should have NULL id\n");
    COMPS_OBJECT_DESTROY(tmpobj);
    tmpobj = comps_docgroup_get_name((COMPS_DocGroup*)it->comps_obj);
    fail_if(tmpobj, "%d. category should have NULL name\n");
    COMPS_OBJECT_DESTROY(tmpobj);
    tmpobj = comps_docgroup_get_desc((COMPS_DocGroup*)it->comps_obj);
    fail_if(tmpobj, "%d. category should have NULL description\n");
    COMPS_OBJECT_DESTROY(tmpobj);
    comps_parse_parsed_destroy(parsed);
}
END_TEST

START_TEST(test_comps_parse4)
{
    FILE *fp;
    //char *err_log;
    COMPS_Parsed *parsed;
    //COMPS_ListItem *it;
    //int ret,
    int i;
    //COMPS_List * tmplist;
    COMPS_LogEntry* known_errors[15];
    fprintf(stderr, "## Running test_parse4\n");

    known_errors[0] = __log_entry_x(COMPS_ERR_NOPARENT, 3, comps_str("id"),
                                    comps_num(4), comps_num(2));
    known_errors[1] = __log_entry_x(COMPS_ERR_NOPARENT, 3, comps_str("packagereq"),
                                    comps_num(158), comps_num(4));
    known_errors[2] = __log_entry_x(COMPS_ERR_ELEM_ALREADYSET, 3,
                                    comps_str("langonly"),
                                    comps_num(274), comps_num(16));
    known_errors[3] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(588), comps_num(4));
    known_errors[4] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("grouplist"),
                                    comps_num(880), comps_num(2));

    known_errors[5] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(881), comps_num(4));
    known_errors[6] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(882), comps_num(4));
    known_errors[7] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(883), comps_num(4));
    known_errors[8] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(884), comps_num(4));
    known_errors[9] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(885), comps_num(4));
    known_errors[10] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(886), comps_num(4));
    known_errors[11] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(887), comps_num(4));


    known_errors[12] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("id"),
                                    comps_num(1210), comps_num(2));
    known_errors[13] = __log_entry_x(COMPS_ERR_NOPARENT, 3,
                                    comps_str("groupid"),
                                    comps_num(1228), comps_num(4));
    known_errors[14] = __log_entry_x(COMPS_ERR_ELEM_ALREADYSET, 3,
                                     comps_str("optionlist"),
                                     comps_num(1244), comps_num(4));

    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    fp = fopen("sample_comps_bad2.xml", "r");
    comps_parse_file(parsed, fp, NULL);

    fail_if(parsed->log->entries->first == NULL);
    check_errors(parsed->log, known_errors, 15);

    for (i = 0; i < 15; i++) {
        comps_log_entry_destroy(known_errors[i]);
    }
    comps_parse_parsed_destroy(parsed);
}
END_TEST

START_TEST(test_comps_parse5)
{
    FILE *fp;
    //char *err_log;
    COMPS_Parsed *parsed;
    //COMPS_ListItem *it;
    //int ret
    int i;
    fprintf(stderr, "## Running test_parse5\n");
    COMPS_LogEntry* known_errors[2];

    known_errors[0] = __log_entry_x(COMPS_ERR_TEXT_BETWEEN, 3,
                                    comps_str("some stray"), comps_num(6),
                                    comps_num(4));
    known_errors[1] = __log_entry_x(COMPS_ERR_TEXT_BETWEEN, 3,
                                    comps_str("    some stray"), comps_num(189),
                                    comps_num(2));

    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    fp = fopen("sample_comps_bad3.xml", "r");
    comps_parse_file(parsed, fp, NULL);
    //comps_log_print(parsed->log);

    fail_if(parsed->log->entries->first == NULL);
    check_errors(parsed->log, known_errors, 2);
    //comps2xml_f(parsed->comps_doc, "fed2.xml", 0);

    for (i = 0; i < 2; i++) {
        comps_log_entry_destroy(known_errors[i]);
    }
    comps_parse_parsed_destroy(parsed);
}
END_TEST

START_TEST(test_comps_fedora_parse)
{
    COMPS_Parsed *parsed;
    //COMPS_HSList *list;
    
    //COMPS_ListItem *it;
    FILE *fp;
    //char *tmp;
    fprintf(stderr, "## Running test_parse fedora\n");
    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    fp = fopen("fedora_comps.xml", "r");
    comps_parse_file(parsed, fp, NULL);
    //printf("log len:%d\n", parsed->log->logger_data->len);
    fail_if(parsed->fatal_error != 0, "Some fatal errors found after parsing");
    //printf("log len:%d\n", parsed->log->logger_data->len);
    //err_str = comps_log_str(parsed->log);
    //printf("parsed err log: %s", err_str);
    //free(err_str);
    //printf("fedora comps groups:%d\n", comps_doc_groups(parsed->comps_doc)->len);
    //printf("fedora comps categories:%d\n", comps_doc_categories(parsed->comps_doc)->len);

    //print_all_str(((COMPS_DocGroup*)comps_doc_groups(parsed->comps_doc)
    //                                ->first->data)->name_by_lang);
    comps2xml_f(parsed->comps_doc, "fed2.xml", 0, NULL, NULL);
    //printf("%s\n", tmp);

    //free(tmp);
    comps_parse_parsed_destroy(parsed);
}
END_TEST

START_TEST(test_main2)
{
    COMPS_Parsed *parsed;
    FILE *fp;
    //char *tmp;
    fprintf(stderr, "## Running test_parse main2\n");
    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    fp = fopen("main_comps2.xml", "r");
    comps_parse_file(parsed, fp, NULL);
    fail_if(parsed->fatal_error != 0, "Some fatal errors found after parsing");
    //comps2xml_f(parsed->comps_doc, "fed2.xml", 0);
    comps_parse_parsed_destroy(parsed);
}
END_TEST

START_TEST(test_arch)
{
    COMPS_Parsed *parsed;
    COMPS_Doc *doc2;
    COMPS_ObjList *arches, *list, *list2;
    COMPS_DocGroup *g;
    COMPS_DocCategory *c;
    COMPS_DocEnv *e;
    COMPS_Str *str;
    int x;
    COMPS_ObjListIt *it;
    char *grps[3][5] = {{"group1", "group3", "group4", NULL, NULL},
                       {"group1", "group2", "group3", "group4", NULL},
                       {"group3", "group4", NULL, NULL, NULL}};
    char *g1_pkgs[3][8] = {{"pkg3", "pkg4", "pkg5", "pkg6", "pkg7", NULL, NULL, NULL},
                        {"pkg3", "pkg4", "pkg7", NULL, NULL, NULL, NULL, NULL},
                        {"pkg1", "pkg2", "pkg3", "pkg4", "pkg5", "pkg6", "pkg7",
                         NULL}};

    char *cats[3][5] = {{"cat1", "cat4", NULL, NULL, NULL},
                       {"cat1", "cat2", "cat4", NULL, NULL},
                       {"cat3", "cat4", NULL, NULL, NULL}};
    char *c1_gids[3][9] = {{"g1", "g3", "g4", "g6", "g7", NULL, NULL, NULL, NULL},
                           {"g1", "g2", "g4", "g5", "g7", NULL, NULL, NULL, NULL},
                           {"g4", "g5", "g6", "g7", NULL, NULL, NULL, NULL, NULL}};

    char *envs[3][5] = {{"env1", "env4", NULL, NULL, NULL},
                       {"env1", "env2", "env3", "env4", NULL},
                       {"env1", "env2", "env4", NULL, NULL}};
    char *e1_gids[3][8] = {{"g1", "g3", "g4", NULL, NULL, NULL, NULL, NULL},
                           {"g1", "g2", "g5", NULL, NULL, NULL, NULL, NULL},
                           {"g2", "g4", NULL, NULL, NULL,NULL, NULL, NULL}};
    char *e1_oids[3][8] = {{"o1", "o3", "o4", NULL, NULL, NULL, NULL, NULL},
                           {"o1", "o2", "o5", NULL, NULL, NULL, NULL, NULL},
                           {"o2", "o4", NULL, NULL, NULL,NULL, NULL, NULL}};

    FILE *fp;
    //char *tmp;
    fprintf(stderr, "## Running test_parse arch\n");
    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    fp = fopen("main_arches.xml", "r");
    comps_parse_file(parsed, fp, NULL);
    //comps_log_print(parsed->log);
    fail_if(parsed->fatal_error != 0, "Some fatal errors found after parsing");
    //comps2xml_f(parsed->comps_doc, "fed2.xml", 0);
    arches = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo, NULL);
    comps_objlist_append_x(arches, (COMPS_Object*)comps_str("x86"));

    doc2 = comps_doc_arch_filter(parsed->comps_doc, arches);
    COMPS_OBJECT_DESTROY(arches);

    list = comps_doc_groups(doc2);
    ck_assert(list->len == 3);
    for (it = list->first, x=0; it != NULL; it = it->next, x++) {
        g = (COMPS_DocGroup*)it->comps_obj;
        str = (COMPS_Str*)comps_docgroup_get_id(g);
        ck_assert_msg(strcmp(str->val, grps[0][x]) == 0, "%s != %s",
                      str->val, grps[0][x]);
        COMPS_OBJECT_DESTROY(str);
    }
    g = (COMPS_DocGroup*)list->first->comps_obj;
    list2 = g->packages;
    for (x=0, it = list2->first; it != NULL; it = it->next, x++) {
        //printf("%s\n", ((COMPS_DocGroupPackage*)it->comps_obj)->name->val);
        if (g1_pkgs[0][x] == NULL)
            break;
        ck_assert(strcmp(((COMPS_DocGroupPackage*)it->comps_obj)->name->val,
                         g1_pkgs[0][x]) == 0);
    }
    COMPS_OBJECT_DESTROY(list);

    list = comps_doc_categories(doc2);
    ck_assert(list->len == 2);
    for (it = list->first, x=0; it != NULL; it = it->next, x++) {
        c = (COMPS_DocCategory*)it->comps_obj;
        str = (COMPS_Str*)comps_doccategory_get_id(c);
        ck_assert_msg(strcmp(str->val, cats[0][x]) == 0, "%s != %s",
                      str->val, cats[0][x]);
        COMPS_OBJECT_DESTROY(str);
    }
    c = (COMPS_DocCategory*)list->first->comps_obj;
    list2 = c->group_ids;
    for (x=0, it = list2->first; it != NULL; it = it->next, x++) {
        //printf("%s\n", ((COMPS_DocGroupId*)it->comps_obj)->name->val);
        if (c1_gids[0][x] == NULL)
            break;
        ck_assert(strcmp(((COMPS_DocGroupId*)it->comps_obj)->name->val,
                         c1_gids[0][x]) == 0);
    }
    COMPS_OBJECT_DESTROY(list);

    list = comps_doc_environments(doc2);
    ck_assert(list->len == 2);
    for (it = list->first, x=0; it != NULL; it = it->next, x++) {
        e = (COMPS_DocEnv*)it->comps_obj;
        str = (COMPS_Str*)comps_docenv_get_id(e);
        ck_assert_msg(strcmp(str->val, envs[0][x]) == 0, "%s != %s",
                      str->val, envs[0][x]);
        COMPS_OBJECT_DESTROY(str);
    }
    e = (COMPS_DocEnv*)list->first->comps_obj;
    list2 = e->group_list;
    for (x=0, it = list2->first; it != NULL; it = it->next, x++) {
        //printf("%s\n", ((COMPS_DocGroupId*)it->comps_obj)->name->val);
        if (e1_gids[0][x] == NULL)
            break;
        ck_assert_msg(strcmp(((COMPS_DocGroupId*)it->comps_obj)->name->val,
                         e1_gids[0][x]) == 0, "%s != %s",
                         ((COMPS_DocGroupId*)it->comps_obj)->name->val,
                         e1_gids[0][x]);
    }
    list2 = e->option_list;
    for (x=0, it = list2->first; it != NULL; it = it->next, x++) {
        if (e1_oids[0][x] == NULL)
            break;
        ck_assert(strcmp(((COMPS_DocGroupId*)it->comps_obj)->name->val,
                         e1_oids[0][x]) == 0);
    }
    COMPS_OBJECT_DESTROY(list);

    COMPS_OBJECT_DESTROY(doc2);
    comps_parse_parsed_destroy(parsed);
}
END_TEST

Suite* basic_suite (void)
{
    Suite *s = suite_create ("Basic Tests");
    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_comps_parse1);
    tcase_add_test (tc_core, test_comps_parse2);
    tcase_add_test (tc_core, test_comps_parse3);
    tcase_add_test (tc_core, test_comps_parse4);
    tcase_add_test (tc_core, test_comps_parse5);
    tcase_add_test (tc_core, test_comps_fedora_parse);

    tcase_add_test (tc_core, test_main2);
    tcase_add_test (tc_core, test_arch);

    tcase_set_timeout(tc_core, 15);
    suite_add_tcase (s, tc_core);

    return s;
}

int main(int argc, char * argv[])
{
    int number_failed;
    (void)argc;
    (void)argv;

    Suite *s = basic_suite ();
    SRunner *sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
