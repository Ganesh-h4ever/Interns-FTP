#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
using namespace std;


void start_tcp_server(const string& ip,int port);
void start_udp_server(const string& ip ,int port);
string process_message(const string& message);
string handle_list(const string& message);
string handle_create_file(const string& message);
string handle_remove_file(const string& message);
string handle_copy_file(const string& message);

#endif
