#pragma once

#include <unordered_map>
#include <mutex>
#include <boost/asio.hpp>

enum playerCommands
{
      UP,
      DOWN,
      RIGHT,
      LEFT,
      SHOOT,
      QUIT,
};

class udpHash {
    public:
        std::size_t operator()(const boost::asio::ip::udp::endpoint& endpoint) const;
    private:
};

class playerInfo
{
      public:
            boost::asio::ip::udp::endpoint playerEndpoint;
            int gameNumber, playerNumber;
            int ping;
            int x,y;

            playerInfo(int gameNumber, int playerNumber, boost::asio::ip::udp::endpoint&& playerEndpoint);

      private:
};

class gameInfo
{
      public:
            int nextPlayer;
            int gameNumber;
            bool gameStarted;
            boost::asio::ip::udp::endpoint& host;
            std::unordered_map<boost::asio::ip::udp::endpoint, std::shared_ptr<playerInfo>, udpHash> players;

            std::mutex eventMtx;

            std::condition_variable cv;
            std::mutex startM;

            std::vector<std::pair<int, playerCommands>> eventQueue;
            
            gameInfo(int gameNumber, boost::asio::ip::udp::endpoint& host);

};

static int nextGameNumber = 1;
extern std::unordered_map<std::string, playerCommands> playerCommandsDict;
