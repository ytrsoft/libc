#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

#include "libc.h"

u32 GetProcesses(Process** processes) {
    u32 p = 0;
    u32 capacity = 10;
    PROCESSENTRY32 pe32;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return p;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    *processes = (Process*)malloc(capacity * sizeof(Process));
    if (*processes == NULL) {
        CloseHandle(hProcessSnap);
        return p;
    }
    do {
        if (pe32.th32ProcessID == 0 || pe32.th32ParentProcessID == 0) {
            continue;
        }
        if (p >= capacity) {
            u32 new_capacity = capacity * 2;
            Process* t = realloc(*processes, new_capacity * sizeof(Process));
            if (t == NULL) {
                free(*processes);
                CloseHandle(hProcessSnap);
                return p;
            }
            *processes = t;
            capacity = new_capacity;
        }
        (*processes)[p].pid = pe32.th32ProcessID;
        (*processes)[p].ppid = pe32.th32ParentProcessID;
        p++;
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return p;
}
