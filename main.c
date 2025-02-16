#include <stdio.h>

#include "libc.h"

int main() {
    struct Process* processes;
    u32 size = GetProcesses(&processes);
    for (int i = 0; i < size; ++i) {
        printf("%s - %d\n", processes[i].name, processes[i].bits);
    }
    return 0;
}