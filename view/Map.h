////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... The Fury of Dracula
// Map.h: an interface to a Map data type
// You can change this as much as you want!
// If you modify this, you should submit the new version.
//
// 2017-11-30   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdlib.h>

#include "Places.h"

#ifndef FOD__MAP_H_
#define FOD__MAP_H_

typedef struct connNode *ConnList;
struct connNode {
	PlaceId p; // ALICANTE, etc.
	TransportType type; // ROAD, RAIL, BOAT
	ConnList next; // link to next node
	int numTraps;
	int vampireState;		// 0 if there is no vampire, 1 if it is immature
	int DraculasTrail;		// int value to represent where it is in the trail
};

// Map representation is hidden
typedef struct map *Map;

/** Creates a new map. */
Map MapNew(void);

/** Frees all memory allocated for the given map. */
void MapFree(Map m);

/** Prints a map to `stdout`. */
void MapShow(Map m);

/** Gets the number of places in the map. */
int MapNumPlaces(Map m);

/** Gets the number of connections of a particular type. */
int MapNumConnections(Map m, TransportType type);

/**
 *  Gets a list of connections from the given place.
 *  The returned list should NOT be modified or freed.
 */
ConnList MapGetConnections(Map m, PlaceId p);


/** Adds a trap to a given city*/
void AddTrapToLoc(PlaceId id, Map m);

/** Adds a vampire to a given city*/
void AddVampireToLoc(PlaceId id, Map m);

/** Removes a mature vampire from a given city */
void RemoveVampireFromLoc(PlaceId id, Map m);


/** Removes a trave from a given city */
void RemoveTrapFromLoc(PlaceId id, Map m);

/** Removes all traps from a given city */
void RemoveTrapsFromLoc(PlaceId id, Map m);


/** Returns number of traps at a location */
int GetTrapsLoc(PlaceId Loc, Map m);


#endif // !defined(FOD__MAP_H_)
