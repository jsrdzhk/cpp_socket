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

   public:
    bool send_data(void* buf, size_t len, bool block, int timeout);
    int receive_data(void* buf, size_t len, bool block, int timeout);
    bool is_connected() {
        return _connected;
    }

   private:
    bool create_socket();
};
}  // namespace cppsocket
#endif