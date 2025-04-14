#include "server.h"
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

void run_server(int argc, char* argv[]) {
    int port = 0;
    std::string protocol;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-p" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "-tcp" || arg == "-udp") {
            protocol = arg;
        }
    }

    int server_fd;
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (protocol == "-tcp") {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
    } else {
        server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    }

    if (server_fd < 0) {
        perror("Socket creation failed");
        return;
    }

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return;
    }

    if (protocol == "-tcp") {
        if (listen(server_fd, 3) < 0) {
            perror("Listen failed");
            return;
        }
        std::cout << "TCP Server listening on port " << port << std::endl;

        while (true) {
            int addrlen = sizeof(address);
            int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                perror("Accept failed");
                continue;
            }

            char buffer[8192] = {0};
            int valread = read(new_socket, buffer, 8192);
            std::string request(buffer);
            std::string response;

            if (request.starts_with("WRITE ")) {
                size_t pos = request.find('\n');
                if (pos == std::string::npos) {
                    response = "Invalid WRITE format.";
                } else {
                    std::string filename = request.substr(6, pos - 6);
                    std::string filedata = request.substr(pos + 1);
                    std::ofstream file(filename, std::ios::binary);
                    if (file) {
                        file << filedata;
                        response = "File written successfully.";
                    } else {
                        response = "Failed to open file for writing.";
                    }
                }
            } else if (request.starts_with("LIST ")) {
                std::string path = request.substr(5);
                try {
                    for (const auto& entry : fs::directory_iterator(path)) {
                        response += entry.path().string() + "\n";
                    }
                } catch (...) {
                    response = "Invalid directory path.";
                }
            } else if (request.starts_with("DELETE ")) {
                std::string filepath = request.substr(7);
                response = fs::remove(filepath) ? "File deleted." : "Failed to delete file.";
            } else if (request.starts_with("CREATE ")) {
                std::string filename = request.substr(7);
                std::ofstream file(filename);
                response = file ? "File created." : "Failed to create file.";
            } else if (request.starts_with("READ ")) {
                std::string filename = request.substr(5);
                std::ifstream file(filename);
                if (file) {
                    response = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                } else {
                    response = "Failed to read file.";
                }
            } else {
                response = "Unknown command.";
            }

            send(new_socket, response.c_str(), response.length(), 0);
            close(new_socket);
        }
    } else {
        std::cout << "UDP Server listening on port " << port << std::endl;
        char buffer[8192];
        sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);

        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int n = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
            std::string request(buffer);
            std::string response;

            if (request.starts_with("WRITE ")) {
                size_t pos = request.find('\n');
                if (pos == std::string::npos) {
                    response = "Invalid WRITE format.";
                } else {
                    std::string filename = request.substr(6, pos - 6);
                    std::string filedata = request.substr(pos + 1);
                    std::ofstream file(filename, std::ios::binary);
                    if (file) {
                        file << filedata;
                        response = "File written.";
                    } else {
                        response = "Failed to open file.";
                    }
                }
            } else if (request.starts_with("LIST ")) {
                std::string path = request.substr(5);
                try {
                    for (const auto& entry : fs::directory_iterator(path)) {
                        response += entry.path().string() + "\n";
                    }
                } catch (...) {
                    response = "Invalid directory path.";
                }
            } else if (request.starts_with("DELETE ")) {
                std::string filepath = request.substr(7);
                response = fs::remove(filepath) ? "File deleted." : "Failed to delete file.";
            } else if (request.starts_with("CREATE ")) {
                std::string filename = request.substr(7);
                std::ofstream file(filename);
                response = file ? "File created." : "Failed to create file.";
            } else if (request.starts_with("READ ")) {
                std::string filename = request.substr(5);
                std::ifstream file(filename);
                if (file) {
                    response = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                } else {
                    response = "Failed to read file.";
                }
            } else {
                response = "Unknown command.";
            }

            sendto(server_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&cliaddr, len);
        }
    }

    close(server_fd);
}

