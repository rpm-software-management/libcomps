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
#include "../src/comps_validate.h"

#include "check_utils.h"

START_TEST(test_comps_doc_basic)
{
    printf("### start test_comps_doc_basic\n");
    const char *groups_ids[] = {"g1", "g2", "g3", "g4"};
    const char *groups_names[] = {"group1", "group2", "group3", "group4"};
    const char *group_mpackages[][4] = {{"pkg1","pkg2","pkg3","pkg4"},
                                      {"p1", "p2", "p3", "pkg4"},
                                      {"package1", "package2", "pkg3", "p4"},
                                      {"pack1", "pack2","p3","pkg4"}};
    const char *group_opackages[][4] = {{"opkg1","opkg2","opkg3","opkg4"},
                                      {"op1", "op2", "op3", "opkg4"},
                                      {"opackage1", "opackage2", "opkg3", "op4"},
                                      {"opack1", "opack2","op3","opkg4"}};
    const char *cats_ids[] = {"c1", "c2", "c3"};
    const char *cats_names[] = {"cat1", "cat2", "cat3"};
    const char *cat_gids[][3] = {{"g1","g2","g3"},
                                 {"g2", "g3", "g4"},
                                 {"g1", "g2", "g4"}};

    COMPS_Doc *doc;
    COMPS_DocGroup *g;
    COMPS_DocCategory *c;
    COMPS_DocGroupPackage *p;
    COMPS_DocGroupId *gid;
    COMPS_ObjList *tmplist;
    COMPS_Str *tmpstr = comps_str("UTF-8");

    doc = (COMPS_Doc*)comps_object_create(&COMPS_Doc_ObjInfo,
                              (COMPS_Object*[]){(COMPS_Object*)tmpstr});
    COMPS_OBJECT_DESTROY(tmpstr);
    for (int i=0; i<2; i++) {
        g = (COMPS_DocGroup*)comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
        comps_docgroup_set_id(g, (char*)groups_ids[i], 1);
        comps_docgroup_set_name(g, (char*)groups_names[i], 1);
        for (int x=0; x<4; x++) {
            p = (COMPS_DocGroupPackage*)
                comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
            comps_docpackage_set_name(p, (char*)group_mpackages[i][x],1 );
            comps_docpackage_set_type(p, COMPS_PACKAGE_MANDATORY, false);
            comps_docgroup_add_package(g, p);
            p = (COMPS_DocGroupPackage*)
                comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
            comps_docpackage_set_name(p, (char*)group_opackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_OPTIONAL, false);
            comps_docgroup_add_package(g, p);
        }
        comps_doc_add_group(doc, g);
    }
    for (int i=0; i<3; i++) {
        c = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo,
                                                    NULL);
        comps_doccategory_set_id(c, (char*)cats_ids[i], 1);
        comps_doccategory_set_name(c, (char*)cats_names[i], 1);
        for (int x=0; x<3; x++) {
            gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
            comps_docgroupid_set_name(gid, (char*)cat_gids[i][x], 1);
            comps_doccategory_add_groupid(c, gid);
        }
        comps_doc_add_category(doc, c);
    }
    tmplist = comps_doc_groups(doc);
    fail_if(tmplist->len == 0, "No groups found");
    g = (COMPS_DocGroup*)tmplist->first->comps_obj;
    COMPS_OBJECT_DESTROY(tmplist);

    tmplist = comps_doc_get_groups(doc, "g1", NULL, NULL, NULL, 0);
    fail_if(tmplist->len == 0, "Group with id 'g1' should be in groups, but"
                               "cant get it");
    g = (COMPS_DocGroup*)tmplist->first->comps_obj;
    COMPS_OBJECT_DESTROY(tmplist);

    tmplist = comps_docgroup_get_packages(g, NULL, COMPS_PACKAGE_OPTIONAL);
    fail_if(tmplist->len != 4, "Group with id 'g1' should have 4 optional "
                               "packages. But have %d.", tmplist->len);
    COMPS_OBJECT_DESTROY(tmplist);
    tmplist = comps_docgroup_get_packages(g, NULL, COMPS_PACKAGE_MANDATORY);
    fail_if(tmplist->len != 4, "Group with id 'g1' should have 4 optional "
                               "packages. But have %d.", tmplist->len);
    COMPS_OBJECT_DESTROY(tmplist);
    COMPS_OBJECT_DESTROY(doc);
}END_TEST

