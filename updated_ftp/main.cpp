#include <iostream>
#include <string>
#include "client.h"
#include "server.h"

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: iftp <-cl/-sr> <-tcp/-udp> <-cp/-ls/-rm/-cr/-rd> -ip <IP> -p <PORT> <src> <dst>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string protocol = argv[2];

    if (mode == "-cl") {
        run_client(argc, argv);
    } else if (mode == "-sr") {
        run_server(argc, argv);
    } else {
        std::cerr << "Invalid mode. Use -cl for client or -sr for server." << std::endl;
        return 1;
    }
    return 0;
}

