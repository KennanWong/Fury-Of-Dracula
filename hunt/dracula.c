////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "Places.h"

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"

// helper methods decleration
PlaceId analysemoves(DraculaView dv, PlaceId *possiblemoves, int numReturnedLocs1);
int Trail(DraculaView dv, PlaceId place);

void decideDraculaMove(DraculaView dv) {
	// Trail Helper method
	int Trail(DraculaView dv, PlaceId place) {

		PlaceId *trail = malloc(sizeof(*trail)*TRAIL_SIZE);
		//draculatrail(dv, PLAYER_DRACULA, trail);
		for (int i = 0; i < TRAIL_SIZE; i++) {
			if (trail[i] == place) {
				return 1; //True
			}
		}
		return 0;//False
	}

	// Analysemoves helper method
	PlaceId analysemoves(DraculaView dv, PlaceId *possiblemoves, int numReturnedLocs1) {

		int finalmove = NOWHERE;

		printf("possible places drac can travel to");

		for (int i = 0; i < numReturnedLocs1; i++) {
			printf("%d\n", possiblemoves[i]);
		}

		for (int i = 0; i < numReturnedLocs1 && Trail(dv, possiblemoves[i]); i++) {
			finalmove = possiblemoves[i];
		}

		return finalmove;
	}

	int numReturnedLocs1;
	
	if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == UNKNOWN_PLACE) {
		registerBestPlay("GE", "going to Geneva");
		return;
	}

	int *possiblemoves = DvWhereCanIGo(dv, &numReturnedLocs1);
	int finalmove =  analysemoves(dv, possiblemoves, numReturnedLocs1);

	registerBestPlay(placeIdToAbbrev(finalmove), "Mwuhahahaha");
}

