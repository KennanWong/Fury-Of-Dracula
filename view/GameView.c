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
	char **PastPlaysArray;

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


void showPastPlaysArray(GameView gv);

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
	printf("%s",pastPlays);
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
	new->PastPlaysArray = malloc(sizeof(*new->PastPlaysArray));
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
			printf("move = %s\n", str);
			new->PastPlaysArray[new->turnCounter] = strdup(str);
			
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
	showPastPlaysArray(new);
	free(tempPastPlays);
	
	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	for (int i = 0; i < 5; i++) {
		free(gv->players[i]);
	}
	for (int i = 0; i < gv->turnCounter; i++) {
		free(gv->PastPlaysArray[i]);
	}
	free(gv->pastGamePlays);
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
	//printf("\nStart of GvGetMoveHistory function\n");
	PlaceId *GGMH = malloc(gv->players[player]->numTurns*sizeof(PlaceId));
	int pInitial = 0;
	if(player == 0) pInitial = 'G';
	else if(player == 1) pInitial = 'S';
	else if(player == 2) pInitial = 'H';
	else if(player == 3) pInitial = 'M';
	else if(player == 4) pInitial = 'D';
	assert(pInitial != 0);
	//printf("pInitial is %d\n",pInitial);
	//printf("gv->pastGamePlays is %s\n",gv->pastGamePlays);
	char *tempPastPlays = strdup(gv->pastGamePlays);
	int n = 0;
	char *str = strtok(tempPastPlays," ");
	while(str != NULL) {
		if(str[0] == pInitial) {
			char placeAbbrev[2];
			placeAbbrev[0] = str[1];
			placeAbbrev[1] = str[2]; 
			//If the given abbrev is city move (unknown city)
			if(placeAbbrev[0] == 'C' && placeAbbrev[1] == '?') GGMH[n] = CITY_UNKNOWN;

			//If the given abbrev is sea move (unknown sea)
			else if(placeAbbrev[0] == 'S' && placeAbbrev[1] == '?') GGMH[n] = SEA_UNKNOWN;

			//If the given abbrev is Dn
			else if(placeAbbrev[0] == 'D' && placeAbbrev[1] != 'U') {
				if(placeAbbrev[1] == 49) GGMH[n] = DOUBLE_BACK_1;
				else if(placeAbbrev[1] == 50) GGMH[n] = DOUBLE_BACK_2;
				else if(placeAbbrev[1] == 51) GGMH[n] = DOUBLE_BACK_3;
				else if(placeAbbrev[1] == 52) GGMH[n] = DOUBLE_BACK_4;
				else if(placeAbbrev[1] == 53) GGMH[n] = DOUBLE_BACK_5;
			}

			//If the given abbrev is HI
			else if(placeAbbrev[0] == 'H' && placeAbbrev[1] == 'I') {
				GGMH[n] = HIDE;
			}
			else {
				GGMH[n] = CityIdFromMove(str);
			}
			n++;
		}
		str = strtok(NULL," ");
		
	}
	printf("NumTurn is %d\n",gv->players[player]->numTurns);
	*numReturnedMoves = gv->players[player]->numTurns;
	*canFree = false;
	free(tempPastPlays);
	//printf("move[0] is %d\n", GGMH[0]);
	//printf("n is %d\n",n);
	return GGMH;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *GGLM = malloc(gv->players[player]->numTurns*sizeof(PlaceId));
	int start = gv->players[player]->numTurns - numMoves;
	printf("numMoves is %d\n",gv->players[player]->numTurns);
	printf("numLocs is %d\n",numMoves);
	printf("start is %d\n", start);
	int end = numMoves;
	if(start < 0) {
		start = 0;
		end = gv->players[player]->numTurns;
	}
	PlaceId *temp = GvGetMoveHistory(gv,player,numReturnedMoves,canFree);
	int count = 0;
	while(count < end) {
		GGLM[count] = temp[count + start];
		count++;
	}
	*numReturnedMoves = count;
	*canFree = false;
	return GGLM;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *GGLH = malloc(gv->players[player]->numTurns*sizeof(PlaceId));
	int pInitial = 0;
	if(player == 0) pInitial = 'G';
	else if(player == 1) pInitial = 'S';
	else if(player == 2) pInitial = 'H';
	else if(player == 3) pInitial = 'M';
	else if(player == 4) pInitial = 'D';
	assert(pInitial != 0);
	//printf("pInitial is %d\n",pInitial);
	//printf("gv->pastGamePlays is %s\n",gv->pastGamePlays);
	char *tempPastPlays = strdup(gv->pastGamePlays);
	int n = 0;
	char *str = strtok(tempPastPlays," ");
	if(pInitial == 'D') {
		while(str != NULL) {
			if(str[0] == pInitial) {
				char placeAbbrev[2];
				placeAbbrev[0] = str[1];
				placeAbbrev[1] = str[2]; 
				//If the given abbrev is city move (unknown city)
				if(placeAbbrev[0] == 'C' && placeAbbrev[1] == '?') GGLH[n] = CITY_UNKNOWN;

				//If the given abbrev is sea move (unknown sea)
				else if(placeAbbrev[0] == 'S' && placeAbbrev[1] == '?') GGLH[n] = SEA_UNKNOWN;

				//If the given abbrev is Dn
				else if(placeAbbrev[0] == 'D' && placeAbbrev[1] != 'U') {
					if(placeAbbrev[1] == 49) GGLH[n] = GGLH[n-1];
					else if(placeAbbrev[1] == 50) GGLH[n] = GGLH[n-2];
					else if(placeAbbrev[1] == 51) GGLH[n] = GGLH[n-3];
					else if(placeAbbrev[1] == 52) GGLH[n] = GGLH[n-4];
					else if(placeAbbrev[1] == 53) GGLH[n] = GGLH[n-5];
				}

				//If the given abbrev is HI
				else if(placeAbbrev[0] == 'H' && placeAbbrev[1] == 'I') {
					GGLH[n] = GGLH[n-1];
				}
				else {
					GGLH[n] = CityIdFromMove(str);
				}
				n++;
			}
			str = strtok(NULL," ");
			
		}
	} else GGLH = GvGetMoveHistory(gv,player,numReturnedLocs,canFree);
	*numReturnedLocs = gv->players[player]->numTurns;
	*canFree = false;
	free(tempPastPlays);
	//printf("move[4] is %d\n", GGLH[4]);
	//printf("n is %d\n",n);
	return GGLH;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	/*PlaceId *GGLL = malloc(gv->players[player]->numTurns*sizeof(PlaceId));
	int start = gv->players[player]->numTurns - numLocs;
	printf("numMoves is %d\n",gv->players[player]->numTurns);
	printf("numLocs is %d\n",numLocs);
	printf("start is %d\n", start);
	int end = numLocs;
	if(start < 0) {
		start = 0;
		end = gv->players[player]->numTurns;
	}
	PlaceId *temp = GvGetLocationHistory(gv,player,numReturnedLocs,canFree);
	int count = 0;
	while(count < end) {
		GGLL[count] = temp[count + start];
		count++;
	}
	printf("count is %d\n", count);
	*numReturnedLocs = count;
	*canFree = false;
	return GGLL;
	*/
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
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
	char *abbrev = malloc(sizeof(*abbrev));
	abbrev[0] = str[1];
	abbrev[1] = str[2];
	PlaceId id = placeAbbrevToId(abbrev);
	free(abbrev);
	return id;
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
		showPastPlaysArray(gv);
		printf("move = %s\n", move);
		int DBval = move[2] - '0';
		printf("gv->turnCounter - DBval*5 = %d\n", gv->turnCounter - DBval*5);
		currLoc = CityIdFromMove(gv->PastPlaysArray[gv->turnCounter - DBval*5]);
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
		//printf("adding trap\n");
		AddTrapToLoc(currLoc, gv->map);
		//printf("added trap to location\n");
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
		/*
		int NumRetrunedLocations;
		bool canFree = true;
		PlaceId *DraculaTrail = malloc(8*sizeof(PlaceId));
		DraculaTrail = GvGetLastLocations(gv, gv->players[PLAYER_DRACULA]->player, 7, &NumRetrunedLocations, &canFree);
		if (NumRetrunedLocations == 7) {
			RemoveTrapsFromLoc(DraculaTrail[6], gv->map);
		}
		free(DraculaTrail);
		*/
	} else if (action[2] == 'V') {
		// Mature Vamp
		MatureVampire(gv);
	}
	printf("finished processing draculas turn\n");
}


