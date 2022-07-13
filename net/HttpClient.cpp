#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h>

#include <sys/epoll.h>
#include <pthread.h>

#define HTTP_VERSION "HTTP/1.1"
#define USER_AGENT "User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:10.0.2) Gecko/20100101 Firefox/10.0.2\r\n"
#define ENCODE_TYPE "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
#define CONNECTION_TYPE "Connection: close\r\n"

#define BUFFER_SIZE 4096

#define ASYNC_CLIENT_NUM 1024
#define HOSTNAME_LENGTH 128

typedef void (*async_result_cb)(const char *hostname, const char *result);

struct ep_arg
{
    int sockfd;
    char hostname[HOSTNAME_LENGTH];
    async_result_cb cb;
};

struct async_context
{
    int epfd;
    pthread_t thread_id;
};

struct http_request
{
    char *hostname;
    char *resource;
};

int http_async_client_commit(struct async_context *ctx, const char *hostname, const char *resource, async_result_cb cb)
{

    char *ip = host_to_ip(hostname);
    if (ip == NULL)
        return -1;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin = {0};
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_port = htons(80);
    sin.sin_family = AF_INET;

    if (-1 == connect(sockfd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)))
    {
        return -1;
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    char buffer[BUFFER_SIZE] = {0};

    int len = sprintf(buffer,
                      "GET %s %s\r\n\
Host: %s\r\n\
%s\r\n\
\r\n",
                      resource, HTTP_VERSION,
                      hostname,
                      CONNECTION_TYPE);

    printf("request buffer:%s\n", buffer);
    int slen = send(sockfd, buffer, strlen(buffer), 0);

    struct ep_arg *eparg = (struct ep_arg *)calloc(1, sizeof(struct ep_arg));
    if (eparg == NULL)
        return -1;
    eparg->sockfd = sockfd;
    eparg->cb = cb;

    struct epoll_event ev;
    ev.data.ptr = eparg;
    ev.events = EPOLLIN;

    int ret = epoll_ctl(ctx->epfd, EPOLL_CTL_ADD, sockfd, &ev);

    return ret;
}

static void *http_async_client_callback(void *arg)
{

    struct async_context *ctx = (struct async_context *)arg;
    int epfd = ctx->epfd;

    while (1)
    {

        struct epoll_event events[ASYNC_CLIENT_NUM] = {0};

        int nready = epoll_wait(epfd, events, ASYNC_CLIENT_NUM, -1);
        if (nready < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        else if (nready == 0)
        {
            continue;
        }

        printf("nready:%d\n", nready);
        int i = 0;
        for (i = 0; i < nready; i++)
        {

            struct ep_arg *data = (struct ep_arg *)events[i].data.ptr;
            int sockfd = data->sockfd;

            char buffer[BUFFER_SIZE] = {0};
            struct sockaddr_in addr;
            size_t addr_len = sizeof(struct sockaddr_in);
            int n = recv(sockfd, buffer, BUFFER_SIZE, 0);

            data->cb(data->hostname, buffer); // call cb

            int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
            // printf("epoll_ctl DEL --> sockfd:%d\n", sockfd);

            close(sockfd);

            free(data);
        }
    }
}

struct async_context *http_async_client_init(void)
{

    int epfd = epoll_create(1); //
    if (epfd < 0)
        return NULL;

    struct async_context *ctx = calloc(1, sizeof(struct async_context));
    if (ctx == NULL)
    {
        close(epfd);
        return NULL;
    }
    ctx->epfd = epfd;

    int ret = pthread_create(&ctx->thread_id, NULL, http_async_client_callback, ctx);
    if (ret)
    {
        perror("pthread_create");
        return NULL;
    }
    usleep(1);

    return ctx;
}

int http_async_client_uninit(struct async_context *ctx)
{

    close(ctx->epfd);
    pthread_cancel(ctx->thread_id);

    return 0;
}

static void http_async_client_result_callback(const char *hostname, const char *result)
{

    printf("hostname:%s, result:%s\n\n\n\n", hostname, result);
}

int main(int argc, char *argv[])
{

    struct async_context *ctx = http_async_client_init();
    if (ctx == NULL)
        return -1;

    int count = sizeof(reqs) / sizeof(reqs[0]);
    int i = 0;
    for (i = 0; i < count; i++)
    {
        http_async_client_commit(ctx, reqs[i].hostname, reqs[i].resource, http_async_client_result_callback);
    }

    getchar();
}