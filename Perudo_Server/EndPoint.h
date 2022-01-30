
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
#include <vector>
#include "Player.h"
#include "Game.h"

#ifndef ENDPOINT_H
#define ENDPOINT_H

class EndPoint
{
private:
	const int MAXDATASIZE;
	const int BACKLOG;
	int connection_port;
	bool is_alive;
	bool init_winsocks;

#ifdef _WIN32
	SOCKET connection_socket;
#else
	int connection_socket;
#endif
	std::thread thread;

	bool open();
#ifdef _WIN32
	SOCKET accept_connection();
#else
	int accept_connection();
#endif
	bool close();
	void execute_thread();

public:

	EndPoint(int, const int, const int, bool); 
	~EndPoint();

	void start_thread();
	void end_thread();
	void join_thread();

	std::vector<Player*> players;
	
};

#endif
