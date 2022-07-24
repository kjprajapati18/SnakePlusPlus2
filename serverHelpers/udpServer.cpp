#include "udpServer.h"
#include <chrono>
#include <sstream>

using boost::asio::ip::udp;
using boostError = boost::system::error_code;

udp_server::udp_server(boost::asio::io_context &io_context)
    : socket_(io_context, udp::endpoint(udp::v4(), 3000)),
      gameDict({})
{
    start_receive();
}

//
// Private Functions
//
void udp_server::start_receive()
{
    std::cout << "Waiting for Data\n";
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_server::handle_receive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void udp_server::handle_receive(const boostError &error, std::size_t bytesRead)
{
    if (!error)
    {
        std::thread(&udp_server::demultiplex, this, remote_endpoint_, std::string(recv_buffer_.data()).substr(0, bytesRead)).detach();
        start_receive();
    }
}

void udp_server::handle_send(boost::shared_ptr<std::string>, const boostError &, std::size_t) {}

void udp_server::demultiplex(udp::endpoint client, std::string command)
{
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << client << " + " << command << "\n";

    if (command == "host")
        return createGame(client);

    if (command.length() > 4 && command.substr(0, 4) == "join")
    {
        int gameNumber = std::stoi(command.substr(4));
        if (gameNumber > 0)
            return joinGame(client, gameNumber);
    }

    // Player should exists at this point
    auto player = playerDict.find(client);
    if (player == playerDict.end())
        return sendMessage(client, "Error: Bad Command");

    int gameNumber = player->second;
    gameInfo &game = *(gameDict.find(gameNumber)->second);

    if (command == "start")
    {
        sendMessage(client, "Success: Game starting...");
        return runGame(game);
    }
    if (command == "status")
    {
        return waitGame(client, game);
    }

    auto input = playerCommandsDict.find(boost::algorithm::to_lower_copy(command));
    auto gameNum = playerDict.find(client);
    if (gameNum != playerDict.end() && input != playerCommandsDict.end())
    {
        std::shared_ptr<gameInfo> game = gameDict.find(gameNum->second)->second;
        return queueInput(client, *game, input->second);
    }

    return sendMessage(client, "Error: Bad Command");
}

void udp_server::queueInput(udp::endpoint &client, gameInfo &game, playerCommands input)
{
    if (!game.gameStarted)
        return sendMessage(client, "Error: Game has not started");

    int playerNum = game.players.find(client)->second->playerNumber;
    {
        std::lock_guard<std::mutex> eventLock(game.eventMtx);
        game.eventQueue.emplace_back(std::pair<int, playerCommands>(playerNum, input));
    }
}

void udp_server::sendMessage(udp::endpoint &host, std::string &&errorM)
{
    boost::shared_ptr<std::string> message(new std::string(std::move(errorM)));
    socket_.async_send_to(boost::asio::buffer(*message), host,
                          boost::bind(&udp_server::handle_send, this, message,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void udp_server::createGame(udp::endpoint &host)
{
    // Set up stuff with playerDict & gameDict && gameInfo && playerInfo
    if (playerDict.count(host))
        return sendMessage(host, "Error: You cannot host a game since you are a part of another!");

    int gameNumber = nextGameNumber++;

    std::shared_ptr<playerInfo> p1(new playerInfo(gameNumber, 1, std::move(host)));
    std::shared_ptr<gameInfo> game(new gameInfo(gameNumber, p1->playerEndpoint));

    gameDict[gameNumber] = game;
    game->players[p1->playerEndpoint] = p1;
    playerDict[p1->playerEndpoint] = gameNumber;

    randomizeSpawn(*game, *p1);

    sendMessage(host, str(boost::format("Success: You created game %1%!") % std::to_string(gameNumber)));
}

void udp_server::joinGame(udp::endpoint &host, int gameNum)
{

    if (playerDict.count(host))
        return sendMessage(host, "Error: You cannot join a game since you are a part of another!");
    if (!gameDict.count(gameNum))
        return sendMessage(host, "Error: That game does not exist!");

    std::shared_ptr<gameInfo> game = gameDict[gameNum];
    std::shared_ptr<playerInfo> p(new playerInfo(gameNum, game->nextPlayer++, std::move(host)));
    game->players[p->playerEndpoint] = p;

    randomizeSpawn(*game, *p);

    playerDict[p->playerEndpoint] = gameNum;

    sendMessage(host, str(boost::format("Success: You joined game %1% as player %2%!") % std::to_string(gameNum) % std::to_string(p->playerNumber)));
}

void udp_server::waitGame(udp::endpoint &client, gameInfo &game)
{
    std::unique_lock<std::mutex> startLock(game.startM);

    if (game.cv.wait_for(startLock, std::chrono::milliseconds(5000), [&]()
                         { return game.gameStarted; }))
    {
        return sendMessage(client, "start");
    }

    return sendMessage(client, "waiting");
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

// DEBUGGING ONLY
void prettyPrint(std::array<std::array<int,8>,8> board)
{
    for(int y = 0; y < 8; y++)
    {
        std::cout << "[ ";
        for(int x = 0; x < 8; x++)
        {
            std::cout << board[x][y] << " ";
        }
        std::cout << "]\n";
    }
}

void udp_server::runGame(gameInfo &game)
{
    using namespace std::chrono;
    // HANDLE ACTUAL UPDATES
    game.gameStarted = true;
    game.cv.notify_all();

    int tickRate = 2;
    float MS_PER_TICK = 1000 / tickRate;

    uint64_t initTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint64_t delta = 0;
    int count = 0;

    while (true)
    {
        uint64_t curTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        delta += curTime - initTime;
        initTime = curTime;
        if (delta >= MS_PER_TICK)
        {
            delta -= MS_PER_TICK;
            // Perform Update here
            if(count >= 10) {
                std::cout << "Game " << game.gameNumber << " is running\n";
                count -= 10;
            }
            sendUpdateToPlayers(game);
            count++;
            //  TODO:: Check eventQueue population
            //
            // for(int i = 0; i < game.eventQueue.size(); i++){
            //     std::cout << game.eventQueue.at(i).first << "," << game.eventQueue.at(i).first << " ";
            // }
            // std::cout << "\n";
            // prettyPrint(game.board); //For debugging
        }
    }
}

void udp_server::sendUpdateToPlayers(gameInfo &game)
{
    std::stringstream ss;
    auto firstPlayer = game.players.begin();
    for (auto iter = firstPlayer; iter != game.players.end(); iter++)
    {
        if (iter != firstPlayer)
            ss << " ";
        auto player = iter->second; // pointer to PlayerInfo Struct
        int pNum = player->playerNumber;
        int pX = player->x;
        int pY = player->y;
        ss << pNum << "," << pX << "," << pY;
    }
    
    for (auto iter = game.players.begin(); iter != game.players.end(); iter++)
    {
        udp::endpoint &playerEndpoint = iter->second->playerEndpoint; // pointer to PlayerInfo Struct
        sendMessage(playerEndpoint, ss.str());
    }
}

void udp_server::randomizeSpawn(gameInfo &game, playerInfo &player)
{
    int x, y;
    do
    {
        x = std::rand() % 8;
        y = std::rand() % 8;
        std::cout << x << ',' << y << '\n';
    } while (game.board[x][y] != 0);
    game.board[x][y] = player.playerNumber;
    player.x = x;
    player.y = y;
    prettyPrint(game.board);
}