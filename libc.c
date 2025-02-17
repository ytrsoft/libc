#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

#include "libc.h"

int GetProcessIds(u32** ids) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return -1;
    }
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return -1;
    }
    u32 capacity = 10;
    int count = 0;
    *ids = (u32*)malloc(capacity * sizeof(u32));
    if (*ids == NULL) {
        CloseHandle(hProcessSnap);
        return -1;
    }
    do {
        if (count >= capacity) {
            capacity *= 2;
            u32* temp = realloc(*ids, capacity * sizeof(u32));
            if (temp == NULL) {
                free(*ids);
                CloseHandle(hProcessSnap);
                return -1;
            }
            *ids = temp;
        }
        (*ids)[count++] = pe32.th32ProcessID;
    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
   return count;
}