START_TEST(test_comps_doc_setfeats)
{
    printf("### start test_comps_doc_setfeats\n");

    const char *groups_ids[] = {"g1", "g2", "g3", "g4"};
    const char *groups_names[] = {"group1", "group2", "group3", "group4"};
    const char *group_mpackages[][4] = {{"pkg1","pkg2","pkg3","pkg4"},
                                      {"p1", "p2", "p3", "pkg4"},
                                      {"package1", "package2", "pkg3", "p4"},
                                      {"pack1", "pack2","p3","pkg4"}};
    const char *group_opackages[][4] = {{"opkg1","opkg2","opkg3","opkg4"},
                                      {"op1", "op2", "op3", "opkg4"},
                                      {"opackage1", "opackage2", "opkg3", "op4"},
                                      {"opack1", "opack2","op3","opkg4"}};
    const char *cats_ids[] = {"c1", "c2", "c3"};
    const char *cats_names[] = {"cat1", "cat2", "cat3"};
    const char *cat_gids[][3] = {{"g1","g2","g3"},
                                 {"g2", "g3", "g4"},
                                 {"g1", "g2", "g4"}};
    const char *envs_ids[] = {"env1", "env2", "env3"};
    const char *envs_names[] = {"environment 1", "environment 2", "environment3"};
    const char *env_gids[][3] = {{"g1","g2","g3"},
                                 {"g2", "g3", "g4"},
                                 {"g1", "g2", "g4"}};
    const char *env_opts[][4] = {{"opt1","opt2","opt3","opt4"},
                                 {"o1", "o2", "opt3", "o4"},
                                 {"op1", "o2", "opt3", "op4"},
                                 {"opt1", "2", "3", "op4"}};

    const char *groups_ids2[] = {"g1", "g22", "g32", "g4"};
    const char *groups_names2[] = {"group1", "group22", "group32", "group4"};
    const char *group_mpackages2[][4] = {{"pkg21","pkg2","pkg23","pkg4"},
                                      {"p1", "p2", "p3", "pkg4"},
                                      {"package1", "package2", "pkg3", "p4"},
                                      {"pack1", "pack22","p3","pkg4"}};
    const char *group_opackages2[][4] = {{"opkg1","opkg2","opkg3","opkg4"},
                                      {"op1", "op2", "op3", "opkg4"},
                                      {"opackage1", "opackage2", "opkg3", "op4"},
                                      {"opack1", "opack2","op3","opkg4"}};
    const char *cats_ids2[] = {"c1", "c2", "c32"};
    const char *cats_names2[] = {"cat1", "cat2", "cat32"};
    const char *cat_gids2[][3] = {{"g1","g2","g32"},
                                 {"g22", "g3", "g4"},
                                 {"g21", "g2", "g4"}};
    const char *envs_ids2[] = {"env21", "env2", "env3"};
    const char *envs_names2[] = {"environment 21", "environment 2", "environment3"};
    const char *env_gids2[][3] = {{"g21","g2","g3"},
                                 {"g2", "g23", "g4"},
                                 {"g1", "g2", "g24"}};
    const char *env_opts2[][4] = {{"opt21","opt2","opt3","opt4"},
                                 {"o1", "o22", "opt3", "o4"},
                                 {"op1", "o2", "opt23", "op4"},
                                 {"opt1", "2", "3", "op24"}};

    COMPS_Doc *doc, *doc2, *tmpdoc;
    COMPS_DocCategory *c, *c1, *c2;
    COMPS_DocEnv *e, *e1, *e2;
    COMPS_DocGroup *g, *g1, *g2;
    COMPS_DocGroupPackage *p;
    COMPS_DocGroupId *gid;
    COMPS_Str *str;
    COMPS_ObjList *list;

    str = comps_str("UTF-8");
    doc = (COMPS_Doc*)comps_object_create(&COMPS_Doc_ObjInfo,
                                (COMPS_Object*[]){(COMPS_Object*)str});
    COMPS_OBJECT_DESTROY(str);
    for (int i=0; i<4; i++) {
        g = (COMPS_DocGroup*)comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
        comps_docgroup_set_id(g, (char*)groups_ids[i], 1);
        comps_docgroup_set_name(g, (char*)groups_names[i], 1);
        for (int x=0; x<4; x++) {
            p = (COMPS_DocGroupPackage*)
                comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
            comps_docpackage_set_name(p, (char*)group_mpackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_MANDATORY, false);
            comps_docgroup_add_package(g, p);
            p = (COMPS_DocGroupPackage*)
                comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
            comps_docpackage_set_name(p, (char*)group_opackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_OPTIONAL, false);
            comps_docgroup_add_package(g, p);
        }
        comps_doc_add_group(doc, g);
    }
    for (int i=0; i<3; i++) {
        e = (COMPS_DocEnv*)comps_object_create(&COMPS_DocEnv_ObjInfo, NULL);
        comps_docenv_set_id(e, (char*)envs_ids[i], 1);
        comps_docenv_set_name(e, (char*)envs_names[i], 1);
        for (int x=0; x<3; x++) {
            gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
            comps_docgroupid_set_name(gid, (char*)env_gids[i][x], 1);
            comps_docenv_add_groupid(e, gid);
        }
        for (int x=0; x<4; x++) {
            gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
            comps_docgroupid_set_name(gid, (char*)env_opts[i][x], 1);
            comps_docenv_add_optionid(e, gid);
        }
        comps_doc_add_environment(doc, e);
    }
    for (int i=0; i<3; i++) {
        c = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo, NULL);
        comps_doccategory_set_id(c, (char*)cats_ids[i], 1);
        comps_doccategory_set_name(c, (char*)cats_names[i], 1);
        for (int x=0; x<3; x++) {
            gid = (COMPS_DocGroupId*)comps_object_create(
                                               &COMPS_DocGroupId_ObjInfo, NULL);
            comps_docgroupid_set_name(gid, (char*)cat_gids[i][x], 1);
            comps_doccategory_add_groupid(c, gid);
        }
        comps_doc_add_category(doc, c);
    }

    str = comps_str("UTF-8");
    doc2 = (COMPS_Doc*)comps_object_create(&COMPS_Doc_ObjInfo,
                                (COMPS_Object*[]){(COMPS_Object*)str});
    COMPS_OBJECT_DESTROY(str);
    for (int i=0; i<4; i++) {
        g = (COMPS_DocGroup*)comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
        comps_docgroup_set_id(g, (char*)groups_ids2[i], 1);
        comps_docgroup_set_name(g, (char*)groups_names2[i], 1);
        for (int x=0; x<4; x++) {
            p = (COMPS_DocGroupPackage*)
                comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
            comps_docpackage_set_name(p, (char*)group_mpackages2[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_MANDATORY, false);
            comps_docgroup_add_package(g, p);
            p = (COMPS_DocGroupPackage*)
                comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
            comps_docpackage_set_name(p, (char*)group_opackages2[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_OPTIONAL, false);
            comps_docgroup_add_package(g, p);
        }
        comps_doc_add_group(doc2, g);
    }
    for (int i=0; i<3; i++) {
        e = (COMPS_DocEnv*)comps_object_create(&COMPS_DocEnv_ObjInfo, NULL);
        comps_docenv_set_id(e, (char*)envs_ids2[i], 1);
        comps_docenv_set_name(e, (char*)envs_names2[i], 1);
        for (int x=0; x<3; x++) {
            gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
            comps_docgroupid_set_name(gid, (char*)env_gids2[i][x], 1);
            comps_docenv_add_groupid(e, gid);
        }
        for (int x=0; x<4; x++) {
            gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
            comps_docgroupid_set_name(gid, (char*)env_opts2[i][x], 1);
            comps_docenv_add_optionid(e, gid);
        }
        comps_doc_add_environment(doc2, e);
    }
    for (int i=0; i<3; i++) {
        c = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo, NULL);
        comps_doccategory_set_id(c, (char*)cats_ids2[i], 1);
        comps_doccategory_set_name(c, (char*)cats_names2[i], 1);
        for (int x=0; x<3; x++) {
            gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
            comps_docgroupid_set_name(gid, (char*)cat_gids2[i][x], 1);
            comps_doccategory_add_groupid(c, gid);
        }
        comps_doc_add_category(doc2, c);
    }

    list = comps_doc_groups(doc);
    g1 = (COMPS_DocGroup*)list->first->comps_obj;
    g2 = (COMPS_DocGroup*)list->first->next->comps_obj;
    g = comps_docgroup_union(g1, g2);
    fail_if(g->packages->len != 14, "Union of (g1 v g2) should have 14 packages"
            " have %d", g->packages->len);
    COMPS_OBJECT_DESTROY(g);
    g = comps_docgroup_intersect(g1, g2);
    fail_if(g->packages->len != 2, "Intersect of (g1 ^ g2) should have 2"
            "packages, have %d", g->packages->len);
    COMPS_OBJECT_DESTROY(g);
    g1 = (COMPS_DocGroup*)list->first->next->comps_obj;
    g2 = (COMPS_DocGroup*)list->first->next->next->comps_obj;
    g = comps_docgroup_union(g1, g2);
    fail_if(g->packages->len != 16, "Union of (g2 v g3) should have 16 packages"
            " have %d", g->packages->len);
    COMPS_OBJECT_DESTROY(g);
    g = comps_docgroup_intersect(g1, g2);
    fail_if(g->packages->len != 0, "Intersect of (g2 ^ g3) should have 0"
            "packages, have %d", g->packages->len);
    COMPS_OBJECT_DESTROY(g);
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_categories(doc);
    c1 = (COMPS_DocCategory*)list->first->comps_obj;
    c2 = (COMPS_DocCategory*)list->first->next->comps_obj;

    c = comps_doccategory_union(c1, c2);
    //print_category(c);
    fail_if(c->group_ids->len != 4, "Union of (c1 v c2) should have 4 "
            "group ids, have %d", c->group_ids->len);
    COMPS_OBJECT_DESTROY(c);

    c = comps_doccategory_intersect(c1, c2);
    fail_if(c->group_ids->len != 2, "Intersect of (c1 ^ c2) should have 2"
            "group ids, have %d", c->group_ids->len);
    COMPS_OBJECT_DESTROY(c);

    c1 = (COMPS_DocCategory*)list->first->next->comps_obj;
    c2 = (COMPS_DocCategory*)list->first->next->next->comps_obj;
    c = comps_doccategory_union(c1, c2);
    fail_if(c->group_ids->len != 4, "Union of (c2 v c3) should have 4 "
            "group ids, have %d", c->group_ids->len);
    COMPS_OBJECT_DESTROY(c);
    c = comps_doccategory_intersect(c1, c2);
    fail_if(c->group_ids->len != 2, "Intersect of (c2 ^ c3) should have 2"
            "group ids, have %d", c->group_ids->len);
    COMPS_OBJECT_DESTROY(c);
    COMPS_OBJECT_DESTROY(list);

    list = comps_doc_environments(doc);
    e1 = (COMPS_DocEnv*)list->first->comps_obj;
    e2 = (COMPS_DocEnv*)list->first->next->comps_obj;
    e = comps_docenv_union(e1, e2);
    fail_if(e->group_list->len != 4, "Union of (e1 v e2) should have 4 "
            "groud ids have %d", e->group_list->len);
    fail_if(e->option_list->len != 7, "Union of (e1 v e2) should have 7 "
            "option ids have %d", e->option_list->len);
    COMPS_OBJECT_DESTROY(e);
    e = comps_docenv_intersect(e1, e2);
    fail_if(e->group_list->len != 2, "Intersect of (e1 ^ e2) should have 2"
            "group ids have %d", e->group_list->len);
    fail_if(e->option_list->len != 1, "Union of (e1 v e2) should have 1 "
            "option ids have %d", e->option_list->len);
    COMPS_OBJECT_DESTROY(e);

    e1 = (COMPS_DocEnv*)list->first->next->comps_obj;
    e2 = (COMPS_DocEnv*)list->first->next->next->comps_obj;
    e = comps_docenv_union(e1, e2);
    fail_if(e->group_list->len != 4, "Union of (e2 v e3) should have 4 "
            "groud ids have %d", e->group_list->len);
    fail_if(e->option_list->len != 6, "Union of (e2 v e3) should have 7 "
            "option ids have %d", e->option_list->len);
    COMPS_OBJECT_DESTROY(e);
    e = comps_docenv_intersect(e1, e2);
    fail_if(e->group_list->len != 2, "Intersect of (e2 ^ e3) should have 2 "
            "groupids  have %d", e->group_list->len);
    fail_if(e->option_list->len != 2, "Intersect of (e2 v e3) should have 2 "
            "option ids have %d", e->option_list->len);
    COMPS_OBJECT_DESTROY(e);
    COMPS_OBJECT_DESTROY(list);

    tmpdoc = comps_doc_union(doc, doc2);
    list = comps_doc_groups(tmpdoc);
    fail_if(list->len != 6, "Union of (doc ^ doc2) should have 6 "
            "groups  have %d", list->len);
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_categories(tmpdoc);
    fail_if(list->len != 4, "Union of (doc ^ doc2) "
            "should have 4 categories  have %d", list->len);
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_environments(tmpdoc);
    fail_if(list->len != 4, "Union of (doc ^ doc2) "
            "should have 4 environments  have %d", list->len);
    COMPS_OBJECT_DESTROY(list);
    COMPS_OBJECT_DESTROY(tmpdoc);
    tmpdoc = comps_doc_intersect(doc, doc2);
    list = comps_doc_groups(tmpdoc);
    fail_if(list->len != 2, "Intersect of (doc ^ doc2) "
            "should have 2 groups  have %d", list->len);
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_categories(tmpdoc);
    fail_if(list->len != 2, "Intersect of (doc ^ doc2)"
            " should have 2 categories  have %d", list->len);
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_environments(tmpdoc);
    fail_if(list->len != 2, "Intersect of "
            "(doc ^ doc2) should have 2 environments  have %d", list->len);
    COMPS_OBJECT_DESTROY(list);
    COMPS_OBJECT_DESTROY(tmpdoc);
    COMPS_OBJECT_DESTROY(doc2);
    COMPS_OBJECT_DESTROY(doc);
}
END_TEST

