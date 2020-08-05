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

// add your own #includes here
#include "Places.h"
#include "Queue.h"
#include <string.h>

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct{
	Player player;
	int health;
	PlaceId currLoc; 
	int numTurns;
}Players;

struct hunterView {
	// TODO: ADD FIELDS HERE
	GameView gv;
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
	PlaceId DraculaLastKNownLoc;
};

////////////////////////////////////////////////////////////////////////
// Function declerations

// takes in a 7 char turn string and returns place id of new location
PlaceId HvCityIdFromMove(char *str); 

// take the 7 char turn string and fills in an action string
void HvActionFromMove(char *str, char *action);

// Processes Draculas actions
void HvProcessDracula(HunterView hv, char *move);


void HvProcessHunter(HunterView hv, char *move, Players *player);


void HvMatureVampire(HunterView hv);


PlaceId HunterEncounters(HunterView hv, Players player, Players Dracula);

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	HunterView hv = malloc(sizeof(*hv));
	hv->gv = GvNew(pastPlays, messages);
	hv->DraculaLastKNownLoc = NOWHERE;



	return hv;
}

void HvFree(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GvFree(hv->gv);
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	Round roundNum = GvGetRound(hv->gv);
	return roundNum;
}

Player HvGetPlayer(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	Player target= GvGetPlayer(hv->gv);
	return target;
}

int HvGetScore(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int score = GvGetScore(hv->gv);
	return score;
}

