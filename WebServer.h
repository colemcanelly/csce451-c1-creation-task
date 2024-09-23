#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <netinet/in.h>

class WebServer {
private:
    int port;
    int sockfd;
    struct sockaddr_in serverAddr;

    void setupSocket(int port);

public:
    WebServer();
    WebServer(int port);
    ~WebServer();
    void acceptAndRespond();
    void respond(int connfd);
};


#endif // !WEBSERVER_H
