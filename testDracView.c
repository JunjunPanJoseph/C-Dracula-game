////////////////////////////////////////////////////////////////////////
// COMP2521 18x1 ... the Fury of Dracula
// testDracView.c: test the DracView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>

#define NDEBUG // use TAP please
#include "test1511.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Map.h"
#include "Places.h"
#include "DracView.h"

#define IS_AT(who,where) \
	TEST (whereIs (dv, (who)) == (where), \
		"whereIs(dv, %s) == %s %s", \
		player_to_s (who), idToAbbrev (where), idToName (where))
#define HISTORY_IS(who,n,where) \
	TEST (history[(n)] == (where), \
		"giveMeTheTrail(dv, %s): [%d] %s %s", \
		player_to_s (who), n, idToAbbrev (where), idToName (where))
#define EXPECT_TO_SEE(where) \
	TEST (seen[where], "... %s %s", idToAbbrev (where), idToName (where))

static const char *player_to_s ( PlayerID );
bool locTest = true;
//make test easier: use these functions every-time, which covered every possible tests.
void testGeneralInfo ( DracView dv, char *trail, int round, int score ) {
	TEST ( giveMeTheRound ( dv ) == round,
	      "giveMeTheRound ( dv ) == %d", round );
	TEST ( giveMeTheScore ( dv ) == score,
	      "giveMeTheScore ( dv ) == %d", score );
}

