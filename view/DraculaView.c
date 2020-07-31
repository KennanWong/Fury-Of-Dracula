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
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	new->map = MapNew();					// initialise the new map
	new->round = 0;							// initialise round number to 1
	new->CurrentScore = GAME_START_SCORE;	// initialise game score to START_SCORE
	new->players = malloc(5*sizeof(Players));	// an array of PlayerStructs
	new->turnCounter = 0;
	new->VampireLocation = NOWHERE;
	new->VampireStatus = 0;
	new->RoundOfVampire = -1;

	// intialise the players
	for (int i = 0; i < 5; i++){
		Players *newPlayer = malloc(sizeof(Players));
		if (i == 0) {
			newPlayer->player = PLAYER_LORD_GODALMING;
		} else if (i == 1) {
			newPlayer->player = PLAYER_DR_SEWARD;
		} else if (i == 2) {
			newPlayer->player = PLAYER_VAN_HELSING;
		} else if (i == 3) {
			newPlayer->player = PLAYER_MINA_HARKER;
		} else {
			newPlayer->player = PLAYER_DRACULA;
		}
		if (i <= 3) {
			newPlayer->health = GAME_START_HUNTER_LIFE_POINTS;
		} else {
			newPlayer->health = GAME_START_BLOOD_POINTS;
		}
		newPlayer->currLoc = NOWHERE;
		
		newPlayer->numTurns = 0;
		new->players[i] = newPlayer;
	
	}

	new->messages = messages;

	// process pastplays and seperate each to their respective players
	// This is done so by first tokenising the past plays string splitting them into 6 character strings
	// 'i' will be used to count the number of turns taken, this will be used to determine whose turn it is
	// (i%5) and what game round we are in (i/5), i will also be used to determine the turn counter.

	/* when processing past plays
	1. determine whose play it was --> update location and add to their past plays
	2. then determine any actions, deduct life points as directed or process traps
		this must be split as it weill be different for hunters and the dracula
	3. move onto the next player
	4. once we process one entire round, update round counter, and deduct points accordingly

	*/
	// Processing the moves of each player
	char *tempPastPlays = strdup(pastPlays);
	if (strlen(pastPlays) != 0) {
		// take each 6 char play seperately
		char *str = strtok(tempPastPlays, " ");
		int i = 0;
		while (str != NULL) {	
			// i%5 will determine who made the move
			int playerId = i%5;
			Players *currPlayer = new->players[playerId];
			
			// add the 6char string to the players past plays
			char *tmp = strdup(str);
			printf("move = %s\n", tmp);
			assert(tmp != NULL);
			currPlayer->numTurns += 1;
			i+=1;
			// if we are processing draculas move
			if (str[0] == 'D') {
				ProcessDracula(new, str);
			} else {
				if (currPlayer->health == 0) {
					currPlayer->health = GAME_START_HUNTER_LIFE_POINTS;
				}
				ProcessHunter(new, str, currPlayer);
			}
			
			// Move onto next person turn
			str = strtok(NULL, " ");
			
			if (new->round != i/5) {
				// we have reached a new round, deduct points
				new->CurrentScore = new->CurrentScore - SCORE_LOSS_DRACULA_TURN;
				new->round = i/5;
			}

			new->turnCounter = i;
		}
		new->round = i/5;
		
	}
	new->pastGamePlays = tempPastPlays;
	free(tempPastPlays);
	
	return new;
}

void DvFree(DraculaView dv)
{
	
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(dv->players->pastPlays);
	free(dv->players);
	
	MapFree(dv->map);
	free(dv);

	
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetRound(dv->gv);

}

