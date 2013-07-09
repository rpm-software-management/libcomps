#ifndef COMPS_UTILS_H
#define COMPS_UTILS_H

#include <stdlib.h>
#include <string.h>

char __comps_strcmp(const char *s1, const char *s2);
char __comps_strcmp_v(void *s1, void *s2);
void* __comps_str_clone(void *str);

#endif
