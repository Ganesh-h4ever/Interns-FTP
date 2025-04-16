#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
using namespace std;



void client_run(const string& protocol, const string& ip, int port);


void communicate(const string& protocol,const string& ip,int port,const string& message, string& response);
//client operations
//void client_write_file(const string& protocol,const string& ip,const int port,const string& filename);
//void client_read_file(const string& protocol,const string& ip,const int port,const string& filename);
void client_copy(const string& protocol, const string& ip, const int port, const string& src, const string& dst);
void client_list(const string& protocol, const string& ip,const int port,const string& dir);
void client_create_file(const string& protocol,const string& ip,const int port,const string& filename);
void client_remove(const string& protocol,const string& ip,const int port,const string& filename);

void send_tcp_message(const string& ip, int port, const string& message,string& response);
void send_udp_message(const string& ip, int port, const string& message, string& response);


#endif 

