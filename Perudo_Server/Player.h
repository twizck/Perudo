#pragma once
#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <thread>

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
private:
	const int MAXDATASIZE;
	int id;
	bool is_ready;
	int nbDice;

#ifdef _WIN32
	SOCKET socket;
#else
	int socket;
#endif
	std::thread thread;
	char* buffer;
	char* playerDice;
	bool playersAllReady = false;

	bool send_message(const char*);
	//bool send_messageToAllPlayers(std::vector<SOCKET> allPlayerSocket);
	int recv_message();
	bool close_socket();
	void execute_thread();


public:

#ifdef _WIN32
	Player(int, SOCKET, const int MAXDATASIZE);
#else
	Player(int, int, const int MAXDATASIZE);
#endif
	~Player();

	void start_thread();
	void end_thread();
	void join_thread();

	bool checkReady();
	int getIdPlayer();
	SOCKET getSocketPlayer();

	void giveDice(int nbdice);

};

#endif


