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
#include "Queue.h"
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



bool ChecKIdinList(PlaceId *list, int numLocs, PlaceId id);


void ShowPlaceIdList(PlaceId *list, int numLoc, char *name);

PlaceId *ReturnTrailAtRound(GameView gv, Round round, int *DcNumReturnedLoc, bool *canFree);

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

	new->PastPlaysArray = malloc(sizeof(char *)*(strlen(pastPlays)/8));	// Designate memory to store move strings
	new->messages = messages;


	// Processing the past plays string, we will determine who the player is, their location move and then their
	// action
	char *tempPastPlays = strdup(pastPlays);

	if (strlen(pastPlays) != 0) {
		char *str;
		int i = 0;														// i will determine the turn counter
		while ((str = strsep(&tempPastPlays, " ")) != NULL) {	
			int playerId = i%5;											// i%5 will determine who made the move
			Players *currPlayer = new->players[playerId];
			new->PastPlaysArray[new->turnCounter] = strdup(str);		// strdup the string to save in memory
			currPlayer->numTurns += 1;									// increment the number turns the player has made
			i+=1;
			if (str[0] == 'D') {
				// if we are processing draculas movz
				ProcessDracula(new, new->PastPlaysArray[new->turnCounter]);
			} else {
				// otherwise we are processing a hunters turn
				if (currPlayer->health == 0) {
					// If the player has no health, reset health back to the start
					currPlayer->health = GAME_START_HUNTER_LIFE_POINTS;
				}
				ProcessHunter(new, new->PastPlaysArray[new->turnCounter], currPlayer);
			}
			
			if (new->round != i/5) {
				// we have reached a new round, deduct points
				new->CurrentScore = new->CurrentScore - SCORE_LOSS_DRACULA_TURN;
				new->round = i/5;
			}
			new->turnCounter = i;
		}
	}
	
	
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
	*numTraps = 0;
	PlaceId *TrapLocations = malloc(sizeof(PlaceId)*(GvGetRound(gv)+1));
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
	
	PlaceId *GGMH = malloc(gv->players[player]->numTurns*sizeof(PlaceId));
	int pInitial = 0;
	if(player == 0) pInitial = 'G';
	else if(player == 1) pInitial = 'S';
	else if(player == 2) pInitial = 'H';
	else if(player == 3) pInitial = 'M';
	else if(player == 4) pInitial = 'D';
	assert(pInitial != 0);

	int mCounter = 0;
	for (int i = 0; i < gv->turnCounter; i++) {
		if (gv->PastPlaysArray[i][0] == pInitial) {
			
			GGMH[mCounter] = CityIdFromMove(gv->PastPlaysArray[i]);
			mCounter++;
		}
	}
	
	
	*numReturnedMoves = gv->players[player]->numTurns;

	
	
	*canFree = true;
	return GGMH;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *GGLM = malloc(gv->players[player]->numTurns*sizeof(PlaceId));
	int start = gv->players[player]->numTurns - numMoves;
	
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
	
	int LCounter = 0;
	for (int i = 0; i < gv->turnCounter; i++) {
		if (gv->PastPlaysArray[i][0] == pInitial) {
			/*
			GGMH[mCounter] = CityIdFromMove(gv->PastPlaysArray[i]);
			mCounter++;
			*/
			PlaceId LId = CityIdFromMove(gv->PastPlaysArray[i]);
			
			if (pInitial == 'D') {
				// if we processing dracular
				if (LId > MAX_REAL_PLACE) {

					// move is either double back or hide
					if (LId == HIDE) {
						LId = GGLH[LCounter-1];
					} else if (LId >= DOUBLE_BACK_1) {
						int DBval = LId - HIDE;
						LId = GGLH[LCounter-DBval];
					}
				}	
			}
			GGLH[LCounter] = LId;
			
			
			LCounter++;
		}
	}
	
	*numReturnedLocs = gv->players[player]->numTurns;
	
	*canFree = false;
	
	return GGLH;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	int pInitial = 0;
	if(player == 0) pInitial = 'G';
	else if(player == 1) pInitial = 'S';
	else if(player == 2) pInitial = 'H';
	else if(player == 3) pInitial = 'M';
	else if(player == 4) pInitial = 'D';
	PlaceId *temp = malloc(numLocs*sizeof(PlaceId));
	PlaceId *GGLL = malloc(numLocs*sizeof(PlaceId));

	int c = 0;
	
	while (c < numLocs) {
		for (int i = gv->turnCounter -1; i >= 0; i--) {
			if (gv->PastPlaysArray[i][0] == pInitial) {
				
				temp[c] = CityIdFromMove(gv->PastPlaysArray[i]);
				c++;
				if (c == numLocs) {
					break;
				}
			}
		}
	}
	
	*numReturnedLocs = c;
	// copy temp into GGLL and reverse order
	for (int i = 0; i < c; i++) {
		GGLL[i] = temp[c-1-i];
	}

	free(temp);
	*canFree = true;
	return GGLL;
	
}

