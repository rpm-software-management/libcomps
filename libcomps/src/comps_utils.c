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
inline char __comps_strcmp(const char *s1, const char *s2) {
    if (s1 == NULL && s1 == NULL) return 1;
    else if ((s1 == NULL && s2 != NULL) || (s1 != NULL && s2 == NULL))
        return 0;
    return (strcmp(s1, s2) == 0);
}

inline char __comps_strcmp_v(void *s1, void *s2) {
    __comps_strcmp((const char*)s1, (const char*)s2);
}

