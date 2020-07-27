////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct{
	Player player;
	int health;
	PlaceId currLoc; 
	char **pastPlays;
	int numTurns;
}Players;

struct hunterView {
	// TODO: ADD FIELDS HERE
	Map map;
	Round round;
	Players **players;
	Message *messages;
	int CurrentScore;
	char *pastGamePlays;
	int turnCounter;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}
	
	new->map = MapNew();
	new->round = 0;
	new->CurrentScore = GAME_START_SCORE;
	new->players = malloc(4*sizeof(Players));
	new->turnCounter = 0;

	//Initialising all players
	for(int i = 0; i < 5; i++) {
		Players *newPlayer = malloc(sizeof(*newPlayer));
		if(i == 0) newPlayer->player = PLAYER_LORD_GODALMING;
		else if (i == 1) newPlayer->player = PLAYER_DR_SEWARD;
		else if (i == 2) newPlayer->player = PLAYER_VAN_HELSING;
		else if (i == 3) newPlayer->player = PLAYER_MINA_HARKER;
		else if (i == 4) newPlayer->player = PLAYER_DRACULA;
		if (i == 4) newPlayer->health = GAME_START_BLOOD_POINTS;
		else newPlayer->health = GAME_START_HUNTER_LIFE_POINTS;
		newPlayer->currLoc = NOWHERE;
		newPlayer->pastPlays = malloc(sizeof *newPlayer->pastPlays);
		newPlayer->numTurns = 0;
		new->players[i] = newPlayer;
	}
	
	new->messages = messages;
	return new;
}

void HvFree(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	for (int i = 0; i < 5; i++) {
		printf("freeing %d past turns\n", hv->players[i]->numTurns);
		for (int h = 0; (h < (hv->players[i]->numTurns)); h++) {
			printf("freed pastplay[%d]: %s\n", h, hv->players[i]->pastPlays[h]);
			free(hv->players[i]->pastPlays[h]);
		}	
		// free(gv->players[i]->pastPlays);
		printf("freeing player\n");
		free(hv->players[i]);
	}
	printf("flag\n");
	MapFree(hv->map);
	
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return hv->round;
}

Player HvGetPlayer(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return hv->players[hv->turnCounter%5]->player;
}

int HvGetScore(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return hv->CurrentScore;
}

int HvGetHealth(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return hv->players[player]->health;
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return hv->players[player]->currLoc;
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*round = 0;
	return NOWHERE;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*pathLength = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