////////////////////////////////////////////////////////////////////////
// Making a Move

// Test 
// numReturnedLocs is the length of idList
PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId* idList = malloc(sizeof(PlaceId) * NUM_REAL_PLACES);

	*numReturnedLocs = 0;

	// use realloc to allocate memory every time you add an array element

	// Alternative to useing realloc
	// Add it to a list
	// Allocate an array of size = size of list
	// copy the list elements into an array
	// 
	
	int maxDst = (round + player)%4;
	// printf("this is round number %d\n", round);
	// printf("maxDistance player can travel = %d\n", (round+player)%4);
	for (ConnList curr = MapGetConnections(gv->map, from); curr != NULL; curr = curr->next) {
		// the hunter can go to an adjacent city by road, boat or rail
		if (player != PLAYER_DRACULA) { 
			if (curr->type == ROAD) {
				// increase length of idList by 1 using realloc and adds the name to the idlist.
				// idList = realloc(idList, (*numReturnedLocs + 1) * sizeof(*idList));
				idList[(*numReturnedLocs)] = curr->p;
				(*numReturnedLocs)++;
			}
			if (curr->type == BOAT) {
				// idList = realloc(idList, (*numReturnedLocs + 1) * sizeof(*idList));
				idList[(*numReturnedLocs)] = curr->p;
				(*numReturnedLocs)++;
			}
			if (curr->type == RAIL) {
				// idList = realloc(idList, (*numReturnedLocs + 1) * sizeof(*idList));
				if (!ChecKIdinList(idList, (*numReturnedLocs), curr->p)) {
					idList[(*numReturnedLocs)] = curr->p;
					(*numReturnedLocs)++;
				}	
				if (maxDst > 1) {
					int numLocCheck = 0;
					PlaceId *locCheck2 = GvGetReachableByType(gv, player, round-1, curr->p, false, true , false, &numLocCheck);
					for (int i = 0; i < numLocCheck; i++) {
						// check if we have already added the id to the list
						if (ChecKIdinList(idList, (*numReturnedLocs), locCheck2[i]) == false && locCheck2[i] != from) {
							idList[(*numReturnedLocs)] = locCheck2[i];
							(*numReturnedLocs)++;
						}
					}
					free(locCheck2);
				}
			}
		}

		// dracula can only go by road to boat. 
		bool canFree = false;
		bool hasVisited = false;

		if (player == PLAYER_DRACULA) {
			// check if dracula has already visited the location.
			int numPlaces = 0;
			PlaceId* visitedList = GvGetLastLocations(gv, player, 6, &numPlaces, &canFree);
			
			// if location already visited set hasVisited to true. 
			for (int i = 0; i < numPlaces; i++) {
				if(curr->p == visitedList[i]) {
					hasVisited = true;
				}
			}

			if (hasVisited != true) {
				if (curr->type == ROAD) {
					// idList = realloc(idList, (*numReturnedLocs + 1) * sizeof(*idList));
					idList[(*numReturnedLocs)] = curr->p;
					(*numReturnedLocs)++;
				}

				if (curr->type == BOAT) {
					// idList = realloc(idList, (*numReturnedLocs + 1) * sizeof(*idList));
					idList[(*numReturnedLocs)] = curr->p;
					(*numReturnedLocs)++;
				}
			}
		} 
	}
	if (ChecKIdinList(idList, (*numReturnedLocs), from) == false) {
		idList[(*numReturnedLocs)] = from;
		(*numReturnedLocs)++;
	}
	return idList;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	PlaceId* idList = malloc(sizeof(PlaceId)*NUM_REAL_PLACES);
	*numReturnedLocs = 0;
	int maxDst = (round + player)%4;
	// printf("maxDistance player can travel = %d\n", (round+player)%4);
	// MapShow(gv->map);
	for (ConnList curr = MapGetConnections(gv->map, from); curr != NULL; curr = curr->next) {
		// the hunter can go to an adjacent city by road, boat or rail

		if (player != PLAYER_DRACULA) { 
			// printf("checking for a hunter\n");
			// printf("type = %d\n", curr->type);
			if (curr->type == ROAD && road == true) {
				// increase length of idList by 1 using realloc and adds the name to the idlist.
				idList[(*numReturnedLocs)] = curr->p;
				(*numReturnedLocs)++;
			}

			if (curr->type == BOAT && boat == true) {
				idList[(*numReturnedLocs)] = curr->p;
				(*numReturnedLocs)++;
			}

			if (curr->type == RAIL && rail == true) {
				if (ChecKIdinList(idList, (*numReturnedLocs), curr->p) == false) {
					idList[(*numReturnedLocs)] = curr->p;
					(*numReturnedLocs)++;
				}
				// if the distance is greater than 1, we may look further
				if (maxDst > 1) {
					// Must check the round number, to determine the distance player can travel by train
					// Emulate a depth first search, go to each location, if there is room to move, 
					// recurse into that location
					int numLocCheck = 0;
					PlaceId *locCheck2 = GvGetReachableByType(gv, player, round-1, curr->p, false, true, false, &numLocCheck);
					
					for (int i = 0; i < numLocCheck; i++) {
						// check if we have already added the id to the list
						if (ChecKIdinList(idList, (*numReturnedLocs), locCheck2[i]) == false && locCheck2[i] != from) {
							idList[(*numReturnedLocs)] = locCheck2[i];
							(*numReturnedLocs)++;
						}
					}
					free(locCheck2);

				}
				
				
			}
		}

		// dracula can only go by road to boat. 
		bool canFree;
		bool hasVisited = false;
		
		if (player == PLAYER_DRACULA) {
			// check if dracula has already visited the location.
			int numPlaces = 0;
			PlaceId* visitedList = GvGetLastLocations(gv, player, 6, &numPlaces, &canFree);
			
			// if location already visited set hasVisited to true. 
			for (int i = 0; i < numPlaces; i++) {
				if(curr->p == visitedList[i]) {
					hasVisited = true;
				}
			}
			
			if (hasVisited != true) {
				if (curr->type == ROAD && road == true && curr->p != ST_JOSEPH_AND_ST_MARY) {
					// idList = realloc(idList, (*numReturnedLocs + 1) * sizeof(*idList));
					idList[(*numReturnedLocs)] = curr->p;
					(*numReturnedLocs)++;
				}
				if (curr->type == BOAT && boat == true && curr->p != ST_JOSEPH_AND_ST_MARY) {
					// idList = realloc(idList, (*numReturnedLocs + 1) * sizeof(*idList));
					idList[(*numReturnedLocs)] = curr->p;
					(*numReturnedLocs)++;
				}
			}
			
			free(visitedList);
		} 
	}
	if (ChecKIdinList(idList, (*numReturnedLocs), from) == false) {
		idList[(*numReturnedLocs)] = from;
		(*numReturnedLocs)++;
	}
	// For debugging
	/*
	printf("returned locations = %d\n", (*numReturnedLocs));
	for (int i = 0; i < (*numReturnedLocs); i++) {
		printf("idList[%d] = %s\n", i, placeIdToName(idList[i]));
	}
	*/
	return idList;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

