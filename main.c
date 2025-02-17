#include <windows.h>
#include <stdio.h>

#include "libc.h"

int main() {
    byte* icon = NULL;
    LONG size = GetProcessIcon(23784, icon);
    printf("size: %ld\n", size);
    return 0;
}