START_TEST(test_comps_doc_xml)
{
    printf("### start test_comps_doc_xml\n");
    const char *groups_ids[] = {"g1", "g2", "g3", "g4"};
    const char *groups_names[] = {"group1", "group2", "group3", "group4"};
    const char *group_mpackages[][4] = {{"pkg1","pkg2","pkg3","pkg4"},
                                      {"p1", "p2", "p3", "pkg4"},
                                      {"package1", "package2", "pkg3", "p4"},
                                      {"pack1", "pack2","p3","pkg4"}};
    const char *group_opackages[][4] = {{"opkg1","opkg2","opkg3","opkg4"},
                                      {"op1", "op2", "op3", "opkg4"},
                                      {"opackage1", "opackage2", "opkg3", "op4"},
                                      {"opack1", "opack2","op3","opkg4"}};
    const char *cats_ids[] = {"c1", "c2", "c3"};
    const char *cats_names[] = {"cat1", "cat2", "cat3"};
    const char *cat_gids[][3] = {{"g1","g2","g3"},
                                 {"g2", "g3", "g4"},
                                 {"g1", "g2", "g4"}};

    COMPS_Doc *doc;
    COMPS_DocGroup *g;
    COMPS_DocCategory *c;
    COMPS_DocGroupPackage *p;
    COMPS_DocGroupId *gid;
    //COMPS_List *tmplist;
    COMPS_Str *str;
    str = comps_str("UTF-8");
    doc = (COMPS_Doc*)comps_object_create(&COMPS_Doc_ObjInfo,
                                          (COMPS_Object*[]){(COMPS_Object*)str});
    COMPS_OBJECT_DESTROY(str);
    for (int i=0; i<2; i++) {
        g = (COMPS_DocGroup*)comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
        comps_docgroup_set_id(g, (char*)groups_ids[i], 1);
        comps_docgroup_set_name(g, (char*)groups_names[i], 1);
        for (int x=0; x<4; x++) {
            p = (COMPS_DocGroupPackage*)
                comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
            comps_docpackage_set_name(p, (char*)group_mpackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_MANDATORY, false);
            comps_docgroup_add_package(g, p);
            p = (COMPS_DocGroupPackage*)
                comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
            comps_docpackage_set_name(p, (char*)group_opackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_OPTIONAL, false);
            comps_docgroup_add_package(g, p);
        }
        comps_doc_add_group(doc, g);
    }

    for (int i=0; i<3; i++) {
        c = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo,
                                                    NULL);
        comps_doccategory_set_id(c, (char*)cats_ids[i], 1);
        comps_doccategory_set_name(c, (char*)cats_names[i], 1);
        for (int x=0; x<3; x++) {
            gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
            comps_docgroupid_set_name(gid, (char*)cat_gids[i][x], 1);
            comps_doccategory_add_groupid(c, gid);
        }
        comps_doc_add_category(doc, c);
    }
    comps2xml_f(doc, "testfile.xml", 1, NULL, NULL);
    COMPS_OBJECT_DESTROY(doc);
}
END_TEST

