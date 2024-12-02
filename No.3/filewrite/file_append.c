#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>

int main(int argc, char *argv[]){

    int fd;
    char *buf = "this is a test.\n";
    ssize_t cnt; //쓰기 숫자
    int flags = O_WRONLY | O_CREAT | O_APPEND;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ; // 0644

    if(argv<2){
        fprintf(stderr,"오류발생이요");
        exit(1);
    }

    if((fd=open(argv[1],flags,mode))==-1){
        perror("open");
        exit(1);
    }

    cnt=write(fd,buf,strlen(buf));

    printf("write count = %d\n",cnt);

    close(fd);
}