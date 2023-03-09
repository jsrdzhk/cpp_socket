/*
 * @file_path: tcp_client.h
 * @author: Rodney Cheung
 * @date: 2023-03-09 16:16:22
 * @last_author: Rodney Cheung
 * @last_edit_time: 2023-03-09 16:20:00
 */
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_
#include "tcp_socket.h"
namespace cppsocket {
class tcp_client : public tcp_socket {
   public:
    tcp_client(const char* host, const short port) : tcp_socket(host, port) {
    }

    bool open_connection();
};
}  // namespace cppsocket
#endif