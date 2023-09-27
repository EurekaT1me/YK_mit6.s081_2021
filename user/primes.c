//
// Created by Joshua on 2023/9/27.
//
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void feed(int fd,int start,int end){
    for(int i=start;i<=end;i++){
        write(fd,&i,sizeof(int ));
    }
}
void csp(int fd[]){
    // fd: file descpritor of old pipe

    close(fd[1]);
    int prime;
    if(read(fd[0],&prime,sizeof(int))==0)return; // leave recusive call
    fprintf(2,"prime %d\n",prime);


    int p[2];
    pipe(p);

    if(fork()==0){
        // recusively call csp()
        close(fd[0]);
        csp(p);
    }else{
        // read from old pipe
        // write to new pipe
        close(p[0]);
        int next;
        while(read(fd[0],&next,sizeof(int))){
            if(next%prime) write(p[1],&next,sizeof (int));
        }
        close(fd[0]);
        close(p[1]);
        wait(0);
    }
}
int main(){
    int p[2];
    pipe(p);
    if(fork()==0){
        // 子进程
        // a.read from pipe
        // b.write to new pipe
        // c.fork grand children (need recusive call)
        csp(p);
    }else{
        // 父进程
        // feed 2,3,4...35
        close(p[0]);
        feed(p[1],2,35);
        close(p[1]);
        wait(0);
    }
    exit(0);

}