void testPlayerInfo ( DracView dv, PlayerID name, int HP, LocationID myLoc,
                    LocationID locID[6], int numLoc, LocationID *expectLoc,  bool road, bool rail, bool sea ) {
	//new added tests
	TEST ( howHealthyIs ( dv, name ) == HP,
	      "howHealthyIs ( dv, %s ) == %d", player_to_s ( name ), HP );
	IS_AT ( name, myLoc );
	LocationID history[TRAIL_SIZE];
	giveMeTheTrail ( dv, name, history );
	HISTORY_IS ( name, 0, locID[0]);
	printf ("%s\n",idToAbbrev ( locID[0]));
	HISTORY_IS ( name, 1, locID[1]);
	printf ("%s\n",idToAbbrev ( locID[1]));
	HISTORY_IS ( name, 2, locID[2]);
	printf ("%s\n",idToAbbrev ( locID[2]));
	HISTORY_IS ( name, 3, locID[3]);
	printf ("%s\n",idToAbbrev ( locID[3]));
	HISTORY_IS ( name, 4, locID[4]);
	printf ("%s\n",idToAbbrev ( locID[4]));
	HISTORY_IS ( name, 5, locID[5]);
	printf ("%s\n",idToAbbrev ( locID[5]));
	if ( locTest ) {
		int size;
		int *edges;

		edges = whereCanTheyGo (
		            dv, &size, name, road, rail, sea );
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for ( int i = 0; i < size; i++)
			seen[edges[i]] = true;

		TEST ( size == numLoc,
		      "whereCanTheyGo ( dv, %s, road: %d   rail: %d   sea: %d ): %d locations", player_to_s ( name ), road, rail, sea, numLoc );
		for ( int i = 0; i < numLoc; i++) {
			EXPECT_TO_SEE ( expectLoc[i]);
		}
		free ( edges );
	}
}
void completeTest ( char *trail, PlayerMessage *messages,
                  PlayerID name, int round, int score, int HP,
                  LocationID myLoc, LocationID locID[6], //history
                  int numLoc, LocationID *expectLoc,
                  bool road, bool rail, bool sea ) {
	//test function
	DracView dv = newDracView ( trail, messages );

	TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
	TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

	testGeneralInfo ( dv,trail, round , score );
	testPlayerInfo ( dv, name, HP, myLoc, locID, numLoc, expectLoc,  road, rail, sea );

	disposeDracView ( dv );
	TEST ( mem_address_is_poisoned ( dv ),
	      "dv is no longer a valid allocation");
}
int
main ( void ) {
	puts ("1..many");

	do {////////////////////////////////////////////////////////////////
		char *symbols[] = {
			"newDracView", "disposeDracView", "giveMeTheRound",
			"giveMeTheScore", "howHealthyIs", "whereIs", "lastMove",
			"whatsThere", "giveMeTheTrail", "whereCanIgo",
			"whereCanTheyGo"
		};
		for ( size_t i = 0; i < nitems ( symbols ); i++)
			TEST ( have_symbol ( symbols[i]), "have '%s'", symbols[i]);
	} while ( 0 );
	do {////////////////////////////////////////////////////////////////
		//test the locations that can move to at 0 round, 4 turn
		//and the rail travel.
		char *trail = "GMR.... SBO.... HCG.... MSA.... ";
		PlayerMessage messages[] = {};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newHunterView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");
		//                               Name        rounds       score
		testGeneralInfo ( dv, trail, 0 , GAME_START_SCORE );
		//initiate all variables.
		LocationID locID[6];
		LocationID expectLoc[NUM_MAP_LOCATIONS];//maximum location number.
		int numLoc;
		int i;
		//DRACULA
		locID[0] =  UNKNOWN_LOCATION ;
		locID[1] =  UNKNOWN_LOCATION ;
		locID[2] =  UNKNOWN_LOCATION ;
		locID[3] =  UNKNOWN_LOCATION ;
		locID[4] =  UNKNOWN_LOCATION ;
		locID[5] =  UNKNOWN_LOCATION ;

		numLoc = NUM_MAP_LOCATIONS - 1;
		int index = 0;
		for ( i = 0; i < numLoc; i++) {
			if ( i + MIN_MAP_LOCATION != ST_JOSEPH_AND_ST_MARYS ) {
				expectLoc[index] = i + MIN_MAP_LOCATION;
				index++;
			}
		}
		testPlayerInfo ( dv, PLAYER_DRACULA, GAME_START_BLOOD_POINTS, UNKNOWN_LOCATION,
		                locID, numLoc, expectLoc,  true, true, true );

		LocationID locID_2[6];
		LocationID expectLoc_2[3] = {MARSEILLES, MEDITERRANEAN_SEA, PARIS};//maximum location number.
		int numLoc_2 = 3;
		//PLAYER_LORD_GODALMING
		locID_2[0] =  MARSEILLES ;
		locID_2[1] =  UNKNOWN_LOCATION ;
		locID_2[2] =  UNKNOWN_LOCATION ;
		locID_2[3] =  UNKNOWN_LOCATION ;
		locID_2[4] =  UNKNOWN_LOCATION ;
		locID_2[5] =  UNKNOWN_LOCATION ;

		testPlayerInfo ( dv, PLAYER_LORD_GODALMING, GAME_START_HUNTER_LIFE_POINTS, MARSEILLES,
		                locID_2, numLoc_2, expectLoc_2,  false, true, true );

		LocationID locID_3[6];
		LocationID expectLoc_3[11] = {BORDEAUX, NANTES, CLERMONT_FERRAND, TOULOUSE, SARAGOSSA, PARIS, LE_HAVRE, BRUSSELS, MARSEILLES, MADRID, BARCELONA};//maximum location number.
		int numLoc_3 = 11;
		//PLAYER_DR_SEWARD
		locID_3[0] =  BORDEAUX ;
		locID_3[1] =  UNKNOWN_LOCATION ;
		locID_3[2] =  UNKNOWN_LOCATION ;
		locID_3[3] =  UNKNOWN_LOCATION ;
		locID_3[4] =  UNKNOWN_LOCATION ;
		locID_3[5] =  UNKNOWN_LOCATION ;

		testPlayerInfo ( dv, PLAYER_DR_SEWARD, GAME_START_HUNTER_LIFE_POINTS, BORDEAUX,
		                locID_3, numLoc_3, expectLoc_3,  true, true, false );
		LocationID locID_4[6];
		LocationID expectLoc_4[3] = {CAGLIARI, TYRRHENIAN_SEA, MEDITERRANEAN_SEA};//maximum location number.
		int numLoc_4 = 3;
		//PLAYER_VAN_HELSING
		locID_4[0] =  CAGLIARI ;
		locID_4[1] =  UNKNOWN_LOCATION ;
		locID_4[2] =  UNKNOWN_LOCATION ;
		locID_4[3] =  UNKNOWN_LOCATION ;
		locID_4[4] =  UNKNOWN_LOCATION ;
		locID_4[5] =  UNKNOWN_LOCATION ;
		testPlayerInfo ( dv, PLAYER_VAN_HELSING, GAME_START_HUNTER_LIFE_POINTS, CAGLIARI,
		                locID_4, numLoc_4, expectLoc_4,  true, true, true );
		LocationID locID_5[6];
		LocationID expectLoc_5[1] = {SALONICA};//maximum location number.
		int numLoc_5 = 1;
		//PLAYER_MINA_HARKER
		locID_5[0] =  SALONICA ;
		locID_5[1] =  UNKNOWN_LOCATION ;
		locID_5[2] =  UNKNOWN_LOCATION ;
		locID_5[3] =  UNKNOWN_LOCATION ;
		locID_5[4] =  UNKNOWN_LOCATION ;
		locID_5[5] =  UNKNOWN_LOCATION ;
		testPlayerInfo ( dv, PLAYER_MINA_HARKER, GAME_START_HUNTER_LIFE_POINTS, SALONICA,
		                locID_5, numLoc_5, expectLoc_5,  false, true, false );



		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
	//printf ("---------------------------");

	do {////////////////////////////////////////////////////////////////
		// Test for basic functions, just before Dracula's first move

		char *trail =
		    "GST.... SAO.... HZU.... MBB....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", ""
		};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		TEST ( giveMeTheRound ( dv ) == 0,
		      "giveMeTheRound ( dv ) == 0");
		TEST ( howHealthyIs ( dv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS,
		      "howHealthyIs ( dv, Dracula ) == 40 ( start )");
		IS_AT ( PLAYER_LORD_GODALMING, STRASBOURG );
		IS_AT ( PLAYER_DR_SEWARD, ATLANTIC_OCEAN );
		IS_AT ( PLAYER_VAN_HELSING, ZURICH );
		IS_AT ( PLAYER_MINA_HARKER, BAY_OF_BISCAY );
		IS_AT ( PLAYER_DRACULA, UNKNOWN_LOCATION );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		// Test for encountering Dracula and hunter history

		char *trail =
		    "GST.... SAO.... HCD.... MAO.... DGE.... "
		    "GGED... SAO.... HCD.... MAO....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", ""
		};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		TEST ( giveMeTheRound ( dv ) == 1,
		      "giveMeTheRound ( dv ) == 1");
		IS_AT ( PLAYER_DRACULA, GENEVA );
		IS_AT ( PLAYER_LORD_GODALMING, GENEVA );
		TEST ( howHealthyIs ( dv, PLAYER_LORD_GODALMING ) == 5,
		      "howHealthyIs ( dv, Godalming ) == 5");
		TEST ( howHealthyIs ( dv, PLAYER_DRACULA ) == 30,
		      "howHealthyIs ( dv, Dracula ) == 30");

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( dv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, GENEVA );
		HISTORY_IS ( PLAYER_DRACULA, 1, UNKNOWN_LOCATION );

		giveMeTheTrail ( dv, PLAYER_LORD_GODALMING, history );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 0, GENEVA );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 1, STRASBOURG );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 2, UNKNOWN_LOCATION );

		giveMeTheTrail ( dv, PLAYER_DR_SEWARD, history );
		HISTORY_IS ( PLAYER_DR_SEWARD, 0, ATLANTIC_OCEAN );
		HISTORY_IS ( PLAYER_DR_SEWARD, 1, ATLANTIC_OCEAN );
		HISTORY_IS ( PLAYER_DR_SEWARD, 2, UNKNOWN_LOCATION );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		// Test for Dracula leaving minions

		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DED.V.. "
		    "GST.... SST.... HST.... MST.... DMNT... "
		    "GST.... SST.... HST.... MST....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"
		};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		int nT, nV;
		whatsThere ( dv, EDINBURGH, &nT, &nV );
		TEST ( nT == 0, "whatsThere ( dv, ED Edinburgh ): 0 trap ( s )");
		TEST ( nV == 1, "whatsThere ( dv, ED Edinburgh ): 1 vampire ( s )");
		whatsThere ( dv, MANCHESTER, &nT, &nV );
		TEST ( nT == 1, "whatsThere ( dv, MN Manchester ): 1 trap ( s )");
		TEST ( nV == 0, "whatsThere ( dv, MN Manchester ): 0 vampire ( s )");
		IS_AT ( PLAYER_DRACULA, MANCHESTER );

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( dv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, MANCHESTER );
		HISTORY_IS ( PLAYER_DRACULA, 1, EDINBURGH );
		HISTORY_IS ( PLAYER_DRACULA, 2, UNKNOWN_LOCATION );

		giveMeTheTrail ( dv, PLAYER_MINA_HARKER, history );
		HISTORY_IS ( PLAYER_MINA_HARKER, 0, STRASBOURG );
		HISTORY_IS ( PLAYER_MINA_HARKER, 1, STRASBOURG );
		HISTORY_IS ( PLAYER_MINA_HARKER, 2, GENEVA );
		HISTORY_IS ( PLAYER_MINA_HARKER, 3, UNKNOWN_LOCATION );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
	do {////////////////////////////////////////////////////////////////
		// Test for Dracula leaving minions -- MUNLIPLE and HIDE, DB

		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DED.V.. "
		    "GST.... SST.... HST.... MST.... DHIT... "
		    "GST.... SST.... HST.... MST.... DD1T..."
		    "GST.... SST.... HST.... MST....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"
		};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		int nT, nV;
		whatsThere ( dv, EDINBURGH, &nT, &nV );
		TEST ( nT == 2, "whatsThere ( dv, ED Edinburgh ): 2 trap ( s ) - %d",nT );
		TEST ( nV == 1, "whatsThere ( dv, ED Edinburgh ): 1 vampire ( s )");

		IS_AT ( PLAYER_DRACULA, EDINBURGH );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
	do {////////////////////////////////////////////////////////////////
		// Test for Dracula leaving minions -- Haven't finished
		// break all T and kill V

		char *trail =
		    "GGE.... SGE.... HGE.... MMN.... DED.V.. "
		    "GST.... SST.... HST.... MMN.... DHIT... "
		    "GST.... SST.... HST.... MMN.... DD1T..."
		    "GST.... SST.... HST.... MEDTTV.";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"
		};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		int nT, nV;
		whatsThere ( dv, EDINBURGH, &nT, &nV );
		TEST ( nT == 0, "whatsThere ( dv, ED Edinburgh ): 0 trap ( s ) - %d",nT );
		TEST ( nV == 0, "whatsThere ( dv, ED Edinburgh ): 0 vampire ( s )");
		TEST ( howHealthyIs ( dv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS ,
		      "howHealthyIs ( dv, Dracula ) == 30 ( start )");
		TEST ( howHealthyIs ( dv, PLAYER_MINA_HARKER ) == 5,
		      "howHealthyIs ( dv, PLAYER_MINA_HARKER ) == 1 ( start )");
		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
	do {////////////////////////////////////////////////////////////////
		// Test for Dracula leaving minions -- break one T
		// score: kill hunter

		char *trail =
		    "GGE.... SGE.... HGE.... MSW.... DED.V.. "
		    "GST.... SST.... HST.... MSW.... DHIT... "
		    "GST.... SST.... HST.... MSW.... DMNT..."
		    "GST.... SST.... HST.... MSW.... DLVT..."
		    "GST.... SST.... HST.... MLVTD.. DD3T..."
		    "GST.... SST.... HST.... MMNT... DNS.... "
		    "GST.... SST.... HST.... MEDT... ";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"
		};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		TEST ( howHealthyIs ( dv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 10 - 2,
		      "howHealthyIs ( dv, Dracula ) == 28 ( start )");
		TEST ( howHealthyIs ( dv, PLAYER_MINA_HARKER ) == 0,
		      "howHealthyIs ( dv, PLAYER_MINA_HARKER ) == 0");
		testGeneralInfo ( dv, trail, 6, 366 - 6 - 6 );

		int nT, nV;
		whatsThere ( dv, EDINBURGH, &nT, &nV );
		TEST ( nT == 1, "whatsThere ( dv, ED Edinburgh ): 1 trap ( s ) - %d",nT );
		TEST ( nV == 1, "whatsThere ( dv, ED Edinburgh ): 1 vampire ( s )");

		IS_AT ( PLAYER_DRACULA, NORTH_SEA );
		IS_AT ( PLAYER_MINA_HARKER, ST_JOSEPH_AND_ST_MARYS );


		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( dv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, NORTH_SEA );
		HISTORY_IS ( PLAYER_DRACULA, 1, EDINBURGH );
		HISTORY_IS ( PLAYER_DRACULA, 2, LIVERPOOL );
		HISTORY_IS ( PLAYER_DRACULA, 3, MANCHESTER );
		HISTORY_IS ( PLAYER_DRACULA, 4, EDINBURGH );
		HISTORY_IS ( PLAYER_DRACULA, 5, EDINBURGH );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Test for Dracula leaving minions -- break one T
		// score: grow V
		// drop T

		char *trail =
		    "GGE.... SGE.... HGE.... MSW.... DED.V.. "
		    "GGE.... SST.... HST.... MSW.... DHIT... "
		    "GGE.... SGE.... HGE.... MSW.... DMNT... "
		    "GST.... SST.... HST.... MSW.... DLVT... "
		    "GST.... SST.... HST.... MSW.... DD3T..."
		    "GST.... SST.... HST.... MSW.... DLVT..."
		    "GST.... SST.... HST.... MSW.... DIR..V."
		    "GST.... SST.... HST.... MSW.... DAO..M. "
		    "GST.... SST.... HST.... MSW.... ";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"
		};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");


		testGeneralInfo ( dv, trail, 8, 366 - 8 - 13 );
		TEST ( howHealthyIs ( dv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 4,
		      "howHealthyIs ( dv, Dracula ) == 36 ( start ) %d",howHealthyIs ( dv, PLAYER_DRACULA ));
		int nT, nV;
		whatsThere ( dv, EDINBURGH, &nT, &nV );
		TEST ( nT == 1, "whatsThere ( dv, ED Edinburgh ): 1 trap ( s ) - %d",nT );
		TEST ( nV == 0, "whatsThere ( dv, ED Edinburgh ): 0 vampire ( s )");


		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Checking Galatz road connections

		char *trail = "GGA....";
		PlayerMessage messages[] = {"Gone to Galatz"};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		int size, *edges = whereCanTheyGo (
		                       dv, &size, PLAYER_LORD_GODALMING, true, false, false );
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for ( int i = 0; i < size; i++)
			seen[edges[i]] = true;

		TEST ( size == 5,
		      "whereCanTheyGo ( dv, Godalming, road ): 5 locations");
		EXPECT_TO_SEE ( GALATZ );
		EXPECT_TO_SEE ( CONSTANTA );
		EXPECT_TO_SEE ( BUCHAREST );
		EXPECT_TO_SEE ( KLAUSENBURG );
		EXPECT_TO_SEE ( CASTLE_DRACULA );

		free ( edges );
		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		// Checking Ionian Sea sea connections

		char *trail = "GIO....";
		PlayerMessage messages[] = {"Sailing the Ionian"};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		int size, *edges = whereCanTheyGo (
		                       dv, &size, PLAYER_LORD_GODALMING, false, false, true );
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for ( int i = 0; i < size; i++)
			seen[edges[i]] = true;

		TEST ( size == 7,
		      "whereCanTheyGo ( dv, Godalming, sea ): 7 locations");
		EXPECT_TO_SEE ( IONIAN_SEA );
		EXPECT_TO_SEE ( BLACK_SEA );
		EXPECT_TO_SEE ( ADRIATIC_SEA );
		EXPECT_TO_SEE ( TYRRHENIAN_SEA );
		EXPECT_TO_SEE ( ATHENS );
		EXPECT_TO_SEE ( VALONA );
		EXPECT_TO_SEE ( SALONICA );

		free ( edges );
		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		// Checking Athens rail connections ( none )

		char *trail = "GAT....";
		PlayerMessage messages[] = {"Leaving Athens by train"};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		int size, *edges = whereCanTheyGo (
		                       dv, &size, PLAYER_LORD_GODALMING, false, true, false );

		bool seen[NUM_MAP_LOCATIONS] = {false};
		for ( int i = 0; i < size; i++)
			seen[edges[i]] = true;

		TEST ( size == 1,
		      "whereCanTheyGo ( dv, Godalming, rail ): 1 location");
		EXPECT_TO_SEE ( ATHENS );

		free ( edges );
		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital
		// and for dracula hiding

		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... MCO.... DSRT... "
		    "GSNV... SMR.... HCD.... MAM.... DMAT... "
		    "GSRT... SCF.... HGA.... MBU.... DHIT... "
		    "GMATTD. SCF.... HGA.... MBU....";

		PlayerMessage messages[24] = {"Going somewhere",""};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		IS_AT ( PLAYER_LORD_GODALMING, ST_JOSEPH_AND_ST_MARYS );

		LocationID from, to;
		lastMove ( dv, PLAYER_LORD_GODALMING, &from, &to );
		TEST ( from == SARAGOSSA && to == MADRID,
		      "lastMove ( Godalming ): Saragossa -> Madrid");

		IS_AT ( PLAYER_DRACULA, MADRID );

		lastMove ( dv, PLAYER_DRACULA, &from, &to );
		TEST ( from == MADRID && to == HIDE,
		      "lastMove ( dracula ): Madrid -> Hide");



		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital
		// and for dracula hiding and doubling back

		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... MCO.... DSRT... "
		    "GSNV... SMR.... HCD.... MAM.... DMAT... "
		    "GSRT... SCF.... HGA.... MBU.... DHIT... "
		    "GMATTD. SCF.... HGA.... MBU.... DD1T... "
		    "GZA.... SCF.... HGA....";

		PlayerMessage messages[28] = {"Going somewhere",""};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		IS_AT ( PLAYER_LORD_GODALMING, ZAGREB );

		LocationID from, to;
		lastMove ( dv, PLAYER_LORD_GODALMING, &from, &to );
		TEST ( from == MADRID && to == ZAGREB,
		      "lastMove ( Godalming ): Madrid ( via Hospital ) -> Zagreb");

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( dv, PLAYER_LORD_GODALMING, history );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 0, ZAGREB );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 1, MADRID );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 2, SARAGOSSA );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 3, SANTANDER );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 4, MADRID );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 5, LISBON );

		IS_AT ( PLAYER_DRACULA, MADRID );

		lastMove ( dv, PLAYER_DRACULA, &from, &to );
		TEST ( from == HIDE && to == DOUBLE_BACK_1,
		      "lastMove ( dracula ): Hide -> Double Back 1");

		giveMeTheTrail ( dv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, MADRID );
		HISTORY_IS ( PLAYER_DRACULA, 1, MADRID );
		HISTORY_IS ( PLAYER_DRACULA, 2, MADRID );
		HISTORY_IS ( PLAYER_DRACULA, 3, SARAGOSSA );
		HISTORY_IS ( PLAYER_DRACULA, 4, SANTANDER );
		HISTORY_IS ( PLAYER_DRACULA, 5, UNKNOWN_LOCATION );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital
		// in turn 0 and 1

		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... ";

		PlayerMessage messages[28] = {"Going somewhere",""};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");


		LocationID from, to;
		lastMove ( dv, PLAYER_LORD_GODALMING, &from, &to );
		TEST ( from == LISBON && to == MADRID,
		      "lastMove ( LISBON )-> MADRID");


		IS_AT ( PLAYER_DRACULA, SANTANDER );

		lastMove ( dv, PLAYER_DRACULA, &from, &to );
		TEST ( from == UNKNOWN_LOCATION && to == SANTANDER,
		      "lastMove ( dracula ): UNKNOWN_LOCATION -> SANTANDER");


		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
	do {////////////////////////////////////////////////////////////////
		// Checking where dracula can go

		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... MCO.... DSRT... "
		    "GSNV... SMR.... HCD.... MAM.... DMAT... "
		    "GSRT... SCF.... HGA.... MBU.... DHIT... "
		    "GMATTD. SCF.... HGA.... MBU....";

		PlayerMessage messages[24] = {"Going somewhere",""};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		int size, *edges = whereCanIgo ( dv, &size, true, true );
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for ( int i = 0; i < size; i++)
			seen[edges[i]] = true;

		TEST ( size == 5,
		      "whereCanIgo ( dv, Dracula, road,sea ): 5 locations");
		EXPECT_TO_SEE ( ALICANTE );
		EXPECT_TO_SEE ( CADIZ );
		EXPECT_TO_SEE ( GRANADA );
		EXPECT_TO_SEE ( LISBON );
		EXPECT_TO_SEE ( MADRID );

		free ( edges );
		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Checking where dracula can go

		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... MCO.... DSRT... "
		    "GSNV... SMR.... HCD.... MAM.... DMAT... "
		    "GSRT... SCF.... HGA.... MBU.... DHIT... "
		    "GMATTD. SCF.... HGA.... MBU.... DD1T... "
		    "GZA.... SCF.... HGA....";

		PlayerMessage messages[28] = {"Going somewhere",""};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");


		int size, *edges = whereCanIgo ( dv, &size, true, true );
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for ( int i = 0; i < size; i++)
			seen[edges[i]] = true;

		//printf ("size = %d\n",size );
		TEST ( size == 5,
		      "whereCanIgo ( dv, Dracula, road,sea ): 5 locations");
		EXPECT_TO_SEE ( ALICANTE );
		EXPECT_TO_SEE ( CADIZ );
		EXPECT_TO_SEE ( GRANADA );
		EXPECT_TO_SEE ( LISBON );
		EXPECT_TO_SEE ( MADRID );

		free ( edges );
		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Checking where dracula can go with TP

		char *trail =  "GMA.... SMN.... HAM.... MGA.... DGA.V.. "
		               "GAL.... SED.... HBU.... MGAVD.. DCDT... "
		               "GMS.... SNS.... HCO.... MCDTD.. DKLT... "
		               "GMR.... SHA.... HFR.... MCD.... DD2T... "
		               "GGO.... SBR.... HBR.... MCDTD.. DHIT... "
		               "GVE.... SPR.... HBD.... MJM.... DTPT... "
		               "GBD.... SBD.... HKLT... MBE....";


		PlayerMessage messages[34] = {"Going somewhere",""};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");
		//printf ("%s\n",idToAbbrev ( whereIs ( dv, PLAYER_DRACULA )));
		IS_AT ( PLAYER_DRACULA, CASTLE_DRACULA );

		int from, to;
		lastMove ( dv, PLAYER_DRACULA, &from, &to );
		TEST ( from == HIDE && to == TELEPORT,
		      "lastMove ( dracula ): Hide -> Teleport");

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( dv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, CASTLE_DRACULA );
		HISTORY_IS ( PLAYER_DRACULA, 1, CASTLE_DRACULA );
		HISTORY_IS ( PLAYER_DRACULA, 2, CASTLE_DRACULA );
		HISTORY_IS ( PLAYER_DRACULA, 3, KLAUSENBURG );
		HISTORY_IS ( PLAYER_DRACULA, 4, CASTLE_DRACULA );
		HISTORY_IS ( PLAYER_DRACULA, 5, GALATZ );


		int size, *edges = whereCanIgo ( dv, &size, true, true );
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for ( int i = 0; i < size; i++)
			seen[edges[i]] = true;

		TEST ( size == 2,
		      "whereCanIgo ( dv, Dracula, road,sea ): 2 locations");
		EXPECT_TO_SEE ( GALATZ );
		EXPECT_TO_SEE ( CASTLE_DRACULA );

		free ( edges );
		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
///////////////////////////
	do {////////////////////////////////////////////////////////////////
		// Test for Dracula doubling back at sea, and losing blood
		// points ( Drac View )

		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DBS.... "
		    "GST.... SST.... HST.... MST.... DD1...."
		    "GST.... SST.... HST.... MST....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"
		};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");


		TEST ( howHealthyIs ( dv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 4,
		      "howHealthyIs ( dv, Dracula ) == 36");
		IS_AT ( PLAYER_DRACULA, BLACK_SEA );

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( dv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, BLACK_SEA );
		HISTORY_IS ( PLAYER_DRACULA, 1, BLACK_SEA );
		HISTORY_IS ( PLAYER_DRACULA, 2, UNKNOWN_LOCATION );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Test for Dracula doubling back at sea, and losing blood
		// points ( Drac View ) ( from city )

		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DBS.... "
		    "GST.... SST.... HST.... MST.... DCNT..."
		    "GST.... SST.... HST.... MST.... DD2...."
		    "GST.... SST.... HST.... MST....";
		PlayerMessage messages[] = {};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		TEST ( howHealthyIs ( dv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 4,
		      "howHealthyIs ( dv, Dracula ) == 36");
		//AVOID hp > 9
		TEST ( howHealthyIs ( dv, PLAYER_LORD_GODALMING ) == GAME_START_HUNTER_LIFE_POINTS ,
		      "howHealthyIs ( dv, PLAYER_LORD_GODALMING ) == 9");
		IS_AT ( PLAYER_DRACULA, BLACK_SEA );

		LocationID history[TRAIL_SIZE];
		giveMeTheTrail ( dv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, BLACK_SEA );
		HISTORY_IS ( PLAYER_DRACULA, 1, CONSTANTA );
		HISTORY_IS ( PLAYER_DRACULA, 2, BLACK_SEA );
		HISTORY_IS ( PLAYER_DRACULA, 3, UNKNOWN_LOCATION );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
//*****************************//
	PlayerMessage messages[] = {};
	//Dracula hp can over 40
	//G will be tp to hospital since he is killed before rest.
	do {
		//Trail
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.."// +10hp
		              "GKL.... SBO.... HBO.... MBO.... DGAT..."
		              "GKL.... SBO.... HBO.... MBO.... DD2T..."//+10hp
		              "GKL.... SBO.... HBO.... MBO.... DHIT..."//+10hp
		              "GKL.... SBO.... HBO.... MBO.... DKLT..."
		              "GKLTD.. SBO.... HBO.... MBO.... DBET..."
		              "GKLTD.. SBO.... HBO.... MBO.... ";//--6score
		DracView dv = newDracView ( trail, messages );
		//Player's name
		PlayerID name = PLAYER_LORD_GODALMING;
		int HP = 0;
		testGeneralInfo ( dv, trail, 6, 366 - 6 - 6 );
		TEST ( howHealthyIs ( dv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS + 10,
		      "howHealthyIs ( dv, Dracula ) == 50 ( start ) %d",howHealthyIs ( dv, PLAYER_DRACULA ));
		// current player's location
		LocationID myLoc = ST_JOSEPH_AND_ST_MARYS;
		// current player's history trail
		LocationID locID[6] = { KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG
		                      };
		// total number of location that player can go to there.
		int numLoc = 5;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[5] = { ST_JOSEPH_AND_ST_MARYS, ZAGREB, SARAJEVO, BELGRADE, SZEGED};
		bool road = true;
		bool rail = true;
		bool sea = true;

		testPlayerInfo ( dv,
		               name, HP,
		               myLoc, locID, //history
		               numLoc, expectLoc,
		               road, rail, sea );
		disposeDracView ( dv );
	} while ( 0 );
	// in same turn: hp = 0
	do {
		//getchar ();
		//Trail
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.."// +10hp
		              "GKL.... SBO.... HBO.... MBO.... DGAT..."
		              "GKL.... SBO.... HBO.... MBO.... DD2T..."//+10hp
		              "GKL.... SBO.... HBO.... MBO.... DHIT..."//+10hp
		              "GKL.... SBO.... HBO.... MBO.... DKLT..."
		              "GKLTD.. SBO.... HBO.... MBO.... DBET..."
		              "GKLTD.. SBO.... HBO.... MBO.... ";
		//Player's name
		//PlayerID name = PLAYER_DR_SEWARD;
		int round = 6;
		int score = GAME_START_SCORE - 6 - 6;
		int HP = 0;
		// current player's location
		LocationID myLoc = ST_JOSEPH_AND_ST_MARYS;
		// current player's history trail
		LocationID locID[6] = { KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG,
		                        KLAUSENBURG
		                      };
		// total number of location that player can go to there.
		int numLoc = 5;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[5] = { ST_JOSEPH_AND_ST_MARYS, ZAGREB, SARAJEVO, BELGRADE, SZEGED};
		bool road = true;
		bool rail = false;
		bool sea = true;

		DracView gv = newDracView ( trail, messages );

		TEST ( gv != NULL, "gv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		testGeneralInfo ( gv,trail, round , score );
		testPlayerInfo ( gv, PLAYER_LORD_GODALMING, HP, myLoc, locID, numLoc, expectLoc,  road, rail, sea );

		disposeDracView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
		//getchar ();
	} while ( 0 );
	do {
		//hp > 40
		//Trail
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.. "// +10hp
		              "GCD.... SBO.... HBO.... MBO.... ";
		//Player's name
		PlayerID name = PLAYER_DRACULA;
		int HP = 50;
		// current player's location
		LocationID myLoc = CASTLE_DRACULA;
		// current player's history trail
		LocationID locID[6] = { CASTLE_DRACULA,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 3;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[3] = { CASTLE_DRACULA, GALATZ, KLAUSENBURG};
		bool road = true;
		bool rail = true;
		bool sea = true;

		DracView gv = newDracView ( trail, messages );

		TEST ( gv != NULL, "gv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		testPlayerInfo ( gv, name, HP, myLoc, locID, numLoc, expectLoc,  road, rail, sea );

		disposeDracView ( gv );
		TEST ( mem_address_is_poisoned ( gv ), "gv is a valid allocation");
	} while ( 0 );


	do {
		//long Dracula history: trail with Hide, double back
		//Trail
		char *trail = "GBO.... SBO.... HBO.... MBO.... DCD.V.."// +10hp
		              "GBO.... SBO.... HBO.... MBO.... DHIT..."
		              "GBO.... SBO.... HBO.... MBO.... DGAT..."
		              "GBO.... SBO.... HBO.... MBO.... DBCT..."
		              "GBO.... SBO.... HBO.... MBO.... DSZT..."
		              "GBO.... SBO.... HBO.... MBO.... DKLT..."
		              "GBO.... SBO.... HBO.... MBO.... DD5T.V."
		              "GBO.... SBO.... HBO.... MBO.... DHIT..."
		              "GBO.... SBO.... HBO....  ";
		//Player's name
		PlayerID name = PLAYER_DRACULA;
		//int round = 8;
		//int score = GAME_START_SCORE - 8 - 13;
		int HP = 80;
		// current player's location
		LocationID myLoc = CASTLE_DRACULA;
		// current player's history trail
		LocationID locID[6] = { CASTLE_DRACULA,
		                        CASTLE_DRACULA,
		                        KLAUSENBURG,
		                        SZEGED,
		                        BUCHAREST,
		                        GALATZ
		                      };
		// total number of connected location
		int numLoc = 2;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[3] = {CASTLE_DRACULA, GALATZ};
		bool road = true;
		bool rail = true;
		bool sea = true;

		DracView dv = newDracView ( trail, messages );

		int nT, nV;
		whatsThere ( dv, CASTLE_DRACULA, &nT, &nV );
		TEST ( nT == 3, "whatsThere ( dv, ED Edinburgh ): 0 trap ( s ) - %d",nT );
		TEST ( nV == 0, "whatsThere ( dv, ED Edinburgh ): 0 vampire ( s )");

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		testPlayerInfo ( dv, name, HP, myLoc, locID, numLoc, expectLoc,  road, rail, sea );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ), "dv is a valid allocation");
	} while ( 0 );




	do {////////////////////////////////////////////////////////////////
		//If a location is connected with road and rail
		//the get connection still can get the city that connected with the location via rail.
		char *trail = "GMR.... SBO.... HCG.... MSA.... ";
		PlayerMessage messages[] = {};
		DracView dv = newDracView ( trail, messages );

		TEST ( dv != NULL, "dv := newDracView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( dv ), "dv is a valid allocation");

		LocationID locID_3[6];
		LocationID expectLoc_3[11] = {BORDEAUX, NANTES, CLERMONT_FERRAND, TOULOUSE, SARAGOSSA, PARIS, LE_HAVRE, BRUSSELS, MARSEILLES, MADRID, BARCELONA};//maximum location number.
		int numLoc_3 = 11;
		//PLAYER_DR_SEWARD
		locID_3[0] =  BORDEAUX ;
		locID_3[1] =  UNKNOWN_LOCATION ;
		locID_3[2] =  UNKNOWN_LOCATION ;
		locID_3[3] =  UNKNOWN_LOCATION ;
		locID_3[4] =  UNKNOWN_LOCATION ;
		locID_3[5] =  UNKNOWN_LOCATION ;

		testPlayerInfo ( dv, PLAYER_DR_SEWARD, GAME_START_HUNTER_LIFE_POINTS, BORDEAUX,
		                locID_3, numLoc_3, expectLoc_3,  true, true, false );

		disposeDracView ( dv );
		TEST ( mem_address_is_poisoned ( dv ),
		      "dv is no longer a valid allocation");
	} while ( 0 );
	return EXIT_SUCCESS;
}


static const char *
player_to_s (
    PlayerID p ) {
	switch ( p ) {
	case PLAYER_LORD_GODALMING:
		return "Godalming";
	case PLAYER_DR_SEWARD:
		return "Seward";
	case PLAYER_VAN_HELSING:
		return "vHelsing";
	case PLAYER_MINA_HARKER:
		return "Harker";
	case PLAYER_DRACULA:
		return "Dracula";
	default:
		return "???";
	}
}
