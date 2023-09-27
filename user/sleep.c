//
// Created by Joshua on 2023/9/24.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char *argv[]){
    if(argc<2||argc>2){
        printf("Usage: sleep [the number of clock ticks]...\n");
        exit(1);
    }

    // 调用sys_sleep,单位clock ticks
    sleep(atoi(argv[1]));
    exit(0);
}