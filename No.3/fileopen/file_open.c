#include <stdlib.h>
#include <fcntl.h>

#define PERMS 0644

char *workfile="junk";

int main(){
    int filedes;

    if((filedes=open(workfile,O_RDWR|O_CREAT,PERMS)==-1)){
        printf("%s 파일 열기 불가능",workfile);
        exit(1);
    }

    exit(0);
}