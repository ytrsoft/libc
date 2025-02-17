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

string GetProcessName(u32 pid) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return NULL;
    }
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (pe32.th32ProcessID == pid) {
                string name = malloc(MAX_PATH * sizeof(char));
                if (name != NULL) {
                    strcpy(name, pe32.szExeFile);
                }
                CloseHandle(hProcessSnap);
                return name;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }
    CloseHandle(hProcessSnap);
    return NULL;
}

string GetProcessPath(u32 pid) {
    HANDLE hProcess = OpenProcess(PROCESS_READ_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        return NULL;
    }
    string path = malloc(MAX_PATH * sizeof(char));
    if (path == NULL) {
        CloseHandle(hProcess);
        return NULL;
    }
    DWORD pathLength = MAX_PATH;
    if (QueryFullProcessImageNameA(hProcess, 0, path, &pathLength) == 0) {
        free(path);
        CloseHandle(hProcess);
        return NULL;
    }
    CloseHandle(hProcess);
    return path;
}

int ReadMemory(u32 pid, long address, int* value) {
    HANDLE hProcess = OpenProcess(PROCESS_READ_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        return FALSE;
    }
    int status = ReadProcessMemory(hProcess, (LPCVOID)address, value, sizeof(int), NULL);
    CloseHandle(hProcess);
    return status;
}

int WriteMemory(u32 pid, long address, int value) {
    HANDLE hProcess = OpenProcess(PROCESS_WRITE_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        return FALSE;
    }
    int status = WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(int), NULL);
    CloseHandle(hProcess);
    return status;
}