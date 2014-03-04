#include "comps_log_codes.h"

inline void __expand(char *str, const char *fmt, char out, ...) {
    va_list list;
    (void) str;
    va_start(list, out);
    if (out)
        vprintf(fmt, list);
    else
        vsprintf(str, fmt, list);
    va_end(list);
}

void expand(char *str, const char *fmt, char **args, int len, int out) {
    switch(len) {
        case 0:
            expand0(str, fmt, args, out);
        break;
        case 1:
            expand1(str, fmt, args, out);
        break;
        case 2:
            expand2(str, fmt, args, out);
        break;
        case 3:
            expand3(str, fmt, args, out);
        break;
        case 4:
            expand4(str, fmt, args, out);
        break;
        case 5:
            expand5(str, fmt, args, out);
        break;
        default:
        break;
    }
}
