#include <iostream>
#include <string>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::udp;
using ipAdd = boost::asio::ip::address;

class Board {
public:
    Board(){
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++) {
                board[i][j] = 0;
            }
        }
    }

    Board(const Board& other) {
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++) {
                board[i][j] = other.board[i][j];
            }
        }
    }

    bool operator==(const Board& other) {
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++) {
                if(board[i][j] != other.board[i][j]){
                    return false;
                }
            }
        }
        return true;
    }

    int board[8][8];
private:
};

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

void prettyPrint(Board board)
{
    std::cout << "\n";
    for(int y = 0; y < 8; y++)
    {
        std::cout << "[ ";
        for(int x = 0; x < 8; x++)
        {
            std::cout << board.board[x][y] << " ";
        }
        std::cout << "]\n";
    }
}

void parseAndUpdateBoard(Board& updatedBoard, std::string& res) {
    int tripleStart = 0;

    while(tripleStart <= res.length()) {
        int playerNumEnd = res.find(",", tripleStart);
        int playerXEnd = res.find(",", playerNumEnd+1);
        int playerYEnd = res.find(" ", playerXEnd+1);
        if(playerYEnd == std::string::npos)
            playerYEnd = res.length();

        int playerNum = std::stoi(res.substr(tripleStart, playerNumEnd));
        int playerX =  std::stoi(res.substr(playerNumEnd+1, playerXEnd));
        int playerY =  std::stoi(res.substr(playerXEnd + 1, playerYEnd));

        tripleStart = playerYEnd + 1;
        // std::cout << playerNum << "," << playerX << "," << playerY << " ";
        updatedBoard.board[playerX][playerY] = playerNum;
    }
    // std::cout << '\n';

}

void receiveUpdate(udp::socket& socket, Board& board, udp::endpoint& server_endpoint) {
    // FILL OUT THIS FUNCTION TO UPDATE AND PRINT BOARD
    boost::array<char, 128> recv_buf;
    std::string res;
    udp::endpoint sender_endpoint;

    while(board.board[0][0] != -1) {
        Board updatedBoard;
        receiveMessage(socket, server_endpoint, sender_endpoint, recv_buf, res);
        parseAndUpdateBoard(updatedBoard, res);
        
        if(updatedBoard == board)  continue;
        prettyPrint(updatedBoard);
        board = updatedBoard;
    }
}

void runClient(char** argv)
{
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

    // Get into a game
    while (input != "quit")
    {
        socket.send_to(boost::asio::buffer(input, input.length()), server_endpoint);

        std::cout << "Debug: Sent " << input << " to server!\n";

        receiveMessage(socket, server_endpoint, sender_endpoint, recv_buf, res);
        std::cout << "Debug: " << res << "\n";

        // Host started the game, or player joined successfully
        if (res.find("Success") != std::string::npos && input != "host")
            break;
        
        if (res.find("Success") != std::string::npos && input == "host")
            std::cout << "Game successfully created! Please enter 'start' to begin the game for all players\n";
        
        getline(std::cin, input);
    }

    // Wait for game to start
    while (res != "start" && input != "start")
    {
        std::cout << "Waiting for game to start ...\n";
        socket.send_to(boost::asio::buffer("status", 6), server_endpoint);

        receiveMessage(socket, server_endpoint, sender_endpoint, recv_buf, res);
    }

    Board board;
    //std::thread(receiveUpdate, socket, board, server_endpoint).detach(); // NEED TO FILL OUT THIS FUNCTION TO HANDLE SERVER RESPOSNE
    std::cout << "Game started!\n";


    // Rewrite code to utilize async_receive for threading
    // boost::array<char, 128> recvBuf;
    // socket.async_receive_from(
    //     boost::asio::buffer(recvBuf), server_endpoint,
    //     boost::bind(receiveUpdate, board, server_endpoint, recvBuf,
    //         boost::asio::placeholders::error,
    //         boost::asio::placeholders::bytes_transferred
    //     )
    // )

    receiveUpdate(socket, board, server_endpoint);

    // Get movements
    while (input != "quit")
    {
        socket.send_to(boost::asio::buffer(input, input.length()), server_endpoint);
        getline(std::cin, input);
    }

    socket.send_to(boost::asio::buffer("update", 6), server_endpoint);
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

        runClient(argv);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}