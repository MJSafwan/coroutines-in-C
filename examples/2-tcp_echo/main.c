#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include "routine.h"
#include "scheduler.h"

#define HOST_ADDR "127.0.0.1"
#define HOST_PORT 8080
#define BL 10

int fd;

__attribute__((format(printf, 1, 2)))
[[noreturn]] void fatal(const char *fmt, ...) {
    va_list l;
    va_start(l, fmt);
    fprintf(stderr, "[FATAL] ");
    vfprintf(stderr, fmt, l);
    va_end(l);
    exit(1);
}

typedef struct {
    int rfd;
    size_t len;
    char *buff;
    size_t bytes_read;
} read_t;

void async_read(void *args) {
    read_t *data = (read_t *)args;

    struct pollfd p[1];
    p[0].fd = data->rfd;
    p[0].events = POLLIN | POLLHUP;

    for (;;) {
        int poll_ret = poll(p, 1, 0);
        if (poll_ret == 1) {
            if ((p[0].revents & POLLHUP) != 0) {
                data->bytes_read = -1;
                routine_finish();
            }
            break;
        }
        routine_sleep(10);
    }
    data->bytes_read = read(data->rfd, data->buff, data->len);
    routine_finish();
}

void async_write(void *args) {
    read_t *data = (read_t *)args;

    struct pollfd p[1];
    p[0].fd = data->rfd;
    p[0].events = POLLOUT | POLLHUP;

    for (;;) {
        int poll_ret = poll(p, 1, 0);
        if (poll_ret == 1) {
            if ((p[0].revents & POLLHUP) != 0) {
                routine_finish();
            }
            break;
        }
        routine_sleep(10);
    }
    write(data->rfd, data->buff, data->bytes_read);
    routine_finish();
}

void handle_connection(int *rfd_ptr) {
    int rfd = *rfd_ptr;
    char buff[1024];

    read_t d = {0};
    d.rfd = rfd;
    d.len = 1024;
    d.buff = buff;

    routine_t r = routine(scheduler_get_context(), async_read, &d);
    if (d.bytes_read == -1) {
        close(rfd);
        free(rfd_ptr);
        routine_finish();
    }
    routine_t w = routine(scheduler_get_context(), async_write, &d);
    routine_await(r);
    routine_await(w);

    close(rfd);
    free(rfd_ptr);
    routine_finish();
}

void listening_loop(void *a) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    for (;;) {
        struct sockaddr_in rsockaddr = {0};
        socklen_t rsocklen;
        int rfd = accept(fd, (struct sockaddr *)&rsockaddr, &rsocklen);
        if (rfd == -1) {
            if (errno == EWOULDBLOCK) {
                routine_sleep(10);
            } else {
                close(fd);
                fatal("%s\n", strerror(errno));
            }
        } else {
            int *rfd_mallocd = malloc(sizeof(int));
            *rfd_mallocd = rfd;
            routine_t r = routine(scheduler_get_context(), handle_connection, rfd_mallocd);
            scheduler_append(handle_connection, rfd_mallocd);
            routine_yield();
        }
    }
    // routine_finish();
}

int main(void) {
    fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (fd == -1) {
        fatal("Could not open socket!\n%s\n", strerror(errno));
    }

    struct sockaddr_in sockaddr = {0};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(HOST_PORT);
    
    if (inet_aton(HOST_ADDR, &sockaddr.sin_addr) == 0) {
        close(fd);
        fatal("%s", strerror(errno));
    }

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        close(fd);
        fatal("An error occured while trying to set SO_REUSEADDRESS option\n%s\n"
                ,strerror(errno));
    }

    if (bind(fd,(struct sockaddr*) (&sockaddr), sizeof(struct sockaddr_in)) == -1) 
    {
        close(fd);
        fatal("Could not bind to %s:%d\n%s\n", HOST_ADDR, HOST_PORT, strerror(errno));

    }

    listen(fd, BL);
    scheduler_init();    
    scheduler_append(listening_loop, NULL);
    scheduler_run();
    scheduler_uninit();
    close(fd);
    return 0;
}