START_TEST(test_comps_doc_union)
{
    printf("### start test_comps_doc_union\n");

    COMPS_DocGroup *g1, *g2, *g3;
    COMPS_DocCategory *c1, *c2, *c3;
    COMPS_DocGroupPackage *p;
    COMPS_DocGroupId *gid;

    g1 = (COMPS_DocGroup*)comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
    comps_docgroup_set_id(g1, "g1", 1);
    comps_docgroup_set_name(g1, "group1", 1);
    
    p = (COMPS_DocGroupPackage*)
        comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
    comps_docpackage_set_name(p, "package1", 1);
    
    comps_docgroup_add_package(g1, p);
    p = (COMPS_DocGroupPackage*)
        comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
    comps_docpackage_set_name(p, "package2", 1);
    comps_docgroup_add_package(g1, p);

    g2 = (COMPS_DocGroup*)comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
    comps_docgroup_set_id(g2, "g2", 1);
    comps_docgroup_set_name(g2, "group2", 1);
    p = (COMPS_DocGroupPackage*)
        comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
    comps_docpackage_set_name(p, "package3", 1);
    comps_docgroup_add_package(g2, p);
    p = (COMPS_DocGroupPackage*)
        comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
    comps_docpackage_set_name(p, "package4", 1);
    comps_docgroup_add_package(g2, p);

    g3 = comps_docgroup_union(g1, g2);
    COMPS_OBJECT_DESTROY(g1);
    COMPS_OBJECT_DESTROY(g2);
    COMPS_OBJECT_DESTROY(g3);


    c1 = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo, NULL);
    comps_doccategory_set_id(c1, "c1", 1);
    comps_doccategory_set_name(c1, "category 1", 1);
    comps_doccategory_set_desc(c1, "cat desc 1", 1);
    gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
    comps_docgroupid_set_name(gid, "g1", 1);
    comps_doccategory_add_groupid(c1, gid);
    gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
    comps_docgroupid_set_name(gid, "g2", 1);
    comps_doccategory_add_groupid(c1, gid);

    c2 = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo, NULL);
    comps_doccategory_set_id(c2, "c1", 1);
    comps_doccategory_set_name(c2, "category 1", 1);
    comps_doccategory_set_desc(c2, "cat desc 1", 1);
    gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
    comps_docgroupid_set_name(gid, "g1", 1);
    comps_doccategory_add_groupid(c2, gid);
    gid = (COMPS_DocGroupId*)comps_object_create(&COMPS_DocGroupId_ObjInfo, NULL);
    comps_docgroupid_set_name(gid, "g2", 1);
    comps_doccategory_add_groupid(c2, gid);

    c3 = comps_doccategory_union(c1, c2);
    COMPS_OBJECT_DESTROY(c1);
    COMPS_OBJECT_DESTROY(c2);
    COMPS_OBJECT_DESTROY(c3);

    COMPS_Parsed *parsed;
    FILE *fp;
    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    fp = fopen("sample_comps.xml", "r");
    comps_parse_file(parsed, fp, NULL);

    COMPS_Doc *doc1, *doc2, *doc3;
    doc1 = parsed->comps_doc;
    parsed->comps_doc = NULL;
    fp = fopen("f21-rawhide-comps.xml", "r");
    comps_parse_file(parsed, fp, NULL);
    doc2 = parsed->comps_doc;
    parsed->comps_doc = NULL;
    comps_parse_parsed_destroy(parsed);
    /*COMPS_ObjList *list= comps_doc_environments(c1);
    COMPS_DocEnv*e11,*e12,*e21,*e22, *e;
    e11 = list->first->next->comps_obj;
    e12 = list->first->next->next->next->comps_obj;

    for (COMPS_ObjListIt* it = list->first; it != NULL; it = it->next) {
        COMPS_Object *str;
        str = comps_docenv_get_id(it->comps_obj);
        COMPS_OBJECT_DESTROY(str);
    }
    COMPS_OBJECT_DESTROY(list);
    list= comps_doc_environments(c2);
    e22 = (COMPS_DocEnv*)comps_objlist_get(list, 8);
    e21 = (COMPS_DocEnv*)comps_objlist_get(list, 9);
    for (COMPS_ObjListIt* it = list->first; it != NULL; it = it->next) {
        COMPS_Object *str;
        str = comps_docenv_get_id(it->comps_obj);
        COMPS_OBJECT_DESTROY(str);
    }
    COMPS_OBJECT_DESTROY(list);
    e = comps_docenv_union(e21, e11);
    COMPS_OBJECT_DESTROY(e);
    //e = comps_docenv_union(e12, e22);
    //COMPS_OBJECT_DESTROY(e);
    COMPS_OBJECT_DESTROY(e21);
    COMPS_OBJECT_DESTROY(e22);
    */
    doc3 = comps_doc_union(doc2, doc1);
    COMPS_OBJECT_DESTROY(doc1);
    COMPS_OBJECT_DESTROY(doc2);
    COMPS_OBJECT_DESTROY(doc3);
}END_TEST

