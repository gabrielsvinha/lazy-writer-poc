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
    return 0;
}

int write_file(char *out, int size)
{
    
    return 0;
}