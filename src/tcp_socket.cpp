#include "tcp_socket.h"

cppsocket::tcp_socket::tcp_socket(const char* host, const short port) {
    sprintf_s(_host, "%s", host);
    sprintf_s(_port, "%d", port);
}

bool cppsocket::tcp_socket::send_data(void* buf, size_t len, int timeout) {
    size_t nleft = len;
    ssize_t nwritten;
    const char* ptr = reinterpret_cast<const char*>(buf);
    while (nleft > 0) {
        // DBG("try write %d via fd %d", (int)nleft, fd);
        if (!_conn_blocked) {
            if (!epoll_wait(timeout, false, true)) {
                return false;
            }
        }
        nwritten = write(_conn, ptr, nleft);
        if (nwritten <= 0) {
            if (nwritten < 0 && (errno == EINTR || errno == EAGAIN)) {
                nwritten = 0;
            } else {
                sprintf_s(_message, "tcp write err:%d", errno);
                // strerror_r(errno, _message, ERR_MSG_BUF_LEN);
                return -1;
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return nleft == 0;
}

int cppsocket::tcp_socket::receive_data(void* buf, size_t len, int timeout) {
    size_t nleft = len;
    ssize_t nread;
    char* ptr = (char*)buf;
    while (nleft > 0) {
        if (!_conn_blocked) {
            if (!epoll_wait(timeout, true, false)) {
                break;
            }
        }
        nread = read(_conn, ptr, nleft);
        if (nread < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                nread = 0;
            } else {
                sprintf_s(_message, "tcp read err:%d", errno);
                // strerror_r(errno, _message, ERR_MSG_BUF_LEN);
                break;
            }
        } else if (nread == 0) {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return (len - nleft);
}

void cppsocket::tcp_socket::close() {
    if (_conn > 0) {
#ifdef _WIN32
        ::closesocket(_conn);
#else
        ::close(_conn);
#endif
    }
    cleanup();
    _connected = false;
}

bool cppsocket::tcp_socket::create_socket() {
    // Initialize Winsock, returning on failure
    if (!init_socket()) {
        return false;
    }
    // Set up client address info
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // Resolve the server address and port, returning on failure
    int iResult = getaddrinfo(_host, _port, &hints, &_address_info);
    if (iResult != 0) {
        sprintf_s(_message, "getaddrinfo() failed with error: %d", iResult);
        close();
        return false;
    }
    // Create a SOCKET for connecting to server, returning on failure
    _sock = socket(_address_info->ai_family, _address_info->ai_socktype, _address_info->ai_protocol);
    if (_sock == INVALID_SOCKET) {
        // strerror_r(errno, _message, ERR_MSG_BUF_LEN);
        sprintf_s(_message, "tcp socket failed:%d", errno);
        close();
        return false;
    }
    return true;
}
