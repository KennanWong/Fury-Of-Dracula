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
	char **pastPlays;
}Players;

struct gameView {
	Map map;
	int CurrentScore;
	Round round;
	Players **players;
	Message *messages;
	char *pastGamePlays;
	int turnCounter;
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
	new->turnCounter = 0;

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
		newPlayer->pastPlays = malloc(sizeof *newPlayer->pastPlays);
		new->players[i] = newPlayer;
	
	}

	new->messages = messages;

	// process pastplays and seperate each to their respective players
	// This is done so by first tokenising the past plays string splitting them into 6 character strings
	// 'i' will be used to count the number of turns taken, this will be used to determine whose turn it is
	// (i%5) and what game round we are in (i/5), i will also be used to determine the turn counter.
	char *tempPastPlays = strdup(pastPlays);
	if (strlen(pastPlays) != 0) {
		// take each 6 char play seperately
		printf("checking previous players moves\n");
		char *str = strtok(tempPastPlays, " ");
		printf("str = %s\n", str);
		int i = 0;
		while (str != NULL) {
			// i%5 will determine who made the move
			int playerId = i%5;
			int gameRound = i/5;
			Players *currPlayer = new->players[playerId];
			
			// add the 6char string to the players past plays
			char *tmp = strdup(str);
			currPlayer->pastPlays[gameRound] = tmp;
			
			// used to concatenate the 6 char string to the 2char city abbreviation
			char placeAbbrev[2];
			for (int i = 1; i < 3; i++) {
				placeAbbrev[i-1] = str[i];
			}
			// Update the players current location
			currPlayer->currLoc = placeAbbrevToId(placeAbbrev);
			printf ("player %d went to %s\n", playerId, placeIdToName(placeAbbrevToId(placeAbbrev)));
			i+=1;
			str = strtok(NULL, " ");
			new->round = gameRound;
			new->turnCounter = i;
		}
		new->round = i/5;
		
	}
	
	new->pastGamePlays = tempPastPlays;
	free(tempPastPlays);
	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	for (int i = 0; i < 5; i++) {
		free(gv->players[i]->pastPlays);
		free(gv->players[i]);
	}
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
	return gv->players[gv->turnCounter%5]->player;
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
	PlaceId *target = malloc(gv->round*sizeof(PlaceId));
	int n = 0;
	while(n < gv->players[player]->numTurns) {
		char placeAbbrev[2];
		placeAbbrev[0] = gv->players[player]->pastPlays[n][1];
		placeAbbrev[1] = gv->players[player]->pastPlays[n][2];
		target[n] = placeAbbrevToId(placeAbbrev);

		//If the given abbrev is city move (unknown city)
		if(placeAbbrev[0] == 'C' && placeAbbrev[1] == '?') target[n] = CITY_UNKNOWN;

		//If the given abbrev is sea move (unknown sea)
		if(placeAbbrev[0] == 'S' && placeAbbrev[1] == '?') target[n] = SEA_UNKNOWN;

		//If the given abbrev is Dn
		if(placeAbbrev[0] == 'D' && placeAbbrev[1] != 'U') {
			if(placeAbbrev[1] == 49) target[n] = DOUBLE_BACK_1;
			if(placeAbbrev[1] == 50) target[n] = DOUBLE_BACK_2;
			if(placeAbbrev[1] == 51) target[n] = DOUBLE_BACK_3;
			if(placeAbbrev[1] == 52) target[n] = DOUBLE_BACK_4;
			if(placeAbbrev[1] == 53) target[n] = DOUBLE_BACK_5;
				
		}

		//If the given abbrev is HI
		if(placeAbbrev[0] == 'H' && placeAbbrev[1] == 'I') {
			target[n] = target[n-1];
		}
		n++;
	}
	*numReturnedMoves = gv->players[player]->numTurns;
	*canFree = false;
	return target;
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
	PlaceId *target = malloc(gv->round*sizeof(PlaceId));
	int n = 0;
	//Checking if the player is Dracula
	if(player == PLAYER_DRACULA) {
		
		while(gv->players[player]->pastPlays[n] != NULL) {
			char placeAbbrev[2];
			placeAbbrev[0] = gv->players[player]->pastPlays[n][1];
			placeAbbrev[1] = gv->players[player]->pastPlays[n][2];
			target[n] = placeAbbrevToId(placeAbbrev);

			//If the given abbrev is city move (unknown city)
			if(placeAbbrev[0] == 'C' && placeAbbrev[1] == '?') target[n] = CITY_UNKNOWN;

			//If the given abbrev is sea move (unknown sea)
			if(placeAbbrev[0] == 'S' && placeAbbrev[1] == '?') target[n] = SEA_UNKNOWN;

			//If the given abbrev is Dn
			if(placeAbbrev[0] == 'D' && placeAbbrev[1] != 'U') {
				if(placeAbbrev[1] == 49) target[n] = target[n-1];
				if(placeAbbrev[1] == 50) target[n] = target[n-2];
				if(placeAbbrev[1] == 51) target[n] = target[n-3];
				if(placeAbbrev[1] == 52) target[n] = target[n-4];
				if(placeAbbrev[1] == 53) target[n] = target[n-5];
			}

			//If the given abbrev is HI
			if(placeAbbrev[0] == 'H' && placeAbbrev[1] == 'I') {
				target[n] = target[n-1];
			}
			printf("%d\n",n);
			printf("%s\n",placeIdToName(target[n]));
			printf("%s\n",placeAbbrev);
			n++;
			
		}
		*numReturnedLocs = n;
	}
	//Otherwise it is a hunter
	else {
		target = GvGetMoveHistory(gv,player,numReturnedLocs,canFree);
	}
	
	*canFree = false;
	return target;
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
