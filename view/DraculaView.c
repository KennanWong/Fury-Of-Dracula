////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

// add your own #includes here
// TODO: ADD YOUR OWN STRUCTS HERE

typedef struct{
	Player player;
	int health;
	PlaceId currLoc; 
	char *pastPlays;
}Players;

struct draculaView {
	GameView gv;
	Map map;
	int CurrentScore;
	Round round;
	Players **players;
	Message *messages;
	char *numTraps;
	char *pastGamePlays;
	int turnCounter;
	int VampireStatus;		// -> 0 if there is no vampire, 1 if it is immature, 2 if it has matured
	int RoundOfVampire;
	PlaceId VampireLocation;
	
	// TODO: ADD FIELDS HERE
};

////////////////////////////////////////////////////////////////////////
//Function declarations
PlaceId DvCityIdFromMove(char *str);
void DvActionFromMove(char *str, char *action);
void DvProcessDracula(DraculaView dv, char *move);
void DvMatureVampire(DraculaView dv);
void draculatrail(DraculaView dv, Player player, PlaceId trail[TRAIL_SIZE]);
////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	DraculaView dv = malloc(sizeof(*dv));
	dv->gv = GvNew(pastPlays, messages);
	
	return dv;
}

void DvFree(DraculaView dv)
{
	
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GvFree(dv->gv);
	free(dv);

	
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	Round roundNum = GvGetRound(dv->gv);
	return roundNum;

}

int DvGetScore(DraculaView dv)
{
	//Note: Dracula wants the game score to be low.

	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int score = GvGetScore(dv->gv);
	return score;
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int TargetHealth = GvGetHealth(dv->gv, player);
	return TargetHealth;
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId playerLoc = GvGetPlayerLocation(dv->gv, player);
	return playerLoc;
}


PlaceId DvGetVampireLocation(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId vampireLoc = GvGetVampireLocation(dv->gv);
	return vampireLoc;
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// Cycle through each city and gather all traps, return number of traps back
	PlaceId *trapLoc = malloc(sizeof(*trapLoc));
	trapLoc =  GvGetTrapLocations(dv->gv, numTraps);
	return trapLoc;

}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId trail[TRAIL_SIZE] = {0};

	//gets the last 6 locations of the dracula (trail)
	PlaceId* visitedList = draculatrail(dv, PLAYER_DRACULA, trail[TRAIL_SIZE]);

	//current location of dracula
	PlaceId draclocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	//all the connected locations to the current location
	PlaceId listofconnections = GvGetReachable(dv->gv,PLAYER_DRACULA,dv->round,draclocation,numReturnedLocs);

	int size = *numReturnedLocs;
	int invalid = 0;

	//if there is no location for the dracula to go to, it will teleport to Castle Dracula
	int flag = 0;
	if (draclocation == NOWHERE) {
		flag = 1;
	} 
	
	if (listofconnections == NULL) {
		flag = 1;
	}
	
	int hide = 0;
	int doubleback = 0;
	while (listofconnections != NULL) {

		//checks to see if trail or double back is in the trail
		//if it is present, marks either doubleback or hide as true as these are locations that cannot be accessed now
		int i = 0;
		while (i < TRAIL_SIZE) {
			if(trail[i] == DOUBLE_BACK_1 || trail[i] == DOUBLE_BACK_2 || trail[i] == DOUBLE_BACK_3 ||
			trail[i] == DOUBLE_BACK_4 ||trail[i] == DOUBLE_BACK_5) {
				doubleback = 1;
			}

			if(trail[i] == HIDE) {
				hide = 1;
			}
			i++;
		}

		//incrememnts count everytime a possible location the dracula can travel to is also the same location
		//which is present in trail
		if(doubleback == 1){
			for(int d = 0; d < TRAIL_SIZE; d++){
				for(int k = 0; k < size; k++){
					if(trail[d] == listofconnections[k]){
						invalid++;
						listofconnections[k] = -1;
					}
				}
			}
		}

		//incrememnts count everytime a possible location the dracula can travel to is also the same location
		//which is present in trail
		if(hide == 1){
			for(int p = 0; p < size; p++){
				if(listofconnections[p] == draclocation){
					listofconnections[p] = -1;
					invalid++;
				}
			}
		}

		//the real amount of locations the dracula can travel to after adapting to dracula restrictions
		*numReturnedLocs = size - invalid;
		PlaceId *newconnections = malloc(sizeof(PlaceId) * (*numReturnedLocs));

		int l = 0, m = 0;
		while(l < numReturnedLocs) {
			if(listofconnections[m] != -1){
				if ((listofconnections->type == ROAD) || (listofconnections->type == BOAT)) {
					newconnections[l] = listofconnections[m];
					l++;
				}
			}
			m++;
		}
	}

	if (flag == 1) {
		*numReturnedLocs = 0;
		return NULL;
	}

	return newconnections;

}


PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId trail[TRAIL_SIZE] = {0};

	//gets the last 6 locations of the dracula (trail)
	PlaceId* visitedList = draculatrail(dv, PLAYER_DRACULA, trail[TRAIL_SIZE]);

	//current location of dracula
	PlaceId draclocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	//all the connected locations to the current location
	PlaceId listofconnections = GvGetReachableByType(dv->gv,PLAYER_DRACULA,dv->round,draclocation, road, 0, boat, numReturnedLocs);

	int size = *numReturnedLocs;
	int invalid = 0;
	int hide = 0;
	int doubleback = 0;
	int flag = 0;
	//if there is no location for the dracula to go to, it will teleport to Castle Dracula
	

	if (draclocation == NOWHERE) {
		flag = 1;
	} 
	
	if (listofconnections == NULL) {
		flag = 1;
	}

	while (listofconnections != NULL) {

		//checks to see if trail or double back is in the trail
		//if it is present, marks either doubleback or hide as true as these are locations that cannot be accessed now
		int i = 0;
		while (i < TRAIL_SIZE) {
			if(trail[i] == DOUBLE_BACK_1 || trail[i] == DOUBLE_BACK_2 || trail[i] == DOUBLE_BACK_3 ||
			trail[i] == DOUBLE_BACK_4 ||trail[i] == DOUBLE_BACK_5) {
				doubleback = 1;
			}

			if(trail[i] == HIDE) {
				hide = 1;
			}
			i++;
		}

		//incrememnts count everytime a possible location the dracula can travel to is also the same location
		//which is present in trail
		if(doubleback == 1){
			for(int d = 0; d < TRAIL_SIZE; d++){
				for(int k = 0; k < size; k++){
					if(trail[d] == listofconnections[k]){
						invalid++;
						listofconnections[k] = -1;
					}
				}
			}
		}

		//incrememnts count everytime a possible location the dracula can travel to is also the same location
		//which is present in trail
		if(hide == 1){
			for(int p = 0; p < size; p++){
				if(listofconnections[p] == draclocation){
					listofconnections[p] = -1;
					invalid++;
				}
			}
		}

		//the real amount of locations the dracula can travel to after adapting to dracula restrictions
		*numReturnedLocs = size - invalid;
		PlaceId *newconnections = malloc(sizeof(PlaceId) * (*numReturnedLocs));

		int l = 0, m = 0;
		while(l < numReturnedLocs) {
			if(listofconnections[m] != -1){
				if ((listofconnections->type == ROAD) || (listofconnections->type == BOAT)) {
					newconnections[l] = listofconnections[m];
					l++;
				}
			}
			m++;
		}
	}

	if (flag == 1) {
		*numReturnedLocs = 0;
		return NULL;
	}

	return newconnections;

}


PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}


PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
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

