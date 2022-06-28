#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "comm.hpp"

using boost::asio::ip::udp;

static int count = 1;

enum playerCommands
{
      UP,
      DOWN,
      RIGHT,
      LEFT,
      SHOOT,
      QUIT,
};
std::unordered_map<std::string, playerCommands> playerCommandsDict({
      {"up", UP},
      {"down", DOWN},
      {"right", RIGHT},
      {"left", LEFT},
      {"shoot", SHOOT},
      {"quit", QUIT},
});

class udpHash {
      public:
            std::size_t operator()(const udp::endpoint& endpoint) const{
                  std::ostringstream stream;
                  stream << endpoint;
                  std::hash<std::string> hasher;

                  return hasher(stream.str());
            }
};

class playerInfo
{
      public:
            playerInfo(int gameNumber, int playerNumber, udp::endpoint&& playerEndpoint) :
                  gameNumber(gameNumber), playerNumber(playerNumber), playerEndpoint(std::move(playerEndpoint)), 
                  ping(0), x(0), y(0)
            {}

            udp::endpoint playerEndpoint;
            int gameNumber, playerNumber;
            int ping;
            int x,y;
      private:


};

class gameInfo
{
      public:
            int nextPlayer = 2;
            int gameNumber;
            udp::endpoint& host;
            std::unordered_map<udp::endpoint, std::shared_ptr<playerInfo>, udpHash> players;

            std::mutex eventMtx;
            std::vector<std::pair<int, playerCommands>> eventQueue;
            
            gameInfo(int gameNumber, udp::endpoint& host) :
                  gameNumber(gameNumber), host(host)
            {}

};

class udp_server
{
      public:
            udp_server(boost::asio::io_context& io_context)
                  : socket_(io_context, udp::endpoint(udp::v4(), 3000)),
                  gameDict({})
            {
                  start_receive();
            }
      
      private:
            void start_receive()
            {
                  std::cout << "Waiting for Data\n";
                  socket_.async_receive_from(
                        boost::asio::buffer(recv_buffer_), remote_endpoint_,
                        boost::bind(&udp_server::handle_receive, this,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred)
                  );
            }

            void handle_receive(const boost::system::error_code& error, std::size_t bytesRead)
            {
                  if(!error)
                  {
                        std::thread(&udp_server::demultiplex, this, remote_endpoint_, std::string(recv_buffer_.data()).substr(0,bytesRead)).detach();
                        for(auto i = playerDict.begin(); i != playerDict.end(); ++i){
                              std::cout << i->first << ", " << i->second << "\n";
                        }
                        start_receive();
                  }
            }

            void handle_send(boost::shared_ptr<std::string>,
                  const boost::system::error_code&, std::size_t)
                  {}

            void demultiplex(udp::endpoint client, std::string command) {
                  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                  std::cout << client << " + " << command << "\n";

                  if(command == "host") {
                        createGame(client);
                        return;
                  }
                  if(command.length() >4 && command.substr(0,4) == "join"){
                        int gameNumber = std::stoi(command.substr(4));
                        if(gameNumber >0) {
                              joinGame(client, gameNumber);
                              return;
                        }
                  }
                  auto input = playerCommandsDict.find(command);
                  auto gameNum = playerDict.find(client);
                  if(gameNum != playerDict.end() && input != playerCommandsDict.end()){
                        std::shared_ptr<gameInfo> game = gameDict.find(gameNum->second)->second;
                        queueInput(client, *game, input->second);
                        return;
                  }
                  
                  boost::shared_ptr<std::string> message (new std::string("Hi from server! Bad Command"));
                  socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                        boost::bind(&udp_server::handle_send, this, message,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
            }

            void queueInput(udp::endpoint& client, gameInfo& game, playerCommands input){
                  int playerNum = game.players.find(client)->second->playerNumber;
                  game.eventMtx.lock();
                  game.eventQueue.emplace_back(std::pair<int, playerCommands>(playerNum, input));
                  game.eventMtx.unlock();
            }

            void sendError(udp::endpoint& host, string&& errorM){
                  boost::shared_ptr<std::string> message (new std::string(std::move(errorM)));
                  socket_.async_send_to(boost::asio::buffer(*message), host,
                              boost::bind(&udp_server::handle_send, this, message,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred));
            }

            void createGame(udp::endpoint& host){
                  //Set up stuff with playerDict & gameDict && gameInfo && playerInfo
                  if(playerDict.count(host)) {
                        sendError(host, "You cannot host a game since you are a part of another!");
                        return;
                  }
                  
                  int gameNumber = count++;
                  
                  std::shared_ptr<playerInfo> p1(new playerInfo(gameNumber, 1, std::move(host)));
                  std::shared_ptr<gameInfo> game(new gameInfo(gameNumber, p1->playerEndpoint));

                  gameDict[gameNumber] = game;
                  game->players[p1->playerEndpoint] = p1;
                  playerDict[p1->playerEndpoint] = gameNumber;

                  boost::shared_ptr<std::string> message (new std::string(str(boost::format("You created game %1%!") % std::to_string(gameNumber))));
                  socket_.async_send_to(boost::asio::buffer(*message), host,
                              boost::bind(&udp_server::handle_send, this, message,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred));
                  runGame();
            }
            void joinGame(udp::endpoint& host, int gameNum){

                  if(playerDict.count(host)) {
                        sendError(host, "You cannot host a game since you are a part of another!");
                        return;
                  }
                  if(!gameDict.count(gameNum)) {
                        sendError(host, "That game does not exist!");
                        return;
                  }

                  std::shared_ptr<gameInfo> game = gameDict[gameNum];
                  std::shared_ptr<playerInfo> p(new playerInfo(gameNum, game->nextPlayer++, std::move(host)));
                  game->players[p->playerEndpoint] = p;

                  playerDict[p->playerEndpoint] = gameNum;

                  boost::shared_ptr<std::string> message (new std::string(str(boost::format("You joined game %1%!") % std::to_string(gameNum))));
                  socket_.async_send_to(boost::asio::buffer(*message), host,
                              boost::bind(&udp_server::handle_send, this, message,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred));
            }
            void runGame() {
                  std::this_thread::sleep_for(std::chrono::milliseconds(4000));
            }

            udp::socket socket_;
            udp::endpoint remote_endpoint_;

            boost::array<char,16> recv_buffer_;
            
            std::unordered_map<udp::endpoint, int, udpHash> playerDict;
            std::unordered_map<int, std::shared_ptr<gameInfo>> gameDict;
};


int main() {
      try {
            boost::asio::io_context io_context;
            udp_server server(io_context);
            io_context.run();

      }
      catch (std::exception& e){
            std::cerr << e.what() << std::endl;
      }

  return 0;
}