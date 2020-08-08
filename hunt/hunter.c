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

#include <stdio.h>

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include "Places.h"


#include <stdio.h>
#include "Places.h"

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
	if(HvGetPlayerLocation(hv,HvGetPlayer(hv)) == NOWHERE) {
		registerBestPlay("AL","This is a message");
	}
	else {
		int *round = 0;
		int numReturnedLocs = 0;
		// If Dracula's last location is known
		PlaceId dest = HvGetLastKnownDraculaLocation(hv,round);
		PlaceId toGo;
		if(dest != NOWHERE) {
			// Finding the shortest path to last known dracula's location
			int *pathlength = 0;
			PlaceId *shortestPath = HvGetShortestPathTo(hv,HvGetPlayer(hv),dest,pathlength);
			toGo = shortestPath[0];
		} 
		// Else if Dracula's last location is not known
		else {
			// Finding all possible moves that the current player can make
			PlaceId *canGo = HvWhereCanIGo(hv,&numReturnedLocs);
			
			for(int i = 0; i < 4; i++) {
				PlaceId currLoc = HvGetPlayerLocation(hv,i);
				for(int m = 0; m < numReturnedLocs; m++) {
					if(canGo[m] == currLoc) {
						canGo[m] = -1;
					}
				}
			}
			int c = 0;
			while(canGo[c] == -1) {
				c++;
			}
			toGo = canGo[c];
		}
		registerBestPlay(placeIdToAbbrev(toGo),"This is a message");
	}
}
