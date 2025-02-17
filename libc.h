#ifndef LIBC_LIBRARY_H
#define LIBC_LIBRARY_H

#include <stdint.h>

// 读取权限
#define PROCESS_READ_ACCESS (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ)
// 写入权限
#define PROCESS_WRITE_ACCESS (PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE)

#define BUFFER_SIZE 1024

typedef char* string;
typedef uint32_t u32;
typedef unsigned char byte;
typedef uintptr_t hex;


typedef struct tagScan {
    // 地址
    hex address;
    // 当前值
    int current;
    // 先前值
    int previous;
    // 初始值
    int first;
} Scan;

int GetProcessIds(u32** ids);
string GetProcessName(u32 pid);
string GetProcessPath(u32 pid);

int ReadMemory(u32 pid, long address, int* value);
int WriteMemory(u32 pid, long address, int value);

#endif
