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
#include <sstream>
#endif
#include <thread>
#include "Player.h"
#include "Output.h"
#include "EndPoint.h"
#include <regex>
#include "Game.h"

#ifdef _WIN32
Player::Player(int id, SOCKET socket, const int MAXDATASIZE) : id(id), socket(socket), MAXDATASIZE(MAXDATASIZE)
{
	buffer = new char[MAXDATASIZE];
}
#else
Player::Player(int id, int socket, const int MAXDATASIZE) : id(id), socket(socket), MAXDATASIZE(MAXDATASIZE), is_alive(true)
{
	buffer = new char[MAXDATASIZE];
}
#endif

Player::~Player()
{
	end_thread();
	delete[] buffer;
}

bool Player::close_socket()
{
	if (socket == NULL)
		return true;

	int result;
	Output::GetInstance()->print("[PLAYER_", id, "] Closing client socket...\n");

#ifdef _WIN32
	result = closesocket(socket);
#else
	result = close(socket);
#endif

	if (result == -1) {
		char* error = new char[MAXDATASIZE];
		sprintf(error, "[PLAYER_%d] Error while closing socket ", id);
		Output::GetInstance()->print_error(error);
		Output::GetInstance()->print("\n");
		delete[] error;
		return false;
	}
	else {
		Output::GetInstance()->print("[PLAYER_", id, "] Player socket closed successfully.\n");
	}

	return true;
}
bool Player::send_message(const char* buffer)
{
	if (socket == NULL)
		return false;

	if (send(socket, buffer, strlen(buffer), 0) == -1) {
		char* error = new char[MAXDATASIZE];
		sprintf(error, "[PLAYER_%d] Error while sending message to client ", id);
		Output::GetInstance()->print_error(error);
		Output::GetInstance()->print("\n");
		delete[] error;
		return false;
	}

	return true;
}

int Player::recv_message()
{
	if (socket == NULL)
		return -1;

	int length;
	if ((length = recv(socket, buffer, MAXDATASIZE, 0)) == -1)
	{
		char* error = new char[MAXDATASIZE];
		sprintf(error, "[PLAYER_%d] Error while receiving message from client ", id);
		Output::GetInstance()->print_error(error);
		Output::GetInstance()->print("\n");
		delete[] error;
		return length;
	}

	// Suppression des retours chariots (\n et \r)
	while (length > 0 && (buffer[length - 1] == '\n' || buffer[length - 1] == '\r'))
		length--;
	// Ajout de backslash zero a la fin pour en faire une chaine de caracteres
	if (length >= 0 && length < MAXDATASIZE)
		buffer[length] = '\0';

	return length;
}

void Player::execute_thread()
{
	int length;
	time_t time_value;
	struct tm* time_info;
	std::regex pattern{ "\\d(D|d)\\d" };
	std::regex getNbDice{ "^\\d" };
	std::regex getDice{ "\\d$" };

	Output::GetInstance()->print("[PLAYER_", id, "] Thread client starts with id=", id, ".\n");

	// Boucle infinie pour le player
	while (1) {
		
		// On attend un message du player
		if ((length = recv_message()) == -1) {
			break;
		}

		// Affichage du message
		Output::GetInstance()->print("[PLAYER_", id, "] Message received : ", buffer, "\n");

		if (strcmp(buffer, "DISCONNECT") == 0) {
			break;
		}

		else
		{
			std::string target{ buffer };
			bool result = std::regex_match(target, pattern);
			Game game;
			// Traitement du message reçu

			if (strcmp(buffer, "READY") == 0 ) {
				
				if (!is_ready)
				{
					is_ready = true;

					playersAllReady = game.GetInstance()->allPlayerReady();
					Output::GetInstance()->print(playersAllReady);

					if (playersAllReady)
					{
						game.StartGame();
					}
				}

				else send_message("Tu est deja pret");
			}
			else if (strcmp(buffer, "THROW") == 0)
			{
				std::vector<int> resultDice;
				for (int i = 0; i < nbDice; i++)
				{
					int iRand = (rand() % 9) + 1;
					resultDice.push_back(iRand);
				}
				Output::GetInstance()->print(resultDice.data());

			}
			
			else if (result)
			{
				bool siTonTour = game.GetInstance()->tour(id);
				if (!siTonTour)
				{
					send_message("ce n'est pas ton tour");
				}

				else
				{
					std::string str = buffer;
					std::smatch m;
					std::smatch m2;
					std::regex_search(str, m, getNbDice);
					std::regex_search(str, m2, getDice);

					int nbDice = 0;
					int Dice = 0;

					for (auto v : m)
					{
						nbDice = stoi(v);
					}

					for (auto v : m2)
					{
						Dice = stoi(v);
					}

					game.GetInstance()->GetMise(nbDice, Dice, id);
					send_message("Mise correcte , en attente du joueur suivant");
				}	
			}

			else sprintf(buffer, "%s is not recognized as a valid command", buffer);

			if (socket == NULL) {
				return;
			}

			// On envoie le buffer
			Output::GetInstance()->print("[PLAYER_", id, "] Sending message \"", buffer, "\" to client...\n");
			if (!send_message(buffer)) {
				break;
			}

			Output::GetInstance()->print("[PLAYER_", id, "] Message \"", buffer, "\" send to client successfully.\n");
		}
	}

	end_thread();
}

void Player::start_thread()
{
	join_thread();
	// Start player thread
	thread = std::thread(&Player::execute_thread, this);
}

void Player::end_thread()
{

	Output::GetInstance()->print("[PLAYER_", id, "] Thread client is ending...\n");

	// Sending close connection to player
	send_message("CONNECTION_CLOSED");

	// End thread
	thread.detach();
	thread.~thread();

	// Close connection
	close_socket();

	Output::GetInstance()->print("[PLAYER_", id, "] Thread client ends.\n");
}

void Player::join_thread()
{
	if (thread.joinable()) {
		thread.join();
	}
}

bool Player::checkReady()
{
	return is_ready;
}

int Player::getIdPlayer()
{
	return id;
}

SOCKET Player::getSocketPlayer()
{
	return socket;
}

void Player::giveDice(int nbdice)
{
	nbDice = nbdice;
}

