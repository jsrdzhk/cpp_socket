#include "udp_client.h"

cppsocket::udp_client::udp_client(const std::string& host, const short port, const uint32_t timeout_millis)
    : udp_host_(host), udp_port_(port), udp_timeout_(timeout_millis) {
}

cppsocket::udp_client::~udp_client() {
    close();
}

bool cppsocket::udp_client::connect() {
    // Initialize Winsock, returning on failure
    if (!init_socket())
        return false;
    // Create socket
    _sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_sock == SOCKET_ERROR) {
        sprintf_s(_message, "udp socket failed:%d", errno);
        return false;
    }
    // Setup address structure
    memset(&_si, 0, sizeof(_si));
    _si.sin_family = AF_INET;
    _si.sin_addr.s_addr = inet_addr(udp_host_.c_str());
    _si.sin_port = htons(udp_port_);
    // Check for / set up optional timeout for receiveData
    socket_base::set_udp_timeout(_sock, udp_timeout_);
    // bind socket
    if (::bind(_sock, (struct sockaddr*)&_si, sizeof(_si)) < 0) {
        // strerror_r(errno, _message, ERR_MSG_BUF_LEN);
        sprintf_s(_message, "udp bind failed:%d", errno);
        close();
        return false;
    }
    return true;
}

bool cppsocket::udp_client::join_multicast_group(const std::string& multicast_addr, const std::string& local_addr) {
    struct ip_mreq mc_req {};
    mc_req.imr_multiaddr.s_addr = inet_addr(multicast_addr.c_str());  // 组播组IP地址
    mc_req.imr_interface.s_addr = inet_addr(local_addr.c_str());      // 本地网络设备IP地址
    if (::setsockopt(_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mc_req, sizeof(mc_req)) < 0) {
        // strerror_r(errno, _message, ERR_MSG_BUF_LEN);
        sprintf_s(_message, "udp add group failed:%d", errno);
        close();
        return false;
    }
    return true;
}
