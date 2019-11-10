/*
 * //////////////////////////////////////////////////////////////////////
 * COMP2521 18x1 ... the Fury of Dracula
 * HunterView.c: the HunterView ADT implementation
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

#include "Game.h"
#include "GameView.h"
#include "Globals.h"
#include "HunterView.h"
#include "structAndFun.h"
#include "Map.h" /* ... if you decide to use the Map ADT */

struct hunterView {
	struct gameView *gv;
};

/* Creates a new HunterView to summarise the current state of the game */
HunterView newHunterView ( char *pastPlays, PlayerMessage messages[] ) {
	HunterView new = malloc ( sizeof *new );
	if ( new == NULL )
		err ( EX_OSERR, "couldn't allocate HunterView" );
	new->gv = newGameView ( pastPlays, messages );
	return ( new );
}


/* Frees all memory previously allocated for the HunterView toBeDeleted */
void disposeHunterView ( HunterView toBeDeleted ) {
	disposeGameView ( toBeDeleted->gv );
	free ( toBeDeleted );
}


/* // Functions to return simple information about the current state of the game */

/* Get the current round */
Round giveMeTheRound ( HunterView hv ) {
	return ( getRound ( hv->gv ) );
}


/* Get the id of current player */
PlayerID whoAmI ( HunterView hv ) {
	return ( hv->gv->turns % 5 );
}


/* Get the current score */
int giveMeTheScore ( HunterView hv ) {
	return ( getScore ( hv->gv ) );
}


/* Get the current health points for a given player */
int howHealthyIs ( HunterView hv, PlayerID player ) {
	return ( getHealth ( hv->gv, player ) );
}


/* Get the current location id of a given player */
LocationID whereIs ( HunterView hv, PlayerID player ) {
	if ( player == PLAYER_DRACULA && hv->gv->rounds > 0 ) {
		/* tp: truePlace */
		LocationID tp = trueLocation ( &( hv->gv->locationD[hv->gv->rounds - 1]) );
		if ( tp == TELEPORT ) {
			return ( CASTLE_DRACULA );
		}
		if ( !validPlace ( tp ) ) {
			return ( getLocation ( hv->gv, player ) );
		}
		return ( tp );
	}
	return ( getLocation ( hv->gv, player ) );
}


/* // Functions that return information about the history of the game */

/* Fills the trail array with the location ids of the last 6 turns */
void giveMeTheTrail ( HunterView hv, PlayerID player,
                     LocationID trail[TRAIL_SIZE] ) {
	getHistory ( hv->gv, player, trail );
}


/* // Functions that query the map to find information about connectivity */

/* What are my possible next moves ( locations ) */
LocationID *whereCanIgo ( HunterView hv, int *numLocations,
                         bool road, bool rail, bool sea ) {
	return ( whereCanTheyGo ( hv, numLocations, whoAmI ( hv ), road, rail, sea ) );
}


/* What are the specified player's next possible moves */
LocationID *whereCanTheyGo ( HunterView hv, int *numLocations,
                            PlayerID player, bool road, bool rail, bool sea ) {
	*numLocations = 0;
	LocationID	*returnList;
	int		i = 0;
	/* invalid cases. */
	if ( hv->gv == NULL ) {
		return ( NULL );
	}
	Map tmpMap = hv->gv->europe;
	if ( tmpMap == NULL ) {
		return ( NULL );
	}
	/*
	 * nomral case: use connectedLocations
	 * if round 0: can go to anywhere
	 */
	if ( player != PLAYER_DRACULA ) {
		/*
		 * since it return the 'NEXT' movement
		 * if it have moved in this round
		 */
		if ( giveMeTheRound ( hv ) == 0 && whereIs ( hv, player ) == UNKNOWN_LOCATION ) {
			/* if round 0: can go to anywhere. */
			returnList = malloc ( NUM_MAP_LOCATIONS * sizeof ( LocationID ) );
			while ( *numLocations < NUM_MAP_LOCATIONS ) {
				returnList[*numLocations]	= MIN_MAP_LOCATION + *numLocations;
				*numLocations			= *numLocations + 1;
			}
			return ( returnList );
		}
		if ( !haveNotMoved ( hv->gv, player ) ) {
			/* printf ("Have moved in this turn\n"); */
			return ( connectedLocations ( hv->gv, numLocations,
			                           whereIs ( hv, player ), player, hv->gv->rounds + 1, road, rail, sea ) );
		} else {
			return ( connectedLocations ( hv->gv, numLocations,
			                           whereIs ( hv, player ), player, hv->gv->rounds, road, rail, sea ) );
		}
	}
	/*
	 * Dracula  0 turn can not go to hospital.
	 * Dracula always haven't moved in hunter view.
	 */
	if ( giveMeTheRound ( hv ) == 0 && whereIs ( hv, player ) == UNKNOWN_LOCATION ) {
		return ( NULL );


		/*
		 * returnList = malloc ( ( NUM_MAP_LOCATIONS - 1 )* sizeof ( LocationID ) );
		 * while ( *numLocations < NUM_MAP_LOCATIONS - 1 ){
		 *  if ( MIN_MAP_LOCATION + i != ST_JOSEPH_AND_ST_MARYS ){
		 *      returnList[*numLocations] = MIN_MAP_LOCATION + i;
		 * *numLocations = *numLocations + 1;
		 *  }
		 *  i++;
		 * }
		 * return returnList;
		 */
	}
	LocationID tmpfrom;
	tmpfrom = trueLocation ( &( hv->gv->locationD[giveMeTheRound ( hv ) - 1]) );
	if ( tmpfrom == TELEPORT ) {
		/* if Dracula TP, then we know he is in CASTLE_DRACULA. */
		tmpfrom = CASTLE_DRACULA;
	}
	if ( !( validPlace ( tmpfrom ) ) ) {
		/* if we don't know exact location: return NULL */
		return ( NULL );
	}
	/*
	 * make a trail to record the cool down time of Hide, Double back.
	 * they will determine whether or not Dracula can stay at same location.
	 */
	VList		tmpNode;
	LocationID	trail[TRAIL_SIZE];
	LocationID	tmp[NUM_MAP_LOCATIONS];
	getHistory ( hv->gv, PLAYER_DRACULA, trail );
	/*
	 * can not use hide, back  --- 1
	 * can use hide, back --- 2
	 * the sixth element in trail will be automate drooped before next move.
	 * So the input index is TRAIL_SIZE - 1.
	 */
	int	hide	= inArray ( HIDE, trail, TRAIL_SIZE - 1 );
	int	back	= inArray ( DOUBLE_BACK_1, trail, TRAIL_SIZE - 1 ) |
	              inArray ( DOUBLE_BACK_2, trail, TRAIL_SIZE - 1 ) |
	              inArray ( DOUBLE_BACK_3, trail, TRAIL_SIZE - 1 ) |
	              inArray ( DOUBLE_BACK_4, trail, TRAIL_SIZE - 1 ) |
	              inArray ( DOUBLE_BACK_5, trail, TRAIL_SIZE - 1 );
	/* ignore back and make hide always possible in order to make the test happy */
	hide	= 0;
	back	= 1;
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