PlaceId CityIdFromMove(char *str) {
	char buff[3];
	buff[0] = str[1];
	buff[1] = str[2];
	buff[2] = '\0';
	/*
	char *abbrev = malloc(sizeof(*abbrev));
	abbrev[0] = str[1];
	abbrev[1] = str[2];
	*/
	PlaceId id = placeAbbrevToId(buff);
	// free(abbrev);
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
		// if the move is a double back move, trace back to the correct location
		int DBval = move[2] - '0';
		currLoc = CityIdFromMove(gv->PastPlaysArray[gv->turnCounter - DBval*5]);
	} else if (currLoc == HIDE) {
		// if the currLoc is hide, retain the draculas current location
		currLoc = Dracula->currLoc;
		
	} 

	if (currLoc >= HIDE) {
		// double backed to a hide move for example, find the most recent location
		int i  = 0;
		while (currLoc >= HIDE) {
			currLoc = CityIdFromMove(gv->PastPlaysArray[gv->turnCounter - i*5]);
			i++;
		}
	}

	if (placeIdToType(currLoc) == SEA) {
		gv->players[PLAYER_DRACULA]->health = gv->players[PLAYER_DRACULA]->health - LIFE_LOSS_SEA;
	}
	Dracula->currLoc = currLoc;

	if (currLoc == CASTLE_DRACULA) {
		Dracula->health = Dracula->health + LIFE_GAIN_CASTLE_DRACULA;
	}

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
		// Remove a trap that has malfunction as it has left the trail
		RemoveTrapFromLoc(CityIdFromMove(gv->PastPlaysArray[gv->turnCounter - 30]), gv->map);
	} else if (action[2] == 'V') {
		// Mature Vamp
		MatureVampire(gv);
	}
	
}


