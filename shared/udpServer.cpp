#include "udpServer.hpp"

using boost::asio::ip::udp;

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

void udp_server::handle_receive(const boost::system::error_code &error, std::size_t bytesRead)
{
    if (!error)
    {
        std::thread(&udp_server::demultiplex, this, remote_endpoint_, std::string(recv_buffer_.data()).substr(0, bytesRead)).detach();
        start_receive();
    }
}

void udp_server::handle_send(boost::shared_ptr<std::string>, const boost::system::error_code &, std::size_t) {}

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

void udp_server::sendMessage(boost::asio::ip::udp::endpoint &host, std::string &&errorM)
{
    boost::shared_ptr<std::string> message(new std::string(std::move(errorM)));
    socket_.async_send_to(boost::asio::buffer(*message), host,
                          boost::bind(&udp_server::handle_send, this, message,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void udp_server::createGame(boost::asio::ip::udp::endpoint &host)
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

    sendMessage(host, str(boost::format("Success: You created game %1%!") % std::to_string(gameNumber)));
}

void udp_server::joinGame(boost::asio::ip::udp::endpoint &host, int gameNum)
{

    if (playerDict.count(host))
        return sendMessage(host, "Error: You cannot join a game since you are a part of another!");
    if (!gameDict.count(gameNum))
        return sendMessage(host, "Error: That game does not exist!");

    std::shared_ptr<gameInfo> game = gameDict[gameNum];
    std::shared_ptr<playerInfo> p(new playerInfo(gameNum, game->nextPlayer++, std::move(host)));
    game->players[p->playerEndpoint] = p;

    playerDict[p->playerEndpoint] = gameNum;

    sendMessage(host, str(boost::format("Success: You joined game %1%!") % std::to_string(gameNum)));
}

void udp_server::waitGame(boost::asio::ip::udp::endpoint &client, gameInfo &game)
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

void udp_server::runGame(gameInfo &game)
{
    game.gameStarted = true;
    game.cv.notify_all();

    int count = 0;
    while (count < 5)
    {
        std::cout << "Game " << game.gameNumber << " is running\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        count++;
    }
}