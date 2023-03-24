#include "tcp_client.h"

bool cppsocket::tcp_client::open_connection() {
    if (!create_socket()) {
        return false;
    }
    // Connect to server, returning on failure
    if (connect(_sock, _address_info->ai_addr, (int)_address_info->ai_addrlen) == SOCKET_ERROR) {
        close();
        _sock = INVALID_SOCKET;
        sprintf_s(_message, "connect() failed; please make sure server is running");
        return false;
    }

    // For a client, the connection is the same as the main socket
    _conn = _sock;

    // Success!
    _connected = true;
    return true;
}