void ProcessHunter(GameView gv, char *move, Players *player){

	PlaceId currLoc = CityIdFromMove(move);
	if (currLoc == player->currLoc) {
		// the player has rest
		// increase health
		if (player->health + LIFE_GAIN_REST > GAME_START_HUNTER_LIFE_POINTS) {
			player->health = GAME_START_HUNTER_LIFE_POINTS;
		} else {
			player->health = player->health + LIFE_GAIN_REST;
		}	
	} else {
		player->currLoc = currLoc;
	}
	char action[4];
	ActionFromMove(move, action);
	for (int i = 0; i < 4; i++) {
		if (action[i] == 'T') {
			// printf("player encountered a trap\n");
			player->health = player->health - LIFE_LOSS_TRAP_ENCOUNTER;
			// printf("players new health: %d\n", player->health);
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
	printf("finished processing hunter\n"); 
}

// Resets the vampire
void MatureVampire(GameView gv) {
	RemoveVampireFromLoc(gv->VampireLocation, gv->map);
	gv->CurrentScore = gv->CurrentScore - SCORE_LOSS_VAMPIRE_MATURES;
	gv->VampireLocation = NOWHERE;
	gv->VampireStatus = 10;
	gv->RoundOfVampire = -1;
}


void showPastPlaysArray(GameView gv) {
	for (int i = 0; i < gv->turnCounter; i++) {
		printf("PastPlaysArray[i] = %s\n", gv->PastPlaysArray[i]);
	}
}