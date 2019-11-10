/*
 * //////////////////////////////////////////////////////////////////////
 * COMP2521 18x1 ... the Fury of Dracula
 * testHunterView.c: test the HunterView ADT
 *
 * As supplied, these are very simple tests.  You should write more!
 * Don't forget to be rigorous and thorough while writing tests.
 *
 * 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
 * 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
 */

#define NDEBUG /* use TAP please */
#include "test1511.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Map.h"
#include "Places.h"
#include "HunterView.h"

#define IS_AT(who,where) \
	TEST (whereIs (hv, (who)) == (where), \
		"whereIs(hv, %s) == %s %s", \
		player_to_s (who), idToAbbrev (where), idToName (where))
#define HISTORY_IS(who,n,where) \
	TEST (history[(n)] == (where), \
		"giveMeTheTrail(hv, %s): [%d] %s %s", \
		player_to_s (who), n, idToAbbrev (where), idToName (where))
#define EXPECT_TO_SEE(where) \
	TEST (seen[where], "... %s %s", idToAbbrev (where), idToName (where))

static const char *player_to_s ( PlayerID );


bool locTest = true;
/* make test easier: use these functions every-time, which covered every possible tests. */
void testGeneralInfo ( HunterView hv, char *trail, PlayerID name, int round, int score ) {
	TEST ( whoAmI ( hv ) == name,
	      "whoAmI ( hv ) == %s", player_to_s ( name ) );
	TEST ( giveMeTheRound ( hv ) == round,
	      "giveMeTheRound ( hv ) == %d", round );
	TEST ( giveMeTheScore ( hv ) == score,
	      "giveMeTheScore ( hv ) == %d", score );
}


void testPlayerInfo ( HunterView hv, PlayerID name, int HP, LocationID myLoc,
                     LocationID locID[6], int numLoc, LocationID *expectLoc, bool road, bool rail, bool sea ) {
	/* new added tests */
	TEST ( howHealthyIs ( hv, name ) == HP,
	      "howHealthyIs ( hv, %s ) == %d", player_to_s ( name ), HP );
	IS_AT ( name, myLoc );
	LocationID history[TRAIL_SIZE];
	giveMeTheTrail ( hv, name, history );
	HISTORY_IS ( name, 0, locID[0] );
	printf ( "%s\n", idToAbbrev ( locID[0] ) );
	HISTORY_IS ( name, 1, locID[1] );
	printf ( "%s\n", idToAbbrev ( locID[1] ) );
	HISTORY_IS ( name, 2, locID[2] );
	printf ( "%s\n", idToAbbrev ( locID[2] ) );
	HISTORY_IS ( name, 3, locID[3] );
	printf ( "%s\n", idToAbbrev ( locID[3] ) );
	HISTORY_IS ( name, 4, locID[4] );
	printf ( "%s\n", idToAbbrev ( locID[4] ) );
	HISTORY_IS ( name, 5, locID[5] );
	printf ( "%s\n", idToAbbrev ( locID[5] ) );
	if ( locTest ) {
		int	size;
		int	*edges;

		edges = whereCanTheyGo (
		            hv, &size, name, road, rail, sea );
		bool seen[NUM_MAP_LOCATIONS] = { false };
		for ( int i = 0; i < size; i++ )
			seen[edges[i]] = true;

		TEST ( size == numLoc,
		      "whereCanTheyGo ( hv, %s, road: %d   rail: %d   sea: %d ): %d locations", player_to_s ( name ), road, rail, sea, numLoc );
		for ( int i = 0; i < numLoc; i++ ) {
			EXPECT_TO_SEE ( expectLoc[i] );
		}
		free ( edges );
	}
}


void completeTest ( char *trail, PlayerMessage *messages,
                   PlayerID name, int round, int score, int HP,
                   LocationID myLoc, LocationID locID[6], /* history */
                   int numLoc, LocationID *expectLoc,
                   bool road, bool rail, bool sea ) {
	/* test function */
	HunterView hv = newHunterView ( trail, messages );

	TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
	TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

	testGeneralInfo ( hv, trail, name, round, score );
	testPlayerInfo ( hv, name, HP, myLoc, locID, numLoc, expectLoc, road, rail, sea );

	disposeHunterView ( hv );
	TEST ( mem_address_is_poisoned ( hv ),
	      "hv is no longer a valid allocation" );
}


