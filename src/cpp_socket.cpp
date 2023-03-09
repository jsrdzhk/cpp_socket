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

void socket_base::set_udp_timeout(uint32_t msec) {
#ifdef _WIN32
    setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&msec, sizeof(msec));
#else
    struct timeval timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = (msec * 1000) % 1000000;
    setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
}

bool socket_base::set_noblock(int fd, char* err_msg) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        // char errbuf[1024];
        // todo
        strerror_r(errno, err_msg, ERR_MSG_BUF_LEN);
        ::close(fd);
        return -1;
    }
    return 0;
}

bool socket_base::epoll_wait(int timeout, bool read_ready, bool write_ready) {
    if (_epfd < 0) {
        _epfd = epoll_create1(0);
        if (_epfd < 0) {
            strerror_r(errno, _message, ERR_MSG_BUF_LEN);
            // LOG_ERROR("PaseTradeTcpClient::epoll_wait create epoll fd failed");
            return false;
        } else {
            epoll_event event{};
            event.events = EPOLLIN | EPOLLOUT;
            event.data.fd = _sock;
            auto ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, _sock, &event);
            if (ret != 0) {
                if (errno != EEXIST) {
                    strerror_r(errno, _message, ERR_MSG_BUF_LEN);
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
        strerror_r(errno, _message, ERR_MSG_BUF_LEN);
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
#ifdef _WIN32
    ::closesocket(_sock);
#else
    ::close(_sock);
#endif
}
}  // namespace cppsocket