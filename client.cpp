#include <iostream>
#include <string>

#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;
using ipAdd = boost::asio::ip::address;

std::string &receiveMessage(udp::socket &socket, udp::endpoint &server, udp::endpoint &sender, boost::array<char, 128> &buffer, std::string &res)
{
    size_t len = 0;

    do
    {
        len = socket.receive_from(
            boost::asio::buffer(buffer), sender);
    } while (sender != server);

    return res.assign(buffer.data(), len);
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: client <host> <port>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;
        udp::endpoint server_endpoint(ipAdd::from_string(argv[1]), std::stoi(argv[2]));

        // socket creation (open socket since UDP is not stream-oriented)
        udp::socket socket(io_context);
        socket.open(udp::v4());

        boost::array<char, 128> recv_buf;
        udp::endpoint sender_endpoint;
        std::string res;

        std::string input;
        std::cout << "Welcome! Would you like to host a game <host> or join a game <join #>?\n";
        getline(std::cin, input);

        while (input != "quit")
        {
            socket.send_to(boost::asio::buffer(input, input.length()), server_endpoint);

            std::cout << "Debug: Sent " << input << " to server!\n";

            receiveMessage(socket, server_endpoint, sender_endpoint, recv_buf, res);
            std::cout << "Debug: " << res << "\n";

            if (res.find("Success") != std::string::npos && input != "host")
                break; // Host started the game, or player joined successfully
            if (res.find("Success") != std::string::npos && input == "host")
                std::cout << "Game successfully created! Please enter 'start' to begin the game for all players\n";
            getline(std::cin, input);
        }

        while (res != "start" && input != "start")
        {
            std::cout << "Waiting for game to start ...\n";
            socket.send_to(boost::asio::buffer("status", 6), server_endpoint);

            receiveMessage(socket, server_endpoint, sender_endpoint, recv_buf, res);
        }

        std::cout << "Game started!\n";
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}