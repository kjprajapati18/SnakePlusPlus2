#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "serverGameInfo.h"

class udp_server
{
public:
    udp_server(boost::asio::io_context &io_context);

private:
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;

    std::unordered_map<boost::asio::ip::udp::endpoint, int, udpHash> playerDict;
    std::unordered_map<int, std::shared_ptr<gameInfo>> gameDict;

    boost::array<char, 16> recv_buffer_;

    void start_receive();
    void handle_receive(const boost::system::error_code &error, std::size_t bytesRead);
    void handle_send(boost::shared_ptr<std::string>, const boost::system::error_code &, std::size_t);
    
    void sendMessage(boost::asio::ip::udp::endpoint &host, std::string &&errorM);

    void demultiplex(boost::asio::ip::udp::endpoint client, std::string command);

    void createGame(boost::asio::ip::udp::endpoint &host);
    void joinGame(boost::asio::ip::udp::endpoint &host, int gameNum);
    void waitGame(boost::asio::ip::udp::endpoint &client, gameInfo &game);
    void runGame(gameInfo &game);

    void queueInput(boost::asio::ip::udp::endpoint &client, gameInfo &game, playerCommands input);

};