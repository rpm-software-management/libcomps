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

#include "../src/comps_mradix.h"
#include "../src/comps_hslist.h"
#include <stdio.h>

void* int_cloner(void *y) {
    void *x;
    x = malloc(sizeof(int));
    memcpy(x,y, sizeof(int));
    return x;
}

void inc(void *data, void *item){
    *(int*)item += *(int*)data;
}

void print_intp_data(void * data, const char * key) {
    if (data == NULL) printf("data for '%s': NULL\n", key);
    else {
        COMPS_HSListItem * it;
        printf("data for '%s': ", key);
        for (it = ((COMPS_HSList*)data)->first; it!= NULL; it=it->next) {
            printf("%d|", *(int*)it->data);
        }
        printf("\n");
    }
}

void print_data4keys(COMPS_MRTree * mrt, char * keys[], unsigned int len) {
    unsigned int i;
    for (i=0; i<len; i++) {
        print_intp_data(comps_mrtree_get(mrt,keys[i]), keys[i]);
    }
}

void print_keys(COMPS_HSList * keys) {
    COMPS_HSListItem * it;
    for (it = keys->first; it != NULL; it = it->next) {
        printf("key:%s|\n", (char*)it->data);
    }
    printf("\n");
}

int main(int argc, char * argv[]) {

    COMPS_MRTree *tree, *tree2, *cloned1, *cloned2;
    COMPS_HSList * keyslist;

    char * keys[] = {"some key", "some Cray", "some", "anything"};
    int x1 = 10;
    int x2 = 11;
    int x3 = 12;
    int y1 = 20;
    int y2 = 21;
    int y3 = 22;
    int z1 = 30;
    int z2 = 31;
    int z3 = 32;

    int udata = 9;
    void * data;

    tree = comps_mrtree_create(&int_cloner, &int_cloner, &free);

printf(">> Building tree\n");
/*          Root
 */
    comps_mrtree_set(tree,"some key",(void*)&x1);
    comps_mrtree_set(tree,"some key",(void*)&x2);
printf(
"            Root                      \n"
"              ['some key']-->10,11    \n"
"                                      \n"
"");
    comps_mrtree_set(tree,"some Cray",(void*)&y1);
    comps_mrtree_set(tree,"some Cray",(void*)&y2);
printf(
"            Root                           \n"
"              ['some']-->|-['Cray']-->20,21\n"
"                         |-['key']-->10,11 \n"
"");
    comps_mrtree_set(tree,"some",(void*)&z1);
    comps_mrtree_set(tree,"some",(void*)&z3);
printf(
"            Root                           \n"
"              ['some']-->|-['Cray']-->20,21\n"
"                  |      |-['key']-->10,11 \n"
"                  V                        \n"
"                 30,32                     \n"
"");
    comps_mrtree_set(tree,"anything",(void*)&z2);
printf(
"            Root                           \n"
"    [anythink]['some']-->|-['Cray']-->20,21\n"
"        |         |      |-['key']-->10,11 \n"
"        V         V                        \n"
"       31       30,32                      \n"
"");

    printf(">> Incrementing all values by %d\n", udata);
    comps_mrtree_values_walk(tree, &udata, &inc);
    print_data4keys(tree, keys, 4);
    data = comps_mrtree_get(tree, "sAme Cray");
    print_intp_data(data, "sAme Cray");

    printf("------------\n");
    printf(">> Getting all keys for tree:\n");
    keyslist = comps_mrtree_keys(tree);
    print_keys(keyslist);
    comps_hslist_destroy(&keyslist);

    printf("------------\n");
    printf(">> Adding %d to 'some key'\n", x3);
    comps_mrtree_set(tree,"some key",(void*)&x3);
    data = comps_mrtree_get(tree, "some key");
    print_intp_data(data, "some key (modified)");
    printf("------------\n");

    printf(">> Removing 'some Cray'\n");
    comps_mrtree_unset(tree, "some Cray");
    print_data4keys(tree, keys, 4);
    printf("------------\n");

    printf(">> Clonning tree\n");
    cloned1 = comps_mrtree_clone(tree);
    printf(">> Incrementing all values in clonned tree by %d\n", udata);
    comps_mrtree_values_walk(cloned1, &udata, &inc);
    printf(">> Original tree data:\n");
    data = comps_mrtree_get(cloned1, "some key");
    print_data4keys(tree, keys, 4);
    printf("\n");
    printf(">> Cloned tree data:\n");
    print_data4keys(cloned1, keys, 4);
    printf("------------\n");

    printf("\n");
    printf(">> Clonning from clone\n");
    cloned2 = comps_mrtree_clone(cloned1);
    printf(">> Incrementing all values in clonned2 tree by %d\n", udata);
    printf(">> Data for original clone\n");
    comps_mrtree_values_walk(cloned1, &udata, &inc);
    print_data4keys(cloned1, keys, 4);
    printf("\n");
    printf(">> Data for cloned clone\n");
    comps_mrtree_values_walk(cloned1, &udata, &inc);
    print_data4keys(cloned1, keys, 4);
    comps_mrtree_destroy(cloned1);
    comps_mrtree_destroy(cloned2);
    printf("------------\n");

    printf(">> Removing 'some'\n");
    comps_mrtree_unset(tree, "some");

    data = comps_mrtree_get(tree, "some key");
    print_intp_data(data, "some key");

    data = comps_mrtree_get(tree, "some");
    print_intp_data(data, "some");
    printf("------------\n");

    printf(">> Adding %d to key 'anyones' shortened to len 6 [anyone]\n", x3);
    comps_mrtree_set_n(tree,"anyones", 6,(void*)&x3);

    data = comps_mrtree_get(tree, "anyones");
    print_intp_data(data, "anyones");

    data = comps_mrtree_get(tree, "anyone");
    print_intp_data(data, "anyone");

    tree2 = comps_mrtree_create(&int_cloner, &int_cloner, &free);
    printf(">> Building tree2\n");
    comps_mrtree_set(tree2,"foo",(void*)&x1);
    //comps_mrtree_set(tree2,"bar",(void*)&x2);
printf(
"            Root         \n"
"       ['foo']['bar']    \n"
"          |      |       \n"
"          V      V       \n"
"         109    100      \n"
"");
    udata = 99;
    //comps_mrtree_values_walk(tree2, &udata, &inc);
    data = comps_mrtree_get(tree2, "foo");
    print_intp_data(data, "foo");
    data = comps_mrtree_get(tree2, "bar");
    print_intp_data(data, "bar");

   printf(">> Adding tree2 to tree1 (tree1 = tree1 + tree2)\n");
   comps_mrtree_unite(tree, tree2);
    printf("Keylist\n");
    printf(">> Getting all keys for tree:\n");
    keyslist = comps_mrtree_keys(tree);
    print_keys(keyslist);
    comps_hslist_destroy(&keyslist);

    data = comps_mrtree_get(tree, "foo");
    print_intp_data(data, "foo");
    data = comps_mrtree_get(tree, "bar");
    print_intp_data(data, "bar");

    comps_mrtree_destroy(tree);
    comps_mrtree_destroy(tree2);
    return 0;
}
