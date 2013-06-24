#include "check_utils.h"
#ifndef CHECK_UTILS_H
#define CHECK_UTILS_H

void print_list_item(COMPS_ListItem * it, unsigned int num, void * user_data)
{
    (void) user_data;
    printf("%d. %d\n", num, *((int*)it->data));
}

#endif
