#include "Error.hpp"
#include "success.hpp"
#include "server.hpp"
 
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <filesystem>
using namespace std;
 
//To process the command
string process_message(const string& message) {
    if (message == "ping") {
        return SUCCESS_CONNECTED;
    }
    if (message.find("-ls") == 0) {
        return handle_list(message);
    } else if (message.find("-create") == 0) {
        return handle_create_file(message);
    } else if (message.find("-rm") == 0) {
        return handle_remove_file(message);
    } else if (message.find("-cp") == 0) {
        return handle_copy_file(message);
    }else if (message == "exit") {
        return "exit";
    } else {
        return ERR_INVALID_COMMAND;
    }
}
 
namespace fs = std::filesystem;
 
//List Operation
string handle_list(const string& message) {
    string dir = message.substr(3); // Extracting name of Directory
 
    // Removing whitespaces
    dir.erase(0, dir.find_first_not_of(" \t\n\r"));
    dir.erase(dir.find_last_not_of(" \t\n\r") + 1);
 
    if (dir == "/") return PERMISSION_DENIED;
 
    if (!fs::exists(dir) || !fs::is_directory(dir)) return ERR_DIR_NOT_FOUND;
 
    string result;
    for (const auto& entry : fs::directory_iterator(dir)) { //Logic for list operation
        result += entry.path().filename().string() + "\n";
    }
    return result;
}
 
//Create Operation
string handle_create_file(const string& message) {
    string filename = message.substr(7);  // Extract the filename
    ofstream file(filename); //Logic for creating file
    if (!file) {
        return ERR_FILE_CREATE_FAILED;
    }
 
    file.close();
    return SUCCESS_FILE_CREATED;
}
 
//Delete Operation
string handle_remove_file(const string& message) {
    string filename = message.substr(4);
  cout << "Trying to remove: '" << filename << "'" << endl;
    if (remove(filename.c_str()) != 0) {  //Logic for deleting file
        return ERR_FILE_REMOVE_FAILED;
    }
    return SUCCESS_FILE_REMOVED;
}
 
//Copy Operation
string handle_copy_file(const string& message) {
    istringstream iss(message);
    string cmd, src, dest;
    iss >> cmd >> src >> dest;
 
    if (src.empty() || dest.empty()) {
        return ERR_INVALID_COMMAND;
    }
 
    ifstream in(src, ios::binary);
    if (!in) {
        return "Error: Source file not found.";
    }
 
    ofstream out(dest, ios::binary);
    if (!out) {
        return "Error: Could not create destination file.";
 
    }
 
    out << in.rdbuf();
 
    in.close();
    out.close();
 
    return "File copied successfully.";
}
 
 
//Runs a TCP Server
void start_tcp_server(const string& ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);    //Creation of socket
    if (sockfd < 0) {
        cerr << ERR_SOCKET_CREATION << endl;
        return;
    }
 
    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
 
    if (bind(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) { //Binding IP Address and Port
        cerr << ERR_BIND_FAILED << endl;
        close(sockfd);
        return;
    }
 
    if (listen(sockfd, 5) < 0) {  //Server starts Listening
        cerr << ERR_LISTEN_FAILED << endl;
        close(sockfd);
        return;
    }
 
    cout << "Server listening on " << ip << ":" << port << endl;
 
    // Accept client connections
    while (true) {
        sockaddr_in client_addr {};
        socklen_t client_len = sizeof(client_addr);
        int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_len);
        if (client_sockfd < 0) {
            cerr << ERR_ACCEPT_FAILED << endl;
            continue; // Try accepting the next connection
        }
 
        cout << "Client connected" << endl;
 
        //Loop to keep server running continuously
        while (true) {
            char buffer[4096];
            int bytes_received = recv(client_sockfd, buffer, sizeof(buffer) - 1, 0);
 
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                string message(buffer);
                cout << "Received message: " << message << endl;
 
                // Process the message (handle ping and other commands)
                string response = process_message(message);
                send(client_sockfd, response.c_str(), response.size(), 0);
 
                if (message == "exit") {
                    cout << "Client disconnected" << endl;
                    break; // Exit the inner loop
                }
            }
            else if (bytes_received == 0) {
                // If bytes_received is 0, the client has disconnected
                               break;
            } else {
                // Error occurred while receiving data
                cerr << "Error receiving data" << endl;
                break;
            }
        }
 
        close(client_sockfd);  // Close the client socket
        cout << "Waiting for new clients..." << endl;
    }
 
    close(sockfd);  // Close the server socket after exiting the loop
}
 
 
//Starts UDP Server
void start_udp_server(const string& ip, int port) {
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0); // Creation of Socket
    if (server_fd == -1) {
        cerr << ERR_SOCKET_CREATION << endl;
        return;
    }
 
    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
 
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {  //Binding IP Address and Port
        cerr << ERR_BIND_FAILED << endl;
        close(server_fd);
        return;
    }
 
    cout << "UDP server running on " << ip << ":" << port << endl;
    //Loop to keep server running continuosly
    while (true) {
        char buffer[4096];
        sockaddr_in client_addr {};
        socklen_t client_len = sizeof(client_addr);
 
        int bytes_received = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &client_len);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            string message = buffer;
            cout << "Received: " << message << endl;
 
            string response = process_message(message);
            sendto(server_fd, response.c_str(), response.size(), 0, (struct sockaddr*)&client_addr, client_len);
 
            if (message == "exit") {
                cout << "Client disconnected" << endl;
                break;
            }
        }
    }
 
    close(server_fd);
}
