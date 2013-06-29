#include <check.h>
#include <stdio.h>

#include "../src/comps.h"
#include "../src/comps_parse.h"

char * pkg_type2str(COMPS_PackageType type) {
    switch(type) {
        case COMPS_PACKAGE_DEFAULT:
            return "default";
        case COMPS_PACKAGE_OPTIONAL:
            return "optional";
        case COMPS_PACKAGE_MANDATORY:
            return "mandatory";
        case COMPS_PACKAGE_CONDITIONAL:
            return "conditional";
        case COMPS_PACKAGE_UNKNOWN:
            return "unknown";
        break;
    }
    return NULL;
}

void print_package(COMPS_ListItem* item, unsigned int num, void * data) {
    (void)num;
    (void)data;
    printf("package: %s ", ((COMPS_DocGroupPackage*)item->data)->name);
    printf("type: %s\n", pkg_type2str(((COMPS_DocGroupPackage*)item->data)->type));
}

void print_gids(COMPS_ListItem* item, unsigned int num, void * data) {
    (void)num;
    if (item->prev == NULL)
        printf("group ids:");
    printf("%s ", (char*)data);
    if (item->next == NULL)
        printf("\n");
}

void print_group(COMPS_ListItem* item, unsigned int num, void * data) {
    (void)num;
    (void)data;
    char *id, *name, *desc;
    int disp_ord, def, uservis;
    COMPS_Prop *tmp_prop;

    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "id");
    id = (tmp_prop)?tmp_prop->prop.str:"NULL";
    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "name");
    name = (tmp_prop)?tmp_prop->prop.str:"NULL";
    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "desc");
    desc = (tmp_prop)?tmp_prop->prop.str:"NULL";
    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "display_order");
    disp_ord = (tmp_prop)?tmp_prop->prop.num:0;
    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "uservis");
    uservis = (tmp_prop)?tmp_prop->prop.num:0;
    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "def");
    def = (tmp_prop)?tmp_prop->prop.num:0;
    
    printf("id: %s\n", id);
    printf("name: %s\n", name);
    printf("desc: %s\n", desc);
    printf("default: %d\n", def);
    printf("uservisible: %d\n", uservis);
    printf("display_order: %d\n", disp_ord);
    comps_list_walk(((COMPS_DocGroup*)item->data)->packages, &print_package, NULL);
    printf("-------------------------------------\n");
}

void print_category(COMPS_ListItem* item, unsigned int num, void * data) {
    (void)data;
    (void)num;
    char *id, *name, *desc;
    int disp_ord;
    COMPS_Prop *tmp_prop;

    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "id");
    id = (tmp_prop)?tmp_prop->prop.str:"NULL";
    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "name");
    name = (tmp_prop)?tmp_prop->prop.str:"NULL";
    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "desc");
    desc = (tmp_prop)?tmp_prop->prop.str:"NULL";
    tmp_prop = comps_dict_get(((COMPS_DocGroup*)item->data)->properties, "display_order");
    disp_ord = (tmp_prop)?tmp_prop->prop.num:0;
    
    printf("id: %s\n", id);
    printf("name: %s\n", name);
    printf("desc: %s\n", desc);
    printf("display_order: %d\n", disp_ord);
    comps_list_walk(((COMPS_DocCategory*)item->data)->group_ids, &print_gids, NULL);
}

START_TEST(test_comps_doc_basic)
{
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
    COMPS_List *tmplist;

    doc = comps_doc_create("UTF-8");
    comps_doc_init(doc);
    for (int i=0; i<2; i++) {
        g = comps_docgroup_create();
        comps_docgroup_set_id(g, (char*)groups_ids[i], 1);
        comps_docgroup_set_name(g, (char*)groups_names[i], 1);
        for (int x=0; x<4; x++) {
            p = comps_docpackage_create();
            comps_docpackage_set_name(p, (char*)group_mpackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_MANDATORY);
            comps_docgroup_add_package(g, p);
            p = comps_docpackage_create();
            comps_docpackage_set_name(p, (char*)group_opackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_OPTIONAL);
            comps_docgroup_add_package(g, p);
        }
        comps_doc_add_group(doc, g);
    }

    for (int i=0; i<3; i++) {
        c = comps_doccategory_create();
        comps_doccategory_set_id(c, (char*)cats_ids[i], 1);
        comps_doccategory_set_name(c, (char*)cats_names[i], 1);
        for (int x=0; x<3; x++) {
            comps_doccategory_add_groupid(c, (char*)cat_gids[i][x], 1);
        }
        comps_doc_add_category(doc, c);
    }
    tmplist = comps_doc_groups(doc);
    fail_if(tmplist->len == 0, "No groups found");
    tmplist = comps_doc_get_groups(doc, "g1", NULL, NULL, NULL);
    fail_if(tmplist->len == 0, "Group with id 'g1' should be in groups, but"
                               "cant get it");
    g = (COMPS_DocGroup*)tmplist->first->data;
    comps_list_destroy(&tmplist);

    tmplist = comps_docgroup_get_packages(g, NULL, COMPS_PACKAGE_OPTIONAL);
    fail_if(tmplist->len != 4, "Group with id 'g1' should have 4 optional "
                               "packages. But have %d.", tmplist->len);
    comps_list_destroy(&tmplist);
    tmplist = comps_docgroup_get_packages(g, NULL, COMPS_PACKAGE_MANDATORY);
    fail_if(tmplist->len != 4, "Group with id 'g1' should have 4 optional "
                               "packages. But have %d.", tmplist->len);
    comps_list_destroy(&tmplist);
    comps_doc_destroy(&doc);
}END_TEST