int HvGetHealth(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int TargetHealth = GvGetHealth(hv->gv, player);
	return TargetHealth;
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId playerLoc = GvGetPlayerLocation(hv->gv, player);
	return playerLoc;
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId vampireLoc = GvGetVampireLocation(hv->gv);
	return vampireLoc;
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// *round = 0;
	PlaceId LastKnown = NOWHERE;
	// need to find draculas trail, if any of the hunters have been on that path, update last known

	// Round temp = -1;

	for (int i = 0; i < 4; i++) {
		int HuNumReturnedLoc;
		bool HucanFree;
		PlaceId *HuPastLocs = GvGetLocationHistory(hv->gv, i, &HuNumReturnedLoc, &HucanFree);
		for (int h = 0; h < HuNumReturnedLoc; h++) {
			int DnumReturnedLoc;
			bool canFree;
			PlaceId *DrculasTrail = ReturnTrailAtRound(hv->gv, h, &DnumReturnedLoc, &canFree);
			for (int l = 0; l < DnumReturnedLoc; l++){
				if (HuPastLocs[h] == DrculasTrail[l]) {
					if (*round < l) {
						*round = l;
						LastKnown = HuPastLocs[h];
					}		
				}
			}
			if (canFree) {
				free(DrculasTrail);
			}		
		}
	}
	// printf("lastknowLocation =  %s\n", placeIdToName(LastKnown));
	// printf("round number %d\n", *round);
	return LastKnown;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// breadth first search
	// dijkstras algo
	// find all locations that can be immediately reached by the player, add them to queue
	// keep track of distrance from src
	// once we have found the destination, return
	PlaceId *PreviousPlace = malloc(sizeof(PlaceId) * NUM_REAL_PLACES);
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		PreviousPlace[i] = NOWHERE;
	}
	int *DstFromOg = calloc(sizeof(int), NUM_REAL_PLACES);
	int *Visited = calloc(sizeof(int), NUM_REAL_PLACES);
	
	Queue PlacesQueue = newQueue();

	PlaceId currLoc = HvGetPlayerLocation(hv, hunter);
	Visited[currLoc] = 1;

	int counter = 0;		// keeps track of the processed cities
	int addedLocLastCheck = 1;	// keeps track of the added cities
	int RoundCount = 0;

	// int addedLoc = 1;

	PlaceId last = NOWHERE;
	int addedCities = 0;

	


	// If the counter reaches the number of added cities, we move to the next round
	QueueJoin(PlacesQueue, currLoc);
	while(QueueIsEmpty(PlacesQueue) ==  false) {
		// ToCheck is the place we are starting at
		// will look at its connections and see where it can reach
		 
		
		PlaceId ToCheck = QueueLeave(PlacesQueue);
		addedLocLastCheck--;
		counter++;
		int numLocs;

		PlaceId *reachable;

		// Reachable is an array that shows all places a player can reach from their given location

		int MaxDst = (hunter+HvGetRound(hv)+RoundCount)%4;

		if (MaxDst == 0) {
			reachable = GvGetReachableByType(hv->gv, hunter, HvGetRound(hv)+RoundCount, ToCheck, true, false, true, &numLocs);
		} else {
			reachable = GvGetReachable(hv->gv, hunter, HvGetRound(hv)+RoundCount, ToCheck, &numLocs);
		}
		addedCities += numLocs;

		if (last == NOWHERE) {
			// initialise last to a value
			last = reachable[0];
			// printf("Last = %s\n", placeIdToName(last));
		}
		

		if (last == ToCheck) {
			last = lastIdInQueue(PlacesQueue);
			RoundCount++;
			// printf("NumRoundsPassed = %d\n", RoundCount);
		}

		if (RoundCount > DstFromOg[dest] && Visited[dest] == 1) break;

		for (int i = 0; i < numLocs ; i++) {
			// we will loop through reachable, find all places they can arrive at anf fill out dijkstras algo
			// the previopus place will be 'toCheck' and 
			PlaceId temp = reachable[i];
			// printf("%s can reach %s\n", placeIdToName(ToCheck), placeIdToName(temp));
			if (temp != ToCheck && Visited[temp] != 1) {
				if (DstFromOg[temp] == 0 || DstFromOg[temp] > (DstFromOg[ToCheck] + 1)) {
					QueueJoin(PlacesQueue, temp);
					addedLocLastCheck++;
					addedCities++;
					// printf("faster to get to %s from %s than from %s\n", placeIdToName(temp), placeIdToName(ToCheck), placeIdToName(PreviousPlace[temp]));
					PreviousPlace[temp] = ToCheck;
					DstFromOg[temp] = DstFromOg[ToCheck] + 1;
					Visited[temp] = 1;
				}
			}
		}

		// RoundCount++;
		free(reachable);
	}

	PlaceId *ShortestPath = malloc(sizeof(PlaceId)* (DstFromOg[dest]));

	PlaceId previous = dest;

	// printf("previous place from dest = %s\n", placeIdToName(PreviousPlace[dest]));

	while (previous != currLoc) {
		// printf("previous = %s\n", placeIdToName(previous));
		ShortestPath[DstFromOg[previous]-1] = previous;
		previous = PreviousPlace[previous];
	}

	free(PreviousPlace);
	free(DstFromOg);
	free(Visited);
	
	
	*pathLength = DstFromOg[dest];
	/*
	for (int i = 0; i < *pathLength; i++) {
		printf("ShortestPath[%d] = %s\n", i, placeIdToName(ShortestPath[i]));
	}
	*/
	// printf("pathLength = %d\n", *pathLength);
	return ShortestPath;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *HWCIG = malloc(hv->round*sizeof(PlaceId));
	*numReturnedLocs = 0;
	//Calculating the player we are right now
	Player playerId = HvGetPlayer(hv);

	//Defining the current location of the player given as CurrCityId
	PlaceId CurrCityId = GvGetPlayerLocation(hv->gv, playerId);

	//If the player is currently NOWHERE i.e hasn't made a move, return NULL
	if(CurrCityId == NOWHERE) {	
		return NULL;
	}

	//Using MapGetConnection which gives a linked list of all the connections to that city
	PlaceId *List = GvGetReachable(hv->gv,playerId,hv->round,CurrCityId,numReturnedLocs);
	for(int i = 0; i < *numReturnedLocs; i++) {
		HWCIG[i] = List[i];
	}

	return HWCIG;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *HWCIGBT = malloc(hv->round*sizeof(PlaceId));

	Player playerId = HvGetPlayer(hv);

	//Defining the current location of the player given as CurrCityId
	PlaceId CurrCityId = GvGetPlayerLocation(hv->gv, playerId);

	//If the player is currently NOWHERE i.e hasn't made a move, return NULL
	if(CurrCityId == NOWHERE) {
		return NULL;
	}
	
	
	//Transferring all of the places in List into HWCIGBT
	PlaceId *List = GvGetReachableByType(hv->gv,playerId,hv->round,CurrCityId,road,rail,boat,numReturnedLocs);
	// printf("Flag2\n");
	//Transferring all of the places in List into HWCIGBT
	int n = 0;
	while(n < *numReturnedLocs) {
		HWCIGBT[n] = List[n];
		n++;
	}

	return HWCIGBT;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *HWCTG = malloc(hv->round*sizeof(PlaceId));
	*numReturnedLocs = 0;
	//Defining the current location of the player given as CurrCityId
	PlaceId CurrCityId = GvGetPlayerLocation(hv->gv, player);

	//If the player is currently NOWHERE i.e hasn't made a move, return NULL
	if(CurrCityId == NOWHERE) {
		return NULL;
	}

	//Using MapGetConnection which gives a linked list of all the connections to that city
	PlaceId *List = GvGetReachable(hv->gv,player,hv->round,CurrCityId,numReturnedLocs);
	for(int i = 0; i < *numReturnedLocs; i++) {
		HWCTG[i] = List[i];
	}
	//Transferring all of the places in List into HWCTG
	free(List);

	return HWCTG;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *HWCTGBT = malloc(hv->round*sizeof(PlaceId));
	*numReturnedLocs = 0;
	//Defining the current location of the player given as CurrCityId
	PlaceId CurrCityId = GvGetPlayerLocation(hv->gv, player);
	//If the player is currently NOWHERE i.e hasn't made a move, return NULL
	if(CurrCityId == NOWHERE) {
		return NULL;
	}
	
	PlaceId *List = GvGetReachableByType(hv->gv,player,hv->round,CurrCityId,road,rail,boat, numReturnedLocs);
	
	//Transferring all of the places in List into HWCIGBT
	int i = 0;
	while(i < *numReturnedLocs) {
		HWCTGBT[i] = List[i];
		// printf("HWCTGBT[%d] = %s\n", i, placeIdToName(HWCTGBT[i]));
		i++;
	}
	// printf("numReturnedLocs after GvGetReachablebyType is %d\n",*numReturnedLocs);
	free(List);
	return HWCTGBT;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

