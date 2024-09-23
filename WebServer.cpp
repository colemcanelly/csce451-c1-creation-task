#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <string>
#include "WebServer.h"

#define MAX 80 
#define SA struct sockaddr 

WebServer::WebServer() : port(8080) {
	setupSocket(port);
}

WebServer::WebServer(int port) : port(port) {
	setupSocket(port);
}

WebServer::~WebServer() {
	close(sockfd);
}

void WebServer::setupSocket(int port) {
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
	} 

	bzero(&serverAddr, sizeof(serverAddr)); 
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serverAddr.sin_port = htons(port); 

	if ((bind(sockfd, (SA*)&serverAddr, sizeof(serverAddr))) != 0) { 
		printf("Socket bind failed...\n"); 
		exit(0); 
	} 

	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
	} 
}

void WebServer::acceptAndRespond() {
	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);
	int connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server accept failed...\n"); 
		exit(0); 
	} 

	respond(connfd);
}

void WebServer::respond(int connfd) {
	char buff[MAX]; 
	bzero(buff, MAX); 
	int bytes = read(connfd, buff, sizeof(buff)); 
	write(1, buff, bytes); // just wanna see what's in the HTTP request
	bzero(buff, MAX); 
	
	std::string htmlResponse = "<html><body><h1>Hello, world!</h1></body></html>";

	std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html;"
	"charset=UTF-8\r\nContent-Length: " + std::to_string(htmlResponse.length()) + 
	"\r\n\r\n" + htmlResponse;

	send(connfd, httpResponse.c_str(), httpResponse.length(), 0); 
	printf("\nDone\n");
} 
