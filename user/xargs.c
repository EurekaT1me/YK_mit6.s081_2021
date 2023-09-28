//
// Created by Joshua on 2023/9/28.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFSIZ 512

void xargs(int argc, char *argv[]) {
    // a.从标准输入读取数据
    // b. 如果命令行使用了｜（管道）命令，其实现是在子进程中关闭标准输入，然后复制管道读端，这样0号文件描述符就是管道读端.
    //    注意：find写端写完一条信息后会加上一个'\n'，考虑到是多进程并发，读端需要不断循环读直到写端关闭
    // c. 根据题目的意思，buf需要作为command的最后一个参数
    // d. buf中可能有多行，需要根据'\n'来区分并对每行运行exec
    char buf[BUFSIZ];
    int idx = 0;
    int n = read(0, buf, BUFSIZ);
    //  不断读取直到管道写端关闭
    while (n > 0) {
        idx += n;
        n = read(0, &buf[idx], BUFSIZ);
    }
    // 构造命令和新参数
    char *command = argv[1];
    char *newArgv[argc + 1];
    for (int i = 1; i < argc; ++i) {
        newArgv[i - 1] = argv[i];
    }
    newArgv[argc] = 0;
    // 按'\n'划分，并执行
    for (int i = 0, start = 0; i < BUFSIZ; i++) {
        if (buf[i] == '\n') {
            buf[i] = 0;
            newArgv[argc - 1] = buf + start;
            start = i + 1;
            if (fork() == 0) {
                exec(command, newArgv);
                exit(1);
            } else {
                wait(0);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    xargs(argc, argv);
    exit(0);
}