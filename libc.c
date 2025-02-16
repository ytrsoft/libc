#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

#include "libc.h"

void FreeProcesses(struct Process* ps, u32 size) {
    if (ps == NULL) {
        return;
    }
    for (u32 i = 0; i < size; i++) {
        free(ps[i].name);
        free(ps[i].path);
    }
    free(ps);
}

u32 GetProcessBits(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        return 0;
    }
    BOOL isWow64 = FALSE;
    if (!IsWow64Process(hProcess, &isWow64)) {
        return 0;
    }
    if (isWow64) {
        return 32;
    }
    SYSTEM_INFO systemInfo;
    GetNativeSystemInfo(&systemInfo);
    if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
        return 64;
    }
    return 32;
}

char* GetProcessPath(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        return NULL;
    }
    DWORD size = 0;
    QueryFullProcessImageNameA(hProcess, 0, NULL, &size);
    char* path = malloc(size);
    if (path == NULL) {
        CloseHandle(hProcess);
        return NULL;
    }
    if (QueryFullProcessImageNameA(hProcess, 0, path, &size)) {
        CloseHandle(hProcess);
        return path;
    }
    free(path);
    CloseHandle(hProcess);
    return NULL;
}

signed char* GetProcessIcon(const char* path) {
    HICON hIcon = NULL;
    ExtractIconExA(path, 0, &hIcon, NULL, 1);
    if (hIcon == NULL) {
        return NULL;
    }
    ICONINFO iconInfo;
    if (!GetIconInfo(hIcon, &iconInfo)) {
        DestroyIcon(hIcon);
        return NULL;
    }
    BITMAP bmp;
    if (!GetObjectA(iconInfo.hbmColor, sizeof(BITMAP), &bmp)) {
        DeleteObject(iconInfo.hbmColor);
        DeleteObject(iconInfo.hbmMask);
        DestroyIcon(hIcon);
        return NULL;
    }
    DWORD dataSize = bmp.bmWidth * bmp.bmHeight * (bmp.bmBitsPixel / 8);
    signed char* iconData = malloc(dataSize);
    if (iconData == NULL) {
        DeleteObject(iconInfo.hbmColor);
        DeleteObject(iconInfo.hbmMask);
        DestroyIcon(hIcon);
        return NULL;
    }
    BITMAPINFOHEADER bi = {0};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = bmp.bmBitsPixel;
    bi.biCompression = BI_RGB;
    HDC hdc = GetDC(NULL);
    if (!GetDIBits(hdc, iconInfo.hbmColor, 0, bmp.bmHeight, iconData, (BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
        free(iconData);
        DeleteObject(iconInfo.hbmColor);
        DeleteObject(iconInfo.hbmMask);
        DestroyIcon(hIcon);
        ReleaseDC(NULL, hdc);
        return NULL;
    }
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
    DestroyIcon(hIcon);
    ReleaseDC(NULL, hdc);
    return iconData;
}

u32 GetProcesses(struct Process** ps) {
    u32 pos = 0;
    u32 capacity = 10;
    PROCESSENTRY32 pe32;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    *ps = (struct Process*)malloc(capacity * sizeof(struct Process));
    if (*ps == NULL) {
        CloseHandle(hProcessSnap);
        return 0;
    }
    if (!Process32First(hProcessSnap, &pe32)) {
        free(*ps);
        CloseHandle(hProcessSnap);
        return 0;
    }
    do {
        if (pe32.th32ProcessID == 0 || pe32.th32ParentProcessID == 0 || strlen(pe32.szExeFile) == 0) {
            continue;
        }
        if (pos >= capacity) {
            capacity *= 2;
            struct Process* t = realloc(*ps, capacity * sizeof(struct Process));
            if (t == NULL) {
                for (u32 i = 0; i < pos; i++) {
                    free((*ps)[i].name);
                }
                free(*ps);
                CloseHandle(hProcessSnap);
                return 0;
            }
            *ps = t;
        }
        (*ps)[pos].pid = pe32.th32ProcessID;
        (*ps)[pos].ppid = pe32.th32ParentProcessID;
        (*ps)[pos].bits = GetProcessBits((*ps)[pos].pid);
        if ((*ps)[pos].bits == 0) {
            continue;
        }
        (*ps)[pos].name = _strdup(pe32.szExeFile);
        if ((*ps)[pos].name == NULL) {
            for (u32 i = 0; i < pos; i++) {
                free((*ps)[i].name);
            }
            free(*ps);
            CloseHandle(hProcessSnap);
            return 0;
        }
        (*ps)[pos].path = GetProcessPath((*ps)[pos].pid);
        (*ps)[pos].si = GetProcessIcon((*ps)[pos].path);
        (*ps)[pos].si = NULL;
        pos++;
    } while (Process32Next(hProcessSnap, &pe32));
    FreeProcesses(*ps, pos);
    CloseHandle(hProcessSnap);
    return pos;
}