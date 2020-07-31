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
	char *pastGamePlays;
	int turnCounter;
	int VampireStatus;		// -> 0 if there is no vampire, 1 if it is immature, 2 if it has matured
	int RoundOfVampire;
	PlaceId VampireLocation;
	// TODO: ADD FIELDS HERE
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}

	return new;
}

void DvFree(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int DvGetScore(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	return NULL;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
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
	printf("numReturnedLocs before GvGetReachablebyType is %d\n",(*numReturnedLocs));
	printf("Flag1\n");
	printf("%d\n",CurrCityId);
	PlaceId *list = GvGetReachableByType(dv->gv,player,dv->round,CurrCityId,road,rail,boat,numReturnedLocs);
	printf("Flag2\n");
	//Transferring all of the places in List into HWCIGBT
	int i = 0;
	while(i < *numReturnedLocs) {
		idList[i] = list[i];
		i++;
	}
	printf("numReturnedLocs after GvGetReachablebyType is %d\n",*numReturnedLocs);
	free(list);
	return idList;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
