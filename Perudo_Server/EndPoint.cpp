
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
#include "EndPoint.h"
#include "Player.h"
#include "Output.h"


EndPoint::EndPoint(int connection_port, const int BACKLOG, const int MAXDATASIZE, bool init_winsocks) : connection_port(connection_port), BACKLOG(BACKLOG), init_winsocks(init_winsocks), MAXDATASIZE(MAXDATASIZE), connection_socket(NULL), is_alive(true)
{
}

EndPoint::~EndPoint()
{
    end_thread();
}

#ifdef _WIN32
bool EndPoint::open()
{
    SOCKADDR_IN address;

    // Initialisation l'utilisation des WinSocks par un processus
    if (init_winsocks) {
        WSADATA WSAData;
        if (WSAStartup(MAKEWORD(2, 0), &WSAData) == -1) {
            Output::GetInstance()->print_error("[SERVER] Error while starting using WinSocks ");
            return false;
        }
    }

    // Ouverture de la socket de connexion
    if ((connection_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while creating connection socket ");
        return false;
    }

    // Configuration de l'adresse de transport
    address.sin_addr.s_addr = INADDR_ANY;      // adresse, devrait être converti en reseau mais est egal à 0
    address.sin_family = AF_INET;              // type de la socket
    address.sin_port = htons(connection_port); // port, converti en reseau

    // Demarrage du point de connexion : on ajoute l'adresse de transport dans la socket
    if (bind(connection_socket, (SOCKADDR*)&address, sizeof(address)) == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while binding connection socket ");
        return false;
    }

    // Attente sur le point de connexion
    if (listen(connection_socket, BACKLOG) == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while listening connection socket ");
        return false;
    }

    return true;
}

SOCKET EndPoint::accept_connection()
{
    SOCKET player_socket;
    SOCKADDR_IN player_address;
    int sinsize = sizeof(player_address);

    // Acceptation de la connexion
    if ((player_socket = accept(connection_socket, (SOCKADDR*)&player_address, &sinsize)) == -1) {
        if (!is_alive)
            return NULL;
        Output::GetInstance()->print_error("[SERVER] Error while accepting client connection ");
        return NULL;
    }

    if (!is_alive)
        return NULL;

    // Affichage de la connexion
    Output::GetInstance()->print("[SERVER][+] New connection from ", inet_ntoa(player_address.sin_addr), "\n");

    return player_socket;
}
#else
int EndPoint::open()
{
    int connection_socket;
    struct sockaddr_in address;
    int yes = 1;

    // Ouverture de la socket de connexion
    if ((connection_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while creating connection socket ");
        return false;
    }

    // Configuration de la socket de connexion
    if (setsockopt(connection_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while configuring connection socket ");
        return false;
    }

    // Configuration de l'adresse de transport        
    address.sin_addr.s_addr = INADDR_ANY;      // adresse, devrait être converti en reseau mais est egal à 0
    address.sin_family = AF_INET;              // type de la socket
    address.sin_port = htons(connection_port); // port, converti en reseau
    bzero(&(address.sin_zero), 8);             // mise a zero du reste de la structure

    // Demarrage du point de connexion : on ajoute l'adresse de transport dans la socket
    if (bind(connection_socket, (struct sockaddr*)&address, sizeof(struct sockaddr)) == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while binding connection socket ");
        return false;
    }

    // Attente sur le point de connexion
    if (listen(connection_socket, BACKLOG) == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while listening connection socket ");
        return false;
    }

    return true;
}

int accept_connection()
{
    int player_socket;
    struct sockaddr_in player_address;
    unsigned int sinsize = sizeof(struct sockaddr_in);

    // Acceptation de la connexion
    if ((player_socket = accept(connection_socket, (struct sockaddr*)&player_address, &sinsize)) == -1) {
        if (!is_alive)
            return NULL;
        Output::GetInstance()->print_error("[SERVER] Error while accepting client connection ");
        return NULL;
    }

    if (!is_alive)
        return NULL;

    // Affichage de la connexion
    Output::GetInstance()->print("[SERVER][+] New connection from ", inet_ntoa(player_address.sin_addr), "\n");

    return player_socket;
}
#endif

bool EndPoint::close()
{
    if (connection_socket == NULL || !is_alive)
        return true;

    int result;
    Output::GetInstance()->print("[SERVER] Trying to close connection socket...\n");

#ifdef _WIN32
    result = closesocket(connection_socket);
#else
    result = close(connection_socket);
#endif

    if (result == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while closing connection socket ");
        Output::GetInstance()->print("\n");
        return false;
    }

#ifdef _WIN32
    if (init_winsocks && WSACleanup() == -1) {
        Output::GetInstance()->print_error("[SERVER] Error while cleaning WinSocks ");
        Output::GetInstance()->print("\n");
        return false;
    }
#endif

    Output::GetInstance()->print("[SERVER] Connection socket closed successfully !\n");

    return true;
}

void EndPoint::execute_thread()
{
    Output::GetInstance()->print("[SERVER] Thread server starts.\n");

    // Ouverture de la socket de connexion
    Output::GetInstance()->print("[SERVER] Trying to open connection socket on the port ", connection_port, "...\n");
    if (!open()) {
        exit(EXIT_FAILURE);
    }
    Output::GetInstance()->print("[SERVER] Connection socket opened successfully !\n");

    // Boucle infinie pour le serveur (pour accepter les connexions entrantes)
    int threads_count = 0;
    Player* p;
    while (1)
    {
        if (!is_alive)
            return;

        Output::GetInstance()->print("[SERVER] Waiting for client connection...\n");

        threads_count++;
#ifdef _WIN32
        SOCKET player_socket = accept_connection();
#else
        int player_socket = accept_connection();
#endif
        if (!is_alive)
            return;

        if (player_socket != NULL) {
            
            p = new Player(threads_count, player_socket, MAXDATASIZE);
            if (!is_alive) {
                p->~Player();
                return;
            }

            p->start_thread();

            players.push_back(p);

            Game game;
            game.GetInstance()->AddPlayers(p);
            
        }
    }
}

void EndPoint::start_thread()
{
    join_thread();
    // Start server thread
    thread = std::thread(&EndPoint::execute_thread, this);
}

void EndPoint::end_thread()
{
    if (!is_alive)
        return;

    Output::GetInstance()->print("[SERVER] Thread server is ending...\n");

    is_alive = false;

    // End all players
    for (std::vector<Player*>::iterator it = players.begin(); it != players.end(); ++it) {
        (*it)->~Player();
    }

    // End thread
    thread.detach();
    thread.~thread();

    // Close end point connection
    close();

    Output::GetInstance()->print("[SERVER] Thread server ends.\n");
}

void EndPoint::join_thread()
{
    if (thread.joinable()) {
        thread.join();
    }
}


#include "EndPoint.h"