void ProcessHunter(GameView gv, char *move, Players *player){

	PlaceId currLoc = CityIdFromMove(move);
	if (currLoc == player->currLoc) {
		// the player has rested
		// increase health 
		if (player->health + LIFE_GAIN_REST > GAME_START_HUNTER_LIFE_POINTS) {
			// if the players new health exceed the health cap, set it to the cap
			player->health = GAME_START_HUNTER_LIFE_POINTS;
		} else {
			player->health = player->health + LIFE_GAIN_REST;
		}	
	} else {
		// otherwise the players location gets upated
		player->currLoc = currLoc;
	}
	char action[4];
	ActionFromMove(move, action);
	for (int i = 0; i < 4; i++) {
		if (action[i] == 'T') {
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
			gv->CurrentScore = gv->CurrentScore - SCORE_LOSS_HUNTER_HOSPITAL;
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


void showPastPlaysArray(GameView gv) {
	for (int i = 0; i < gv->turnCounter; i++) {
		printf("PastPlaysArray[i] = %s\n", gv->PastPlaysArray[i]);
	}
}


bool ChecKIdinList(PlaceId *list, int numLocs, PlaceId id) {
	for (int i = 0; i< numLocs; i++) {
		if (list[i] == id) {
			return true;
		}
	}

	return false;
}

// From a given round number, return trail array
PlaceId *ReturnTrailAtRound(GameView gv, Round round, int *DcNumReturnedLoc, bool *canFree) {
	PlaceId *DraculasTrail = GvGetLocationHistory(gv, PLAYER_DRACULA, DcNumReturnedLoc,  canFree);
	PlaceId *ReturnedTrailArray = malloc(sizeof(*ReturnedTrailArray));
	
	// if DnumReturnedLoc < round num, then we can only take up to DnumReturnedLoc
	// also if roundNum > 6; we can only take the first six

	if (*DcNumReturnedLoc < 6) {
		for (int i = 0; i < *DcNumReturnedLoc; i++) {
			ReturnedTrailArray[i] = DraculasTrail[i];
		}	
	} else {
		for (int i = 0; (round - 6 + i) < round; i++) {
			ReturnedTrailArray[i] = DraculasTrail[round- 6 + i];
		}
		*DcNumReturnedLoc = 6;
	}
	if (canFree) {
		free(DraculasTrail);
	}
	return ReturnedTrailArray;
}

void ShowPlaceIdList(PlaceId *list, int numLoc, char *name) {
	for (int i = 0; i < numLoc; i++) {
		printf("%s[%d] = %s\n", name, i, placeIdToName(list[i]));
	}
}