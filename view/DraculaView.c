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
	if (dv == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}
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
	// printf("numTraps = %d\n", *numTraps);
	return trapLoc;

}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	/*
	PlaceId *idList = malloc(dv->round*sizeof(PlaceId));
	*numReturnedMoves = 0;

	PlaceId draclocation = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA);
	PlaceId *possibleDMoves = GvGetReachable(dv->gv)

	if(draclocation == NOWHERE) {
		return NULL;
	}

	
	PlaceId *list = GvGetReachable(dv->gv,player,dv->round,draclocation,numReturnedMoves);
	for(int i = 0; i < *numReturnedMoves; i++) {
		idList[i] = list[i];
	}

	return idList;
	*/
	int numReturnedLocs1;
	bool canFree;
	PlaceId* moveList = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 6, &numReturnedLocs1, &canFree);
	
	//current location of dracula
	PlaceId draclocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	
	//all the connected locations to the current location
	int numReturnedLocs2;
	PlaceId *listofconnections = GvGetReachable(dv->gv,PLAYER_DRACULA,DvGetRound(dv), draclocation , &numReturnedLocs2);
	

	// int size = numReturnedLocs2;
	// int invalid = 0;


	int numReturnedTrail;
	bool canFreeTrail;
	PlaceId *trail = GvGetLastLocations(dv->gv, PLAYER_DRACULA, 6, &numReturnedTrail, &canFreeTrail);

	PlaceId *idList = malloc(sizeof(PlaceId)*NUM_REAL_PLACES);
	int idListCount = 0;

	//if there is no location for the dracula to go to, it will teleport to Castle Dracula
	
	if (numReturnedLocs2 == 0) {
		idList[0] = CASTLE_DRACULA;
		*numReturnedMoves = 1;
		return idList;
	} 

	// printf("pass1\n");
	// int hide = FALSE;
	// int doubleback = FALSE;
	bool hide = false;
	// int hidRound = -1;
	bool doubleBack = false;
	// int DBround = -1;
	
	// check if we have double backed once before
	for (int i = 0; i < numReturnedLocs1; i++) {
		if (DOUBLE_BACK_1 <= moveList[i] && moveList[i] <= DOUBLE_BACK_5) {
			doubleBack = true;
			// DBround = i;
		} 
		if (moveList[i] == HIDE) {
			hide = true;
			// hidRound = i;
		}
	}
	// printf("pass2\n");
	for (int i = 0; i < numReturnedLocs2; i++) {
		if (doubleBack) {
			bool hasVisitied = false;
			for (int j = 0; j < numReturnedTrail; j++) {
				if (trail[j] == listofconnections[i]) {
					hasVisitied = true;
				}
			}
			if (!hasVisitied) {
				idList[idListCount] = listofconnections[i];
				idListCount++;
			}
		} else {
			if (listofconnections[i] != DvGetPlayerLocation(dv, PLAYER_DRACULA)) {
				idList[idListCount] = listofconnections[i];
				idListCount++;
			}
		}
	}
	// printf("pass3\n");
	if (!doubleBack) {
		for (int i = 0; i < GvGetRound(dv->gv); i++) {
			idList[idListCount] = DOUBLE_BACK_1 + i;
			idListCount++;
		}
	}

	if (!hide && numReturnedTrail > 0) {
		idList[idListCount] = HIDE;
		idListCount++;
	}
	
	free(moveList);
	free(listofconnections);
	free(trail);

	*numReturnedMoves = idListCount;
	// printf("idlistcount = %d\n", idListCount);
	/*
	for (int i = 0; i < idListCount; i++) {
		printf("idlist[%d] = %s\n", i, placeIdToName(idList[i]));
	}
	*/
	return idList;

}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	//gets the last 6 locations of the dracula (trail)
	int numReturnedLocs1;
	bool canFree;
	PlaceId* moveList = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 6, &numReturnedLocs1, &canFree);

	//current location of dracula
	PlaceId draclocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	//all the connected locations to the current location
	int numReturnedLocs2;
	PlaceId *listofconnections = GvGetReachable(dv->gv,PLAYER_DRACULA,DvGetRound(dv), draclocation , &numReturnedLocs2);

	// int size = numReturnedLocs2;
	// int invalid = 0;


	int numReturnedTrail;
	bool canFreeTrail;
	PlaceId *trail = GvGetLastLocations(dv->gv, PLAYER_DRACULA, 6, &numReturnedTrail, &canFreeTrail);

	PlaceId *idList = malloc(sizeof(PlaceId)*NUM_REAL_PLACES);
	int idListCount = 0;

	//if there is no location for the dracula to go to, it will teleport to Castle Dracula
	
	if (draclocation == NOWHERE || numReturnedLocs2 == 0) {
		idList[0] = CASTLE_DRACULA;
		*numReturnedLocs = 1;

	} 
	
	// int hide = FALSE;
	// int doubleback = FALSE;
	// bool hide = false;
	// int hidRound = -1;
	bool doubleBack = false;
	// int DBround = -1;
	
	// check if we have double backed once before
	for (int i = 0; i < numReturnedLocs1; i++) {
		if (DOUBLE_BACK_1 <= moveList[i] && moveList[i] <= DOUBLE_BACK_5) {
			doubleBack = true;
			// DBround = i;
		} 
		if (moveList[i] == HIDE) {
			// hide = true;
			// hidRound = i;
		}
	}

	
	for (int i = 0; i < numReturnedLocs2; i++) {
		if (doubleBack) {
			bool hasVisitied = false;
			for (int j = 0; j < numReturnedTrail; j++) {
				if (trail[j] == listofconnections[i]) {
					hasVisitied = true;
				}
			}
			if (!hasVisitied) {
				idList[idListCount] = listofconnections[i];
				idListCount++;
			}
		} else {
			idList[idListCount] = listofconnections[i];
			idListCount++;
		}
	}


	
	free(moveList);
	free(listofconnections);
	free(trail);

	*numReturnedLocs = idListCount;
	// printf("idListCount = %d\n", idListCount);

	return idList;

}


PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int numReturnedLocs1;
	bool canFree;
	PlaceId* moveList = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 6, &numReturnedLocs1, &canFree);

	//current location of dracula
	PlaceId draclocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	//all the connected locations to the current location
	int numReturnedLocs2;
	PlaceId *listofconnections = GvGetReachableByType(dv->gv,PLAYER_DRACULA,DvGetRound(dv), draclocation , road, false, boat, &numReturnedLocs2);

	// int size = numReturnedLocs2;
	// int invalid = 0;


	int numReturnedTrail;
	bool canFreeTrail;
	PlaceId *trail = GvGetLastLocations(dv->gv, PLAYER_DRACULA, 6, &numReturnedTrail, &canFreeTrail);

	PlaceId *idList = malloc(sizeof(PlaceId)*NUM_REAL_PLACES);
	int idListCount = 0;

	//if there is no location for the dracula to go to, it will teleport to Castle Dracula
	if (draclocation == NOWHERE || numReturnedLocs2 == 0) {
		idList[0] = CASTLE_DRACULA;
		*numReturnedLocs = 1;

	} 
	
	// int hide = FALSE;
	// int doubleback = FALSE;
	// bool hide = false;
	// int hidRound = -1;
	bool doubleBack = false;
	// int DBround = -1;
	
	// check if we have double backed once before
	for (int i = 0; i < numReturnedLocs1; i++) {
		if (DOUBLE_BACK_1 <= moveList[i] && moveList[i] <= DOUBLE_BACK_5) {
			doubleBack = true;
			// DBround = i;
		} 
		if (moveList[i] == HIDE) {
			// hide = true;
			// hidRound = i;
		}
	}

	
	for (int i = 0; i < numReturnedLocs2; i++) {
		if (doubleBack) {
			bool hasVisitied = false;
			for (int j = 0; j < numReturnedTrail; j++) {
				if (trail[j] == listofconnections[i]) {
					hasVisitied = true;
				}
			}
			if (!hasVisitied) {
				idList[idListCount] = listofconnections[i];
				idListCount++;
			}
		} else {
			idList[idListCount] = listofconnections[i];
			idListCount++;
		}
	}


	
	free(moveList);
	free(listofconnections);
	free(trail);

	return idList;


}


PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *idList = malloc(dv->round*sizeof(PlaceId));
	*numReturnedLocs = 0;
	//Defining the current location of the player given as CurrCityId
	PlaceId CurrCityId = GvGetPlayerLocation(dv->gv, player);

	//If the player is currently NOWHERE i.e hasn't made a move, return NULL
	if(CurrCityId == NOWHERE) {
		return NULL;
	}

	//Using MapGetConnection which gives a linked list of all the connections to that city
	PlaceId *list = GvGetReachable(dv->gv,player,dv->round,CurrCityId,numReturnedLocs);

	for(int i = 0; i < *numReturnedLocs; i++) {
		idList[i] = list[i];
	}
	//Transferring all of the places in List into idList
	free(list);

	return idList;
}


PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *idList = malloc(dv->round*sizeof(PlaceId));
	*numReturnedLocs = 0;
	//Defining the current location of the player given as CurrCityId
	PlaceId CurrCityId = GvGetPlayerLocation(dv->gv, player);
	//If the player is currently NOWHERE i.e hasn't made a move, return NULL
	if(CurrCityId == NOWHERE) {
		return NULL;
	}
	// printf("numReturnedLocs before GvGetReachablebyType is %d\n",(*numReturnedLocs));
	// printf("Flag1\n");
	// printf("%d\n",CurrCityId);
	PlaceId *list = GvGetReachableByType(dv->gv,player,dv->round,CurrCityId,road,rail,boat,numReturnedLocs);
	// printf("Flag2\n");
	//Transferring all of the places in List into HWCIGBT
	int i = 0;
	while(i < *numReturnedLocs) {
		idList[i] = list[i];
		i++;
	}
<<<<<<< HEAD
	printf("numReturnedLocs after GvGetReachablebyType is %d\n",*numReturnedLocs);
	free(list);
=======
	// printf("numReturnedLocs after GvGetReachablebyType is %d\n",*numReturnedLocs);
>>>>>>> e315453874c3dd34d423018de1896dd5c6aa778f
	return idList;
}


////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

/*
void draculatrail(DraculaView dv, Player player, PlaceId trail[TRAIL_SIZE]) {

	GvGetLocationHistory(dv->gv, player, *numReturnedLocs, *canFree);
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
*/
