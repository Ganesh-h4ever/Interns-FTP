#include "Error.hpp"
#include "success.hpp"
#include "client.hpp"
 
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
 
#include<cstring>
 
#include<fstream>
#include<sstream>
 
#include<dirent.h>
 
using namespace std;
 
 
//To establish TCP Connection
void tcp_connect(const string& ip,int port,const string& message,string& response){
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock < 0){
                cerr << ERR_SOCKET_CREATION << endl;
                return;
        }
 
        sockaddr_in server_addr {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
 
        if(connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) <0){
                cerr<<ERR_CONNECTION_FAILED<<endl;
                close(sock);
                return;
        }
 
 
        send(sock, message.c_str(), message.size(), 0);
 
        char buffer[4096];
        int bytes_received = recv(sock, buffer, sizeof(buffer)-1, 0);
        if(bytes_received >0){
                buffer[bytes_received] ='\0';
                response = buffer;
        }
 
        close(sock);
 
}
 
 
//To Establish UDP Connection
void udp_connect(const string& ip, int port, const string& message, string& response) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        cerr << ERR_SOCKET_CREATION << endl;
        return;
    }
 
    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
 
    // Send the message to the server
int sent_bytes =  sendto(sock, message.c_str(), message.size(), 0, (sockaddr*)&server_addr, sizeof(server_addr));
  if (sent_bytes < 0) {
        cerr << "Failed to send UDP message" << endl;
        close(sock);
        return;
    }
 
    // Prepare to receive response
    char buffer[4096];
    sockaddr_in from_addr {};
    socklen_t from_len = sizeof(from_addr);
 
    // Receive server's response
    int bytes_received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&from_addr, &from_len);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        response = buffer;
    } else {
        cerr << ERR_CONNECTION_FAILED << endl;
    }
 
    close(sock);
}
 
 
//Checks for protocol mentioned and calls respective protocol function
void communicate(const string& protocol,const string& ip,int port,const string& message, string& response){
        if (protocol == "-udp") {
           udp_connect(ip, port, message, response);
    } else if (protocol == "-tcp") {
           tcp_connect(ip, port, message, response);
    } else {
        response = ERR_INVALID_PROTOCOL;
    }
}
 
 
//Calls server side Listing Function
void client_list(const string& protocol, const string& ip, int port,const string& dir){
        if(dir == "/"){
                cerr << PERMISSION_DENIED << endl;
                return ;
        }
 
        string msg = "-ls " + dir;
        string response;
        communicate(protocol, ip, port, msg, response);
        cout<< response <<endl;
}
 
 
//Calls server side Create Function
void client_create_file(const string& protocol, const string& ip, int port,const string& filename){
        string msg = "-create" + filename;
        string response;
        communicate(protocol, ip, port, msg, response);
        cout<<response<<endl;
}
 
//Calls server side Remove Function
void client_remove(const string& protocol, const string& ip, int port, const string& filename) {
    string msg = "-rm " + filename;
    string response;
    communicate(protocol, ip, port, msg, response);
    cout << response << endl;
}
 
//Calls server side Copy Funtion
void client_copy(const string& protocol, const string& ip, int port, const string& src, const string& dest) {
    string msg = "-cp " + src + " " + dest;
    string response;
    communicate(protocol, ip, port, msg, response);
    cout << response << endl;
}
 
 
//Main function , runs client wrt protocol mentioned
void client_run(const string& protocol, const string& ip, int port) {
    string response;
    string test_message = "ping";
 
    communicate(protocol, ip, port, test_message, response);
    if (response != SUCCESS_CONNECTED) {
        cerr << "Server not reachable. Response: " << response << endl;
        return;
    }
 
    cout << "Connected to " << ip << " on port " << port << endl;
 
    if (protocol == "-udp") {
        // To interact with server continuosly
        while (true) {
            cout << "Enter command (-ls, -create, -rm,-cp, exit): ";
            string command;
            getline(cin, command);
 
            if (command == "exit") {
                string exit_response;
                communicate(protocol, ip, port, "exit", exit_response);
                cout << exit_response << endl;
                break;
            }
 
            string extra_input;
            if (command == "-ls") {
                cout << "Enter directory to list: ";
                getline(cin, extra_input);
                command += extra_input;
            } else if (command == "-create" || command == "-rm") {
                cout << "Enter filename: ";
                getline(cin, extra_input);
                command += (command == "-rm" ? " " : "") + extra_input;
            }else if (command == "-cp") {
                cout << "Enter source filename: ";
                string src, dest;
                getline(cin, src);
                cout << "Enter destination filename: ";
                getline(cin, dest);
                command += " " + src + " " + dest;
           } else {
                cout << "Invalid command!" << endl;
                continue;
            }
 
            string command_response;
            communicate(protocol, ip, port, command, command_response);
            cout << command_response << endl;
        }
 
    } else if (protocol == "-tcp") {
        // Create socket and keep connection alive
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            cerr << ERR_SOCKET_CREATION << endl;
            return;
        }
 
        sockaddr_in server_addr {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
 
        if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            cerr << ERR_CONNECTION_FAILED << endl;
            close(sock);
            return;
        }
 
        string command;
        char buffer[4096];
        while (true) {
            cout << "Enter command (-ls, -create, -rm,-cp, exit): ";
            getline(cin, command);
 
            string extra_input;
            if (command == "-ls") {
                cout << "Enter directory to list: ";
                getline(cin, extra_input);
                command += extra_input;
            } else if (command == "-create" || command == "-rm") {
                cout << "Enter filename: ";
                getline(cin, extra_input);
                command += (command == "-rm" ? " " : "") + extra_input;
            } else if (command == "exit") {
                send(sock, command.c_str(), command.size(), 0);
                break;
            }else if (command == "-cp") {
                cout << "Enter source filename: ";
                string src, dest;
                getline(cin, src);
                cout << "Enter destination filename: ";
                getline(cin, dest);
                command += " " + src + " " + dest;
           }else {
                cout << "Invalid command!" << endl;
                continue;
            }
 
            send(sock, command.c_str(), command.size(), 0);
            int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                cout << buffer << endl;
            } else {
                cerr << "Server did not respond" << endl;
            }
        }
 
        close(sock);
    } else {
        cerr << ERR_INVALID_PROTOCOL << endl;
    }
}
