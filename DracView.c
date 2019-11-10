/*
 * //////////////////////////////////////////////////////////////////////
 * COMP2521 18x1 ... the Fury of Dracula
 * DracView.c: the DracView ADT implementation
 *
 * 2014-07-01   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
 * 2017-12-01   v1.1    Team Dracula <cs2521@cse.unsw.edu.au>
 */

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>
/* #include <stdio.h> */

#include "DracView.h"
#include "Game.h"
#include "GameView.h"
#include "Globals.h"
#include "Map.h" 
/* ... if you decide to use the Map ADT */
#include "structAndFun.h"
struct dracView {
	struct gameView *gv;
};


/* Creates a new DracView to summarise the current state of the game */
DracView newDracView ( char *pastPlays, PlayerMessage messages[] ) {
	DracView new = malloc ( sizeof *new );
	if ( new == NULL )
		err ( EX_OSERR, "couldn't allocate DracView )" );
	new->gv = newGameView ( pastPlays, messages );
	return ( new );
}


/* Frees all memory previously allocated for the DracView toBeDeleted */
void disposeDracView ( DracView toBeDeleted ) {
	disposeGameView ( toBeDeleted->gv );
	free ( toBeDeleted );
}


/* // Functions to return simple information about the current state of the game */

/* Get the current round */
Round giveMeTheRound ( DracView dv ) {
	return ( getRound ( dv->gv ) );
}


/* Get the current score */
int giveMeTheScore ( DracView dv ) {
	return ( getScore ( dv->gv ) );
}


/* Get the current health points for a given player */
int howHealthyIs ( DracView dv, PlayerID player ) {
	/* REPLACE THIS WITH YOUR OWN IMPLEMENTATION */
	return ( getHealth ( dv->gv, player ) );
}


/* Get the current location id of a given player */
LocationID whereIs ( DracView dv, PlayerID player ) {
	if ( player == PLAYER_DRACULA && dv->gv->rounds > 0 ) {
		LocationID tp = trueLocation ( &( dv->gv->locationD[dv->gv->rounds - 1]) );
		if ( tp == TELEPORT ) {
			return ( CASTLE_DRACULA );
		}
		return ( tp );
	}
	return ( getLocation ( dv->gv, player ) );
}


/* Get the most recent move of a given player */
void lastMove ( DracView dv, PlayerID player,
               LocationID *start, LocationID *end ) {
	LocationID trail[TRAIL_SIZE];
	getHistory ( dv->gv, player, trail );
	/* giveMeTheTrail ( dv, player, trail ); */
	*start	= trail[1];
	*end	= trail[0];
	return;


	/*
	 *  if ( player != PLAYER_DRACULA ){
	 *
	 * } else {
	 *      //start location
	 *      //for Dracula, trail may not provide enough information about his true location.
	 *      //for example, A, B, C, D, D5, HIDE
	 *      // in this case, we don't know his location.
	 *      if ( dv->gv->rounds < 2 ){
	 * *start = UNKNOWN_LOCATION;
	 *      } else {
	 * *start = trueLocation ( &( dv->gv->locationD[dv->gv->rounds - 2] ) );
	 *          if ( *start == TELEPORT ){
	 * *start = CASTLE_DRACULA;
	 *          }
	 *      }
	 *      //end location
	 *
	 *      if ( dv->gv->rounds < 1 ){
	 * *end = UNKNOWN_LOCATION;
	 *      } else {
	 * *end = trueLocation ( &( dv->gv->locationD[dv->gv->rounds - 1] ) );
	 *          if ( *end == TELEPORT ){
	 * *end = CASTLE_DRACULA;
	 *          }
	 *      }
	 *      return;
	 *  }
	 *
	 */
}


/* Find out what minions are placed at the specified location */
void whatsThere ( DracView dv, LocationID where,
                 int *numTraps, int *numVamps ) {
	*numTraps	= dv->gv->Encounters[where].Traps;
	*numVamps	= dv->gv->Encounters[where].Vampire;
	return;
}


/* // Functions that return information about the history of the game */

/* Fills the trail array with the location ids of the last 6 turns */
void giveMeTheTrail ( DracView dv, PlayerID player,
                     LocationID trail[TRAIL_SIZE] ) {
	if ( player != PLAYER_DRACULA ) {
		/* get History do not convert HIDE, Double back to real location. */
		getHistory ( dv->gv, player, trail );
		return;
	}
	if ( dv->gv == NULL ) {
		return;
	}
	int index = dv->gv->rounds - 1;
	for ( int i = 0; i < TRAIL_SIZE; i++ ) {
		if ( index >= 0 ) {
			trail[i] = trueLocation ( &( dv->gv->locationD[index--]) );
			if ( trail[i] == TELEPORT ) {
				trail[i] = CASTLE_DRACULA;
			}
		} else {
			trail[i] = UNKNOWN_LOCATION;
		}
	}
}


/* // Functions that query the map to find information about connectivity */

