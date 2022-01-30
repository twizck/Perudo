
#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <vector>

#include "Output.h"
#include "Player.h"
#include "EndPoint.h"


// Valeurs constantes utilisées dans le programme
// ----------------------------------------------------------------------------------------------------------------

// Nombre maximal de connexions en attente
#define BACKLOG 6

// Nombre maximal d'octets à envoyer ou à lire en une fois
#define MAXDATASIZE 100


// Prototypes des fonctions disponibles dans le programme
// (l'implémentation est faite dans ce fichier)
// ----------------------------------------------------------------------------------------------------------------

bool strtoi(char*, int*);


// Variables globales
// ----------------------------------------------------------------------------------------------------------------

// Liste des threads en cours d'exécution
std::vector<Player*> players;

// Fonctions
// ----------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    Output::GetInstance()->print("*********************************************************\n");
    Output::GetInstance()->print("*           Simple socket server application            *\n");
    Output::GetInstance()->print("*********************************************************\n");
    Output::GetInstance()->print("\n");
    Output::GetInstance()->print("\n");

    // Lecture des paramètres en cours
    int port;
    if (argc != 2 || !strtoi(argv[1], &port)) {
        Output::GetInstance()->print("[MAIN] Invalid parameters !\n");
        Output::GetInstance()->print("[MAIN] program usage : ", argv[0], " connection_port\n");
        exit(EXIT_FAILURE);
    }

    Output::GetInstance()->print("[MAIN] If you want to stop the server, you have to enter 'EXIT'.\n");

    // Création du thread du serveur
    Output::GetInstance()->print("[MAIN] Creating server end point...\n");
    EndPoint connection(port, BACKLOG, MAXDATASIZE, true);
    connection.start_thread();
    
    // Boucle infinie pour ne pas arrêter le programme serveur
    while (1)
    {
        char buffer[MAXDATASIZE];
        std::cin >> buffer;

        // Traitement de la commande reçue
        
        if (strcmp(buffer, "EXIT") == 0 && Output::GetInstance()->confirm_exit()) {
            // On quitte la boucle principale
            break;
        }
        else {
            Output::GetInstance()->print("[MAIN] ", buffer, " is not recognized as a valid command", "\n");
        }
    }

    // Destruction du thread du serveur
    connection.~EndPoint();

    Output::GetInstance()->print("[MAIN] Main program ends.\n");

    return EXIT_SUCCESS;
}

bool strtoi(char* value, int* result)
{
    char* p;
    errno = 0;
    long arg = strtol(value, &p, 10);
    if (*p != '\0' || errno != 0) {
        return false;
    }

    if (arg < INT_MIN || arg > INT_MAX) {
        return false;
    }

    *result = arg;
    return true;
}