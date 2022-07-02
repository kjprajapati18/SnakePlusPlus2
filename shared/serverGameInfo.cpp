#include "serverGameInfo.hpp"

using boost::asio::ip::udp;

std::unordered_map<std::string, playerCommands> playerCommandsDict({
      {"up", UP},
      {"down", DOWN},
      {"right", RIGHT},
      {"left", LEFT},
      {"shoot", SHOOT},
      {"quit", QUIT},
});

// udpHash Class
std::size_t udpHash::operator()(const udp::endpoint& endpoint) const{
    std::ostringstream stream;
    stream << endpoint;
    std::hash<std::string> hasher;

    return hasher(stream.str());
}

// playerInfo class
playerInfo::playerInfo(int gameNumber, int playerNumber, udp::endpoint&& playerEndpoint) :
    gameNumber(gameNumber), playerNumber(playerNumber), playerEndpoint(std::move(playerEndpoint)), 
    ping(0), x(0), y(0)
{
}

// gameInfo class
gameInfo::gameInfo(int gameNumber, udp::endpoint& host) :
    gameNumber(gameNumber), host(host), nextPlayer(2), gameStarted(false)
{}