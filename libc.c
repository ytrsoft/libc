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
            u32 cap = capacity * 2;
            Process* t = realloc(*processes, cap * sizeof(Process));
            if (t == NULL) {
                free(*processes);
                CloseHandle(hProcessSnap);
                return p;
            }
            *processes = t;
            capacity = cap;
        }
        (*processes)[p].pid = pe32.th32ProcessID;
        (*processes)[p].ppid = pe32.th32ParentProcessID;
        (*processes)[p].name = strdup(pe32.szExeFile);
        (*processes)[p].tc = pe32.cntThreads;
        p++;
    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
    return p;
}

typedef unsigned char byte;

LONG GetProcessIcon(u32 pid, byte* icon) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);
    if (!Module32First(hSnapshot, &me32)) {
        CloseHandle(hSnapshot);
        return 0;
    }

    char szFilePath[MAX_PATH];
    strcpy(szFilePath, me32.szExePath);

    SHFILEINFO shFileInfo;
    if (SHGetFileInfo(szFilePath, 0, &shFileInfo, sizeof(shFileInfo), SHGFI_ICON | SHGFI_SMALLICON)) {
        HICON hIcon = shFileInfo.hIcon;

        ICONINFO iconInfo;
        if (GetIconInfo(hIcon, &iconInfo)) {
            BITMAP bmp;
            GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmp);
            LONG width = bmp.bmWidth;
            LONG height = bmp.bmHeight;
            LONG bitsPerPixel = bmp.bmBitsPixel;
            LONG bytesPerPixel = bitsPerPixel / 8;
            LONG rowSize = (width * bytesPerPixel + 3) & ~3;
            LONG imageSize = rowSize * height;

            // Ensure icon is not null and allocated successfully
            if (icon == NULL) {
                CloseHandle(hSnapshot);
                return 0;
            }

            icon = malloc(imageSize);
            if (icon) {
                GetBitmapBits(iconInfo.hbmColor, imageSize, icon);
                DeleteObject(iconInfo.hbmColor);
                DeleteObject(iconInfo.hbmMask);
                DestroyIcon(hIcon);
                CloseHandle(hSnapshot);
                return imageSize;
            }
            DeleteObject(iconInfo.hbmColor);
            DeleteObject(iconInfo.hbmMask);
        }
        DestroyIcon(hIcon);
    }

    CloseHandle(hSnapshot);
    return 0;
}
