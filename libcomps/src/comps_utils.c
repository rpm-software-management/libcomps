#include "comps_utils.h"

void* __comps_str_clone(void *str) {
    char *ret;
    if (str == NULL)
        return NULL;
    ret = malloc(sizeof(char) * (strlen((char*)str)+1));
    if (ret==NULL) return NULL;
    memcpy(ret, str, sizeof(char) * (strlen((char*)str)+1));
    return ret;
}
inline char __comps_strcmp(void *s1, void *s2) {
    return (strcmp((const char*)s1, (const char*)s2) == 0);
}

