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

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
	int *round = 0;
	int *numReturnedLocs = 0;
	// If Dracula's last location is known
	if(HvGetLastKnownDraculaLocation(hv,round) != NOWHERE) {
		// Finding the shortest path to last known dracula's location
		PlaceId dest = HvGetLastKnownDraculaLocation(hv,round);
		int *pathlength = 0;
		PlaceId *path = HvGetShortestPathTo(hv,HvGetPlayer(hv),dest,pathlength);
		printf("path[0] is %d\n",path[0]);

	} 
	// Else if Dracula's last location is not known
	else {
		// Finding all possible moves that the current player can make
		PlaceId *paths = HvWhereCanIGo(hv,numReturnedLocs);
		printf("paths[0] is %d\n",paths[0]);
	}
	registerBestPlay("TO", "Have we nothing Toulouse?");
}
