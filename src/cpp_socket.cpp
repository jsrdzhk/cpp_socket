#include "cpp_socket.h"

namespace cppsocket {
bool socket_base::init_socket() {
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        sprintf_s(_message, "WSAStartup() failed with error: %d\n", iResult);
        return false;
    }
#endif
    return true;
}

void socket_base::cleanup() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void socket_base::inet_pton(const char* host, sockaddr_in& saddr_in) {
#ifdef _WIN32
    WCHAR wsz[64];
    swprintf_s(wsz, L"%S", host);
    InetPton(AF_INET, wsz, &(saddr_in.sin_addr.s_addr));
#else
    ::inet_pton(AF_INET, host, &(saddr_in.sin_addr));
#endif
}

void socket_base::set_udp_timeout(int fd, uint32_t msec) {
#ifdef _WIN32
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&msec, sizeof(msec));
#else
    struct timeval timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = (msec * 1000) % 1000000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
}

bool socket_base::set_noblock(int fd, char* err_msg, int err_msg_len) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        sprintf_s(err_msg, "set nonblock err:%d", errno);
        // strerror_r(errno, err_msg, err_msg_len);
        ::close(fd);
        return false;
    }
    return true;
}

bool socket_base::set_block(int fd, char* err_msg, int err_msg_len) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0 || fcntl(fd, F_SETFL, flags & (~O_NONBLOCK)) < 0) {
        sprintf_s(err_msg, "set block err:%d", errno);
        // strerror_r(errno, err_msg, err_msg_len);
        ::close(fd);
        return false;
    }
    return true;
}

bool socket_base::epoll_wait(int timeout, bool read_ready, bool write_ready) {
    if (_epfd < 0) {
        _epfd = epoll_create1(0);
        if (_epfd < 0) {
            // strerror_r(errno, _message, ERR_MSG_BUF_LEN);
            sprintf_s(_message, "epoll_create1 err:%d", errno);
            // LOG_ERROR("PaseTradeTcpClient::epoll_wait create epoll fd failed");
            return false;
        } else {
            epoll_event event{};
            event.events = EPOLLIN | EPOLLOUT;
            event.data.fd = _sock;
            auto ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, _sock, &event);
            if (ret != 0) {
                if (errno != EEXIST) {
                    // strerror_r(errno, _message, ERR_MSG_BUF_LEN);
                    sprintf_s(_message, "epoll_ctl err:%d", errno);
                    // LOG_ERROR("PaseTradeTcpClient::epoll_wait epoll_ctl failed:{}", errno);
                    ::close(_epfd);
                    _epfd = -1;
                    return false;
                }
            }
        }
    }
    auto count = ::epoll_wait(_epfd, events, MAX_EPOLL_EVENTS, timeout);
    if (count < 0) {
        // strerror_r(errno, _message, ERR_MSG_BUF_LEN);
        sprintf_s(_message, "epoll_wait err:%d", errno);
        return false;
    } else {
        for (auto i = 0; i < count; ++i) {
            if (events[i].data.fd != _sock) {
                continue;
            }
            if (events[i].events & EPOLLIN && read_ready) {
                return true;
            }
            if (events[i].events & EPOLLOUT && write_ready) {
                return true;
            }
        }
    }
    return false;
}

void socket_base::close() {
    if (_sock > 0) {
#ifdef _WIN32
        ::closesocket(_sock);
#else
        ::close(_sock);
#endif
        _sock = INVALID_SOCKET;
    }
    cleanup();
}

bool socket_base::created() {
    return _sock > 0;
}
}  // namespace cppsocket