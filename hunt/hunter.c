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
	printf("hunvter AI v1.2\n");
	printf("now deciding hunter move\n");
	Player currPlayer = HvGetPlayer(hv);
	PlaceId currLoc = HvGetPlayerLocation(hv, currPlayer);
	

    if (HvGetHealth(hv, currPlayer) - LIFE_LOSS_TRAP_ENCOUNTER <  0) {
        registerBestPlay(placeIdToAbbrev(HvGetPlayerLocation(hv, currPlayer)), "this is a message");
        return;
    }

	// first move
    if(HvGetPlayerLocation(hv,HvGetPlayer(hv)) == NOWHERE) {
		printf("players first move\n");
		Player currPlayer = HvGetPlayer(hv);
		// Assign hunters to different locations
		if(currPlayer == PLAYER_LORD_GODALMING) registerBestPlay("BC","This is a message");
		if(currPlayer == PLAYER_DR_SEWARD) registerBestPlay("ZA","This is a message");
		if(currPlayer == PLAYER_VAN_HELSING) registerBestPlay("ST","This is a message");
		if(currPlayer == PLAYER_MINA_HARKER) registerBestPlay("BR","This is a message");
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
			registerBestPlay(placeIdToAbbrev(currLoc),"This is a message");
			free(canGo);
			return;
		}
		// PlaceId toGo;
		if (dest == NOWHERE) {
			printf("we dont know where dracula is\n");
			// Player current = HvGetPlayer(hv);
			// If the current player is LORD GODALMING

			// Get array of players past locations

			PlaceId *CurrentLocOfPlayers = malloc(sizeof(PlaceId)*4);
			for (int i = 0; i < 4; i++) {
				CurrentLocOfPlayers[i] = HvGetPlayerLocation(hv, i);
			}
			
			for (int i = 0; i < numReturnedLocs; i++) {
				if (!placeIdinList(canGo[i], CurrentLocOfPlayers, 4)) {
					toGo = canGo[i];
				}
			}

			
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