PlaceId HvCityIdFromMove(char *str) {
	char *abbrev = malloc(sizeof(*abbrev));
	abbrev[0] = str[1];
	abbrev[1] = str[2];
	PlaceId id = placeAbbrevToId(abbrev);
	free(abbrev);
	return id;
}

void HvActionFromMove(char *str, char *action) {
	for (int i = 3; i < 7; i++) {
		action[i - 3] = str[i];
	}
}

void HvProcessDracula(HunterView hv, char *move) {
	// Process movement
	Players *Dracula = hv->players[PLAYER_DRACULA];
	PlaceId currLoc = HvCityIdFromMove(move);
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
		hv->players[PLAYER_DRACULA]->health = hv->players[PLAYER_DRACULA]->health - LIFE_LOSS_SEA;
	}
	Dracula->currLoc = currLoc;

	// Process action
	char action[4];
	HvActionFromMove(move, action);
	if (action[0] == 'T') {
		// Add trap to location
		//printf("adding trap\n");
		AddTrapToLoc(currLoc, hv->map);
		//printf("added trap to location\n");
	}
	if (action[1] == 'V') {
		// Add vampire to location
		AddVampireToLoc(currLoc, hv->map);
		hv->VampireStatus = 1;
		hv->VampireLocation = currLoc;
		hv->RoundOfVampire = hv->round;
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
		HvMatureVampire(hv);
	}
}


void HvProcessHunter(HunterView hv, char *move, Players *player){

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
	HvActionFromMove(move, action);
	for (int i = 0; i < 4; i++) {
		if (action[i] == 'T') {
			// printf("player encountered a trap\n");
			player->health = player->health - LIFE_LOSS_TRAP_ENCOUNTER;
			// printf("players new health: %d\n", player->health);
			RemoveTrapFromLoc(currLoc, hv->map);
		} else if (action[i] == 'V') {
			// Vanquish vampire
			// RemoveVampireFromLoc(currLoc, gv->map);
			hv->VampireLocation = NOWHERE;
			hv->VampireStatus = 1;
		} else if (action[i] == 'D') {
			// Hunter encounters dracula, loses life points
			player->health = player->health - LIFE_LOSS_DRACULA_ENCOUNTER;
			// Dracula encounters hunter, loses blood points
			hv->players[PLAYER_DRACULA]->health = hv->players[PLAYER_DRACULA]->health - LIFE_LOSS_HUNTER_ENCOUNTER;
			// Set draculas last known location
			hv->DraculaLastKNownLoc = player->currLoc;
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
void HvMatureVampire(HunterView hv) {
	RemoveVampireFromLoc(hv->VampireLocation, hv->map);
	hv->CurrentScore = hv->CurrentScore - SCORE_LOSS_VAMPIRE_MATURES;
	hv->VampireLocation = NOWHERE;
	hv->VampireStatus = 10;
	hv->RoundOfVampire = -1;
}

PlaceId HunterEncounters(HunterView hv, Players player, Players Dracula) {
	return NOWHERE;
}

