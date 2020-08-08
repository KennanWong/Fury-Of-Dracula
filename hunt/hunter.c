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


void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!

	int numReturnedLocs;
	PlaceId *PossPlaces = HvWhereCanIGo(hv, &numReturnedLocs);

	if (numReturnedLocs > 0) {
		printf("recieved locations");
		char *toGo = placeIdToAbbrev(PossPlaces[0]);
		free(PossPlaces);
		registerBestPlay(toGo, "Have we nothing Toulouse?");
	}
	
	

	
	
}
