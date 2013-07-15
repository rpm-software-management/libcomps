#include <check.h>
#include <stdio.h>

#include "../src/comps.h"
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
    COMPS_ListItem *it;
    char *tmp_ch;
    COMPS_Prop *tmp_prop;

    const char* groups_ids[] = {"additional-devel", "backup-client", "backup-server"};
    const char* groups_names[] = {"Additional Development", "Backup Client", "Backup Server"};
    const int group_packages[][3] = {{40, 62, 0}, {0,1,1}, {0,2,2}};

    const char* cats_ids[] = {"cat 1", "cat 2"};
    const char* cats_names[] = {"category 1", "category 2"};
    const int cats_gids[] = {1, 2};
    int ret;

    COMPS_List * tmplist;

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample-comps.xml", "r");
    comps_parse_file(parsed, fp);
    fail_unless(comps_parse_validate_dtd("sample-comps.xml", "comps.dtd"));

    if (parsed->log->logger_data->len != 0) {
        err_log = comps_log_str(parsed->log);
        fail_if(parsed->log->logger_data->len != 0,
                "Some errors have been found (and shouldn't have) during parsing:"
                "\n%s\n", err_log);
        free(err_log);
    }
    fail_if(comps_doc_groups(parsed->comps_doc)->len != 3, "Should have 3 groups parsed."
            "Have %d", comps_doc_groups(parsed->comps_doc)->len);
    fail_if(comps_doc_categories(parsed->comps_doc)->len != 2, "Should have 2 categories"
            "parsed. Have %d", comps_doc_categories(parsed->comps_doc)->len);
    fail_if(comps_doc_environments(parsed->comps_doc)->len != 1,
            "Should have 1 environment parsed. Have %d",
            comps_doc_environments(parsed->comps_doc)->len);

    for (int i=0; i<3; i++) {
        it = comps_list_at(comps_doc_groups(parsed->comps_doc), i);
        fail_if(it == NULL, "Group not found");
        tmp_prop = comps_dict_get(((COMPS_DocGroup*)it->data)->properties, "name");
        tmp_ch = (tmp_prop)?tmp_prop->prop.str:NULL;

        COMPS_HSList *keys;
        COMPS_HSListItem *hsit;

        /*keys = comps_rtree_pairs(((COMPS_DocGroup*)it->data)->properties);
        for (hsit = keys->first; hsit != NULL; hsit = hsit->next) {
            printf("prop:%s", ((COMPS_RTreePair*)hsit->data)->key);
            if (((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop_type
                == COMPS_PROP_STR)
                printf(" %s\n", ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.str);
            else {
                printf(" %d\n", ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.num);
            }
        }
        comps_hslist_destroy(&keys);*/

        //printf("group name:%s\n", tmp_ch);

        tmp_prop = comps_dict_get(((COMPS_DocGroup*)it->data)->properties, "id");
        tmp_ch = (tmp_prop)?tmp_prop->prop.str:NULL;
        fail_if(tmp_ch == NULL, "id null");
        //printf("group id:%s\n", tmp_ch);
        fail_if(strcmp(tmp_ch, groups_ids[i])!=0,
                "Wrong #%d group id(%s). Should be %s", i,
                tmp_ch, groups_ids[i]);

        tmp_prop = comps_dict_get(((COMPS_DocGroup*)it->data)->properties, "name");
        tmp_ch = (tmp_prop)?tmp_prop->prop.str:NULL;
        fail_if(tmp_ch == NULL, "name null");
        fail_if(strcmp(tmp_ch, groups_names[i])!=0,
                "Wrong group #%d name(%s). Should be %s", i,
                tmp_ch, groups_names[i]);
        tmplist = comps_docgroup_get_packages((COMPS_DocGroup*)it->data, NULL,
                                             COMPS_PACKAGE_DEFAULT);
        fail_if(group_packages[i][0] != tmplist->len, "Group #%d should have"
                " %d default packages, Have %d", i, group_packages[i][0],
                tmplist->len);
        comps_list_destroy(&tmplist);
        tmplist = comps_docgroup_get_packages((COMPS_DocGroup*)it->data, NULL,
                                             COMPS_PACKAGE_OPTIONAL);
        fail_if(group_packages[i][1] != tmplist->len, "Group #%d should have"
                " %d optional packages, Have %d", i, group_packages[i][1],
                tmplist->len);
        comps_list_destroy(&tmplist);
    }

    for (int i=0; i<2; i++) {
        it = comps_list_at(comps_doc_categories(parsed->comps_doc), i);
        tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "id");
        tmp_ch = (tmp_prop)?tmp_prop->prop.str:NULL;
        fail_if(strcmp(tmp_ch, cats_ids[i])!=0,
                "Wrong #%d category id(%s). Should be %s", i,
                tmp_ch, cats_ids[i]);
        tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "name");
        tmp_ch = (tmp_prop)?tmp_prop->prop.str:NULL;
        fail_if(strcmp(tmp_ch, cats_names[i])!=0,
                "Wrong category #%d name(%s). Should be %s", i,
                tmp_ch, cats_names[i]);
        fail_if(((COMPS_DocCategory*)it->data)->group_ids->len != cats_gids[i],
                "Category #%d should have %d groupids, have %d", i,
                cats_gids[i], ((COMPS_DocCategory*)it->data)->group_ids->len);
    }

    fp = fopen("sample-bad-elem.xml", "r");

    comps_parse_file(parsed, fp);
    ret = comps_parse_validate_dtd("sample-bad-elem.xml", "comps.dtd");
    fail_if(ret >0, "XML shouldn't be valid. Validation returned: %d", ret);

    if (parsed->log->logger_data->len != 0) {
        err_log = comps_log_str(parsed->log);
        fail_if(parsed->log->logger_data->len == 0,
                "No errors have found during parsing (and should have)");
        free(err_log);
    }
    comps_parse_parsed_destroy(parsed);
}
END_TEST

