////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include <stdio.h>

bool placeIdinList(PlaceId check, PlaceId *list, int ListCount);

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
	printf("hunvter AI v1.1\n");
	printf("now deciding hunter move\n");
	Player currPlayer = HvGetPlayer(hv);
	PlaceId currLoc = HvGetPlayerLocation(hv, currPlayer);
	

    if (HvGetHealth(hv, currPlayer) - LIFE_LOSS_TRAP_ENCOUNTER <  0) {
        registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, currPlayer)), "this is a message");
        return;
    }

	// first move
    if(HvGetPlayerLocation(hv,HvGetPlayer(hv)) == NOWHERE) {
		// go to somewhere where another hunter has not been to
		PlaceId *playersHaveBeen = malloc(sizeof(PlaceId)*4);
		for (int i = 0; i < currPlayer; i++) {
			playersHaveBeen[i] = HvGetPlayerLocation(hv, i);
		}
		PlaceId temp = ALICANTE;
		while (placeIdinList(temp, playersHaveBeen, currPlayer)) {
			temp++;
		}
		registerBestPlay(placeIdToAbbrev(temp),"This is a message");
		free(playersHaveBeen);
		return;
	}
	else {
		printf("not players first move\n");
		printf("player currently at %s\n", placeIdToName(currLoc));
		int round = 0;
		int numReturnedLocs = 0;
		PlaceId *canGo = HvWhereCanIGo(hv,&numReturnedLocs);
		PlaceId toGo;
		// If Dracula's last location is known
		PlaceId dest = HvGetLastKnownDraculaLocation(hv, &round);
		if (dest == currLoc) {
			// we are at draculas location, stick with him
			printf("we are in the same city as dracula\n");
			printf("player currently at %s\n", placeIdToAbbrev(currLoc));
			toGo = currLoc;
		} else {
			if (dest == NOWHERE) {
				printf("we dont know where dracula is\n");
				toGo = canGo[0];
			} else {
				printf("draculas location %s\n", placeIdToName(dest));
				int pathLength = 0;
				PlaceId *PathToDrac = HvGetShortestPathTo(hv, currPlayer, dest, &pathLength);
				toGo = PathToDrac[0];
				free(PathToDrac);
			}
		}
		// PlaceId toGo;
		
		
		registerBestPlay(placeIdToAbbrev(toGo),"This is a message");
		free(canGo);
	}
    return;
}


bool placeIdinList(PlaceId check, PlaceId *list, int ListCount) {
	for (int i = 0 ; i < ListCount; i++) {
		if (check == list[i]) {
			return true;
		}
	}
	return false;
}