#include <iostream>

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "comm.hpp"


int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address, cli;
    
    address.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr.s_addr);
    address.sin_port = htons(8000);
    

    connect(sockfd, (struct sockaddr *)(&address), sizeof(address));

    std::cout<< receiveMessage(sockfd) << std::endl;
    std::cout << "Done";
}