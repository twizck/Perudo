#pragma once
#include <vector>
#include "Player.h"

class Game
{

private:

	static Game* singleton_;

	int nbDiceMisee;
	int typeDyceMisee;
	int idJoueurEnCours;
	int leTour = 1;

public:

	Game();
	~Game();

	std::vector<Player*> joueurs;

	std::vector<SOCKET> allPlayersSocket;

	std::vector<int> allPlayersId;

	static Game* GetInstance();
	void AddPlayers(Player* p);
	bool allPlayerReady();

	void StartGame();

	void GetMise(int dice, int type, int idJoueur);

	bool tour(int idJoueur);

	//std::vector<int> GetAllPlayerId();
	//std::vector<SOCKET> getAllSocket(); 
	//bool send_message();

};

