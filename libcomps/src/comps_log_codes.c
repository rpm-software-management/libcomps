#include "comps_log_codes.h"

void __expand(char *str, const char *fmt, char out, ...) {
    va_list list;
    (void) str;
    va_start(list, out);
    if (out)
        vprintf(fmt, list);
    else
        vsprintf(str, fmt, list);
    va_end(list);
}

void expand0(char *str, const char *fmt, char **args, char out) {
    (void)args;
    __expand(str, fmt, out);
}

void expand1(char *str, const char *fmt, char **args, char out) {
    __expand(str, fmt, out, args[0]);
}

void expand2(char *str, const char *fmt, char **args, char out) {
    __expand(str, fmt, out, args[0],
                          args[1]);
}

void expand3(char *str, const char *fmt, char **args, char out) {
    __expand(str, fmt, out, args[0],
                           args[1],
                           args[2]);
}

void expand4(char *str, const char *fmt, char **args, char out) {
    __expand(str, fmt, out, args[0],
                           args[1],
                           args[2],
                           args[3]);
}

void expand5(char *str, const char *fmt, char **args, char out) {
    __expand(str, fmt, out, args[0],
                           args[1],
                           args[2],
                           args[3],
                           args[4]);
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

void expand_out(const char *fmt, char **args, int len) {
    expand(NULL, fmt, args, len, 1);
}

void expand_s(char *str, const char *fmt, char **args, int len) {
    expand(str, fmt, args, len, 0);
}
