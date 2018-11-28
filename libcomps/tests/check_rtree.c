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

#include "../src/comps_radix.h"
#include <stdio.h>

void print_intp_data(void * data, const char * key) {
    if (data == NULL) printf("data for '%s': NULL\n", key);
    else printf("data for '%s': %d\n", key, *(int*)data);
}

void print_intp_hslist(COMPS_HSList * list) {
    COMPS_HSListItem * it;
    if (list == NULL)
        return;
    for (it = list->first; it != NULL; it = it->next) {
        printf("%d ", *(int*)it->data);
    }
    printf("\n");
}

void* int_cloner(void * int_v) {
    void * ret = malloc(sizeof(int));
    memcpy(ret, int_v, sizeof(int));
    return ret;
}

void* str_cloner(void *str) {
    void *ret = malloc(sizeof(char)* (strlen((char*)str)+1));
    memcpy(ret, str, (strlen((char*)str)+1));
    return ret;
}

void print_all(COMPS_RTree *rt) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;
    int *x;

    printf("-------- print pairs --------------\n");
    pairlist = comps_rtree_pairs(rt);

    for (it = pairlist->first; it != NULL; it = it->next) {
        x = ((COMPS_RTreePair*)it->data)->data;
        printf("%s = %d \n",  ((COMPS_RTreePair*)it->data)->key,
                              *x);

    }
    comps_hslist_destroy(&pairlist);
    printf("-----------------------------------\n");
}

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


COMPS_RTree * load_acrodict(char *filename) {
    char buffer[100];
    char *pch;
    FILE *f;
    COMPS_RTree *rt;

    void *val;

    rt = comps_rtree_create(NULL, NULL, &free);
    if (!rt) return NULL;

    f = fopen(filename, "r");
    if (!f) {
        comps_rtree_destroy(rt);
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
        val = str_cloner(pch+1);
        //printf("%s = %s\n", (char*)val, pch+1);
        comps_rtree_set(rt, buffer, val);
        //printf("--------------------\n");
        /*for (COMPS_HSListItem *it = rt->subnodes->first;
             it != NULL; it = it->next) {
            printf("%s\n", ((COMPS_RTreeData*)it->data)->key);
        }*/
    }
    fclose(f);
    return rt;
}


int main(int argc, char * argv[]) {

    int x, x2, y, z, a;
    COMPS_RTree * tree, *clonned;
    //COMPS_HSList * values;

    (void)argc;
    (void)argv;

    tree = comps_rtree_create( NULL, &int_cloner, &free);
    //comps_rtree_init(tree);
    x = 10;
    x2 = 11;
    y = 20;
    z = 30;
    a = 1;
    void * data;


/*          Root
 */
    comps_rtree_set(tree,"some key",int_cloner((void*)&x));
/*          Root
 *              ['some key']-->10
 */
    comps_rtree_set(tree,"some Cray",int_cloner((void*)&y));
/*          Root
 *              ['some']-->|-['Cray']-->20
 *                         |-['key']-->10
 */
    comps_rtree_set(tree,"some",int_cloner((void*)&z));
/*          Root
 *              ['some']-->|-['Cray']-->20
 *                  |      |-['key']-->10
 *                  V
 *                 30
 */
    comps_rtree_set(tree,"anything",int_cloner((void*)&a));
/*          Root
 *              ['some']-->|-['Cray']-->20
 *                  |      |-['key']-->10
 *                  V
 *                 30
 */

    print_all(tree);
    //values = comps_rtree_values(tree);
    //print_intp_hslist(values);
    //comps_hslist_destroy(&values);

    data = comps_rtree_get(tree, "some key");
    print_intp_data(data, "some key");

    data = comps_rtree_get(tree, "some Cray");
    print_intp_data(data, "some Cray");

    data = comps_rtree_get(tree, "sAme Cray");
    print_intp_data(data, "sAme Cray");

    data = comps_rtree_get(tree, "some");
    print_intp_data(data, "some");

    data = comps_rtree_get(tree, "anything");
    print_intp_data(data, "anything");
    printf("------------\n");
    printf("-data for clonned tree-\n");
    clonned = comps_rtree_clone(tree);
    printf("end clonning\n");
    data = comps_rtree_get(clonned, "some key");
    print_intp_data(data, "some key");

    data = comps_rtree_get(clonned, "some Cray");
    print_intp_data(data, "some Cray");

    data = comps_rtree_get(clonned, "sAme Cray");
    print_intp_data(data, "sAme Cray");

    data = comps_rtree_get(clonned, "some");
    print_intp_data(data, "some");

    data = comps_rtree_get(clonned, "anything");
    print_intp_data(data, "anything");
    comps_rtree_destroy(clonned);
    printf("------------\n");

    comps_rtree_set(tree,"some key",int_cloner((void*)&x2));
/*          Root
 *              ['some']-->|-['Cray']-->20
 *                  |       |-['key']-->11
 *                  V
 *                 30
 */
    data = comps_rtree_get(tree, "some key");
    print_intp_data(data, "some key (rewrited)");
    printf("------------\n");


    printf("removing 'some Cray'\n");
    comps_rtree_unset(tree, "some Cray");
    printf("------------\n");
/*          Root
 *              ['some']-->|
 *                  |      |-['key']-->11
 *                  V
 *                 30
 */

    data = comps_rtree_get(tree, "some key");
    print_intp_data(data, "some key");

    data = comps_rtree_get(tree, "some Cray");
    print_intp_data(data, "some cray");

    data = comps_rtree_get(tree, "some");
    print_intp_data(data, "some");

    data = comps_rtree_get(tree, "anything");
    print_intp_data(data, "anything");
    printf("------------\n");
    printf("removing 'some'\n");
    comps_rtree_unset(tree, "some");
/*          Root
 *              ['some']-->|
 *                         |-['key']-->11
 */

    data = comps_rtree_get(tree, "some key");
    print_intp_data(data, "some key");

    data = comps_rtree_get(tree, "some");
    print_intp_data(data, "some");

    comps_rtree_destroy(tree);

    printf("------------\n");
    printf("clonning empty rtree\n");
    tree = comps_rtree_create(NULL, &int_cloner, &free);
    clonned = comps_rtree_clone(tree);
    comps_rtree_destroy(clonned);
    comps_rtree_destroy(tree);
    printf("------------\n");
    printf("loading acronym dict\n");
    tree = load_acrodict("dict-test.txt");
    print_all_str(tree);
    printf("------------\n");
    comps_rtree_destroy(tree);

    tree = load_acrodict("dict-test2.txt");
    print_all_str(tree);
    /*for (COMPS_HSListItem *it = tree->subnodes->first;
         it != NULL; it = it->next) {
        printf("%s\n", ((COMPS_RTreeData*)it->data)->key);
    }*/
    comps_rtree_destroy(tree);

    return 0;
}
