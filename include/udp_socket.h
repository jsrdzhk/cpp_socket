/*
 * @file_path: udp_socket.h
 * @author: Rodney Cheung
 * @date: 2023-03-16 09:47:55
 * @last_author: Rodney Cheung
 * @last_edit_time: 2023-03-23 17:06:28
 */

#ifndef _UDP_SOCKET_H_
#define _UDP_SOCKET_H_
#include "cpp_socket.h"

namespace cppsocket {
class udp_socket : public socket_base {
   protected:
    sockaddr_in _si{};

   public:
    void send_data(void* buf, size_t len) {
        sendto(_sock, (const char*)buf, (int)len, 0, (struct sockaddr*)&_si, sizeof(sockaddr_in));
    }

    ssize_t recv_data(char* buf, size_t len) {
        return recvfrom(_sock, (char*)buf, (int)len, 0, nullptr, nullptr);
    }
};
}  // namespace cppsocket
#endif