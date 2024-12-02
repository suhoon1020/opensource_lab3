#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>

#define SEMNAME_WRITE "/write_sem" // 쓰기 세마포어
#define SEMNAME_READ "/read_sem"   // 읽기 세마포어

#define SHMNAME "/my_shm" // 공유메모리 이름
#define SHMSIZE 1024

#define BUFSIZE 1024 // 파읽을 읽을 버퍼 사이즈

void p(sem_t *semd); // 세마포어 획득
void v(sem_t *semd); // 세마포어 해제

void writeShm(char *source, sem_t *write_semd, sem_t *read_semd); // 자식 프로세스 : 파일 읽어서 공유 메모리에 쓰기
void readShm(char *dest, sem_t *write_semd, sem_t *read_semd);    // 부모 프로세스 : 공유 메모리 읽어서 파일에 쓰기

int main(int argc, char *argv[])
{

    sem_t *write_semd;
    sem_t *read_semd;
    pid_t pid;

    if (argc < 3)
    {
        fprintf(stderr, "복사할 파일과 저장할 파일 입력하시오.\n");
        exit(1);
    }

    // 세마포어 열기 (없으면 생성)
    if ((write_semd = sem_open(SEMNAME_WRITE, O_CREAT, 0600, 1)) == SEM_FAILED)
    {
        perror("sem_open failed");
        exit(1);
    }
    if ((read_semd = sem_open(SEMNAME_READ, O_CREAT, 0600, 0)) == SEM_FAILED)
    {
        perror("sem_open failed");
        exit(1);
    }

    pid = fork();

    // 자식 프로세스(파일 읽기용)
    if (pid == 0)
    {
        writeShm(argv[1], write_semd, read_semd);
    }
    // 부모 프로세스(파일 쓰기용)
    else if (pid > 0)
    {
        readShm(argv[2], write_semd, read_semd);
        wait(NULL);
        if ((sem_close(write_semd) == -1) || (sem_close(read_semd) == -1))
        {
            perror("sem_close failed");
            exit(1);
        }
        if ((sem_unlink(SEMNAME_WRITE) == -1))
        {
            perror("sem_unlink failed for write semaphore");
            printf("Error number: %d\n", errno);
        }
        if ((sem_unlink(SEMNAME_READ) == -1))
        {
            perror("sem_unlink failed for read semaphore");
            printf("Error number: %d\n", errno);
        }
    }
    else
    {
        perror("fork failed");
        exit(1);
    }
}
void p(sem_t *semd)
{
    int ret;
    // 세마포어를 획득
    if ((ret = sem_trywait(semd)) != 0 && errno == EAGAIN)
        sem_wait(semd); // 세마포어가 사용 중이면 대기
    else if (ret != 0)
    {
        perror("sem_trywait failed");
        exit(1);
    }
}

void v(sem_t *semd)
{
    // 세마포어를 해제
    if (sem_post(semd) != 0)
    {
        perror("sem_post failed");
        exit(1);
    }
}

void writeShm(char *source, sem_t *write_semd, sem_t *read_semd)
{

    p(write_semd); // 세마포어를 획득
    int shmd;      // 공유 메모리 디스크립터
    void *shmaddr; // 공유 메모리 주소
                   //
    char buffer[BUFSIZE];
    int fd;
    ssize_t nread;
    long total = 0;

    // 파일에서 읽기
    if ((fd = open(source, O_RDONLY)) == -1)
    {
        perror("open failed");
        exit(1);
    }

    // 공유 메모리 열기 (없으면 생성)
    if ((shmd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0666)) == -1)
    {
        perror("shm_open failed");
        exit(1);
    }

    // 공유 메모리 크기 설정
    if (ftruncate(shmd, SHMSIZE) != 0)
    {
        perror("ftruncate failed");
        exit(1);
    }

    // 공유 메모리를 프로세스 주소 공간에 맵핑
    if ((shmaddr = mmap(0, SHMSIZE, PROT_WRITE, MAP_SHARED, shmd, 0)) == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }

    // 공유 메모리 잠금
    if (mlock(shmaddr, SHMSIZE) != 0)
    {
        perror("mlock failed");
        exit(1);
    }

    // source 파일 읽어서 공유 메모리에 문자열 쓰기
    // 파일에서 읽어서 공유 메모리에 쓰기
    while ((nread = read(fd, buffer, BUFSIZE)) > 0)
    {
        memcpy((char *)shmaddr + total, buffer, nread);
        total += nread;
    }

    // 공유 메모리에 데이터 크기 저장
    memcpy((char *)shmaddr + total, "\0", 1);

    v(read_semd); // 세마포어를 해제

    close(fd);
    // 공유 메모리 언맵
    if (munmap(shmaddr, SHMSIZE) == -1)
    {
        perror("munmap failed");
        exit(1);
    }
}

void readShm(char *dest, sem_t *write_semd, sem_t *read_semd)
{
    p(read_semd);  // 세마포어를 획득
    int shmd;      // 공유 메모리 디스크립터
    void *shmaddr; // 공유 메모리 주소

    // 읽을 파일 관련
    char buffer[BUFSIZE];
    int fd;
    ssize_t nread = 0;
    long total = 0;

    // 공유 메모리 열기
    if ((shmd = shm_open(SHMNAME, O_RDWR, 0666)) == -1)
    {
        perror("shm_open failed");
        exit(1);
    }

    // 공유 메모리를 프로세스 주소 공간에 맵핑
    if ((shmaddr = mmap(0, SHMSIZE, PROT_READ, MAP_SHARED, shmd, 0)) == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }

    // 공유 메모리 잠금
    if (mlock(shmaddr, SHMSIZE) != 0)
    {
        perror("mlock failed");
        exit(1);
    }

    // 공유 메모리에서 데이터 읽어서 dest 파일에 쓰기
    if ((fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
    {
        perror("open failed");
        exit(1);
    }

    while (((char *)shmaddr)[total] != '\0')
    {
        buffer[nread++] = ((char *)shmaddr)[total++];
        if (nread == BUFSIZE || ((char *)shmaddr)[total] == '\0')
        {
            write(fd, buffer, nread);
            nread = 0;
        }
    }
    if (nread > 0)
    {
        write(fd, buffer, nread);
    }
    // 공유 메모리 언맵
    if (munmap(shmaddr, SHMSIZE) == -1)
    {
        perror("munmap failed");
        exit(1);
    }

    v(write_semd); // 세마포어를 해제
    close(shmd);   // 공유 메모리 닫기
    close(fd);

    // 공유 메모리 이름 해제
    if (shm_unlink(SHMNAME) == -1)
    {
        perror("shm_unlink failed");
        exit(1);
    }
}