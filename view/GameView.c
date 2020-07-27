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
	int numTurns;
}Players;

struct gameView {
	Map map;
	int CurrentScore;
	Round round;
	Players **players;
	Message *messages;
	char *pastGamePlays;
	int turnCounter;
	int VampireStatus;		// -> 0 if there is no vampire, 1 if it is immature, 2 if it has matured
	int RoundOfVampire;
	PlaceId VampireLocation;
	// TODO: ADD FIELDS HERE
};

////////////////////////////////////////////////////////////////////////
// Function declerations

// takes in a 7 char turn string and returns place id of new location
PlaceId CityIdFromMove(char *str); 

// take the 7 char turn string and fills in an action string
void ActionFromMove(char *str, char *action);

// Processes Draculas actions
void ProcessDracula(GameView gv, char *move);


void ProcessHunter(GameView gv, char *move, Players *player);


void MatureVampire(GameView gv);


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
	new->players = malloc(5*sizeof(Players));	// an array of PlayerStructs
	new->turnCounter = 0;
	new->VampireLocation = NOWHERE;
	new->VampireStatus = 0;
	new->RoundOfVampire = -1;

	// intialise the players
	for (int i = 0; i < 5; i++){
		Players *newPlayer = malloc(sizeof(Players));
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
		
		newPlayer->numTurns = 0;
		new->players[i] = newPlayer;
	
	}

	new->messages = messages;

	// process pastplays and seperate each to their respective players
	// This is done so by first tokenising the past plays string splitting them into 6 character strings
	// 'i' will be used to count the number of turns taken, this will be used to determine whose turn it is
	// (i%5) and what game round we are in (i/5), i will also be used to determine the turn counter.

	/* when processing past plays
	1. determine whose play it was --> update location and add to their past plays
	2. then determine any actions, deduct life points as directed or process traps
		this must be split as it weill be different for hunters and the dracula
	3. move onto the next player
	4. once we process one entire round, update round counter, and deduct points accordingly

	*/
	// Processing the moves of each player
	char *tempPastPlays = strdup(pastPlays);
	if (strlen(pastPlays) != 0) {
		// take each 6 char play seperately
		char *str = strtok(tempPastPlays, " ");
		int i = 0;
		while (str != NULL) {	
			// i%5 will determine who made the move
			int playerId = i%5;
			Players *currPlayer = new->players[playerId];
			
			// add the 6char string to the players past plays
			char *tmp = strdup(str);
			printf("move = %s\n", tmp);
			assert(tmp != NULL);
			currPlayer->numTurns += 1;
			i+=1;
			// if we are processing draculas move
			if (str[0] == 'D') {
				ProcessDracula(new, str);
			} else {
				if (currPlayer->health == 0) {
					currPlayer->health = GAME_START_HUNTER_LIFE_POINTS;
				}
				ProcessHunter(new, str, currPlayer);
			}
			
			// Move onto next person turn
			str = strtok(NULL, " ");
			
			if (new->round != i/5) {
				// we have reached a new round, deduct points
				new->CurrentScore = new->CurrentScore - SCORE_LOSS_DRACULA_TURN;
				new->round = i/5;
			}

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
	return gv->VampireLocation;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// Cycle through each city and gather all traps, return number of traps back
	PlaceId *TrapLocations = malloc(sizeof(*TrapLocations));
	for (PlaceId i = 0; i < NUM_REAL_PLACES; i++) {
		int NumTrapsAtLoc = GetTrapsLoc(i, gv->map);
		for (int t = 0; t < NumTrapsAtLoc; t++) {
			TrapLocations[*numTraps] = i;
			*numTraps = *numTraps + 1;
		}
	}

	return TrapLocations;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
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

PlaceId CityIdFromMove(char *str) {
	char abbrev[2];
	for (int i = 1; i < 3; i++) {
		abbrev[i-1] = str[i];
	}
	return placeAbbrevToId(abbrev);
}

void ActionFromMove(char *str, char *action) {
	for (int i = 3; i < 7; i++) {
		action[i - 3] = str[i];
	}
}

void ProcessDracula(GameView gv, char *move) {
	// Process movement
	Players *Dracula = gv->players[PLAYER_DRACULA];
	PlaceId currLoc = CityIdFromMove(move);
	if (move[1] == 'D') {
		// Dracula has double backed
		// need to get list of previous locations
		int NumRetrunedLocations;
		bool canFree = true;
		PlaceId *DraculaTrail = malloc(7*sizeof(PlaceId));
		DraculaTrail = GvGetLastLocations(gv, Dracula->player, 7, &NumRetrunedLocations, &canFree);
		if ((move[2] - '0') < NumRetrunedLocations) {
			currLoc = DraculaTrail[move[2] - '0'];
		}
		
		free(DraculaTrail);
	} else if (currLoc == HIDE) {
		currLoc = Dracula->currLoc;
	} else if (placeIdToType(currLoc) == SEA) {
		gv->players[PLAYER_DRACULA]->health = gv->players[PLAYER_DRACULA]->health - LIFE_LOSS_SEA;
	}
	Dracula->currLoc = currLoc;

	// Process action
	char action[4];
	ActionFromMove(move, action);
	if (action[0] == 'T') {
		// Add trap to location
		printf("adding trap\n");
		AddTrapToLoc(currLoc, gv->map);
		printf("added trap to location\n");
	}
	if (action[1] == 'V') {
		// Add vampire to location
		AddVampireToLoc(currLoc, gv->map);
		gv->VampireStatus = 1;
		gv->VampireLocation = currLoc;
		gv->RoundOfVampire = gv->round;
	}
	if (action[2] == 'M') {
		// Remove a trap
		int NumRetrunedLocations;
		bool canFree = true;
		PlaceId *DraculaTrail = malloc(8*sizeof(PlaceId));
		DraculaTrail = GvGetLastLocations(gv, gv->players[PLAYER_DRACULA]->player, 7, &NumRetrunedLocations, &canFree);
		if (NumRetrunedLocations == 7) {
			RemoveTrapsFromLoc(DraculaTrail[6], gv->map);
		}
		free(DraculaTrail);
	} else if (action[2] == 'V') {
		// Mature Vamp
		MatureVampire(gv);
	}
}


void ProcessHunter(GameView gv, char *move, Players *player){

	PlaceId currLoc = CityIdFromMove(move);
	if (currLoc == player->currLoc) {
		// the player has rest
		// increase health
		player->health = player->health + LIFE_GAIN_REST;
	} else {
		player->currLoc = currLoc;
	}
	char action[4];
	ActionFromMove(move, action);
	for (int i = 0; i < 4; i++) {
		if (action[i] == 'T') {
			printf("player encountered a trap\n");
			player->health = player->health - LIFE_LOSS_TRAP_ENCOUNTER;
			RemoveTrapFromLoc(currLoc, gv->map);
		} else if (action[i] == 'V') {
			// Vanquish vampire
			// RemoveVampireFromLoc(currLoc, gv->map);
			gv->VampireLocation = NOWHERE;
			gv->VampireStatus = 1;
		} else if (action[i] == 'D') {
			// Hunter encounters dracula, loses life points
			player->health = player->health - LIFE_LOSS_DRACULA_ENCOUNTER;
			// Dracula encounters hunter, loses blood points
			gv->players[PLAYER_DRACULA]->health = gv->players[PLAYER_DRACULA]->health - LIFE_LOSS_HUNTER_ENCOUNTER;
		}
		if (player->health <= 0) {
			// the hunter has died, and will need to telelport to hospital
			player->currLoc = ST_JOSEPH_AND_ST_MARY;
			player->health = 0;
			return;
		}
	} 
}

// Resets the vampire
void MatureVampire(GameView gv) {
	RemoveVampireFromLoc(gv->VampireLocation, gv->map);
	gv->CurrentScore = gv->CurrentScore - SCORE_LOSS_VAMPIRE_MATURES;
	gv->VampireLocation = NOWHERE;
	gv->VampireStatus = 10;
	gv->RoundOfVampire = -1;
}