PlaceId DvCityIdFromMove(char *str) {
	char *abbrev = malloc(sizeof(*abbrev));
	abbrev[0] = str[1];
	abbrev[1] = str[2];
	PlaceId id = placeAbbrevToId(abbrev);
	free(abbrev);
	return id;
}

void DvActionFromMove(char *str, char *action) {
	for (int i = 3; i < 7; i++) {
		action[i - 3] = str[i];
	}
}

void DvProcessDracula(DraculaView dv, char *move) {
	// Process movement
	Players *Dracula = dv->players[PLAYER_DRACULA];
	PlaceId currLoc = DvCityIdFromMove(move);
	if (move[1] == 'D') {
		/*
		// Dracula has double backed
		// need to get list of previous locations
		int NumRetrunedLocations;
		bool canFree = true;
		PlaceId *DraculaTrail = malloc(7*sizeof(PlaceId));
		DraculaTrail = HvGetLastLocations(hv, Dracula->player, 7, &NumRetrunedLocations, &canFree);
		if ((move[2] - '0') < NumRetrunedLocations) {
			currLoc = DraculaTrail[move[2] - '0'];
		}
		
		free(DraculaTrail);
		*/
	} else if (currLoc == HIDE) {
		currLoc = Dracula->currLoc;
	} else if (placeIdToType(currLoc) == SEA) {
		dv->players[PLAYER_DRACULA]->health = dv->players[PLAYER_DRACULA]->health - LIFE_LOSS_SEA;
	}
	Dracula->currLoc = currLoc;

	// Process action
	char action[4];
	HvActionFromMove(move, action);
	if (action[0] == 'T') {
		// Add trap to location
		//printf("adding trap\n");
		AddTrapToLoc(currLoc, dv->map);
		//printf("added trap to location\n");
	}
	if (action[1] == 'V') {
		// Add vampire to location
		AddVampireToLoc(currLoc, dv->map);
		dv->VampireStatus = 1;
		dv->VampireLocation = currLoc;
		dv->RoundOfVampire = dv->round;
	}
	if (action[2] == 'M') {
		// Remove a trap
		/*
		int NumRetrunedLocations;
		bool canFree = true;
		PlaceId *DraculaTrail = malloc(8*sizeof(PlaceId));
		DraculaTrail = GvGetLastLocations(hv, hv->players[PLAYER_DRACULA]->player, 7, &NumRetrunedLocations, &canFree);
		if (NumRetrunedLocations == 7) {
			RemoveTrapsFromLoc(DraculaTrail[6], hv->map);
		}
		free(DraculaTrail);
		*/
	} else if (action[2] == 'V') {
		// Mature Vamp
		DvMatureVampire(dv);
	}
}

// Resets the vampire
void DvMatureVampire(DraculaView dv) {
	RemoveVampireFromLoc(dv->VampireLocation, dv->map);
	dv->gv->CurrentScore = dv->gv->CurrentScore - SCORE_LOSS_VAMPIRE_MATURES;
	dv->gv->VampireLocation = NOWHERE;
	dv->gv->VampireStatus = 10;
	dv->gv->RoundOfVampire = -1;
}

void draculatrail(DraculaView dv, Player player, PlaceId trail[TRAIL_SIZE]) {

	GvGetLocationHistory(dv->gv, player, numReturnedLocs, canFree);
	if(player == PLAYER_DRACULA){
			for(int i = 0; i < TRAIL_SIZE; i++){

				switch(trail[i]){
					case DOUBLE_BACK_1:
					trail[i] = trail[i + 1]; 
					break;

					case DOUBLE_BACK_2: 
					trail[i] = trail[i + 2];
					break;

					case DOUBLE_BACK_3: 
					trail[i] = trail[i + 3]; 
					break;

					case DOUBLE_BACK_4: 
					trail[i] = trail[i + 4]; 
					break;

					case DOUBLE_BACK_5: 
					trail[i] = trail[i + 5]; 
					break;

					case HIDE: 
					trail[i] = trail[i+1]; 
					break;
				}
			}
		}
}
