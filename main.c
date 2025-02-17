#include <stdio.h>
#include "libc.h"

int main() {
    SetConsoleOutputCP(65001);
    Process* processes = NULL;
    u32 count = GetProcesses(&processes);
    printf("共计: %u\n", count);
    for (u32 i = 0; i < count; i++) {
        printf("%u - %u\n", processes[i].pid, processes[i].ppid);
    }
    return 0;
}