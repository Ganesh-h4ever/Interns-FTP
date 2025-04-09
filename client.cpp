#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
 
#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 2121

using namespace std;

int main(){
	int clientsockfd = socket(AF_INET,SOCK_STREAM,0);
	if(clientsockfd == -1){
		cout<<"Socket creation failed"<<endl;
		return -1;
         }

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP , &serverAddr.sin_addr);

	int clientconnect = connect(clientsockfd , (struct sockaddr*)&serverAddr , sizeof(serverAddr));

	if(clientconnect == -1){
		cout<<"Connection failed"<<endl;
		close(clientsockfd);
		return -1;
	}

	cout<<"Connected to the server"<<endl;

	while(true){
		char buffer[1024] ={0};
                int bytesReceived = recv(clientsockfd, buffer, sizeof(buffer)-1, 0);
                if(bytesReceived >0){
                        buffer[bytesReceived] = '\0';
                        cout<<"Server:" <<buffer <<endl;
                }
		string message;
		cout<<"Enter the message(type 'exit' to quit):";
		getline(cin, message);
		if(message == "exit")
		{
			send(clientsockfd, message.c_str(), message.length(), 0);
			break;
		}
		send(clientsockfd , message.c_str(), message.length(), 0);

	}
	close(clientsockfd);
	cout<<"Client Disconnected"<<endl;

}

