#ifndef LIBC_LIBRARY_H
#define LIBC_LIBRARY_H

#include <stdint.h>

// 读取进程信息和虚拟内存
#define PROCESS_READ_ACCESS (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ)
// 写入进程的虚拟内存
#define PROCESS_WRITE_ACCESS (PROCESS_VM_WRITE)
// 终止进程
#define PROCESS_TERMINATE_ACCESS (PROCESS_TERMINATE)
// 创建线程
#define PROCESS_THREAD_ACCESS (PROCESS_CREATE_THREAD)
// 复制进程句柄
#define PROCESS_DUP_HANDLE_ACCESS (PROCESS_DUP_HANDLE)
 // 操作虚拟内存
#define PROCESS_VM_OPERATION_ACCESS (PROCESS_VM_OPERATION)

typedef char* string;
typedef uint32_t u32;

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

#endif