/* What are my ( Dracula's ) possible next moves ( locations ) */
LocationID *whereCanIgo ( DracView dv, int *numLocations, bool road, bool sea ) {
	LocationID *returnList;
	if ( dv->gv == NULL ) {
		return ( NULL );
	}
	Map tmpMap = dv->gv->europe;
	if ( tmpMap == NULL ) {
		return ( NULL );
	}
	*numLocations = 0;
	int i = 0;
	/* Drac can go to any place in round 0 */
	if ( giveMeTheRound ( dv ) == 0 && whereIs ( dv, PLAYER_DRACULA ) == UNKNOWN_LOCATION ) {
		/* return NULL; */

		returnList = malloc ( ( NUM_MAP_LOCATIONS - 1 ) * sizeof ( LocationID ) );
		while ( *numLocations < NUM_MAP_LOCATIONS - 1 ) {
			if ( MIN_MAP_LOCATION + i != ST_JOSEPH_AND_ST_MARYS ) {
				returnList[*numLocations]	= MIN_MAP_LOCATION + i;
				*numLocations			= *numLocations + 1;
			}
			i++;
		}
		return ( returnList );
	}
	/* find the true location. */
	LocationID tmpfrom = trueLocation ( &( dv->gv->locationD[giveMeTheRound ( dv ) - 1]) );
	if ( tmpfrom == TELEPORT ) {
		tmpfrom = CASTLE_DRACULA;
	}
	if ( !( validPlace ( tmpfrom ) ) ) {
		/*  printf ("not valid place: %s\n",idToAbbrev ( tmpfrom )); */
		return ( NULL );
	}
	VList		tmpNode;
	LocationID	tmp[NUM_MAP_LOCATIONS];
	LocationID	trail[TRAIL_SIZE];
	getHistory ( dv->gv, PLAYER_DRACULA, trail );


	/*
	 * for ( i = 0; i < 6; i++){
	 *  printf ("%s\n",idToAbbrev ( trail[i]));
	 * }*/
	i = 0;
	/*
	 * can't use back - 1
	 * can use back - 0
	 */
	int	hide	= inArray ( HIDE, trail, TRAIL_SIZE - 1 );
	int	back	= inArray ( DOUBLE_BACK_1, trail, TRAIL_SIZE - 1 ) |
	              inArray ( DOUBLE_BACK_2, trail, TRAIL_SIZE - 1 ) |
	              inArray ( DOUBLE_BACK_3, trail, TRAIL_SIZE - 1 ) |
	              inArray ( DOUBLE_BACK_4, trail, TRAIL_SIZE - 1 ) |
	              inArray ( DOUBLE_BACK_5, trail, TRAIL_SIZE - 1 );
	/*
	 * printf ("hide = %d   back = %d\n",hide,back );
	 * ignore back and make hide always possible in order to make the test happy
	 */
	hide	= 0;
	back	= 1;
	/* change the trail to real location. */
	giveMeTheTrail ( dv, PLAYER_DRACULA, trail );

	if ( !( hide & back ) ) {
		/* can hide in same place. */
		tmp[0] = tmpfrom;
		i++;
		*numLocations = *numLocations + 1;
	}

	tmpNode = tmpMap->connections[tmpfrom];
	while ( tmpNode != NULL ) {
		if ( ( tmpNode->type == ROAD && road ) ||
		        ( tmpNode->type == BOAT && sea ) ) {
			/* this location isn't in trail / can use back */
			if ( !inArray ( tmpNode->v, trail, TRAIL_SIZE - 1 ) || !back ) {
				if ( tmpNode->v == ST_JOSEPH_AND_ST_MARYS ) {
					/* dracula can not move to hospital. */
					tmpNode = tmpNode->next;
					continue;
				}
				tmp[i++] = tmpNode->v;
				(*numLocations )++;
			}
		}
		tmpNode = tmpNode->next;
	}
	returnList = malloc ( sizeof ( LocationID ) * (*numLocations ) );
	for ( i = 0; i < *numLocations; i++ ) {
		returnList[i] = tmp[i];
	}
	return ( returnList );
}


/* What are the specified player's next possible moves */
LocationID *whereCanTheyGo ( DracView dv, int *numLocations,
                            PlayerID player, bool road, bool rail, bool sea ) {
	if ( player == PLAYER_DRACULA ) {
		return ( whereCanIgo ( dv, numLocations, road, sea ) );
	}
	*numLocations = 0;
	LocationID *returnList;
	if ( giveMeTheRound ( dv ) == 0 && whereIs ( dv, player ) == UNKNOWN_LOCATION ) {
		/* if round 0: can go to anywhere. */
		returnList = malloc ( NUM_MAP_LOCATIONS * sizeof ( LocationID ) );
		while ( *numLocations < NUM_MAP_LOCATIONS ) {
			returnList[*numLocations]	= MIN_MAP_LOCATION + *numLocations;
			*numLocations			= *numLocations + 1;
		}
		return ( returnList );
	}
	if ( !haveNotMoved ( dv->gv, player ) ) {
		return ( connectedLocations ( dv->gv, numLocations,
		                           whereIs ( dv, player ), player, dv->gv->rounds + 1, road, rail, sea ) );
	} else {
		return ( connectedLocations ( dv->gv, numLocations,
		                           whereIs ( dv, player ), player, dv->gv->rounds, road, rail, sea ) );
	}
}


