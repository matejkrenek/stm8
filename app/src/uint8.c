#include "uint8.h"
#include "str.h"

UINT8_Module uint8 = {
    .toString = UINT8_ToString,
};

void UINT8_ToString(char *to, uint8_t from)
{
    sprintf(to, "%u", from);

    if (from < 10)
    {
        str.prepend(to, "0");
    }
}