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
#include "check_utils.h"

void * int_constructor(void *x){
    void * y;
    if ((y = malloc(sizeof(int)))==NULL)
        return NULL;
    memcpy(y, x, sizeof(int));
    return y;
}

void append_int_data(COMPS_List * list, int size,int * data)
{
    COMPS_ListItem * item;
    int i;

    for (i=0; i<size; i++) {
        item = comps_list_item_create((void*)&data[i], &int_constructor, &free);
        comps_list_append(list, item);
    }
}

START_TEST(test_init_destroy)
{
    COMPS_List * list;
    list = comps_list_create();
    comps_list_init(list);
    fail_unless(list->first == NULL, "Wrong list init");
    fail_unless(list->last == NULL, "Wrong list init");
    comps_list_destroy(&list);
    fail_unless(list == NULL, "Wrong list destroy");
}
END_TEST

START_TEST(test_append)
{
    COMPS_ListItem * item;
    COMPS_List * list;
    int i, test_data_len = 10;
    int test_data[] = {11,22,33,44,55,66,77,88,99,00};

    list = comps_list_create();
    comps_list_init(list);

    append_int_data(list, test_data_len, test_data);
    fail_unless(list->len == 10, "List len should be 10");
    comps_list_destroy(&list);

    list = comps_list_create();
    comps_list_init(list);
    append_int_data(list, test_data_len, test_data);

    for (i=0; i<test_data_len+1; i++) {
        item = comps_list_at(list, i);
        if (item != NULL) {
            fail_if(*(int*)item->data != test_data[i],
                    "item at pos %d should be %d not %d", i,
                    test_data[i], item);
        }
        if (i>=test_data_len)
            fail_unless(item == NULL, "Item at should be NULL");
        else
            fail_if(item == NULL, "Item shouldn't be NULL");
        if (item == NULL) continue;
        fail_unless(item->data != NULL, "Item shouldn't have NULL data");
        if (item->data == NULL) continue;
    }
    comps_list_destroy(&list);
}
END_TEST

START_TEST(test_remove)
{
    COMPS_ListItem *it, *oldit;
    COMPS_List * list;
    int tmp, i, test_data_len = 10;
    int test_data[] = {11,22,33,44,55,66,77,88,99,100};

    list = comps_list_create();
    comps_list_init(list);

    append_int_data(list, test_data_len, test_data);

    comps_list_remove_pos(list, 0);
    fail_unless(list->len == 9, "List length should be 9");
    it = comps_list_at(list, 0);
    fail_unless(*((int*)it->data) == 22, "Item data after remove should be 22");
    comps_list_remove_pos(list, 4);
    it = comps_list_at(list, 0);
    fail_unless(*((int*)it->data) == 22, "Item data after remove should be 22");
    it = comps_list_at(list, 4);
    fail_unless(*(int*)it->data == 77,
                "Item data after remove should be 77 not %d",
                *(int*)it->data);
    fail_unless(list->len == 8, "List lenght should be 8");
    tmp = list->len;

    for (i=0; i < tmp; i++) {
        comps_list_remove_pos(list, 0);
    }
    fail_unless(list->len == 0, "List should be empty");

    append_int_data(list, test_data_len, test_data);

    it = comps_list_at(list, 4);
    fail_unless(*((int*)it->data) == 55, "Item data after remove should be 55");
    oldit = NULL;
    for (it = list->first->next; it != list->last; oldit = it, it = it->next) {
        if (oldit) {
            comps_list_remove_item(list, oldit);
            /*oldit = it;
            for (it = list->first; it != list->last; it = it->next)
                printf("%d ", *(int*)it->data);
            printf("%d\n", *(int*)it->data);
            it = oldit;*/
        }
        //free(it->data);
        free(oldit);
    }
    comps_list_remove_item(list, oldit);
    free(oldit);

    fail_unless(list->len == 2, "List should be len 2 not %d", list->len);
    comps_list_destroy(&list);
}
END_TEST

START_TEST(test_insert)
{
    COMPS_ListItem * item;
    COMPS_List * list;
    int data, i, test_data_len = 10;
    int test_data[] = {11,22,33,44,55,66,77,88,99,00};

    list = comps_list_create();
    comps_list_init(list);

    for (i=0; i<test_data_len; i++) {
        item = comps_list_item_create((void*)&test_data[i],
                                      &int_constructor, &free);
        comps_list_insert(list, item, 0);
    }
    for (i=0; i<test_data_len; i++) {
        item = comps_list_at(list, test_data_len-i-1);
        fail_unless(*((int*)item->data) == test_data[i], "Item data doens't match");
    }
    fail_unless(list->len == 10, "List lenght should be 10");

    data = 14;
    item = comps_list_item_create((void*)&data,
                                  &int_constructor, &free);
    comps_list_insert(list, item, 5);

    item = comps_list_at(list, 5);
    fail_unless(*((int*)item->data) == 14, "Item data after remove should be 55");

    data = 214;
    item = comps_list_item_create((void*)&data,
                                  &int_constructor, &free);
    comps_list_insert(list, item, 11);

    //printf("%d\n", list->len);
    item = comps_list_at(list, 11);
    fail_unless(*((int*)item->data) == 214, "Item data after remove should be 55");
    comps_list_destroy(&list);
}
END_TEST

START_TEST(test_pop)
{
    COMPS_ListItem * it;
    COMPS_List * list;
    int i, test_data_len = 2;
    int test_data[] = {11, 22};

    list = comps_list_create();
    comps_list_init(list);
    append_int_data(list, test_data_len, test_data);
    for (i=0; i<test_data_len; i++) {
        it = comps_list_pop(list);
        fail_unless(*((int*)it->data) == test_data[test_data_len - i -1], "Wrong item data");
        comps_list_item_destroy(it);
    }
    it = comps_list_pop(list);
    fail_unless(it == NULL, "Returned item should be NULL");
    fail_unless(list->first == NULL, "List should be empty(first not null   )");
    fail_unless(list->last == NULL, "List should be empty(last not null)");
    fail_unless(list->len == 0, "List length should be zero");
    append_int_data(list, test_data_len, test_data);
    fail_unless(list->len == 2, "List length should be zero");
    comps_list_destroy(&list);
}
END_TEST

START_TEST(test_shift)
{
    COMPS_ListItem * it;
    COMPS_List * list;
    int i, test_data_len = 2;
    int test_data[] = {11, 22};

    list = comps_list_create();
    comps_list_init(list);
    append_int_data(list, test_data_len, test_data);
    for (i=0; i<test_data_len; i++) {
        it = comps_list_shift(list);
        fail_unless(*((int*)it->data) == test_data[i], "Wrong item data");
        comps_list_item_destroy(it);
    }
    it = comps_list_shift(list);
    fail_unless(it == NULL, "Returned item should be NULL");
    fail_unless(list->first == NULL, "List should be empty");
    fail_unless(list->last == NULL, "List should be empty");
    fail_unless(list->len == 0, "List length should be zero");
    append_int_data(list, test_data_len, test_data);
    fail_unless(list->len == 2, "List length should be zero");
    comps_list_destroy(&list);
}
END_TEST

Suite* basic_suite (void)
{
    Suite *s = suite_create ("Basic Tests");
    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_init_destroy);
    tcase_add_test (tc_core, test_append);
    tcase_add_test (tc_core, test_remove);
    tcase_add_test (tc_core, test_insert);
    tcase_add_test (tc_core, test_pop);
    tcase_add_test (tc_core, test_shift);
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
