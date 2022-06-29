# SnakePlusPlus2
Reattempting the game from a more bare bones perspective to add more features, including multiplayer.

Requirements:
  Boost 1.71.0
  Cmake 3.16.3 or higher

# To run game:
  1. Clone repository
  2. Open command line to <repo>/build directory
  3. Run the following commands:
      cmake ../
      cmake --build .
  
  The server and client binaries should now be compiled.
 
# Server:
Run the server binary (cd to build directory and run ./server). The server is now ready to accept client

# Client:
Run the client binary (cd to build directory and run ./client <host> <port>).

The host is the ip address of the server (127.0.0.1 if you are running locally). The port is 3000 by default (will be able to change in future versions).
The client will wait for the user to type commands if successfully connected.

Commands-
  1. host
        This will create a new game on the server and make the client the host of this game.
        Clients can only be a part of 1 game, so they can no longer join/host another game without quitting.
  2. join <gameNumber>
        This will let the client join the given game number.
        Clients can only be a part of 1 game, so they can no longer join/host another game without quitting. If the game does not exist, an error message is returned.
  3. Inputs <to be implemented>
        Enter UP, DOWN, LEFT, RIGHT, or SHOOT to perform an action.