int
main ( void ) {
	puts ( "1..many" );

	do {    /* ////////////////////////////////////////////////////////////// */
		char *symbols[] = {
			"newHunterView",  "disposeHunterView", "giveMeTheRound",
			"whoAmI",	  "giveMeTheScore",    "howHealthyIs",	"whereIs",
			"giveMeTheTrail", "whereCanIgo",       "whereCanTheyGo"
		};
		for ( size_t i = 0; i < nitems ( symbols ); i++ )
			TEST ( have_symbol ( symbols[i] ), "have '%s'", symbols[i] );
	} while ( 0 );


	do {    /* ////////////////////////////////////////////////////////////// */
		char		*trail		= "";
		PlayerMessage	messages[]	= {};
		HunterView	hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );
		/*                               Name        rounds       score */
		testGeneralInfo ( hv, trail, PLAYER_LORD_GODALMING, 0, GAME_START_SCORE );
		/* initiate all variables. */
		LocationID	locID[6];
		LocationID	expectLoc[NUM_MAP_LOCATIONS]; /* maximum location number. */
		int		numLoc;
		int		i;
		/* PLAYER_LORD_GODALMING */
		locID[0]	= UNKNOWN_LOCATION;
		locID[1]	= UNKNOWN_LOCATION;
		locID[2]	= UNKNOWN_LOCATION;
		locID[3]	= UNKNOWN_LOCATION;
		locID[4]	= UNKNOWN_LOCATION;
		locID[5]	= UNKNOWN_LOCATION;

		numLoc = NUM_MAP_LOCATIONS;
		for ( i = 0; i < numLoc; i++ ) {
			expectLoc[i] = i + MIN_MAP_LOCATION;
		}
		testPlayerInfo ( hv, PLAYER_LORD_GODALMING, GAME_START_HUNTER_LIFE_POINTS, UNKNOWN_LOCATION,
		                locID, numLoc, expectLoc, true, true, true );

		testPlayerInfo ( hv, PLAYER_DR_SEWARD, GAME_START_HUNTER_LIFE_POINTS, UNKNOWN_LOCATION,
		                locID, numLoc, expectLoc, true, false, true );

		testPlayerInfo ( hv, PLAYER_VAN_HELSING, GAME_START_HUNTER_LIFE_POINTS, UNKNOWN_LOCATION,
		                locID, numLoc, expectLoc, true, false, false );

		testPlayerInfo ( hv, PLAYER_MINA_HARKER, GAME_START_HUNTER_LIFE_POINTS, UNKNOWN_LOCATION,
		                locID, numLoc, expectLoc, false, true, false );

		testPlayerInfo ( hv, PLAYER_DRACULA, GAME_START_BLOOD_POINTS, UNKNOWN_LOCATION,
		                locID, 0, NULL, true, true, true );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* test for turn 1 */
		char *trail = "GST.... SAO.... HZU.... MBB.... DC?...."
		              "GBU....";
		PlayerMessage	messages[] = {};
		HunterView	hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );
		/*                               Name        rounds       score */
		testGeneralInfo ( hv, trail, PLAYER_DR_SEWARD, 1, GAME_START_SCORE - 1 );
		/* initiate all variables. */
		LocationID	locID[6];
		LocationID	expectLoc[NUM_MAP_LOCATIONS]; /* assume maximum location number is 20. */
		int		numLoc;

		/* PLAYER_PLAYER_DR_SEWARD */
		locID[0]	= ATLANTIC_OCEAN;
		locID[1]	= UNKNOWN_LOCATION;
		locID[2]	= UNKNOWN_LOCATION;
		locID[3]	= UNKNOWN_LOCATION;
		locID[4]	= UNKNOWN_LOCATION;
		locID[5]	= UNKNOWN_LOCATION;

		numLoc		= 9;
		expectLoc[0]	= ATLANTIC_OCEAN;
		expectLoc[1]	= NORTH_SEA;
		expectLoc[2]	= GALWAY;
		expectLoc[3]	= IRISH_SEA;
		expectLoc[4]	= ENGLISH_CHANNEL;
		expectLoc[5]	= BAY_OF_BISCAY;
		expectLoc[6]	= LISBON;
		expectLoc[7]	= CADIZ;
		expectLoc[8]	= MEDITERRANEAN_SEA;

		testPlayerInfo ( hv, PLAYER_DR_SEWARD, GAME_START_HUNTER_LIFE_POINTS, ATLANTIC_OCEAN,
		                locID, numLoc, expectLoc, false, true, true );

		locID[0]	= BRUSSELS;
		locID[1]	= STRASBOURG;
		locID[2]	= UNKNOWN_LOCATION;
		locID[3]	= UNKNOWN_LOCATION;
		locID[4]	= UNKNOWN_LOCATION;
		locID[5]	= UNKNOWN_LOCATION;

		numLoc		= 11;
		expectLoc[0]	= BRUSSELS;
		expectLoc[1]	= AMSTERDAM;
		expectLoc[2]	= COLOGNE;
		expectLoc[3]	= STRASBOURG;
		expectLoc[4]	= PARIS;
		expectLoc[5]	= LE_HAVRE;
		expectLoc[6]	= MADRID;
		expectLoc[7]	= BARCELONA;
		expectLoc[8]	= LE_HAVRE;
		expectLoc[9]	= BRUSSELS;
		expectLoc[10]	= MARSEILLES;

		testPlayerInfo ( hv, PLAYER_LORD_GODALMING, GAME_START_HUNTER_LIFE_POINTS, BRUSSELS,
		                locID, numLoc, expectLoc, true, true, true );


		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* Test for Dracula trail and basic functions */
		char *trail =
		    "GST.... SAO.... HZU.... MBB.... DC?....";
		PlayerMessage	messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"
		};
		HunterView	hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		TEST ( whoAmI ( hv ) == PLAYER_LORD_GODALMING,
		      "whoAmI ( hv ) == Godalming" );
		TEST ( giveMeTheRound ( hv ) == 1,
		      "giveMeTheRound ( hv ) == 1" );
		IS_AT ( PLAYER_LORD_GODALMING, STRASBOURG );
		IS_AT ( PLAYER_DR_SEWARD, ATLANTIC_OCEAN );
		IS_AT ( PLAYER_VAN_HELSING, ZURICH );
		IS_AT ( PLAYER_MINA_HARKER, BAY_OF_BISCAY );
		IS_AT ( PLAYER_DRACULA, CITY_UNKNOWN );
		TEST ( howHealthyIs ( hv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS,
		      "howHealthyIs ( hv, Dracula ) == 40 ( start )" );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );


	do {    /* ////////////////////////////////////////////////////////////// */
		/* Test for encountering Dracula and hunter history */
		char *trail =
		    "GST.... SAO.... HCD.... MAO.... DGE.... "
		    "GGED...";
		PlayerMessage	messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!"
		};
		HunterView	hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		IS_AT ( PLAYER_DRACULA, GENEVA );
		IS_AT ( PLAYER_LORD_GODALMING, GENEVA );
		TEST ( howHealthyIs ( hv, PLAYER_LORD_GODALMING ) == 5,
		      "howHealthyIs ( hv, Godalming ) == 5" );
		TEST ( howHealthyIs ( hv, PLAYER_DRACULA ) == 30,
		      "howHealthyIs ( hv, Dracula ) == 30" );

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( hv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, GENEVA );
		HISTORY_IS ( PLAYER_DRACULA, 1, UNKNOWN_LOCATION );

		giveMeTheTrail ( hv, PLAYER_LORD_GODALMING, history );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 0, GENEVA );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 1, STRASBOURG );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 2, UNKNOWN_LOCATION );

		giveMeTheTrail ( hv, PLAYER_DR_SEWARD, history );
		HISTORY_IS ( PLAYER_DR_SEWARD, 0, ATLANTIC_OCEAN );
		HISTORY_IS ( PLAYER_DR_SEWARD, 1, UNKNOWN_LOCATION );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );


	do {    /* ////////////////////////////////////////////////////////////// */
		/* Test for Dracula doubling back at sea, and losing blood */
		/* points ( Hunter View ) */

		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DS?.... "
		    "GST.... SST.... HST.... MST.... DD1....";
		PlayerMessage	messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!",	 "",	    "",	     "", "Back I go"
		};
		HunterView	hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		TEST ( whoAmI ( hv ) == PLAYER_LORD_GODALMING,
		      "whoAmI ( hv ) == Godalming" );
		TEST ( howHealthyIs ( hv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 4,
		      "howHealthyIs ( hv, Dracula ) == 36" );
		IS_AT ( PLAYER_DRACULA, DOUBLE_BACK_1 );

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( hv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, DOUBLE_BACK_1 );
		HISTORY_IS ( PLAYER_DRACULA, 1, SEA_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 2, UNKNOWN_LOCATION );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* Test for Dracula doubling back at sea, and losing blood */
		/* points ( Hunter View ) ( from city ) */

		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DS?.... "
		    "GST.... SST.... HST.... MST.... DC?...."
		    "GST.... SST.... HST.... MST.... DD2....";
		PlayerMessage	messages[] = {};
		HunterView	hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		TEST ( whoAmI ( hv ) == PLAYER_LORD_GODALMING,
		      "whoAmI ( hv ) == Godalming" );
		TEST ( howHealthyIs ( hv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 4,
		      "howHealthyIs ( hv, Dracula ) == 36" );
		/* AVOID hp > 9 */
		TEST ( howHealthyIs ( hv, PLAYER_LORD_GODALMING ) == GAME_START_HUNTER_LIFE_POINTS,
		      "howHealthyIs ( hv, PLAYER_LORD_GODALMING ) == 9" );
		IS_AT ( PLAYER_DRACULA, DOUBLE_BACK_2 );

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( hv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, DOUBLE_BACK_2 );
		HISTORY_IS ( PLAYER_DRACULA, 1, CITY_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 2, SEA_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 3, UNKNOWN_LOCATION );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking Galatz road connections */
		char		*trail		= "GGA....";
		PlayerMessage	messages[]	= { "Gone to Galatz" };
		HunterView	hv		= newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		int size, *edges = whereCanTheyGo (
		                       hv, &size, PLAYER_LORD_GODALMING, true, false, false );
		bool seen[NUM_MAP_LOCATIONS] = { false };
		for ( int i = 0; i < size; i++ )
			seen[edges[i]] = true;

		TEST ( size == 5,
		      "whereCanTheyGo ( hv, Godalming, road ): 5 locations" );
		EXPECT_TO_SEE ( GALATZ );
		EXPECT_TO_SEE ( CONSTANTA );
		EXPECT_TO_SEE ( BUCHAREST );
		EXPECT_TO_SEE ( KLAUSENBURG );
		EXPECT_TO_SEE ( CASTLE_DRACULA );

		free ( edges );
		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );


	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking Ionian Sea sea connections */
		char		*trail		= "GIO....";
		PlayerMessage	messages[]	= { "Sailing the Ionian" };
		HunterView	hv		= newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		int size, *edges = whereCanTheyGo (
		                       hv, &size, PLAYER_LORD_GODALMING, false, false, true );
		bool seen[NUM_MAP_LOCATIONS] = { false };
		for ( int i = 0; i < size; i++ )
			seen[edges[i]] = true;

		TEST ( size == 7,
		      "whereCanTheyGo ( hv, Godalming, sea ): 7 locations" );
		EXPECT_TO_SEE ( IONIAN_SEA );
		EXPECT_TO_SEE ( BLACK_SEA );
		EXPECT_TO_SEE ( ADRIATIC_SEA );
		EXPECT_TO_SEE ( TYRRHENIAN_SEA );
		EXPECT_TO_SEE ( ATHENS );
		EXPECT_TO_SEE ( VALONA );
		EXPECT_TO_SEE ( SALONICA );

		free ( edges );
		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );


	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking Athens rail connections ( none ) */
		char		*trail		= "GAT....";
		PlayerMessage	messages[]	= { "Leaving Athens by train" };
		HunterView	hv		= newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		int size, *edges = whereCanTheyGo (
		                       hv, &size, PLAYER_LORD_GODALMING, false, true, false );

		bool seen[NUM_MAP_LOCATIONS] = { false };
		for ( int i = 0; i < size; i++ )
			seen[edges[i]] = true;

		TEST ( size == 1,
		      "whereCanTheyGo ( hv, Godalming, rail ): 1 location" );
		EXPECT_TO_SEE ( ATHENS );

		free ( edges );
		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );


	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking where is and trail for players sent to the hospital */
		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... MCO.... DSRT... "
		    "GSNV... SMR.... HCD.... MAM.... DMAT... "
		    "GSRT... SCF.... HGA.... MBU.... DHIT... "
		    "GMATTD. SCF.... HGA.... ";

		PlayerMessage	messages[24]	= { "Going somewhere" };
		HunterView	hv		= newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		IS_AT ( PLAYER_LORD_GODALMING, ST_JOSEPH_AND_ST_MARYS );
		TEST ( howHealthyIs ( hv, PLAYER_LORD_GODALMING ) == 0, "Hunter is killed" );

		IS_AT ( PLAYER_DRACULA, MADRID );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking where is and trail for players sent to the hospital */
		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... MCO.... DSRT... "
		    "GSNV... SMR.... HCD.... MAM.... DMAT... "
		    "GSRT... SCF.... HGA.... MBU.... DHIT... "
		    "GMATTD. SCF.... HGA.... MBU.... DD1T... "
		    "GZA.... SCF.... HGA....";

		PlayerMessage	messages[28]	= { "Going somewhere", "" };
		HunterView	hv		= newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		IS_AT ( PLAYER_LORD_GODALMING, ZAGREB );
		TEST ( howHealthyIs ( hv, PLAYER_LORD_GODALMING ) == 9,
		      "Godalming is revived" );

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( hv, PLAYER_LORD_GODALMING, history );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 0, ZAGREB );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 1, MADRID );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 2, SARAGOSSA );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 3, SANTANDER );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 4, MADRID );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 5, LISBON );
		IS_AT ( PLAYER_DRACULA, MADRID );
		giveMeTheTrail ( hv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, DOUBLE_BACK_1 );
		HISTORY_IS ( PLAYER_DRACULA, 1, HIDE );
		HISTORY_IS ( PLAYER_DRACULA, 2, MADRID );
		HISTORY_IS ( PLAYER_DRACULA, 3, SARAGOSSA );
		HISTORY_IS ( PLAYER_DRACULA, 4, SANTANDER );
		HISTORY_IS ( PLAYER_DRACULA, 5, UNKNOWN_LOCATION );


		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking where is an whereCanTheyGo for dracula */
		char *trail =
		    "GED.... SGE.... HZU.... MCA.... DCF.V.. "
		    "GMN.... SCFVD.. HGE.... MLS.... DC?T... "
		    "GLO.... SMR.... HCF.... MMA.... DC?T... "
		    "GPL.... SMS.... HMR.... MGR.... DBAT... "
		    "GLO.... SBATD.. HMS.... MMA.... DC?T... "
		    "GPL.... SSJ.... HBA.... MGR.... DD1T...";

		PlayerMessage messages[30] = { "Going somewhere", "" };
		/* printf ("*****"); */
		HunterView hv = newHunterView ( trail, messages );
		/* printf ("----"); */
		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		IS_AT ( PLAYER_DRACULA, DOUBLE_BACK_1 );
		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( hv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, DOUBLE_BACK_1 );
		HISTORY_IS ( PLAYER_DRACULA, 1, CITY_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 2, BARCELONA );
		HISTORY_IS ( PLAYER_DRACULA, 3, CITY_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 4, CITY_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 5, CLERMONT_FERRAND );

		int	size, *edges = whereCanTheyGo ( hv, &size, PLAYER_DRACULA, true, false, true );
		bool	seen[NUM_MAP_LOCATIONS] = { false };
		for ( int i = 0; i < size; i++ )
			seen[edges[i]] = true;
		TEST ( size == 0,
		      "whereCanTheyGo ( dv, Dracula, road,sea ): 0 locations" );
		free ( edges );
		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking where is an whereCanTheyGo for dracula */
		char *trail =
		    "GED.... SCF.... HZU.... MCA.... DTO.V.. "
		    "GMN.... STOVD.. HGE.... MLS.... DD1T... "
		    "GLO.... SMR.... HCF.... MMA.... DC?T... "
		    "GPL.... SMS.... HMR.... MGR.... DBAT... "
		    "GLO.... SBATD..";

		PlayerMessage	messages[22]	= { "Going somewhere", "" };
		HunterView	hv		= newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		IS_AT ( PLAYER_DRACULA, BARCELONA );
		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( hv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, BARCELONA );
		HISTORY_IS ( PLAYER_DRACULA, 1, CITY_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 2, DOUBLE_BACK_1 );
		HISTORY_IS ( PLAYER_DRACULA, 3, TOULOUSE );
		HISTORY_IS ( PLAYER_DRACULA, 4, UNKNOWN_LOCATION );
		HISTORY_IS ( PLAYER_DRACULA, 5, UNKNOWN_LOCATION );

		int	size, *edges = whereCanTheyGo ( hv, &size, PLAYER_DRACULA, true, false, true );
		bool	seen[NUM_MAP_LOCATIONS] = { false };
		for ( int i = 0; i < size; i++ )
			seen[edges[i]] = true;

		TEST ( size == 3,
		      "whereCanTheyGo ( dv, Dracula, road,sea ): 3 locations" );
		/* printf ("%s\n",idToName ( edges[0])); */
		EXPECT_TO_SEE ( MEDITERRANEAN_SEA );
		EXPECT_TO_SEE ( SARAGOSSA );
		EXPECT_TO_SEE ( BARCELONA );

		free ( edges );
		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking where is and trail for dracula with TP */
		char *trail = "GMA.... SMN.... HAM.... MGA.... DGA.V.. "
		              "GAL.... SED.... HBU.... MGAVD.. DCDT... "
		              "GMS.... SNS.... HCO.... MCDTD.. DKLT... "
		              "GMR.... SHA.... HFR.... MCD.... DD2T... "
		              "GGO.... SBR.... HBR.... MCDTD.. DHIT... "
		              "GVE.... SPR.... HBD.... MJM.... DTPT... "
		              "GBD.... SBD.... HKLT...";

		PlayerMessage	messages[33]	= { "Going somewhere", "" };
		HunterView	hv		= newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		IS_AT ( PLAYER_DRACULA, CASTLE_DRACULA );
		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( hv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, TELEPORT );
		HISTORY_IS ( PLAYER_DRACULA, 1, HIDE );
		HISTORY_IS ( PLAYER_DRACULA, 2, DOUBLE_BACK_2 );
		HISTORY_IS ( PLAYER_DRACULA, 3, KLAUSENBURG );
		HISTORY_IS ( PLAYER_DRACULA, 4, CASTLE_DRACULA );
		HISTORY_IS ( PLAYER_DRACULA, 5, GALATZ );


		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* Checking the traps. */
		char *trail =
		    "GLS.... SCA.... HCD.... MAM.... DSR.V.. "
		    "GMA.... SCA.... HGA.... MCO.... DHIT... "
		    "GMA.... SCA.... HCD.... MAM.... DD1T... "
		    "GSRTTVD. SCA.... HCD.... MAM.... DSAT... "

		    "GAL.... SCA.... HGA.... MBU.... DMAT... "
		    "GMAT... SMATD.. HGA.... ";

		PlayerMessage	messages[24]	= { "Going somewhere" };
		HunterView	hv		= newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		IS_AT ( PLAYER_LORD_GODALMING, ST_JOSEPH_AND_ST_MARYS );
		TEST ( howHealthyIs ( hv, PLAYER_LORD_GODALMING ) == 0, " HP = %d\n", howHealthyIs ( hv, PLAYER_LORD_GODALMING ) );

		IS_AT ( PLAYER_DR_SEWARD, MADRID );
		TEST ( howHealthyIs ( hv, PLAYER_DR_SEWARD ) == 3, "Hunter is ALIVED" );

		IS_AT ( PLAYER_DRACULA, MADRID );
		TEST ( howHealthyIs ( hv, PLAYER_DRACULA ) == 20, "Hp 20" );
		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );

	PlayerMessage messages[50] = { "----" };
	/* test the rail connection. */
	locTest = false;
	/*
	 * location in unknown location hvae been tested in test 1
	 * so close it
	 */
	do {
		/* Trail */
		char *trail = "";
		/* Player's name */
		PlayerID	name	= PLAYER_LORD_GODALMING;
		int		round	= 0;
		int		score	= GAME_START_SCORE;
		int		HP	= GAME_START_HUNTER_LIFE_POINTS;
		/* current player's location */
		LocationID myLoc = UNKNOWN_LOCATION;
		/* current player's history trail */
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 1;
		/* list of connected location, the start location is in 0 index. */
		LocationID expectLoc[1] = { BORDEAUX };

		bool	road	= false;
		bool	rail	= true;
		bool	sea	= false;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );

	do {
		/* Trail */
		char *trail = "GBO....";
		/* Player's name */
		PlayerID	name	= PLAYER_DR_SEWARD;
		int		round	= 0;
		int		score	= GAME_START_SCORE;
		int		HP	= GAME_START_HUNTER_LIFE_POINTS;
		/* current player's location */
		LocationID myLoc = UNKNOWN_LOCATION;
		/* current player's history trail */
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 3;
		/* list of connected location, the start location is in 0 index. */
		LocationID expectLoc[3] = { BORDEAUX, PARIS, SARAGOSSA };

		bool	road	= false;
		bool	rail	= true;
		bool	sea	= false;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );

	do {
		/* Trail */
		char *trail = "GBO.... SBO....";
		/* Player's name */
		PlayerID	name	= PLAYER_VAN_HELSING;
		int		round	= 0;
		int		score	= GAME_START_SCORE;
		int		HP	= GAME_START_HUNTER_LIFE_POINTS;
		/* current player's location */
		LocationID myLoc = UNKNOWN_LOCATION;
		/* current player's history trail */
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 8;
		/* list of connected location, the start location is in 0 index. */
		LocationID expectLoc[8] = { BORDEAUX, PARIS,	  SARAGOSSA, LE_HAVRE,
		                            BRUSSELS, MARSEILLES, MADRID,    BARCELONA
		                          };

		bool	road	= false;
		bool	rail	= true;
		bool	sea	= false;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );

	do {
		/* Trail */
		char *trail = "GBO.... SBO.... HBO....";
		/* Player's name */
		PlayerID	name	= PLAYER_MINA_HARKER;
		int		round	= 0;
		int		score	= GAME_START_SCORE;
		int		HP	= GAME_START_HUNTER_LIFE_POINTS;
		/* current player's location */
		LocationID myLoc = UNKNOWN_LOCATION;
		/* current player's history trail */
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 12;
		/* list of connected location, the start location is in 0 index. */
		LocationID expectLoc[12] = { BORDEAUX, PARIS,	   SARAGOSSA, LE_HAVRE,
		                             BRUSSELS, MARSEILLES, MADRID,    BARCELONA,
		                             COLOGNE,  SANTANDER,  ALICANTE,  LISBON
		                           };

		bool	road	= false;
		bool	rail	= true;
		bool	sea	= false;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );

	locTest = true;
	do {
		/* Trail */
		char *trail = "GBO.... SBO.... HBO.... MBO.... DC?.V.. ";
		/* Player's name */
		PlayerID	name	= PLAYER_LORD_GODALMING;
		int		round	= 1;
		int		score	= GAME_START_SCORE - 1;
		int		HP	= GAME_START_HUNTER_LIFE_POINTS;
		/* current player's location */
		LocationID myLoc = BORDEAUX;
		/* current player's history trail */
		LocationID locID[6] = { BORDEAUX,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 3;
		/* list of connected location, the start location is in 0 index. */
		LocationID expectLoc[3] = { BORDEAUX, PARIS, SARAGOSSA };

		bool	road	= false;
		bool	rail	= true;
		bool	sea	= false;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );

	do {
		/* Trail */
		char *trail = "GBO.... SBO.... HBO.... MBO.... DC?.V.. "
		              "GBO.... SBO.... HBO.... MBO.... DC?T...";
		/* Player's name */
		PlayerID	name	= PLAYER_LORD_GODALMING;
		int		round	= 2;
		int		score	= GAME_START_SCORE - 2;
		int		HP	= GAME_START_HUNTER_LIFE_POINTS;
		/* current player's location */
		LocationID myLoc = BORDEAUX;
		/* current player's history trail */
		LocationID locID[6] = { BORDEAUX,
		                        BORDEAUX,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 8;
		/* list of connected location, the start location is in 0 index. */
		LocationID expectLoc[8] = { BORDEAUX, PARIS,	  SARAGOSSA, LE_HAVRE,
		                            BRUSSELS, MARSEILLES, MADRID,    BARCELONA
		                          };

		bool	road	= false;
		bool	rail	= true;
		bool	sea	= false;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );

	do {
		/* Trail */
		char *trail = "GBO.... SBO.... HBO.... MBO.... DC?.V.. "
		              "GBO.... SBO.... HBO.... MBO.... DC?T... "
		              "GBO.... SBO.... HBO.... MBO.... DC?T... ";
		/* Player's name */
		PlayerID	name	= PLAYER_LORD_GODALMING;
		int		round	= 3;
		int		score	= GAME_START_SCORE - 3;
		int		HP	= GAME_START_HUNTER_LIFE_POINTS;
		/* current player's location */
		LocationID myLoc = BORDEAUX;
		/* current player's history trail */
		LocationID locID[6] = { BORDEAUX,
		                        BORDEAUX,
		                        BORDEAUX,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 12;
		/* list of connected location, the start location is in 0 index. */
		LocationID expectLoc[12] = { BORDEAUX, PARIS,	   SARAGOSSA, LE_HAVRE,
		                             BRUSSELS, MARSEILLES, MADRID,    BARCELONA,
		                             COLOGNE,  SANTANDER,  ALICANTE,  LISBON
		                           };

		bool	road	= false;
		bool	rail	= true;
		bool	sea	= false;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );

	/*
	 * Dracula hp can over 40
	 * G will be tp to hospital since he is killed before rest.
	 */
	do {
		/* Trail */
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.."         /* +10hp */
		              "GKL.... SBO.... HBO.... MBO.... DC?T..."
		              "GKL.... SBO.... HBO.... MBO.... DD2T..."         /* +10hp */
		              "GKL.... SBO.... HBO.... MBO.... DHIT..."         /* +10hp */
		              "GKL.... SBO.... HBO.... MBO.... DC?T..."
		              "GKLTD.. SBO.... HBO.... MBO.... DC?T.B."
		              "GKLTD.. SBO.... HBO.... MBO.... DBDT.V.";        /* -13 score -6score */
		/* Player's name */
		PlayerID	name	= PLAYER_LORD_GODALMING;
		int		round	= 7;
		int		score	= GAME_START_SCORE - 7 - 13 - 6;
		int		HP	= 9;
		/* current player's location */
		LocationID myLoc = ST_JOSEPH_AND_ST_MARYS;
		/* current player's history trail */
		LocationID locID[6] = { KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG
		                      };
		/* total number of location that player can go to there. */
		int numLoc = 5;
		/* list of connected location, the start location is in 0 index. */
		LocationID	expectLoc[5]	= { ST_JOSEPH_AND_ST_MARYS, ZAGREB, SARAJEVO, BELGRADE, SZEGED };
		bool		road		= true;
		bool		rail		= true;
		bool		sea		= true;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );
	/* in same turn: hp = 0 */
	do {
		/* Trail */
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.." /* +10hp */
		              "GKL.... SBO.... HBO.... MBO.... DC?T..."
		              "GKL.... SBO.... HBO.... MBO.... DD2T..." /* +10hp */
		              "GKL.... SBO.... HBO.... MBO.... DHIT..." /* +10hp */
		              "GKL.... SBO.... HBO.... MBO.... DC?T..."
		              "GKLTD.. SBO.... HBO.... MBO.... DC?T.B."
		              "GKLTD..";
		/* Player's name */
		PlayerID	name	= PLAYER_DR_SEWARD;
		int		round	= 6;
		int		score	= GAME_START_SCORE - 6 - 6;
		int		HP	= 0;
		/* current player's location */
		LocationID myLoc = ST_JOSEPH_AND_ST_MARYS;
		/* current player's history trail */
		LocationID locID[6] = { KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG
		                      };
		/* total number of location that player can go to there. */
		int numLoc = 5;
		/* list of connected location, the start location is in 0 index. */
		LocationID	expectLoc[5]	= { ST_JOSEPH_AND_ST_MARYS, ZAGREB, SARAJEVO, BELGRADE, SZEGED };
		bool		road		= true;
		bool		rail		= false;
		bool		sea		= true;

		HunterView hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		testGeneralInfo ( hv, trail, name, round, score );
		testPlayerInfo ( hv, PLAYER_LORD_GODALMING, HP, myLoc, locID, numLoc, expectLoc, road, rail, sea );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );
	do {
		/*
		 * hp > 40
		 * Trail
		 */
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.. " /* +10hp */
		              "GCD.... SBO.... HBO....  ";
		/* Player's name */
		PlayerID	name	= PLAYER_DRACULA;
		int		HP	= 50;
		/* current player's location */
		LocationID myLoc = CASTLE_DRACULA;
		/* current player's history trail */
		LocationID locID[6] = { CASTLE_DRACULA,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 3;
		/* list of connected location, the start location is in 0 index. */
		LocationID	expectLoc[3]	= { CASTLE_DRACULA, GALATZ, KLAUSENBURG };
		bool		road		= true;
		bool		rail		= true;
		bool		sea		= true;

		HunterView hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		testPlayerInfo ( hv, name, HP, myLoc, locID, numLoc, expectLoc, road, rail, sea );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ), "hv is a valid allocation" );
	} while ( 0 );
	do {
		/*
		 * encounter with 2 traps and V: V can not damage hunter.
		 * Trail
		 */
		char *trail = "GCD.... SBO.... HBO.... MBO.... DC?.V.."
		              "GCD.... SBO.... HBO.... MBO.... DHIT..."
		              "GCD.... SBO.... HBO.... MBO.... DD1T..."
		              "GGA.... SBO.... HBO.... MBO.... DC?T..."
		              "GCNTTV. SBO.... HBO.... MBO.... DC?T...";
		/* Player's name */
		PlayerID	name	= PLAYER_LORD_GODALMING;
		int		round	= 5;
		int		score	= GAME_START_SCORE - 5;
		int		HP	= 5;
		/* current player's location */
		LocationID myLoc = CONSTANTA;
		/* current player's history trail */
		LocationID locID[6] = { CONSTANTA,
		                        GALATZ,
		                        CASTLE_DRACULA,
		                        CASTLE_DRACULA,
		                        CASTLE_DRACULA,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 5;
		/* list of connected location, the start location is in 0 index. */
		LocationID	expectLoc[5]	= { CONSTANTA, GALATZ, BUCHAREST, VARNA, BLACK_SEA };
		bool		road		= true;
		bool		rail		= true;
		bool		sea		= true;

		completeTest ( trail, messages,
		              name, round, score, HP,
		              myLoc, locID, /* history */
		              numLoc, expectLoc,
		              road, rail, sea );
	} while ( 0 );

	do {
		/*
		 * long Dracula history: trail with Hide, double back
		 * Trail
		 */
		char *trail = "GBO.... SBO.... HBO.... MBO.... DCD.V.." /* +10hp */
		              "GBO.... SBO.... HBO.... MBO.... DHIT..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DD5T.V."
		              "GBO.... SBO.... HBO.... MBO.... DHIT..."
		              "GBO.... SBO.... HBO....  ";
		/* Player's name */
		PlayerID name = PLAYER_DRACULA;
		/*
		 * int round = 8;
		 * int score = GAME_START_SCORE - 8 - 13;
		 */
		int HP = 80;
		/* current player's location */
		LocationID myLoc = CASTLE_DRACULA;
		/* current player's history trail */
		LocationID locID[6] = { HIDE,
		                        DOUBLE_BACK_5,
		                        CITY_UNKNOWN,
		                        CITY_UNKNOWN,
		                        CITY_UNKNOWN,
		                        CITY_UNKNOWN
		                      };
		/* total number of connected location */
		int numLoc = 3;
		/* list of connected location, the start location is in 0 index. */
		LocationID	expectLoc[3]	= { CASTLE_DRACULA, GALATZ, KLAUSENBURG };
		bool		road		= true;
		bool		rail		= true;
		bool		sea		= true;

		HunterView hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		testPlayerInfo ( hv, name, HP, myLoc, locID, numLoc, expectLoc, road, rail, sea );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ), "hv is a valid allocation" );
	} while ( 0 );


	do {
		/*
		 * where can they go: test others in same turn
		 * Trail
		 */
		char *trail = "GCF.... SBO.... HRO.... MBO.... DCD.V.. " /* +10hp */
		              "GBO.... ";
		/* current player's location */
		LocationID myLoc = BORDEAUX;
		/* current player's history trail */
		LocationID locID[6] = { BORDEAUX,
		                        CLERMONT_FERRAND,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		/* total number of connected location */
		int numLoc = 9;
		/* list of connected location, the start location is in 0 index. */
		LocationID	expectLoc[9] = { BORDEAUX, PARIS,    SARAGOSSA,	 BAY_OF_BISCAY,
		                             LE_HAVRE,	BRUSSELS, MARSEILLES, MADRID,	    BARCELONA
		                          };
		bool		road	= false;
		bool		rail	= true;
		bool		sea	= true;

		HunterView hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "hv is a valid allocation" );

		testPlayerInfo ( hv, PLAYER_LORD_GODALMING, 9, myLoc, locID, numLoc, expectLoc, road, rail, sea );

		LocationID myLoc_2 = ROME;
		/* current player's history trail */
		LocationID locID_2[6] = { ROME,
		                          UNKNOWN_LOCATION,
		                          UNKNOWN_LOCATION,
		                          UNKNOWN_LOCATION,
		                          UNKNOWN_LOCATION,
		                          UNKNOWN_LOCATION
		                        };
		/* total number of connected location */
		int numLoc_2 = 9;
		/* list of connected location, the start location is in 0 index. */
		LocationID expectLoc_2[9] = { ROME, TYRRHENIAN_SEA, FLORENCE, MILAN, ZURICH, GENEVA, NAPLES, BARI, GENOA };
		testPlayerInfo ( hv, PLAYER_VAN_HELSING, 9, myLoc_2, locID_2, numLoc_2, expectLoc_2, road, rail, sea );
		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ), "hv is a valid allocation" );
	} while ( 0 );

	do {    /* ////////////////////////////////////////////////////////////// */
		/* If a location is connected with road and rail */
		/* the get connection still can get the city that connected with the location via rail. */
		char		*trail		= "GMR.... SBO.... HCG.... MSA.... ";
		PlayerMessage	messages[]	= {};
		HunterView	hv = newHunterView ( trail, messages );

		TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST ( !mem_address_is_poisoned ( hv ), "gv is a valid allocation" );

		LocationID	locID_3[6];
		LocationID	expectLoc_3[11] = { BORDEAUX, NANTES, CLERMONT_FERRAND, TOULOUSE, SARAGOSSA, PARIS, LE_HAVRE, BRUSSELS, MARSEILLES, MADRID, BARCELONA }; /* maximum location number. */
		int		numLoc_3	= 11;
		/* PLAYER_DR_SEWARD */
		locID_3[0]	= BORDEAUX;
		locID_3[1]	= UNKNOWN_LOCATION;
		locID_3[2]	= UNKNOWN_LOCATION;
		locID_3[3]	= UNKNOWN_LOCATION;
		locID_3[4]	= UNKNOWN_LOCATION;
		locID_3[5]	= UNKNOWN_LOCATION;

		testPlayerInfo ( hv, PLAYER_DR_SEWARD, GAME_START_HUNTER_LIFE_POINTS, BORDEAUX,
		                locID_3, numLoc_3, expectLoc_3, true, true, false );

		disposeHunterView ( hv );
		TEST ( mem_address_is_poisoned ( hv ),
		      "hv is no longer a valid allocation" );
	} while ( 0 );
	return ( EXIT_SUCCESS );
}