START_TEST(test_comps_doc_setfeats)
{
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
    //COMPS_List *tmplist;

    doc = comps_doc_create("UTF-8");
    comps_doc_init(doc);
    for (int i=0; i<4; i++) {
        g = comps_docgroup_create();
        comps_docgroup_set_id(g, (char*)groups_ids[i], 1);
        comps_docgroup_set_name(g, (char*)groups_names[i], 1);
        for (int x=0; x<4; x++) {
            p = comps_docpackage_create();
            comps_docpackage_set_name(p, (char*)group_mpackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_MANDATORY);
            comps_docgroup_add_package(g, p);
            p = comps_docpackage_create();
            comps_docpackage_set_name(p, (char*)group_opackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_OPTIONAL);
            comps_docgroup_add_package(g, p);
        }
        comps_doc_add_group(doc, g);
    }
    for (int i=0; i<3; i++) {
        e = comps_docenv_create();
        comps_docenv_set_id(e, (char*)envs_ids[i], 1);
        comps_docenv_set_name(e, (char*)envs_names[i], 1);
        for (int x=0; x<3; x++) {
            comps_docenv_add_groupid(e, (char*)env_gids[i][x], 1);
        }
        for (int x=0; x<4; x++) {
            comps_docenv_add_optionid(e, (char*)env_opts[i][x], 1);
        }
        comps_doc_add_env(doc, e);
    }
    for (int i=0; i<3; i++) {
        c = comps_doccategory_create("UTF-8");
        comps_doccategory_set_id(c, (char*)cats_ids[i], 1);
        comps_doccategory_set_name(c, (char*)cats_names[i], 1);
        for (int x=0; x<3; x++) {
            comps_doccategory_add_groupid(c, (char*)cat_gids[i][x], 1);
        }
        comps_doc_add_category(doc, c);
    }

    doc2 = comps_doc_create("UTF-8");
    comps_doc_init(doc2);
    for (int i=0; i<4; i++) {
        g = comps_docgroup_create();
        comps_docgroup_set_id(g, (char*)groups_ids2[i], 1);
        comps_docgroup_set_name(g, (char*)groups_names2[i], 1);
        for (int x=0; x<4; x++) {
            p = comps_docpackage_create();
            comps_docpackage_set_name(p, (char*)group_mpackages2[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_MANDATORY);
            comps_docgroup_add_package(g, p);
            p = comps_docpackage_create();
            comps_docpackage_set_name(p, (char*)group_opackages2[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_OPTIONAL);
            comps_docgroup_add_package(g, p);
        }
        comps_doc_add_group(doc2, g);
    }
    for (int i=0; i<3; i++) {
        e = comps_docenv_create();
        comps_docenv_set_id(e, (char*)envs_ids2[i], 1);
        comps_docenv_set_name(e, (char*)envs_names2[i], 1);
        for (int x=0; x<3; x++) {
            comps_docenv_add_groupid(e, (char*)env_gids2[i][x], 1);
        }
        for (int x=0; x<4; x++) {
            comps_docenv_add_optionid(e, (char*)env_opts2[i][x], 1);
        }
        comps_doc_add_env(doc2, e);
    }
    for (int i=0; i<3; i++) {
        c = comps_doccategory_create();
        comps_doccategory_set_id(c, (char*)cats_ids2[i], 1);
        comps_doccategory_set_name(c, (char*)cats_names2[i], 1);
        for (int x=0; x<3; x++) {
            comps_doccategory_add_groupid(c, (char*)cat_gids2[i][x], 1);
        }
        comps_doc_add_category(doc2, c);
    }


    g1 = (COMPS_DocGroup*)comps_doc_groups(doc)->first->data;
    g2 = (COMPS_DocGroup*)comps_doc_groups(doc)->first->next->data;
    g = comps_docgroup_union(g1, g2);
    fail_if(g->packages->len != 14, "Union of (g1 v g2) should have 14 packages"
            " have %d", g->packages->len);
    comps_docgroup_destroy(g);
    g = comps_docgroup_intersect(g1, g2);
    fail_if(g->packages->len != 2, "Intersect of (g1 ^ g2) should have 2"
            "packages, have %d", g->packages->len);
    comps_docgroup_destroy(g);

    g1 = (COMPS_DocGroup*)comps_doc_groups(doc)->first->next->data;
    g2 = (COMPS_DocGroup*)comps_doc_groups(doc)->first->next->next->data;
    g = comps_docgroup_union(g1, g2);
    fail_if(g->packages->len != 16, "Union of (g2 v g3) should have 16 packages"
            " have %d", g->packages->len);
    comps_docgroup_destroy(g);
    g = comps_docgroup_intersect(g1, g2);
    fail_if(g->packages->len != 0, "Intersect of (g2 ^ g3) should have 0"
            "packages, have %d", g->packages->len);
    comps_docgroup_destroy(g);

    c1 = (COMPS_DocCategory*)comps_doc_categories(doc)->first->data;
    c2 = (COMPS_DocCategory*)comps_doc_categories(doc)->first->next->data;
    c = comps_doccategory_union(c1, c2);
    fail_if(c->group_ids->len != 4, "Union of (c1 v c2) should have 4 "
            "group ids, have %d", c->group_ids->len);
    comps_doccategory_destroy(c);
    c = comps_doccategory_intersect(c1, c2);
    fail_if(c->group_ids->len != 2, "Intersect of (c1 ^ c2) should have 2"
            "group ids, have %d", c->group_ids->len);
    comps_doccategory_destroy(c);

    c1 = (COMPS_DocCategory*)comps_doc_categories(doc)->first->next->data;
    c2 = (COMPS_DocCategory*)comps_doc_categories(doc)->first->next->next->data;
    c = comps_doccategory_union(c1, c2);
    fail_if(c->group_ids->len != 4, "Union of (c2 v c3) should have 4 "
            "group ids, have %d", c->group_ids->len);
    comps_doccategory_destroy(c);
    c = comps_doccategory_intersect(c1, c2);
    fail_if(c->group_ids->len != 2, "Intersect of (c2 ^ c3) should have 2"
            "group ids, have %d", c->group_ids->len);
    comps_doccategory_destroy(c);

    e1 = (COMPS_DocEnv*)comps_doc_environments(doc)->first->data;
    e2 = (COMPS_DocEnv*)comps_doc_environments(doc)->first->next->data;
    e = comps_docenv_union(e1, e2);
    fail_if(e->group_list->len != 4, "Union of (e1 v e2) should have 4 "
            "groud ids have %d", e->group_list->len);
    fail_if(e->option_list->len != 7, "Union of (e1 v e2) should have 7 "
            "groud ids have %d", e->option_list->len);
    comps_docenv_destroy(e);
    e = comps_docenv_intersect(e1, e2);
    fail_if(e->group_list->len != 2, "Intersect of (e1 ^ e2) should have 2"
            "group ids have %d", e->group_list->len);
    fail_if(e->option_list->len != 1, "Union of (e1 v e2) should have 1 "
            "option ids have %d", e->option_list->len);
    comps_docenv_destroy(e);

    e1 = (COMPS_DocEnv*)comps_doc_environments(doc)->first->next->data;
    e2 = (COMPS_DocEnv*)comps_doc_environments(doc)->first->next->next->data;
    e = comps_docenv_union(e1, e2);
    fail_if(e->group_list->len != 4, "Union of (e2 v e3) should have 4 "
            "groud ids have %d", e->group_list->len);
    fail_if(e->option_list->len != 6, "Union of (e2 v e3) should have 7 "
            "option ids have %d", e->option_list->len);
    comps_docenv_destroy(e);
    e = comps_docenv_intersect(e1, e2);
    fail_if(e->group_list->len != 2, "Intersect of (e2 ^ e3) should have 2 "
            "groupids  have %d", e->group_list->len);
    fail_if(e->option_list->len != 2, "Intersect of (e2 v e3) should have 2 "
            "option ids have %d", e->option_list->len);
    comps_docenv_destroy(e);

    tmpdoc = comps_doc_union(doc, doc2);
    fail_if(comps_doc_groups(tmpdoc)->len != 6, "Union of (doc ^ doc2) should have 6 "
            "groups  have %d", comps_doc_groups(tmpdoc)->len);
    fail_if(comps_doc_categories(tmpdoc)->len != 4, "Union of (doc ^ doc2) "
            "should have 4 categories  have %d",
            comps_doc_categories(tmpdoc)->len);
    fail_if(comps_doc_environments(tmpdoc)->len != 4, "Union of (doc ^ doc2) "
            "should have 4 environments  have %d",
            comps_doc_environments(tmpdoc)->len);
    comps_doc_destroy(&tmpdoc);
    tmpdoc = comps_doc_intersect(doc, doc2);
    fail_if(comps_doc_groups(tmpdoc)->len != 2, "Intersect of (doc ^ doc2) "
            "should have 2 groups  have %d", comps_doc_groups(tmpdoc)->len);
    fail_if(comps_doc_categories(tmpdoc)->len != 2, "Intersect of (doc ^ doc2)"
            " should have 2 categories  have %d",
            comps_doc_categories(tmpdoc)->len);
    fail_if(comps_doc_environments(tmpdoc)->len != 2, "Intersect of "
            "(doc ^ doc2) should have 2 environments  have %d",
            comps_doc_environments(tmpdoc)->len);
    comps_doc_destroy(&tmpdoc);


    comps_doc_destroy(&doc2);
    comps_doc_destroy(&doc);
}
END_TEST

START_TEST(test_comps_doc_xml)
{
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
    //COMPS_List *tmplist;

    doc = comps_doc_create("UTF-8");
    comps_doc_init(doc);
    for (int i=0; i<2; i++) {
        g = comps_docgroup_create();
        comps_docgroup_set_id(g, (char*)groups_ids[i], 1);
        comps_docgroup_set_name(g, (char*)groups_names[i], 1);
        for (int x=0; x<4; x++) {
            p = comps_docpackage_create();
            comps_docpackage_set_name(p, (char*)group_mpackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_MANDATORY);
            comps_docgroup_add_package(g, p);
            p = comps_docpackage_create();
            comps_docpackage_set_name(p, (char*)group_opackages[i][x], 1);
            comps_docpackage_set_type(p, COMPS_PACKAGE_OPTIONAL);
            comps_docgroup_add_package(g, p);
        }
        comps_doc_add_group(doc, g);
    }

    for (int i=0; i<3; i++) {
        c = comps_doccategory_create();
        comps_doccategory_set_id(c, (char*)cats_ids[i], 1);
        comps_doccategory_set_name(c, (char*)cats_names[i], 1);
        for (int x=0; x<3; x++) {
            comps_doccategory_add_groupid(c, (char*)cat_gids[i][x], 1);
        }
        comps_doc_add_category(doc, c);
    }
    comps2xml_f(doc, "testfile.xml", 1);
    comps_doc_destroy(&doc);
}
END_TEST

void* str_clonner(void *str) {
    void *ret;
    ret = malloc(sizeof(char) * (strlen((char*)str)+1));
    memcpy(ret, str, sizeof(char) * (strlen((char*)str)+1));
    return ret;
}

START_TEST(test_comps_doc_union)
{
    COMPS_DocGroup *g1, *g2, *g3;
    COMPS_DocGroupPackage *p;

    g1 = comps_docgroup_create();
    comps_docgroup_set_id(g1, "g1", 1);
    comps_docgroup_set_name(g1, "group1", 1);
    
    p = comps_docpackage_create();
    comps_docpackage_set_name(p, "package1", 1);
    
    comps_docgroup_add_package(g1, p);
    p = comps_docpackage_create();
    comps_docpackage_set_name(p, "package2", 1);
    comps_docgroup_add_package(g1, p);

    g2 = comps_docgroup_create();
    comps_docgroup_set_id(g2, "g2", 1);
    comps_docgroup_set_name(g2, "group2", 1);
    p = comps_docpackage_create();
    comps_docpackage_set_name(p, "package3", 1);
    comps_docgroup_add_package(g2, p);
    p = comps_docpackage_create();
    comps_docpackage_set_name(p, "package4", 1);
    comps_docgroup_add_package(g2, p);

    g3 = comps_docgroup_union(g1, g2);
    comps_docgroup_destroy(g1);
    comps_docgroup_destroy(g2);
    comps_docgroup_destroy(g3);
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
