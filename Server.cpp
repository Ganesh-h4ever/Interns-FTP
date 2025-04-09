#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

// This is Server program

#define PORT 2121
using namespace std;

int main(){
	sockaddr_in ser1 ,client;
	socklen_t client_len = sizeof(client);
	char buffer[1024] ={0};
	int client_soc;
	int socfd = socket(AF_INET,SOCK_STREAM,0);
	cout<<"The value of server socfd : "<<socfd<<endl;
	if(socfd == -1){
		cout<< "Unable to create a socket!" <<endl;
		return -1;
	}
	ser1.sin_family = AF_INET;
	ser1.sin_port = htons(PORT);
	inet_pton(AF_INET, "0.0.0.0", &ser1.sin_addr);

	if(bind(socfd, (struct sockaddr*)&ser1, sizeof(ser1)) == -1){
		cout<<"Can't bind to ip port"<<endl;
		return -2;
	}

	if(listen(socfd, SOMAXCONN) == -1){
		cout<<"Cannot listen!"<<endl;
		return -3;
	}

	while(true){
		string msg;
		client_soc = accept(socfd, (struct sockaddr*)&client, &client_len);

		if(client_soc < 0){
			cerr<<"Error ! Accept Failed "<<endl;
			continue;
		}

		cout<< "New client connected!" <<endl;
		send(client_soc, "2121 FTP Server Ready\r\n",22,0);

		while(true){
			memset(buffer,0,sizeof(buffer));
			int bytes_received = recv(client_soc, buffer, sizeof(buffer)-1, 0);
			if(bytes_received <= 0)
				break;
			cout<<"Client: " <<buffer<<endl;
			getline(cin , msg);
			if(strncmp(buffer, "USER", 4)==0){
				send(client_soc,"331 USER OK, need password\r\n",28,0);
			}
			else if(strncmp(buffer, "PASS", 4)==0){
				send(client_soc, "230 User logged in\r\n",21,0);
			}
			else if(strncmp(buffer, "bye", 4)==0){
				send(client_soc, "221 Goodbye\r\n", 13, 0);
				break;
			}
			else{
				send(client_soc,msg.c_str(),msg.length(),0);
			}
		}
		close(client_soc);
		cout<<"Client disconnectd: "<<endl;
	}
	close(socfd);
	return 0;
}


		
