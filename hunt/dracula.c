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
	if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE) {
		registerBestPlay("CD", "MWUHAHAHHHA");
		return;
	}
	
	int returnedLoc = 0;
	PlaceId *canGo = DvGetValidMoves(dv, &returnedLoc);
	if (returnedLoc == 0) {
		// teleport back to castle dracula
		printf("attempting to teleport back to Castle\n");
		registerBestPlay("CD", "MWUHAHAHHHA");
		free(canGo);
		return;
	} else {
		
		registerBestPlay(placeIdToAbbrev(canGo[0]), "MWUHAHAHHHA");
		free(canGo);
		return;
	}
}