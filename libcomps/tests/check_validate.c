
#include <check.h>
#include <stdio.h>

#include "../src/comps_doc.h"
#include "../src/comps_parse.h"
#include "../src/comps_validate.h"

START_TEST(test_docgroup_validate) {
    COMPS_DocGroup *group1;
    COMPS_DocGroupPackage *pkg;
    COMPS_ValGenResult *valresult;

    group1 = (COMPS_DocGroup*)comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
    valresult = comps_validate_execute((COMPS_Object*)group1,
                                 COMPS_DocGroup_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValErrResult_ObjInfo);
    ck_assert(((COMPS_ValErrResult*)valresult)->err_list->len == 1);
    COMPS_OBJECT_DESTROY(valresult);

    comps_docgroup_set_id(group1, "g1", 1);
    valresult = comps_validate_execute((COMPS_Object*)group1,
                                 COMPS_DocGroup_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValOkResult_ObjInfo);
    COMPS_OBJECT_DESTROY(valresult);
    pkg = (COMPS_DocGroupPackage*)comps_object_create(
                                            &COMPS_DocGroupPackage_ObjInfo,
                                            NULL);
    comps_docpackage_set_name(pkg, "pkg1", 1);
    comps_docgroup_add_package(group1, pkg);
    valresult = comps_validate_execute((COMPS_Object*)group1,
                                 COMPS_DocGroup_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValOkResult_ObjInfo);
    COMPS_OBJECT_DESTROY(valresult);

    comps_docgroup_add_package(group1, (COMPS_DocGroupPackage*)
                               COMPS_OBJECT_INCREF(pkg));
    valresult = comps_validate_execute((COMPS_Object*)group1,
                                 COMPS_DocGroup_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValErrResult_ObjInfo);
    COMPS_OBJECT_DESTROY(valresult);
    COMPS_OBJECT_DESTROY(group1);
    return;
}END_TEST

START_TEST(test_doccategory_validate) {
    COMPS_DocCategory *cat1;
    COMPS_DocGroupId *gid;
    COMPS_ValGenResult *valresult;

    cat1 = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo, NULL);
    valresult = comps_validate_execute((COMPS_Object*)cat1,
                                 COMPS_DocCategory_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValErrResult_ObjInfo);
    ck_assert(((COMPS_ValErrResult*)valresult)->err_list->len == 1);
    COMPS_OBJECT_DESTROY(valresult);

    comps_doccategory_set_id(cat1, "c1", 1);
    valresult = comps_validate_execute((COMPS_Object*)cat1,
                                 COMPS_DocCategory_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValOkResult_ObjInfo);
    COMPS_OBJECT_DESTROY(valresult);
    gid = (COMPS_DocGroupId*)comps_object_create(
                                            &COMPS_DocGroupId_ObjInfo,
                                            NULL);
    comps_docgroupid_set_name(gid, "groupid1", 1);
    comps_doccategory_add_groupid(cat1, gid);
    valresult = comps_validate_execute((COMPS_Object*)cat1,
                                 COMPS_DocCategory_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValOkResult_ObjInfo);
    COMPS_OBJECT_DESTROY(valresult);

    comps_doccategory_add_groupid(cat1, (COMPS_DocGroupId*)
                                        COMPS_OBJECT_INCREF(gid));
    valresult = comps_validate_execute((COMPS_Object*)cat1,
                                 COMPS_DocCategory_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValErrResult_ObjInfo);
    COMPS_OBJECT_DESTROY(valresult);
    COMPS_OBJECT_DESTROY(cat1);
}END_TEST

START_TEST(test_docenv_validate) {
    COMPS_DocEnv *env1;
    COMPS_DocGroupId *gid;
    COMPS_ValGenResult *valresult;

    env1 = (COMPS_DocEnv*)comps_object_create(&COMPS_DocEnv_ObjInfo, NULL);
    valresult = comps_validate_execute((COMPS_Object*)env1,
                                 COMPS_DocEnv_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValErrResult_ObjInfo);
    ck_assert(((COMPS_ValErrResult*)valresult)->err_list->len == 1);
    COMPS_OBJECT_DESTROY(valresult);

    comps_docenv_set_id(env1, "c1", 1);
    valresult = comps_validate_execute((COMPS_Object*)env1,
                                 COMPS_DocEnv_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValOkResult_ObjInfo);
    COMPS_OBJECT_DESTROY(valresult);
    gid = (COMPS_DocGroupId*)comps_object_create(
                                            &COMPS_DocGroupId_ObjInfo,
                                            NULL);
    comps_docgroupid_set_name(gid, "groupid1", 1);
    comps_docenv_add_groupid(env1, gid);
    valresult = comps_validate_execute((COMPS_Object*)env1,
                                 COMPS_DocEnv_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValOkResult_ObjInfo);
    COMPS_OBJECT_DESTROY(valresult);

    comps_docenv_add_groupid(env1,(COMPS_DocGroupId*)COMPS_OBJECT_INCREF(gid));
    valresult = comps_validate_execute((COMPS_Object*)env1,
                                 COMPS_DocEnv_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValErrResult_ObjInfo);
    ck_assert(((COMPS_ValErrResult*)valresult)->err_list->len == 1);
    COMPS_OBJECT_DESTROY(valresult);

    comps_docenv_add_optionid(env1,(COMPS_DocGroupId*)COMPS_OBJECT_INCREF(gid));
    comps_docenv_add_optionid(env1,(COMPS_DocGroupId*)COMPS_OBJECT_INCREF(gid));
    valresult = comps_validate_execute((COMPS_Object*)env1,
                                 COMPS_DocEnv_ValidateRules);
    ck_assert(valresult->obj_info == &COMPS_ValErrResult_ObjInfo);
    ck_assert(((COMPS_ValErrResult*)valresult)->err_list->len == 2);
    
    COMPS_OBJECT_DESTROY(valresult);
    COMPS_OBJECT_DESTROY(env1);
}END_TEST

START_TEST(test_doc_validate) {
    COMPS_Doc *doc;
    COMPS_Parsed *parsed;
    COMPS_ValGenResult *result;
    FILE *fp;

    fp = fopen("f21-rawhide-comps.xml", "r");
    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) != 1);
    comps_parse_file(parsed, fp, NULL);
    doc = parsed->comps_doc;
    result = comps_validate_execute((COMPS_Object*)doc, COMPS_Doc_ValidateRules);
    comps_valgenres_print(result, stdout);
    comps_parse_parsed_destroy(parsed);
    COMPS_OBJECT_DESTROY(result);

}END_TEST

Suite* basic_suite (void)
{
    Suite *s = suite_create ("Basic Tests");
    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_docgroup_validate);
    tcase_add_test (tc_core, test_doccategory_validate);
    tcase_add_test (tc_core, test_docenv_validate);
    tcase_add_test (tc_core, test_doc_validate);
    suite_add_tcase (s, tc_core);
    return s;
}

int main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;

    int number_failed;
    Suite *s = basic_suite ();
    SRunner *sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
