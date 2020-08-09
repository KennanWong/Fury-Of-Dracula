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
bool placeIdinList(PlaceId check, PlaceId *list, int ListCount);


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
		PlaceId toGo = NOWHERE;
		for (int c = 0; c < returnedLoc; c++) {
			int conflictingLoc = 0;
			for (int p = 0; p < 4; p++) {
				int numReturnedHunterLoc = 0;
				PlaceId *playerCanGoTo = DvWhereCanTheyGo(dv, p, &numReturnedHunterLoc);
				if (placeIdinList(canGo[c], playerCanGoTo, numReturnedHunterLoc)) {
					conflictingLoc = 1;
				}
				free(playerCanGoTo);
			}
			if (conflictingLoc == 0) {
				toGo = canGo[c];
			}
		}
		if (toGo == NOWHERE) {
			toGo = canGo[DvGetScore(dv)%returnedLoc];
		}
		registerBestPlay(placeIdToAbbrev(toGo), "MWUHAHHAHAH");
		free(canGo);
		return;
	}
}


bool placeIdinList(PlaceId check, PlaceId *list, int ListCount) {
	for (int i = 0 ; i < ListCount; i++) {
		if (check == list[i]) {
			return true;
		}
	}
	return false;
}