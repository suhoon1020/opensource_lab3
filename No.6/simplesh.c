/* simplesh.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    char buf[256];
    char *argv[50];
    int narg;
    pid_t pid;
    while (1)
    {
        printf("shell> ");
        scanf("%s",buf);
        clearerr(stdin);
        narg = getargs(buf, argv);
        pid = fork();
        if (pid == 0)
            execvp(argv[0], argv);
        else if (pid > 0)
            wait((int *)0);
        else
            perror("fork failed");
    }
}

int getargs(char *cmd, char **argv)
{
    int narg = 0;
    while (*cmd) //cmd 문자열의 끝까지 반복
    {
        if (*cmd == ' ' || *cmd == '\t') //공백 또는 탭이면
            *cmd++ = '\0'; //해당 문자를 널로 바꿔서 문자열 분리
        else
        {
            argv[narg++] = cmd++;
            while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t')
                cmd++;
        }
    }
    argv[narg] = NULL;
    return narg;
}