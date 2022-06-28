#include <iostream>

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#include "comm.hpp"

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address, cli;
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8000);
    
    if(bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        std::cout << "Cant bind" << std::endl;
        return 1;
    }

    listen(sockfd, 8);

    int playerOne = accept(sockfd, NULL, NULL);
    sendMessage(playerOne, std::string("You're Player 1! Waiting for Player 2...."));


    close(sockfd);
    std::cout << "Done";
}