/*
 * do {////////////////////////////////////////////////////////////////
 * // Checking the time for hp recover.
 *
 * char *trail =
 * "GMA.... SCA.... HCD.... MAM.... DC?.V.. "
 * "GSNVD.. SCA.... HGA.... MCO.... DHIT... "
 * "GSNTD.. ";
 *
 * PlayerMessage messages[24] = {"Going somewhere"};
 * HunterView hv = newHunterView ( trail, messages );
 *
 * TEST ( hv != NULL, "hv := newHunterView (\"%s\")", trail );
 * TEST (! mem_address_is_poisoned ( hv ), "hv is a valid allocation");
 *
 * IS_AT ( PLAYER_LORD_GODALMING, ST_JOSEPH_AND_ST_MARYS );
 * TEST ( howHealthyIs ( hv, PLAYER_LORD_GODALMING ) == 0, "Hunter is killed");
 *
 * IS_AT ( PLAYER_DRACULA, SANTENDER );
 * TEST ( howHealthyIs ( hv, PLAYER_DRACULA ) == 20, "Hp 20");
 * disposeHunterView ( hv );
 * TEST ( mem_address_is_poisoned ( hv ),
 * "hv is no longer a valid allocation");
 * } while ( 0 );
 * return EXIT_SUCCESS;
 * }
 */

static const char *
player_to_s (
    PlayerID p ) {
	switch ( p ) {
	case PLAYER_LORD_GODALMING:
		return ("Godalming");
	case PLAYER_DR_SEWARD:
		return ("Seward");
	case PLAYER_VAN_HELSING:
		return ("vHelsing");
	case PLAYER_MINA_HARKER:
		return ("Harker");
	case PLAYER_DRACULA:
		return ("Dracula");
	default:
		return ("???");
	}
}


