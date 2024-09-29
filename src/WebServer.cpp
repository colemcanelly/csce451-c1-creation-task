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

    exit(0);
}
