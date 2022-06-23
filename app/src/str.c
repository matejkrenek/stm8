#include "str.h"

String_Module str = {
    .prepend = String_Prepend,
    .length = String_Length,
};

void String_Prepend(char *s, const char *t)
{
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

size_t String_Length(const char *t)
{
    return strlen(t);
}
