
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
        std::cout<< "Welcome! Would you like to host a game <host> or join a game <join #>?\n";
        getline(std::cin, input);

        boost::array<char,128> recv_buf;
        udp::endpoint sender_endpoint;
        std::string res;
        
        while(input != "quit"){
            socket.send_to(boost::asio::buffer(input, input.length()), receiver_endpoint);

            std::cout << "Debug: Sent " << input << " to server!\n";
        
            
            size_t len = socket.receive_from(
                boost::asio::buffer(recv_buf), sender_endpoint
            );

            res.assign(recv_buf.data(), len);
            std::cout << "Debug: " << res << "\n";

            if(res.find("Success") != std::string::npos && input != "host") break; //Host started the game, or player joined successfully
            if(res.find("Success") != std::string::npos && input == "host")
                std::cout << "Game successfully created! Please enter 'start' to begin the game for all players\n";
            getline(std::cin, input);
        }

        while(res != "start" && input != "start"){
            std::cout << "Waiting for game to start ...\n";
            socket.send_to(boost::asio::buffer("status", 6), receiver_endpoint);

            size_t len = socket.receive_from(
                boost::asio::buffer(recv_buf), sender_endpoint
            );
            res.assign(recv_buf.data(), len);
        }

        std::cout<< "Game started!\n";


    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}