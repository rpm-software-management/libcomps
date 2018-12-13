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

#include "../src/comps_objradix.h"
#include "../src/comps_obj.h"

#include <stdio.h>
#include <check.h>

void print_all(COMPS_ObjRTree *rt) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;
    char *str;
    COMPS_Object *x;

    printf("-------- print pairs --------------\n");
    pairlist = comps_objrtree_pairs(rt);

    for (it = pairlist->first; it != NULL; it = it->next) {
        x = ((COMPS_ObjRTreePair*)it->data)->data;
        str = comps_object_tostr(x);
        printf("%s = %s \n",  ((COMPS_ObjRTreePair*)it->data)->key, str);
        free(str);
    }
    comps_hslist_destroy(&pairlist);
    printf("-----------------------------------\n");
}

COMPS_ObjRTree * load_acrodict(char *filename) {
    char buffer[100];
    char *pch;
    FILE *f;
    COMPS_ObjRTree *rt;

    COMPS_Str *val;

    rt = (COMPS_ObjRTree*)comps_object_create(&COMPS_ObjRTree_ObjInfo, NULL);
    if (!rt) return NULL;

    f = fopen(filename, "r");
    if (!f) {
        COMPS_OBJECT_DESTROY(rt);
        return NULL;
    }
    while (!feof(f)) {
        memset(buffer, 0, 100);
        if (!fgets(buffer, 100, f)) {
            fclose(f);
            return rt;
        }
        buffer[strlen(buffer)-1] =0;
        //printf("buffer:%s\n", buffer);
        pch = strrchr(buffer, '-');
        buffer[pch-buffer] = 0;
        val = comps_str(pch+1);
        //printf("%s = %s\n", (char*)val, pch+1);
        comps_objrtree_set_x(rt, buffer, (COMPS_Object*)val);
        //printf("--------------------\n");
        /*for (COMPS_HSListItem *it = rt->subnodes->first;
             it != NULL; it = it->next) {
            printf("%s\n", ((COMPS_RTreeData*)it->data)->key);
  gtgt      }*/
    }
    fclose(f);
    return rt;
}


START_TEST(test_objrtree) {
    int test_vals[] = {10,22,42,11};
    char* test_keys[] = {"some key", "some Cray", "some", "anything", NULL};
    int test_vals2[] = {101,220};
    char* test_keys2[] = {"some key", "some Cray", NULL};
    char* miss_keys[] = {"sAme Cray", "something", NULL};
    char* remove_keys[] = {"some Cray", "some", NULL};
    int remove_check_vals[] = {101,11};
    char* remove_check_keys[] = {"some key", "anything", NULL};

    COMPS_ObjRTree *tree, *clonned;
    COMPS_Object *val;

    tree = (COMPS_ObjRTree*)comps_object_create(&COMPS_ObjRTree_ObjInfo, NULL);
    for (int x=0; test_keys[x] != NULL; x++) {
        comps_objrtree_set_x(tree, test_keys[x],
                             (COMPS_Object*)comps_num(test_vals[x]));
    }
    print_all(tree);
    for (int x=0; test_keys[x] != NULL; x++) {
        val = comps_objrtree_get(tree, test_keys[x]);
        ck_assert(val != NULL);
        ck_assert(val->obj_info == &COMPS_Num_ObjInfo);
        ck_assert(((COMPS_Num*)val)->val == test_vals[x]);
        COMPS_OBJECT_DESTROY(val);
    }
    for (int x=0; miss_keys[x]; x++) {
        val = comps_objrtree_get(tree, miss_keys[x]);
        ck_assert(val == NULL);
        COMPS_OBJECT_DESTROY(val);
    }

    clonned = comps_objrtree_clone(tree);
    for (int x=0; test_keys[x]; x++) {
        val = comps_objrtree_get(clonned, test_keys[x]);
        ck_assert(((COMPS_Num*)val)->val == test_vals[x]);
        COMPS_OBJECT_DESTROY(val);
    }
    for (int x=0; miss_keys[x]; x++) {
        val = comps_objrtree_get(clonned, miss_keys[x]);
        ck_assert(val == NULL);
        COMPS_OBJECT_DESTROY(val);
    }

    for (int x=0; test_keys2[x]; x++) {
        comps_objrtree_set_x(tree, test_keys2[x],
                             (COMPS_Object*)comps_num(test_vals2[x]));
    }
    for (int x=0; test_keys2[x]; x++) {
        val = comps_objrtree_get(tree, test_keys2[x]);
        ck_assert(((COMPS_Num*)val)->val == test_vals2[x]);
        COMPS_OBJECT_DESTROY(val);
    }

    for (int x=0; remove_keys[x]; x++) {
        comps_objrtree_unset(tree, remove_keys[x]);
    }
    for (int x=0; remove_check_keys[x]; x++) {
        val = comps_objrtree_get(tree, remove_check_keys[x]);
        printf("%s\n", remove_check_keys[x]);
        ck_assert_msg(((COMPS_Num*)val)->val == remove_check_vals[x],
                      "%d != %d", ((COMPS_Num*)val)->val, remove_check_vals[x]);
        COMPS_OBJECT_DESTROY(val);
    }
    COMPS_OBJECT_DESTROY(tree);
    COMPS_OBJECT_DESTROY(clonned);


    printf("------------\n");
    printf("loading acronym dict\n");
    tree = load_acrodict("dict-test.txt");
    print_all(tree);
    COMPS_OBJECT_DESTROY(tree);

    printf("------------\n");
    printf("loading acronym dict\n");
    tree = load_acrodict("dict-test2.txt");
    print_all(tree);

    /*for (COMPS_HSListItem *it = tree->subnodes->first;
         it != NULL; it = it->next) {
        printf("%s\n", ((COMPS_RTreeData*)it->data)->key);
    }*/
    COMPS_OBJECT_DESTROY(tree);
} END_TEST

Suite* basic_suite (void)
{
    Suite *s = suite_create ("Basic Tests");
    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_objrtree);
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
