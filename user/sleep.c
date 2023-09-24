//
// Created by 张怿恺 on 2023/9/24.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char *argv[]){
    if(argc<2||argc>2){
        printf("Usage: sleep seconds...\n");
        exit(1);
    }


    sleep(atoi(argv[1]));
    exit(0);
}