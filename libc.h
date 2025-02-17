#ifndef LIBC_LIBRARY_H
#define LIBC_LIBRARY_H

#include <stdint.h>

// 读取权限
#define PROCESS_READ_ACCESS (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ)
// 写入权限
#define PROCESS_WRITE_ACCESS (PROCESS_VM_WRITE)
// 获取图标
#define ICON_FLAGS (SHGFI_ICON | SHGFI_SMALLICON)

typedef char* string;
typedef uint32_t u32;
typedef unsigned char byte;

typedef struct tagProcess {
    // id
    u32 pid;
    // 父id
    u32 ppid;
    // 名称
    string name;
    // 线程数量
    u32 tc;
} Process;

u32 GetProcesses(Process** processes);

LONG GetProcessIcon(u32 pid, byte* icon);

#endif