int check_errors(COMPS_Logger *log, COMPS_LoggerEntry ** known_errors,
                  int known_len) {
    COMPS_ListItem *it;
    int i;

    it = log->logger_data->first;
    for (i = 0; it != NULL && i != known_len; it = it->next, i++) {
        fail_if(strcmp(((COMPS_LoggerEntry*)it->data)->log_message,
                             known_errors[i]->log_message) != 0,
                "%d err opt_message doesn't match (%s != %s)",
            i, ((COMPS_LoggerEntry*)it->data)->log_message,
            known_errors[i]->log_message);
        fail_if(((COMPS_LoggerEntry*)it->data)->code !=
                    known_errors[i]->code, "%d. err code different\n (%d != %d)",
                    i, ((COMPS_LoggerEntry*)it->data)->code,
                    known_errors[i]->code);
        fail_if(((COMPS_LoggerEntry*)it->data)->opt_code1 !=
                    known_errors[i]->opt_code1,
                    "%d\n err optcode1 different (%d != %d)", i,
                    ((COMPS_LoggerEntry*)it->data)->opt_code1,
                    known_errors[i]->opt_code1);
        fail_if(((COMPS_LoggerEntry*)it->data)->opt_code2 !=
                    known_errors[i]->opt_code2,
                    "%d\n err optcode2 different (%d != %d)", i,
                    ((COMPS_LoggerEntry*)it->data)->opt_code2,
                    known_errors[i]->opt_code2);
        fail_if(((COMPS_LoggerEntry*)it->data)->opt_code3 !=
                    known_errors[i]->opt_code3,
                    "%d\n err optcode3 different (%d != %d)", i,
                    ((COMPS_LoggerEntry*)it->data)->opt_code3,
                    known_errors[i]->opt_code3);
    }
    return i;
}

START_TEST(test_comps_parse2)
{
    FILE *fp;
    char *err_log;
    COMPS_Parsed *parsed;
    COMPS_ListItem *it;
    int ret, i;

    COMPS_List * tmplist;
    COMPS_LoggerEntry* known_errors[8];
    known_errors[0] = comps_log_entry_create("description", 0,
                                             COMPS_ERR_NOCONTENT, 265, 18, 0);
    known_errors[1] = comps_log_entry_create("packagelist", 0,
                                             COMPS_ERR_LIST_EMPTY, 270, 4, 0);
    known_errors[2] = comps_log_entry_create("packagelist", 0,
                                             COMPS_ERR_LIST_EMPTY, 320, 4, 0);
    known_errors[3] = comps_log_entry_create("description", 0,
                                             COMPS_ERR_NOCONTENT, 379, 18, 0);
    known_errors[4] = comps_log_entry_create("packagelist", 0,
                                             COMPS_ERR_LIST_EMPTY, 384, 4, 0);
    known_errors[5] = comps_log_entry_create("description", 0,
                                             COMPS_ERR_NOCONTENT, 440, 18, 0);
    known_errors[6] = comps_log_entry_create("packagelist", 0,
                                             COMPS_ERR_LIST_EMPTY, 445, 4, 0);
    known_errors[7] = comps_log_entry_create("optionlist", 0,
                                             COMPS_ERR_ELEM_REQUIRED, 1201, 2, 0);

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample_comps.xml", "r");
    comps_parse_file(parsed, fp);

    fail_if(parsed->log->logger_data->len == 0);
    i = check_errors(parsed->log, known_errors, 8);

    fail_if(i != 8);
    comps_parse_parsed_destroy(parsed);
    for (i = 0; i < 8; i++) {
        comps_log_entry_destroy(known_errors[i]);
    }
}
END_TEST

