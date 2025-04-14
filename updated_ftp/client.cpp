#include "client.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

void run_client(int argc, char* argv[]) {
    std::string operation, ip, src, dst, protocol;
    int port = 0;

    protocol = argv[2];
    int argi = 3;

    for (; argi < argc; ++argi) {
        std::string arg = argv[argi];
        if (arg == "-cp" || arg == "-ls" || arg == "-rm" || arg == "-rd" || arg == "-cr" || arg == "-wr") {
            operation = arg;
        } else if (arg == "-ip" && argi + 1 < argc) {
            ip = argv[++argi];
        } else if (arg == "-p" && argi + 1 < argc) {
            port = std::stoi(argv[++argi]);
        } else {
            break;
        }
    }

    // Remaining args
    if (operation == "-cp" || operation == "-wr") {
        if (argi + 1 >= argc) {
            std::cerr << "Insufficient arguments for " << operation << std::endl;
            return;
        }
        src = argv[argi++];
        dst = argv[argi++];
    } else if (operation == "-ls" || operation == "-rm" || operation == "-rd" || operation == "-cr") {
        if (argi >= argc) {
            std::cerr << "Missing argument for " << operation << std::endl;
            return;
        }
        src = argv[argi++];
    }

    int sock;
    if (protocol == "-tcp") {
        sock = socket(AF_INET, SOCK_STREAM, 0);
    } else if (protocol == "-udp") {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
    } else {
        std::cerr << "Invalid protocol." << std::endl;
        return;
    }

    if (sock < 0) {
        perror("Socket creation error");
        return;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

    if (protocol == "-tcp" && connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return;
    }

    std::string command;
    if (operation == "-cp") {
        std::ifstream file(src, std::ios::binary);
        if (!file) {
            std::cerr << "Cannot open file: " << src << std::endl;
            return;
        }
        std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        command = "WRITE " + dst + "\n" + data;
    } else if (operation == "-wr") {
        std::ostringstream content_stream;
        while (argi < argc) {
            content_stream << argv[argi++];
            if (argi < argc) content_stream << " ";
        }
        std::string content = content_stream.str();
        command = "WRITE " + dst + "\n" + content;
    } else if (operation == "-ls") {
        command = "LIST " + src;
    } else if (operation == "-rm") {
        command = "DELETE " + src;
    } else if (operation == "-cr") {
        command = "CREATE " + src;
    } else if (operation == "-rd") {
        command = "READ " + src;
    }

    if (protocol == "-tcp") {
        send(sock, command.c_str(), command.length(), 0);
        char buffer[8192] = {0};
        int valread = read(sock, buffer, 8192);
        std::cout << buffer << std::endl;
    } else {
        sendto(sock, command.c_str(), command.length(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        char buffer[8192] = {0};
        socklen_t len = sizeof(serv_addr);
        recvfrom(sock, buffer, 8192, 0, (struct sockaddr*)&serv_addr, &len);
        std::cout << buffer << std::endl;
    }

    close(sock);
}

