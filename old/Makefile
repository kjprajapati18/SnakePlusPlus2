all:
	g++ -o server server.cpp comm.cpp -I/mnt/c/libscpp/boost_1_79_0 -lboost_system -pthread
	g++ -o client client.cpp comm.cpp -I/mnt/c/libscpp/boost_1_79_0 -lboost_system -pthread

windows:
	g++ -o server.exe server.cpp -IC:/libscpp/boost_1_79_0 -lboost_system
	g++ -o client.exe client.cpp -IC:/libscpp/boost_1_79_0 -lboost_system

clean:
	rm server client