START_TEST(test_comps_parse3)
{
    FILE *fp;
    char *err_log, *tmp_ch;
    COMPS_Parsed *parsed;
    COMPS_ListItem *it;
    int ret, i;
    COMPS_List *pairlist, *tmplist;
    COMPS_LoggerEntry* known_errors[3];
    COMPS_Prop *tmp_prop;
    COMPS_HSListItem *hsit;

    known_errors[0] = comps_log_entry_create("id", 0,
                                             COMPS_ERR_ELEM_REQUIRED, 188, 2, 0);
    known_errors[1] = comps_log_entry_create("name", 0,
                                             COMPS_ERR_ELEM_REQUIRED, 188, 2, 0);
    known_errors[2] = comps_log_entry_create("description", 0,
                                             COMPS_ERR_ELEM_REQUIRED, 188, 2, 0);

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample_comps_bad1.xml", "r");
    comps_parse_file(parsed, fp);

    fail_if(parsed->log->logger_data->len == 0);
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

    tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "id");
    tmp_ch = (tmp_prop)?tmp_prop->prop.str:NULL;
    fail_if(tmp_ch != NULL,
            "first group should have NULL id");
    tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "name");
    tmp_ch = (tmp_prop)?tmp_prop->prop.str:NULL;
    fail_if(tmp_ch != NULL,
            "first group should have NULL id");
    tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "desc");
    tmp_ch = (tmp_prop)?tmp_prop->prop.str:NULL;
    fail_if(tmp_ch != NULL,
            "first group should have NULL id");
    comps_parse_parsed_destroy(parsed);
}
END_TEST

START_TEST(test_comps_parse4)
{
    FILE *fp;
    char *err_log;
    COMPS_Parsed *parsed;
    COMPS_ListItem *it;
    int ret, i;
    COMPS_List * tmplist;
    COMPS_LoggerEntry* known_errors[15];

    known_errors[0] = comps_log_entry_create("id", 0,
                                             COMPS_ERR_NOPARENT, 4, 2, 0);
    known_errors[1] = comps_log_entry_create("packagereq", 0,
                                             COMPS_ERR_NOPARENT, 158, 4, 0);
    known_errors[2] = comps_log_entry_create("langonly", 0,
                                             COMPS_ERR_ELEM_ALREADYSET, 274, 16, 0);
    known_errors[3] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 588, 4, 0);
    known_errors[4] = comps_log_entry_create("grouplist", 0,
                                             COMPS_ERR_NOPARENT, 880, 2, 0);
    known_errors[5] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 881, 4, 0);
    known_errors[6] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 882, 4, 0);
    known_errors[7] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 883, 4, 0);
    known_errors[8] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 884, 4, 0);
    known_errors[9] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 885, 4, 0);
    known_errors[10] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 886, 4, 0);
    known_errors[11] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 887, 4, 0);
    known_errors[12] = comps_log_entry_create("id", 0,
                                             COMPS_ERR_NOPARENT, 1210, 2, 0);
    known_errors[13] = comps_log_entry_create("groupid", 0,
                                             COMPS_ERR_NOPARENT, 1228, 4, 0);
    known_errors[14] = comps_log_entry_create("optionlist", 0,
                                             COMPS_ERR_ELEM_ALREADYSET, 1244, 4, 0);

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample_comps_bad2.xml", "r");
    comps_parse_file(parsed, fp);

    fail_if(parsed->log->logger_data->len == 0);
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
    char *err_log;
    COMPS_Parsed *parsed;
    COMPS_ListItem *it;
    int ret, i;
    COMPS_List * tmplist;
    COMPS_LoggerEntry* known_errors[2];

    known_errors[0] = comps_log_entry_create("some stray", 0,
                                             COMPS_ERR_TEXT_BETWEEN, 6, 4, 0);
    known_errors[1] = comps_log_entry_create("    some stray", 0,
                                             COMPS_ERR_TEXT_BETWEEN, 189, 2, 0);

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("sample_comps_bad3.xml", "r");
    comps_parse_file(parsed, fp);

    fail_if(parsed->log->logger_data->len == 0);
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
    COMPS_HSList *list;
    
    //COMPS_ListItem *it;
    FILE *fp;
    char *tmp;
    //char *err_str;
    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 1);
    fp = fopen("fedora_comps.xml", "r");
    comps_parse_file(parsed, fp);
    printf("log len:%d\n", parsed->log->logger_data->len);
    fail_if(parsed->fatal_error != 0, "Some fatal errors found after parsing");
    printf("log len:%d\n", parsed->log->logger_data->len);
    //err_str = comps_log_str(parsed->log);
    //printf("parsed err log: %s", err_str);
    //free(err_str);
    printf("fedora comps groups:%d\n", comps_doc_groups(parsed->comps_doc)->len);
    //printf("fedora comps categories:%d\n", comps_doc_categories(parsed->comps_doc)->len);

    //print_all_str(((COMPS_DocGroup*)comps_doc_groups(parsed->comps_doc)
    //                                ->first->data)->name_by_lang);
    comps2xml_f(parsed->comps_doc, "fed2.xml", 1);
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
    tcase_add_test (tc_core, test_comps_parse2);
    tcase_add_test (tc_core, test_comps_parse3);
    tcase_add_test (tc_core, test_comps_parse4);
    tcase_add_test (tc_core, test_comps_parse5);
    tcase_add_test (tc_core, test_comps_fedora_parse);
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
