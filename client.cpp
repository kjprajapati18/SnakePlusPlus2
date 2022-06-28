
#include <iostream>
#include <string>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "comm.hpp"

using boost::asio::ip::udp;


int main(int argc, char* argv[]) {
     try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: client <host> <port>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;
        udp::endpoint receiver_endpoint = udp::endpoint(boost::asio::ip::address::from_string(argv[1]), std::stoi(argv[2]));
        
        //socket creation (open socket since UDP is not stream-oriented)
        udp::socket socket(io_context);
        socket.open(udp::v4());

        std::string input;
        getline(std::cin, input);
        while(input != "quit"){
            socket.send_to(boost::asio::buffer(input, input.length()), receiver_endpoint);

            std::cout << "Sent " << input << " to server!\n";
        
            boost::array<char,128> recv_buf;
            udp::endpoint sender_endpoint;
            size_t len = socket.receive_from(
                boost::asio::buffer(recv_buf), sender_endpoint
            );

            std::cout << "Received message from server: " << std::string(recv_buf.data()).substr(0,len) << "\n";
            getline(std::cin, input);
        }

    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}