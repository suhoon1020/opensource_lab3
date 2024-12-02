#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>  // 추가된 include
#include <sys/wait.h> // 추가된 include

int main(int argc, char *argv[])
{
    pid_t pid;

    if(argc < 2) {  // argc와 argv 사용법 수정
        fprintf(stderr, "올바르게 입력해주세요\n");
        exit(1);
    }

    char *read = argv[1]; // 포인터로 수정
    // argv[1]==*(argv+1)

    pid = fork();

    if (pid == 0)
    {
        // 자식 process
        execl("/bin/sh", "sh", "-c", read, (char *)0); // 실행할 명령어 수정
        perror("execl 실패\n");
        exit(1); // exit 추가
    }
    else if (pid > 0)
    {
        wait((int *)0);
        printf("is completed\n");
        exit(0);
    }
    else
    {
        perror("fork 실패\n");
        exit(1);
    }
}
