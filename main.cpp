#include "client.hpp"
#include "server.hpp"
#include "Error.hpp"
#include "success.hpp"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 7) {
        cerr << ERR_INVALID_ARGS << endl;
        return 1;
    }

    string mode = argv[1];        // -cl or -sr
    string protocol = argv[2];    // -tcp or -udp
    string ip_flag = argv[3];     // -ip
    string ip = argv[4];          // IP address
    string port_flag = argv[5];   // -p
    int port = stoi(argv[6]);     // Port number

    if (ip_flag != "-ip" || port_flag != "-p") {
        cerr << ERR_INVALID_ARGS << endl;
        return 1;
    }

    if (mode == "-cl") {
        client_run(protocol, ip, port);  // run client interactively
    } 
    else if (mode == "-sr") {
        if (argc != 7 || (protocol != "-tcp" && protocol != "-udp")) {
            cerr << ERR_INVALID_ARGS << endl;
            return 1;
        }

        cout << "Server mode selected" << endl;

        // Call the respective function based on the protocol
        if (protocol == "-tcp") {
            start_tcp_server(ip, port);
        } else if (protocol == "-udp") {
            start_udp_server(ip, port);
        }
    }
    else {
        cerr << ERR_INVALID_ARGS << endl;
        return 1;
    }

    return 0;
}

