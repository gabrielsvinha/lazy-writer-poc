#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "io_interface.h"
#include <aio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define QD 64

#define BUF_SIZE 1024

struct aiocb reqs[BUF_SIZE];
int offs = 0;
int r_ops;

int setup_context()
{
    return 0;
}

int read_file(char *fd)
{
    unsigned char check[BUF_SIZE];
    struct aiocb aiocb;
    int i, ret;
    int err;

    fd = open(fd, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        return 1;
    }

    for (int j = 0; j < offs; j++)
    {
        printf("esperando %d\n", j);
        while ((err = aio_error(&reqs[j])) == EINPROGRESS)
            ;
        printf("esperando done %d\n", j);
    }

    aiocb.aio_fildes = fd;
    aiocb.aio_buf = check;
    aiocb.aio_nbytes = BUF_SIZE;
    aiocb.aio_lio_opcode = LIO_READ;
    if (aio_read(&aiocb) == -1)
    {
        printf("esperou\n");
        return 2;
    }
    while ((err = aio_error(&aiocb)) == EINPROGRESS)
        ;
    printf("esperando done\n");
    ret = aio_return(&aiocb);
    printf("aaaa %s\n", check);
    return 0;
}

int write_file(char *out, int size)
{
    int fd;
    struct aiocb aiocb;
    char buf[BUF_SIZE];

    fd = open(out, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    memset(buf, 0xaa, BUF_SIZE);
    memset(&aiocb, 0, sizeof(struct aiocb));

    aiocb.aio_fildes = fd;
    aiocb.aio_buf = buf;
    aiocb.aio_nbytes = BUF_SIZE;
    if (aio_write(&aiocb) == -1)
    {
        close(fd);
        return 1;
    }
    printf("escreveu bom\n");
    close(fd);
    reqs[offs] = aiocb;
    offs++;
    return 0;
}