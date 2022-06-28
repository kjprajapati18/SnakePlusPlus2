#pragma once
#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

string read_(tcp::socket & socket);
void send_(tcp::socket & socket, const string& message);