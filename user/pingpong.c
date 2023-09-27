//
// Created by Joshua on 2023/9/24.
//
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char* argv[]){
    if(argc>1){
        printf("Usage: pingpong...\n");
        exit(1);
    }

    int p[2];
    pipe(p);
    int pid=fork();
    if(pid==0){
        // 子进程先读数据，在写数据
        char buf[16];
        read(p[0],buf,sizeof(buf));
        close(p[0]);
        fprintf(1,"%d: received %s",getpid(),buf);
        write(p[1],"pong\n",5);
        close(p[1]);
    }else{
        // 父进程先写数据，在读数据
        write(p[1],"ping\n",5);
        close(p[1]);
        char buf[16];
        wait(0); // 避免 race condition： 父进程比子进程先exit
        read(p[0],buf,sizeof(buf));
        close(p[0]);
        fprintf(1,"%d: received %s",getpid(),buf);
    }
    exit(0);

}