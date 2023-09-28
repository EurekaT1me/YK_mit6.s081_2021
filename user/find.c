//
// Created by Joshua on 2023/9/28.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 获取路径的basename
// a.获取最后一个 '/'(slash)之后的文件名，如果slash不存在，则path就是文件名
// b.如果文件名长度不够DIRSIZ，则用空格进行填充
char* fmtname(char *path){
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    // DIRSIZ = 14, 此处保证空格填充后的basename长度一定大于等于14
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}
char *basename(char* path){
    char *p;
    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

void find(char* path,char *need){
    struct stat st;
    struct dirent dt;
    int fd;
    char buf[512];
    char *cur;
    // 路径不存在
    if((fd = open(path, 0)) < 0){
//        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态失败
    if(fstat(fd, &st) < 0){
//        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            if(strcmp(basename(path), basename(need))==0){
                fprintf(1,"%s\n",path);
            }
            break;
        case T_DIR:
            // 目录路径太长，目录长度+目录下某个子文件最小长度（14）> 512Bytes
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            // 定位到path最后一个字符的位置
            strcpy(buf,path);
            cur=buf+ strlen(buf);
            *cur++='/';
            // 读取目录文件下的所有文件
            // 对每个文件构造一个新的路径去递归调用
            while(read(fd,&dt,sizeof(dt))==sizeof(dt)){
                if(dt.inum==0) continue;
                if(strcmp(dt.name,".")==0|| strcmp(dt.name,"..")==0){
                    continue;
                }
                memmove(cur,dt.name,DIRSIZ);
                cur[DIRSIZ]=0;
                find(buf,need);
            }
            break;

    }
    // 一定要关闭文件，否则会因为系统文件描述符不够导致无法完全遍历
    close(fd);
}

int main(int argc,char *argv[]){
    if(argc<3){
        fprintf(2,"Usage: find path need...\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}