int DvGetScore(DraculaView dv)
{
	//Note: Dracula wants the game score to be low.

	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetHealth(dv->gv, players[player]);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetPlayerLocation(dv->gv, players[player]);



PlaceId DvGetVampireLocation(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// Cycle through each city and gather all traps, return number of traps back
	return GvGetTrapLocations(dv->gv, numTraps);

}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	LocationID trail[TRAIL_SIZE] = {0};

	//gets the last 6 locations of the dracula (trail)
	PlaceId* visitedList = draculatrail(dv, PLAYER_DRACULA, trail[TRAIL_SIZE])

	//current location of dracula
	LocationID draclocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	//all the connected locations to the current location
	PlaceId listofconnections = GvGetReachable(dv->gv,PLAYER_DRACULA,dv->round,draclocation,numReturnedLocs);

	int size = *numLocations;
	int invalid = 0;
	int hide = FALSE;
	int doubleback = FALSE;

	//if there is no location for the dracula to go to, it will teleport to Castle Dracula
	

	if (draclocation == NOWHERE) {
			flag = 1;
	} 
	
	if (listofconnections == NULL) {
		flag = 1;
	}

	while (listofconnections != NULL) {

		//checks to see if trail or double back is in the trail
		//if it is present, marks either doubleback or hide as true as these are locations that cannot be accessed now
		int i = 0;
		while (i < TRAIL_SIZE) {
			if(trail[i] == DOUBLE_BACK_1 || trail[i] == DOUBLE_BACK_2 || trail[i] == DOUBLE_BACK_3 ||
			trail[i] == DOUBLE_BACK_4 ||trail[i] == DOUBLE_BACK_5) {
				doubleback = TRUE;
			}

			if(dracTrail[i] == HIDE) {
				hide = TRUE;
			}
			i++
		}

		//incrememnts count everytime a possible location the dracula can travel to is also the same location
		//which is present in trail
		if(doubleback == TRUE){
			for(int d = 0; d < TRAIL_SIZE; d++){
				for(int k = 0; k < size; k++){
					if(trail[d] == listofconnections[k]){
						invalid++;
						listofconnections[k] = -1;
					}
				}
			}
		}

		//incrememnts count everytime a possible location the dracula can travel to is also the same location
		//which is present in trail
		if(hide == TRUE){
			for(int p = 0; p < size; p++){
				if(listofconnections[p] == draclocation){
					listofconnections[p] = -1;
					invalid++;
				}
			}
		}

		//the real amount of locations the dracula can travel to after adapting to dracula restrictions
		*numLocations = size - invalid;
		LocationID *newconnections = malloc(sizeof(LocationID) * (*numLocations));

		int l = 0, m = 0;
		while(l < numLocations) {
			if(listofconnections[m] != -1){
				if ((listofconnections->type == ROAD) || (listofconnections->type == BOAT)) {
					newconnections[l] = listofconnections[m];
					l++;
				}
			}
			m++;
		}
	}

	if (flag == 1) {
		*numReturnedLocs = 0;
		return NULL;
	}

	return newconnections;

}


PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	LocationID trail[TRAIL_SIZE] = {0};

	//gets the last 6 locations of the dracula (trail)
	PlaceId* visitedList = draculatrail(dv, PLAYER_DRACULA, trail[TRAIL_SIZE])

	//current location of dracula
	LocationID draclocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	//all the connected locations to the current location
	PlaceId listofconnections = GvGetReachableByType(dv->gv,PLAYER_DRACULA,dv->round,draclocation, road, 0, boat, numReturnedLocs);

	int size = *numLocations;
	int invalid = 0;
	int hide = FALSE;
	int doubleback = FALSE;

	//if there is no location for the dracula to go to, it will teleport to Castle Dracula
	

	if (draclocation == NOWHERE) {
			flag = 1;
	} 
	
	if (listofconnections == NULL) {
		flag = 1;
	}

	while (listofconnections != NULL) {

		//checks to see if trail or double back is in the trail
		//if it is present, marks either doubleback or hide as true as these are locations that cannot be accessed now
		int i = 0;
		while (i < TRAIL_SIZE) {
			if(trail[i] == DOUBLE_BACK_1 || trail[i] == DOUBLE_BACK_2 || trail[i] == DOUBLE_BACK_3 ||
			trail[i] == DOUBLE_BACK_4 ||trail[i] == DOUBLE_BACK_5) {
				doubleback = TRUE;
			}

			if(dracTrail[i] == HIDE) {
				hide = TRUE;
			}
			i++
		}

		//incrememnts count everytime a possible location the dracula can travel to is also the same location
		//which is present in trail
		if(doubleback == TRUE){
			for(int d = 0; d < TRAIL_SIZE; d++){
				for(int k = 0; k < size; k++){
					if(trail[d] == listofconnections[k]){
						invalid++;
						listofconnections[k] = -1;
					}
				}
			}
		}

		//incrememnts count everytime a possible location the dracula can travel to is also the same location
		//which is present in trail
		if(hide == TRUE){
			for(int p = 0; p < size; p++){
				if(listofconnections[p] == draclocation){
					listofconnections[p] = -1;
					invalid++;
				}
			}
		}

		//the real amount of locations the dracula can travel to after adapting to dracula restrictions
		*numLocations = size - invalid;
		LocationID *newconnections = malloc(sizeof(LocationID) * (*numLocations));

		int l = 0, m = 0;
		while(l < numLocations) {
			if(listofconnections[m] != -1){
				if ((listofconnections->type == ROAD) || (listofconnections->type == BOAT)) {
					newconnections[l] = listofconnections[m];
					l++;
				}
			}
			m++;
		}
	}

	if (flag == 1) {
		*numReturnedLocs = 0;
		return NULL;
	}

	return newconnections;

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
	return idList;
}


