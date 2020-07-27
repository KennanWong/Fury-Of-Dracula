////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// Map.c: an implementation of a Map type
// You can change this as much as you want!
// If you modify this, you should submit the new version.
//
// 2017-11-30	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "Map.h"
#include "Places.h"

struct map {
	int nV; // number of vertices
	int nE; // number of edges
	ConnList connections[NUM_REAL_PLACES];
	ConnList BSTHead;
	PlaceId *TrapLocations;
	int numTrapLocations;
	int numTrapsUnknown;
};

static void addConnections(Map m);
static void addConnection(Map m, PlaceId v, PlaceId w, TransportType type);
static inline bool isSentinelEdge(Connection c);

static ConnList connListInsert(ConnList l, PlaceId v, TransportType type);
static bool connListContains(ConnList l, PlaceId v, TransportType type);

////////////////////////////////////////////////////////////////////////

/** Creates a new map. */
// #vertices always same as NUM_REAL_PLACES
Map MapNew(void)
{
	Map m = malloc(sizeof(*m));
	if (m == NULL) {
		fprintf(stderr, "Couldn't allocate Map!\n");
		exit(EXIT_FAILURE);
	}

	m->nV = NUM_REAL_PLACES;
	m->nE = 0;
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		m->connections[i] = NULL;
	}
	addConnections(m);
	
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		// m->connections[i]->traps = malloc(sizeof(*m->connections[i]->traps));
		m->connections[i]->numTraps = 0;
		m->connections[i]->DraculasTrail = 0;
		m->connections[i]->numTraps = 0;
		m->connections[i]->vampireState = 0;
	}

	m->TrapLocations = malloc(sizeof(*m->TrapLocations));
	m->numTrapLocations = 0;
	m->numTrapsUnknown = 0;

	return m;
}

/** Frees all memory allocated for the given map. */
void MapFree(Map m)
{
	assert (m != NULL);

	for (int i = 0; i < m->nV; i++) {
		ConnList curr = m->connections[i];
		// free(m->connections[i]->traps);
		printf("freed traps, now will free adjacency list\n");
		while (curr != NULL) {
			ConnList next = curr->next;
			free(curr);
			curr = next;
		}
		
	}
	free(m->TrapLocations);
	free(m);
}

////////////////////////////////////////////////////////////////////////

/** Prints a map to `stdout`. */
void MapShow(Map m)
{
	assert(m != NULL);

	printf("V = %d, E = %d\n", m->nV, m->nE);
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			printf("%s connects to %s by %s\n",
			       placeIdToName((PlaceId) i),
			       placeIdToName(curr->p),
			       transportTypeToString(curr->type)
			);
		}
	}
}

////////////////////////////////////////////////////////////////////////

/** Gets the number of places in the map. */
int MapNumPlaces(Map m)
{
	assert(m != NULL);
	return m->nV;
}

/** Gets the number of connections of a particular type. */
int MapNumConnections(Map m, TransportType type)
{
	assert(m != NULL);
	assert(transportTypeIsValid(type) || type == ANY);

	int nE = 0;
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			if (curr->type == type || type == ANY) {
				nE++;
			}
		}
	}

	return nE;
}

////////////////////////////////////////////////////////////////////////

/// Add edges to Graph representing map of Europe
static void addConnections(Map m)
{
	assert(m != NULL);

	for (int i = 0; !isSentinelEdge(CONNECTIONS[i]); i++) {
		addConnection(m, CONNECTIONS[i].v, CONNECTIONS[i].w, CONNECTIONS[i].t);
	}
}

/// Add a new edge to the Map/Graph
static void addConnection(Map m, PlaceId start, PlaceId end, TransportType type)
{
	assert(m != NULL);
	assert(start != end);
	assert(placeIsReal(start));
	assert(placeIsReal(end));
	assert(transportTypeIsValid(type));

	// don't add edges twice
	if (connListContains(m->connections[start], end, type)) return;

	m->connections[start] = connListInsert(m->connections[start], end, type);
	m->connections[end]   = connListInsert(m->connections[end], start, type);
	m->nE++;
}

/// Is this the magic 'sentinel' edge?
static inline bool isSentinelEdge(Connection c)
{
	return c.v == -1 && c.w == -1 && c.t == ANY;
}

/// Insert a node into an adjacency list.
static ConnList connListInsert(ConnList l, PlaceId p, TransportType type)
{
	assert(placeIsReal(p));
	assert(transportTypeIsValid(type));

	ConnList new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate ConnNode");
		exit(EXIT_FAILURE);
	}
	
	new->p = p;
	new->type = type;
	new->next = l;

	// form BST

	return new;
}

/// Does this adjacency list contain a particular value?
static bool connListContains(ConnList l, PlaceId p, TransportType type)
{
	assert(placeIsReal(p));
	assert(transportTypeIsValid(type));

	for (ConnList curr = l; curr != NULL; curr = curr->next) {
		if (curr->p == p && curr->type == type) {
			return true;
		}
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////

ConnList MapGetConnections(Map m, PlaceId p)
{
	assert(placeIsReal(p));
	return m->connections[p];
}

////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// Newly added functions

void AddTrapToLoc(PlaceId id, Map m) {
	if (id > MAX_REAL_PLACE) {
		m->numTrapsUnknown += 1;
	} else {
		ConnList Loc = m->connections[id];
		// Loc->traps[Loc->numTraps] = 'T';
		Loc->numTraps += 1;	
	}
	
}

void AddVampireToLoc(PlaceId id, Map m) {
	/*
	May not be necessary
	ConnList Loc = m->connections[id-1]->;
	printf("check1.1\n");
	assert(Loc->p == id);
	Loc->vampireState = 1;
	printf("check1.3\n");
	*/ 
	
}

void RemoveVampireFromLoc(PlaceId id, Map m) {
	ConnList Loc = m->connections[id];
	Loc->vampireState = 0;
	printf("done\n");
}

// Removes a single trap
void RemoveTrapFromLoc(PlaceId id, Map m){
	if (id > MAX_REAL_PLACE) {
		m->numTrapsUnknown -= 1;
	} else {
		ConnList Loc = m->connections[id];
		// Loc->traps[i] = '0';
		Loc->numTraps -= 1;
	}
}

// Removes all traps
void RemoveTrapsFromLoc(PlaceId id, Map m) {
	ConnList Loc = m->connections[id];
	Loc->numTraps = 0;
}


// Returns number of traps at a location
int GetTrapsLoc(PlaceId Loc, Map m) {
	return m->connections[Loc]->numTraps;
}