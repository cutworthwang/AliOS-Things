#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include "iot_import.h"




/**
 * @brief Create a UDP socket.
 *
 * @param [in] port: @n Specify the UDP port of UDP socket
 *
 * @retval  < 0 : Fail.
 * @retval >= 0 : Success, the value is handle of this UDP socket.
 * @see None.
 */
intptr_t HAL_UDP_create(const char *host, unsigned short port)
{
    int flag = 1;
    int ret = -1;
    int socket_id = -1;
    struct sockaddr_in local_addr;  /*local addr*/

    if((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr,"socket create failed\r\n");
        return (intptr_t)-1;
    }

    ret = setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if(ret < 0)
    {
        close(socket_id);
        fprintf(stderr,"setsockopt SO_REUSEADDR failed");
        return (intptr_t)-1;
    }

    flag = 1;
#ifdef IP_RECVPKTINFO
    if((ret = setsockopt(socket_id, IPPROTO_IP, IP_RECVPKTINFO, &flag, sizeof(flag))) < 0)
#else /* IP_RECVPKTINFO */
    if((ret = setsockopt(socket_id, IPPROTO_IP, IP_PKTINFO, &flag, sizeof(flag))) < 0)
#endif /* IP_RECVPKTINFO */
        if (ret < 0)
        {
            close(socket_id);
            fprintf(stderr,"setsockopt IP_PKTINFO failed\r\n");
            return (intptr_t)-1;
        }


    memset(&local_addr, 0x00, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    if(NULL != host){
        inet_aton(host, &local_addr.sin_addr);
    }else{
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    local_addr.sin_port = htons(port);
    ret = bind(socket_id, (struct sockaddr *)&local_addr, sizeof(local_addr));

    fprintf(stderr,"\r\n[%s LINE #%d]  Create socket port %d fd %d ret %d\r\n",
                        __FILE__, __LINE__, port, socket_id, ret);
    return (intptr_t)socket_id;
}


int HAL_UDP_connect()
{
#if 0
    if(NULL != host){
        struct addrinfo         hints;
        struct addrinfo        *res, *ainfo;
        char                    port_ptr[6] = {0};
        struct sockaddr_in     *sa = NULL;

        memset((char *)&hints, 0x00, sizeof(hints));
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_family = AF_INET;
        hints.ai_protocol = IPPROTO_UDP;

        ret = getaddrinfo(host, port_ptr, &hints, &res);
        if (0 != ret) {
            perror("getaddrinfo error");
            close(socket_id);
            return (intptr_t)-1;
        }

        for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
            if (AF_INET == ainfo->ai_family) {
                sa = (struct sockaddr_in *)ainfo->ai_addr;

                if (0 == connect(socket_id, ainfo->ai_addr, ainfo->ai_addrlen)) {
                    break;
                }
            }
        }
        freeaddrinfo(res);
    }
#endif
    return 0;
}

int HAL_UDP_recvfrom(intptr_t          sockfd,
                      NetworkAddr     *p_remote,
                      unsigned char   *p_data,
                      unsigned int     datalen,
                      unsigned int     timeout_ms)
{
    int socket_id = -1;
    struct sockaddr from;
    int count = -1, ret = -1;
    socklen_t addrlen = 0;
    struct timeval      tv;
    fd_set              read_fds;

    if(NULL == p_remote  || NULL == p_data){
        return -1;
    }

    socket_id = (int)sockfd;

    FD_ZERO(&read_fds);
    FD_SET(socket_id, &read_fds);

    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = select(socket_id + 1, &read_fds, NULL, NULL, timeout_ms == 0 ? NULL : &tv);

    /* Zero fds ready means we timed out */
    if (ret == 0) {
        return -2;    /* receive timeout */
    }

    if (ret < 0) {
        if (errno == EINTR) {
            return -3;    /* want read */
        }

        return -4; /* receive failed */
    }

    addrlen = sizeof(struct sockaddr);
    count = recvfrom(socket_id, p_data, (size_t)datalen, 0, &from, &addrlen);
    if(-1 == count)
    {
        return -1;
    }
    if (from.sa_family == AF_INET)
    {
        struct sockaddr_in *sin = (struct sockaddr_in *)&from;
        inet_ntop(AF_INET, &sin->sin_addr, p_remote->addr, NETWORK_ADDR_LEN);
        p_remote->port = ntohs(sin->sin_port);
    }
    return count;
}

int HAL_UDP_recv(intptr_t          sockfd,
                    unsigned char   *p_data,
                    unsigned int     datalen,
                    unsigned int     timeout_ms)
{
    return 0;
}


int HAL_UDP_sendto(intptr_t            sockfd,
                 const NetworkAddr   *p_remote,
                 const unsigned char *p_data,
                 unsigned int         datalen,
                 unsigned int         timeout_ms)
{
    int rc = -1;
    int socket_id = -1;
    struct sockaddr_in remote_addr;

    if(NULL == p_remote || NULL == p_data) {
        return -1;
    }

    socket_id = (int)sockfd;
    remote_addr.sin_family = AF_INET;
    if(1 != (rc = inet_pton(remote_addr.sin_family, p_remote->addr, &remote_addr.sin_addr.s_addr)))
    {
        return -1;
    }
    remote_addr.sin_port = htons(p_remote->port);
    rc = sendto(socket_id, p_data, (size_t)datalen, 0,
              (const struct sockaddr *)&remote_addr, sizeof(remote_addr));
    if(-1 == rc)
    {
        return -1;
    }
    return rc;

}

int HAL_UDP_send(intptr_t            sockfd,
              const unsigned char *p_data,
              unsigned int         datalen,
              unsigned int         timeout_ms)
{
    return 0;
}

int HAL_UDP_joinmulticast(intptr_t           sockfd,
                           const char        *p_group)
{
    int err = -1;
    int socket_id = -1;

    if(NULL == p_group) {
        return -1;
    }

    /*set loopback*/
    int loop = 1;
    socket_id = (int)sockfd;
    err = setsockopt(socket_id, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if(err < 0)
    {
         fprintf(stderr,"setsockopt():IP_MULTICAST_LOOP failed\r\n");
         return err;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(p_group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*default networt interface*/

    /*join to the mutilcast group*/
    err = setsockopt(socket_id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (err < 0)
    {
         fprintf(stderr,"setsockopt():IP_ADD_MEMBERSHIP failed\r\n");
         return err;
    }

    return 0;
}

void HAL_UDP_close(intptr_t sockfd)
{
    int socket_id = -1;

    socket_id = (int)sockfd;
    close(socket_id);
}