////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

PlaceId CityIdFromMove(char *str) {
	char abbrev[2];
	for (int i = 1; i < 3; i++) {
		abbrev[i-1] = str[i];
	}
	printf("abbreviation = %s\n", abbrev);
	return placeAbbrevToId(abbrev);
}

void ActionFromMove(char *str, char *action) {
	for (int i = 3; i < 7; i++) {
		action[i - 3] = str[i];
	}
}

void ProcessDracula(GameView gv, char *move) {
	// Process movement
	Players *Dracula = gv->players[PLAYER_DRACULA];
	PlaceId currLoc = CityIdFromMove(move);
	if (move[1] == 'D') {
		// Dracula has double backed
		// need to get list of previous locations
		int NumRetrunedLocations;
		bool canFree = true;
		PlaceId *DraculaTrail = malloc(7*sizeof(PlaceId));
		DraculaTrail = GvGetLastLocations(gv, Dracula->player, 7, &NumRetrunedLocations, &canFree);
		if ((move[2] - '0') < NumRetrunedLocations) {
			currLoc = DraculaTrail[move[2] - '0'];
		}
		
		free(DraculaTrail);
	} else if (currLoc == HIDE) {
		currLoc = Dracula->currLoc;
	} else if (placeIdToType(currLoc) == SEA) {
		gv->players[PLAYER_DRACULA]->health = gv->players[PLAYER_DRACULA]->health - LIFE_LOSS_SEA;
	}
	Dracula->currLoc = currLoc;

	// Process action
	char action[4];
	ActionFromMove(move, action);
	if (action[0] == 'T') {
		// Add trap to location
		printf("adding trap\n");
		AddTrapToLoc(currLoc, gv->map);
		printf("added trap to location\n");
	}
	if (action[1] == 'V') {
		// Add vampire to location
		AddVampireToLoc(currLoc, gv->map);
		gv->VampireStatus = 1;
		gv->VampireLocation = currLoc;
		gv->RoundOfVampire = gv->round;
	}
	if (action[2] == 'M') {
		// Remove a trap
		int NumRetrunedLocations;
		bool canFree = true;
		PlaceId *DraculaTrail = malloc(8*sizeof(PlaceId));
		DraculaTrail = GvGetLastLocations(gv, gv->players[PLAYER_DRACULA]->player, 7, &NumRetrunedLocations, &canFree);
		if (NumRetrunedLocations == 7) {
			RemoveTrapsFromLoc(DraculaTrail[6], gv->map);
		}
		free(DraculaTrail);
	} else if (action[2] == 'V') {
		// Mature Vamp
		MatureVampire(gv);
	}
}

// Resets the vampire
void MatureVampire(GameView gv) {
	RemoveVampireFromLoc(gv->VampireLocation, gv->map);
	gv->CurrentScore = gv->CurrentScore - SCORE_LOSS_VAMPIRE_MATURES;
	gv->VampireLocation = NOWHERE;
	gv->VampireStatus = 10;
	gv->RoundOfVampire = -1;
}

void MoveLocation(Draculaview gv) {
	if (player == PLAYER_DRACULA) {
		return DvGetPlayerLocation(dv, player);
	} else {
		return GvGetMoveHistory(gv, player, *numReturnedMoves, *canFree);
	}
}

void draculatrail(DracView dv, Player player, LocationID trail[TRAIL_SIZE]) {

	GvGetLocationHistory(dv->gv, player, numReturnedLocs, canFree);
	if(player == PLAYER_DRACULA){
			for(int i = 0; i < TRAIL_SIZE; i++){
				print
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
