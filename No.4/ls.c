#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

void read_dir(char *target)
{

    DIR *pdir;
    struct dirent *pde;
    int i = 0;

    char *dir_queue[1024];
    int dir_i = 0;

    printf("%s/ :\n", target);

    if ((pdir = opendir(target)) == NULL)
    {
        perror("opendir");
        exit(1);
    }

    while ((pde = readdir(pdir)) != NULL)
    {

        // 현재 디렉토리, 상위 디렉토리, 숨김 파일 읽지 않기
        if ((pde->d_name)[0] == '.')
            continue;

        // 디렉토리일 경우
        if (pde->d_type == DT_DIR)
        {
            // 재귀 실행할 큐에 넣음
            char *new_target;
            new_target = (char *)calloc(strlen(target) + strlen(pde->d_name) + 2, sizeof(char));
            strcpy(new_target, target);
            strcat(new_target, "/");
            strcat(new_target, pde->d_name);
            dir_queue[dir_i++] = new_target;
        }
        printf("%-20s ", pde->d_name);
        if (++i % 5 == 0)
        {
            printf("\n");
        }
    }

    printf("\n");
    for (i = 0; i < dir_i; i++)
    {
        printf("\n");
        read_dir(dir_queue[i]);
        free(dir_queue[i]); // 메모리 해제
    }

    closedir(pdir);
}

int main(int argc, char *argv[])
{

    char *target;
    struct stat buf;

    // 1. 사용자에게 탐색할 디렉토리 받아오기
    // 	1) 입력됐으면 해당 디렉토리
    // 	2) 입력없으면 현재 디렉토리
    if (argc < 2)
    {
        target = ".";
    }
    else
    {
        target = argv[1];
    }

    // 2. 사용자의 입력이 디렉토리인지 검사
    if (lstat(target, &buf) < 0)
    {
        perror("lstat");
        exit(1);
    }
    else
    {
        // 3. 디렉토리가 맞으면 파일 하나씩 읽음
        // 4. 읽은 파일이 디렉토리이면 내부로 들어가서 읽음
        if (S_ISDIR(buf.st_mode))
        {
            read_dir(target);
        }
    }

    return 0;
}