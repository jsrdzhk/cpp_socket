/*
 * @file_path: cpp_socket.h
 * @author: Rodney Cheung
 * @date: 2023-03-09 15:38:24
 * @last_author: Rodney Cheung
 * @last_edit_time: 2023-03-09 16:44:43
 */

#ifndef _CPP_SOCKET_H_
#define _CPP_SOCKET_H_

#ifdef _WIN32
// #pragma comment(lib, "ws2_32.lib")
#define WIN32_LEAN_AND_MEAN
#undef TEXT
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define sprintf_s sprintf
using SOCKET = int;
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <stdio.h>
namespace cppsocket {
static const int MAX_EPOLL_EVENTS = 20;
static const int INVALID_SOCKET = -1;
static const int SOCKET_ERROR = -1;
static const int ERR_MSG_BUF_LEN = 256;
class socket_base {
   protected:
    SOCKET _sock = INVALID_SOCKET;
    // err msg
    char _message[ERR_MSG_BUF_LEN];
    SOCKET _epfd = INVALID_SOCKET;
    epoll_event events[MAX_EPOLL_EVENTS];

   protected:
    // some function wrap
    bool init_socket();
    void cleanup();
    void inet_pton(const char* host, struct sockaddr_in& saddr_in);
    void set_udp_timeout(uint32_t msec);
    static bool set_noblock(int fd, char* err_msg);
    bool epoll_wait(int timeout, bool read_ready, bool write_ready);

   public:
    void close();

    char* get_err_msg() {
        return _message;
    }
};
};  // namespace cppsocket

#endif