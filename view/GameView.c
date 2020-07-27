////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
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
#include "Map.h"
#include "Places.h"
// add your own #includes here

#include "string.h"
// TODO: ADD YOUR OWN STRUCTS HERE



typedef struct{
	Player player;
	int health;
	PlaceId currLoc; 
	char *pastPlays;
}Players;

struct gameView {
	Map map;
	int CurrentScore;
	Round round;
	Players **players;
	Message *messages;
	char *pastGamePlays;
	// TODO: ADD FIELDS HERE
};





////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	new->map = MapNew();					// initialise the new map
	new->round = 0;							// initialise round number to 1
	new->CurrentScore = GAME_START_SCORE;	// initialise game score to START_SCORE
	new->players = malloc(5*sizeof(Players));
	
	// intialise the players
	for (int i = 0; i < 5; i++){
		Players *newPlayer = malloc(sizeof(*newPlayer));
		if (i == 0) {
			newPlayer->player = PLAYER_LORD_GODALMING;
		} else if (i == 1) {
			newPlayer->player = PLAYER_DR_SEWARD;
		} else if (i == 2) {
			newPlayer->player = PLAYER_VAN_HELSING;
		} else if (i == 3) {
			newPlayer->player = PLAYER_MINA_HARKER;
		} else {
			newPlayer->player = PLAYER_DRACULA;
		}
		if (i <= 3) {
			newPlayer->health = GAME_START_HUNTER_LIFE_POINTS;
		} else {
			newPlayer->health = GAME_START_BLOOD_POINTS;
		}
		newPlayer->currLoc = NOWHERE;
		new->players[i] = newPlayer;
	}

	new->pastGamePlays = pastPlays;
	new->messages = messages;

	// process pastplays
	/*
	if (strlen(pastPlays) != 0) {
	
	}
	*/
	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	MapFree(gv->map);
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// to find the current player
	// gv->players[round%5]->player 
	return gv->players[gv->round%5]->player;
}

int GvGetScore(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->CurrentScore;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->players[player]->health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->players[player]->currLoc;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	//eg to look at a players past moves:
	// gv->players[LORD_GODALMIN]-> pastPlays

	int *move_array;
	move_array = (int *)malloc(n*sizeof(int)*100);
	
	if (move_array != NULL) {
		for (j = 0; j < 100; j++) {
			move_array[j] = 0;
		}
		int playerId = i%5;
		int gameRound = i/5;
		while (gameRound <= 5) {
			Players *currPlayer = new->players[playerId];
			if (currPlayer != NULL) {
				char *tmp = strdup(str);
				currPlayer->pastPlays[gameRound] = tmp;
			}
		}
	}

	//gv->players[player]->pastPlays;
	*numReturnedMoves = 0;
	while (numReturnedMoves < ) {
		numReturnedMoves++;
	}
	*canFree = false;
	return numReturnedMoves;
	
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
