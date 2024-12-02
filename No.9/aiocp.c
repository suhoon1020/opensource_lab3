/* aiocp.c */
/* POSIX asynchronous io example */
#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFSIZE 32
void setup_aiocb(struct aiocb *aio, int fd, off_t offset, int bytes, char *buf)
{
    /* Initialize the necessary fields in the aiocb */
    aio->aio_fildes = fd;
    aio->aio_nbytes = bytes;
    aio->aio_offset = offset;
    aio->aio_buf = buf;
    aio->aio_sigevent.sigev_notify = SIGEV_NONE;
}
int main(int argc, char **argv)
{
    int r_fd, w_fd, ret, nread, quit = 0;
    struct aiocb r_aiocb, w_aiocb;
    char r_buf[BUFSIZE], w_buf[BUFSIZE];
    struct aiocb *cblist[2];
    if (argc < 3)
    {
        fprintf(stderr, "aiocp source dest\n");
        exit(1);
    }
    r_fd = open(argv[1], O_RDONLY, 0444);
    if (r_fd < 0)
        perror("open");
    w_fd = open(argv[2], O_CREAT | O_WRONLY, 0644);
    if (w_fd < 0)
        perror("open");
    /* Zero out the aiocb structure (recommended) */
    bzero((char *)&r_aiocb, sizeof(struct aiocb));
    bzero((char *)&w_aiocb, sizeof(struct aiocb));
    setup_aiocb(&r_aiocb, r_fd, 0, BUFSIZE, r_buf);
    setup_aiocb(&w_aiocb, w_fd, 0, BUFSIZE, w_buf);
    /* Clear the list. */
    bzero((char *)cblist, sizeof(cblist));
    /* Load one or more references into the list */
    cblist[0] = &r_aiocb;
    /* below block aio */
    ret = aio_read(&r_aiocb);
    if (ret < 0)
        perror("aio_read");
    /* below block aio */
    while (aio_error(&r_aiocb) == EINPROGRESS)
    {
        ret = aio_suspend(cblist, 1, NULL);
        printf("waiting...\n");
    }
    if ((nread = aio_return(&r_aiocb)) > 0)
    {
        /* got ret bytes on the read */
        printf("nread = %d\n", nread);
        if (nread == BUFSIZE)
            r_aiocb.aio_offset += nread;
        else
            printf("source file less than %d\n", BUFSIZE);
    }
    while (1)
    {
        /* copy read buffer to write buffer */
        memcpy(w_buf, r_buf, nread);
        w_aiocb.aio_nbytes = nread;
        /* below block aio */
        ret = aio_write(&w_aiocb);
        if (ret < 0)
            perror("aio_write");
        r_aiocb.aio_nbytes = BUFSIZE;
        ret = aio_read(&r_aiocb);
        if (ret < 0)
            perror("aio_read");
        /* below block aio */
        while (aio_error(&r_aiocb) == EINPROGRESS ||
               aio_error(&w_aiocb) == EINPROGRESS)
        {
            cblist[0] = &r_aiocb;
            cblist[1] = &w_aiocb;
            ret = aio_suspend(cblist, 2, NULL);
            printf("waiting block read&write...\n");
        }
        if (quit)
            break;
        w_aiocb.aio_offset += aio_return(&w_aiocb);
        if ((nread = aio_return(&r_aiocb)) > 0)
        {
            if (nread == BUFSIZE)
                r_aiocb.aio_offset += BUFSIZE;
            else
                quit = 1; /* last block */
        }
    }
}
