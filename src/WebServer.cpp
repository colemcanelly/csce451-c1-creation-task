#include <algorithm>
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include "WebServer.h"

#define MAX 1000
#define SA struct sockaddr 

int deployWebServer() {
    printf("Navigate to http://localhost:8080");
    struct sockaddr_in serverAddr, cli;
    int port = 8080, sockfd, connfd;

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (sockfd == -1) return -1;

    bzero(&serverAddr, sizeof(serverAddr)); 
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(port); 

    if ((bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) != 0) return -1;  
    if ((listen(sockfd, 5)) != 0) return -1;

    socklen_t len = sizeof(cli);
    connfd = accept(sockfd, (struct sockaddr*)&cli, &len); 
    if (connfd < 0) return -1;

    char buff[1000] = {0};
    read(connfd, buff, sizeof(buff));

    char* user = getenv("USER");

    char* htmlResponse = (char*)malloc(200*sizeof(char));
    strcat(htmlResponse, "<h1>Congrats, ");
    strcat(htmlResponse, user);
    strcat(htmlResponse, "! You cracked the code!</h1>");

    char* httpResponse = (char*)malloc(2000*sizeof(char));
    int contentLength = strlen(htmlResponse);
    sprintf(httpResponse, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: %d\n\r\n\r\n%s", contentLength, htmlResponse); 

    send(connfd, httpResponse, strlen(httpResponse), 0); 

    return 0;
}


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

std::string WebServer::parseUserAgentString(std::string userAgentString) {
	std::string response;
	
	if (userAgentString.find("Firefox") != std::string::npos) {
		response = "+1 for using Firefox";
	} else if (userAgentString.find("Chrome") != std::string::npos || userAgentString.find("Chromium") != std::string::npos) {
		response = "Chrome? In 2024? Unbelievable...";
	} else if (userAgentString.find("Safari") != std::string::npos) {
		response = "Safari? Are you serious? Get it together...";
	} else if (userAgentString.find("Opera") != std::string::npos || userAgentString.find("OPR") != std::string::npos) {
		response = "Opera is an interesting choice. At least it's not Safari...";
	}
	
	return response;
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
	//write(1, buff, bytes); // just wanna see what's in the HTTP request
	std::cout << "Bytes read: " << bytes << std::endl;

	std::stringstream request(buff);

	std::string userAgentLine;
	for (; std::getline(request, userAgentLine);) {
		if (userAgentLine.find("User-Agent") != std::string::npos) {
			std::cout << userAgentLine << std::endl;
			break;
		}
	}

	bzero(buff, MAX); 

	std::string userAgentResponse = parseUserAgentString(userAgentLine);
	
	//std::string htmlResponse = "<html><body><h1>Hello, world!</h1></body></html>";
	std::string htmlResponse = "<h1>" + userAgentResponse + "</h1>";

	std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html;"
	"charset=UTF-8\r\nContent-Length: " + std::to_string(htmlResponse.length()) + 
	"\r\n\r\n" + htmlResponse;

	send(connfd, httpResponse.c_str(), httpResponse.length(), 0); 
	printf("\nDone\n");
} 