START_TEST(test_doc_defaults) {
    COMPS_DocGroup *g;
    COMPS_Doc * doc, *doc2;
    COMPS_Parsed *parsed;
    COMPS_ObjList *groups;
    COMPS_Object *tmpobj, *enc;
    COMPS_DocGroupPackage *p;
    char *tmp;

    printf("### start test_comps_default\n");
    
    enc = (COMPS_Object*)comps_str("UTF-8");
    doc = (COMPS_Doc*)comps_object_create(&COMPS_Doc_ObjInfo, (COMPS_Object*[]){enc});
    COMPS_OBJECT_DESTROY(enc);
    g = (COMPS_DocGroup*)
        comps_object_create(&COMPS_DocGroup_ObjInfo, NULL);
    comps_docgroup_set_id(g, "group1", 0);

    p = (COMPS_DocGroupPackage*)
        comps_object_create(&COMPS_DocGroupPackage_ObjInfo, NULL);
    comps_docpackage_set_name(p, "package1", 0);
    comps_docgroup_add_package(g, p);

    comps_doc_add_group(doc, g);
    tmp = comps2xml_str(doc, NULL, NULL);
    //printf("tmp %s\n", tmp);
    parsed = comps_parse_parsed_create();
    fail_if(comps_parse_parsed_init(parsed, "UTF-8", 0) == 0);
    comps_parse_str(parsed, tmp, NULL);
    doc2 = parsed->comps_doc;
    groups = comps_doc_groups(doc2);
    g = (COMPS_DocGroup*)groups->first->comps_obj;
    tmpobj = comps_docgroup_get_uservisible(g);
    ck_assert(((COMPS_Num*)tmpobj)->val == 1);

    COMPS_OBJECT_DESTROY(groups);
    COMPS_OBJECT_DESTROY(tmpobj);
    COMPS_OBJECT_DESTROY(doc);
    free(tmp);
    comps_parse_parsed_destroy(parsed);

}END_TEST

Suite* basic_suite (void)
{
    Suite *s = suite_create ("Basic Tests");
    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_comps_doc_basic);
    tcase_add_test (tc_core, test_comps_doc_xml);
    tcase_add_test (tc_core, test_comps_doc_setfeats);
    tcase_add_test (tc_core, test_comps_doc_union);
    tcase_add_test (tc_core, test_doc_defaults);
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
