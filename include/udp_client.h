/*
 * @file_path: udp_client.h
 * @author: Rodney Cheung
 * @date: 2023-03-16 09:52:41
 * @last_author: Rodney Cheung
 * @last_edit_time: 2023-03-16 10:04:45
 */
#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_
#include <string>

#include "cpp_socket.h"
#include "udp_socket.h"

namespace cppsocket {
class udp_client : public udp_socket {
   public:
    udp_client(const std::string& host, const short port, const uint32_t timeoutMsec = 0);
    ~udp_client();

    bool connect();
    bool join_multicast_group(const std::string& multicast_addr, const std::string& local_addr);

   private:
    std::string udp_host_;
    short udp_port_;
    uint32_t udp_timeout_ = 0;
};
}  // namespace cppsocket
#endif