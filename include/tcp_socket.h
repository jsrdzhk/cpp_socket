#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_
#include "cpp_socket.h"
namespace cppsocket {
class tcp_socket : public socket_base {
   protected:
    tcp_socket(const char* host, const short port);
    char _host[128];
    char _port[8];
    SOCKET _conn = INVALID_SOCKET;
    addrinfo* _address_info = nullptr;
    bool _connected = false;
    bool _conn_blocked = false;

   public:
    bool create_socket();
    bool send_data(void* buf, size_t len, int timeout);
    int receive_data(void* buf, size_t len, int timeout);
    void close() override;
    bool is_connected() {
        return _connected;
    }
    void set_nonblock(bool block) {
        if (block) {
            if (socket_base::set_noblock(_conn, _message, ERR_MSG_BUF_LEN)) {
                _conn_blocked = false;
            }
        } else {
            if (socket_base::set_block(_conn, _message, ERR_MSG_BUF_LEN)) {
                _conn_blocked = true;
            }
        }
    }

   private:
};
}  // namespace cppsocket
#endif