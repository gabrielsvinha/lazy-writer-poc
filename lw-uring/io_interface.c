#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "io_interface.h"
#include "liburing.h"

#define QD 64

#define BUF_SIZE 1024

struct io_uring ring;
int r_ops;

int setup_context()
{
    int ret;
    ret = io_uring_queue_init(QD, &ring, 0);
    if (ret < 0)
    {
        fprintf(stderr, "queue_init: %s\n", strerror(-ret));
        return -1;
    }
    r_ops = 0;
    return 0;
}

int read_file(char *fd)
{
    int outfd;
    int offset = 0;
    struct io_uring_sqe *sqe;
    struct io_uring_cqe *cqe;
    struct iovec iov;
    char buffer[BUF_SIZE];
    iov.iov_base = &buffer;
    iov.iov_len = BUF_SIZE;
    int ret;
    outfd = open(fd, O_RDONLY, 0644);

    // wait the read + fsync result.
    for (int i = 0; i < r_ops; i++)
    {
        ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0)
        {
            printf("wait cqe: %s\n", strerror(ret));
            return 1;
        }
        io_uring_cqe_seen(&ring, cqe);
    }

    // async write
    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_readv(sqe, outfd, &iov, 1, offset);
    io_uring_sqe_set_data(sqe, &iov);
    io_uring_submit(&ring);

    ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0)
    {
        printf("wait cqe: %s\n", strerror(ret));
        return 1;
    }
    io_uring_cqe_seen(&ring, cqe);
    return 0;
}

int write_file(char *out, int size)
{
    int outfd;
    char buffer[BUF_SIZE];
    outfd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int limit_size = BUF_SIZE * 1024;
    int offset = 0;
    int ret;

    struct io_uring_sqe *sqe;
    struct io_uring_cqe *cqe;
    struct iovec iov;
    iov.iov_base = &buffer;
    iov.iov_len = BUF_SIZE;

    // async write
    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqe, outfd, &iov, 1, offset);
    io_uring_sqe_set_data(sqe, &iov);
    io_uring_submit(&ring);

    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_fsync(sqe, outfd, IORING_FSYNC_DATASYNC);
    io_uring_submit(&ring);

    r_ops = r_ops + 2;
    // advance the offset.
    offset += BUF_SIZE;
    close(outfd);
    return 0;
}