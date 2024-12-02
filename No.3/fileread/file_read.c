#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 512

int main()
{
    char buffer[BUFSIZE];
    int filedes;
    ssize_t nread;
    long total=0;

    if((filedes=open("anotherfile",O_RDONLY))==-1){
        printf("오류!\n");
        exit(1);
    }

    while((nread=read(filedes,buffer,BUFSIZE))>0){
        total+=nread;
    }

    printf("전체 문자 수는 %ld\n",total);
    exit(0);
}