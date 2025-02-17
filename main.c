#include <stdio.h>
#include <windows.h>
#include "libc.h"

int main() {
    SetConsoleOutputCP(65001);
    Process* p = NULL;
    u32 count = GetProcesses(&p);
    printf("共计: %u\n", count);
    for (u32 i = 0; i < count; i++) {
        printf("%s - %u - %u - %u\n", p[i].name, p[i].pid, p[i].ppid, p[i].tc);
    }
    return 0;
}