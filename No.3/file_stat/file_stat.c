#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc , char *argv[]){
    struct stat buf;
    char *msg;

    if(argc<2){
        fprintf(stderr,"에러\n");
        exit(1);
    }

    if(lstat(argv[1],&buf)<0){
        perror("stat");
        exit(1);
    }

    if(S_ISREG(buf.st_mode)) msg = "regular file";
    else if(S_ISDIR(buf.st_mode)) msg = "directory";
    else if(S_ISCHR(buf.st_mode)) msg = "charter special file";
    else if(S_ISBLK(buf.st_mode)) msg = "pipe of fifo";
    else if(S_ISLNK(buf.st_mode)) msg ="symbolic link";

    printf("%s is %s \n",argv[1],msg);
}