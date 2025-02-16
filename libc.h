#ifndef LIBC_LIBRARY_H
#define LIBC_LIBRARY_H

#include <stdint.h>
#include <windows.h>

typedef uint32_t u32;
typedef uint64_t u64;

/**
 * 进程
 */
struct Process {
    // id
    DWORD pid;
    // 父id
    DWORD ppid;
    // 位数 32 / 64
    u32 bits;
    // 名称
    char* name;
    // 路径
    char* path;
    // 启动时间
    u64 st;
    // 启动图标
    signed char* si;
};

u32 GetProcesses(struct Process** ps);

#endif
