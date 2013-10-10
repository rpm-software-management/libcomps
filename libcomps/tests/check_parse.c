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
    char *err_log;
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

    const char* envs_ids[] = {"env 1"};
    const char* envs_names[] = {"environment 1"};
    const char* env_gids[][3] = {{"a", "b", "c"}};
    const char* env_opids[][2] = {{" option 1 ", " option 2 "}};

    int ret;
    COMPS_Object *tmpobj, *tmpobj2;
    char *tmpstr;

    COMPS_ObjList * tmplist;
    fprintf(stderr, "## Running test_parse1\n\n");

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);


    fp = fopen("sample-comps.xml", "r");
    comps_parse_file(parsed, fp);
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
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    comps_parse_file(parsed, fp);
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
            fail_if(comps_object_cmp(((COMPS_LogEntry*)it->data)->args[x],
                                     known_errors[i]->args[x]) == 0);
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

    fprintf(stderr, "## Running test_parse2\n\n");
    COMPS_LogEntry* known_errors[8];
    known_errors[0] = __log_entry_x(COMPS_ERR_NOCONTENT, 3,
                                    comps_str("description"),
                                    comps_num(265), comps_num(18));
    known_errors[1] = __log_entry_x(COMPS_ERR_LIST_EMPTY, 3,
                                    comps_str("packagelist"),
                                    comps_num(270), comps_num(4));
    known_errors[2] = __log_entry_x(COMPS_ERR_LIST_EMPTY, 3,
                                    comps_str("packagelist"),
                                    comps_num(320), comps_num(4));
    known_errors[3] = __log_entry_x(COMPS_ERR_NOCONTENT, 3,
                                    comps_str("description"),
                                    comps_num(379), comps_num(18));
    known_errors[4] = __log_entry_x(COMPS_ERR_LIST_EMPTY, 3,
                                    comps_str("packagelist"),
                                    comps_num(384), comps_num(4));
    known_errors[5] = __log_entry_x(COMPS_ERR_NOCONTENT, 3,
                                    comps_str("description"),
                                    comps_num(440), comps_num(18));
    known_errors[6] = __log_entry_x(COMPS_ERR_LIST_EMPTY, 3,
                                    comps_str("packagelist"),
                                    comps_num(445), comps_num(4));
    //known_errors[7] = comps_log_entry_create("optionlist", 0,
    //                                         COMPS_ERR_ELEM_REQUIRED, 1201, 2, 0);

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample_comps.xml", "r");
    comps_parse_file(parsed, fp);

    fail_if(parsed->log->entries->first == NULL);
    i = check_errors(parsed->log, known_errors, 8);

    fail_if(i != 7);

    comps_parse_parsed_destroy(parsed);
    for (i = 0; i < 7; i++) {
        comps_log_entry_destroy(known_errors[i]);
    }
}
END_TEST

START_TEST(test_comps_parse3)
{
    FILE *fp;
    //char *err_log,
    char *tmp_ch;
    COMPS_Parsed *parsed;
    COMPS_ObjListIt *it;
    int i;
    COMPS_ObjList *tmplist;
    COMPS_LogEntry* known_errors[3];
    char *str;
    COMPS_Object *tmpobj;

    fprintf(stderr, "## Running test_parse3\n\n");

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
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample_comps_bad1.xml", "r");
    comps_parse_file(parsed, fp);

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
    fprintf(stderr, "## Running test_parse4\n\n");

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
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample_comps_bad2.xml", "r");
    comps_parse_file(parsed, fp);

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
    fprintf(stderr, "## Running test_parse5\n\n");
    COMPS_LogEntry* known_errors[2];

    known_errors[0] = __log_entry_x(COMPS_ERR_TEXT_BETWEEN, 3,
                                    comps_str("some stray"), comps_num(6),
                                    comps_num(4));
    known_errors[1] = __log_entry_x(COMPS_ERR_TEXT_BETWEEN, 3,
                                    comps_str("    some stray"), comps_num(189),
                                    comps_num(2));

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample_comps_bad3.xml", "r");
    comps_parse_file(parsed, fp);

    fail_if(parsed->log->entries->first == NULL);
    check_errors(parsed->log, known_errors, 2);

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
    comps_parse_parsed_init(parsed, "UTF-8", 0);
    fp = fopen("fedora_comps.xml", "r");
    comps_parse_file(parsed, fp);
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
    comps2xml_f(parsed->comps_doc, "fed2.xml", 0);
    //printf("%s\n", tmp);

    //free(tmp);
    comps_parse_parsed_destroy(parsed);
}
END_TEST

Suite* basic_suite (void)
{
    Suite *s = suite_create ("Basic Tests");
    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_comps_parse1);
    //tcase_add_test (tc_core, test_comps_parse2);
    //tcase_add_test (tc_core, test_comps_parse3);
    //tcase_add_test (tc_core, test_comps_parse4);
    //tcase_add_test (tc_core, test_comps_parse5);
    //tcase_add_test (tc_core, test_comps_fedora_parse);
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
