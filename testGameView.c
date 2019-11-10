////////////////////////////////////////////////////////////////////////
// COMP2521 18x1 ... the Fury of Dracula
// testGameView.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>

#define NDEBUG // use TAP please
#include "test1511.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GameView.h"
#include "Places.h"

#define IS_AT(who,where) \
	TEST (getLocation (gv, (who)) == (where), \
		"getLocation(gv, %s) == %s %s", \
		player_to_s (who), idToAbbrev (where), idToName (where))
#define HISTORY_IS(who,n,where) \
	TEST (history[(n)] == (where), \
		"getHistory(gv, %s): [%d] %s %s", \
		player_to_s (who), n, idToAbbrev (where), idToName (where))
#define EXPECT_TO_SEE(where) \
	TEST (seen[where], "... %s %s", idToAbbrev (where), idToName (where))


static const char *player_to_s ( PlayerID );
//gameView Version
void testGeneralInfo ( GameView gv, char *trail, PlayerID name, int round, int score ) {
	TEST ( getCurrentPlayer ( gv ) == name,
	      "getCurrentPlayer ( gv ) == %s", player_to_s ( name ) );
	TEST ( getRound ( gv ) == round,
	      "getRound ( gv ) == %d",round );
	TEST ( getScore ( gv ) == score,
	      "getScore ( gv ) == %d  %d",score, getScore ( gv ));
}

void testPlayerInfo ( GameView gv, PlayerID name, int HP, LocationID myLoc,
                    LocationID locID[6], int numLoc, LocationID *expectLoc,  bool road, bool rail, bool sea ) {
	//new added tests
	TEST ( getHealth ( gv, name ) == HP,
	      "getHealth ( gv, %s ) == %d %d", player_to_s ( name ), HP, getHealth ( gv, name ));
	IS_AT ( name, myLoc );
	LocationID history[TRAIL_SIZE];
	getHistory ( gv, name, history );
	HISTORY_IS ( name, 0, locID[0]);
	HISTORY_IS ( name, 1, locID[1]);
	HISTORY_IS ( name, 2, locID[2]);
	HISTORY_IS ( name, 3, locID[3]);
	HISTORY_IS ( name, 4, locID[4]);
	HISTORY_IS ( name, 5, locID[5]);
	int size, *edges = connectedLocations ( gv, &size,
	                                       expectLoc[0], name, getRound ( gv ),
	                                       road, rail, sea );
	bool seen[NUM_MAP_LOCATIONS] = {false};
	for ( int i = 0; i < size; i++)
		seen[edges[i]] = true;

	TEST ( size == numLoc,
	      "connected ( hv, %s, road: %d   rail: %d   sea: %d ): %d locations",
	      player_to_s ( name ), road, rail, sea, numLoc );
	for ( int i = 0; i < numLoc; i++) {
		EXPECT_TO_SEE ( expectLoc[i]);
	}

	free ( edges );
}
void completeTest ( char *trail, PlayerMessage *messages,
                  PlayerID name, int round, int score, int HP,
                  LocationID myLoc, LocationID locID[6], //history
                  int numLoc, LocationID *expectLoc,
                  bool road, bool rail, bool sea ) {
	//test function
	GameView gv = newGameView ( trail, messages );

	TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
	TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

	testGeneralInfo ( gv,trail, name , round , score );
	testPlayerInfo ( gv, name, HP, myLoc, locID, numLoc, expectLoc,  road, rail, sea );

	disposeGameView ( gv );
	TEST ( mem_address_is_poisoned ( gv ),
	      "hv is no longer a valid allocation");
}
int
main ( void ) {
	puts ("1..116");

	do {////////////////////////////////////////////////////////////////
		char *symbols[] = {
			"newGameView", "disposeGameView", "getRound",
			"getCurrentPlayer", "getScore", "getHealth",
			"getLocation", "getHistory", "connectedLocations"
		};

		for ( size_t i = 0; i < nitems ( symbols ); i++)
			TEST ( have_symbol ( symbols[i]), "have '%s'", symbols[i]);
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		char *trail = "";
		PlayerMessage messages[] = {};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		TEST ( getCurrentPlayer ( gv ) == PLAYER_LORD_GODALMING,
		      "getCurrentPlayer ( gv ) == Godalming");
		TEST ( getRound ( gv ) == 0,
		      "getRound ( gv ) == 0");
		TEST ( getHealth ( gv, PLAYER_DR_SEWARD ) == GAME_START_HUNTER_LIFE_POINTS,
		      "getHealth ( gv, Seward ) == 9 ( start )");
		TEST ( getHealth ( gv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS,
		      "getHealth ( gv, Dracula ) == 40 ( start )");
		TEST ( getScore ( gv ) == GAME_START_SCORE,
		      "getScore ( gv ) == 366");
		IS_AT ( PLAYER_LORD_GODALMING, UNKNOWN_LOCATION );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		char *trail =
		    "GST.... SAO.... HZU.... MBB.... DC?....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"
		};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		TEST ( getCurrentPlayer ( gv ) == PLAYER_LORD_GODALMING,
		      "getCurrentPlayer ( gv ) == Godalming");
		TEST ( getRound ( gv ) == 1,
		      "getRound ( gv ) == 1");

		IS_AT ( PLAYER_LORD_GODALMING, STRASBOURG );
		IS_AT ( PLAYER_DR_SEWARD, ATLANTIC_OCEAN );
		IS_AT ( PLAYER_VAN_HELSING, ZURICH );
		IS_AT ( PLAYER_MINA_HARKER, BAY_OF_BISCAY );
		IS_AT ( PLAYER_DRACULA, CITY_UNKNOWN );

		TEST ( getHealth ( gv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS,
		      "getHealth ( gv, Dracula ) == 40 ( start )");

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		char *trail =
		    "GST.... SAO.... HCD.... MAO.... DGE.... "
		    "GGED...";
		PlayerMessage messages[] = {
			"Hello", "Rubbish",	 "Stuff", "", "Mwahahah",
			"Aha!"
		};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		IS_AT ( PLAYER_DRACULA, GENEVA );
		TEST ( getHealth ( gv, PLAYER_LORD_GODALMING ) == 5,
		      "getHealth ( gv, Godalming ) == 5");
		TEST ( getHealth ( gv, PLAYER_DRACULA ) == 30,
		      "getHealth ( gv, Dracula ) == 30");
		IS_AT ( PLAYER_LORD_GODALMING, GENEVA );

		LocationID history[TRAIL_SIZE];
		getHistory ( gv, PLAYER_LORD_GODALMING, history );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 0, GENEVA );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 1, STRASBOURG );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 2, UNKNOWN_LOCATION );

		getHistory ( gv, PLAYER_DR_SEWARD, history );
		HISTORY_IS ( PLAYER_DR_SEWARD, 0, ATLANTIC_OCEAN );
		HISTORY_IS ( PLAYER_DR_SEWARD, 1, UNKNOWN_LOCATION );

		getHistory ( gv, PLAYER_VAN_HELSING, history );
		HISTORY_IS ( PLAYER_VAN_HELSING, 0, CASTLE_DRACULA );
		HISTORY_IS ( PLAYER_VAN_HELSING, 1, UNKNOWN_LOCATION );

		getHistory ( gv, PLAYER_MINA_HARKER, history );
		HISTORY_IS ( PLAYER_MINA_HARKER, 0, ATLANTIC_OCEAN );
		HISTORY_IS ( PLAYER_MINA_HARKER, 1, UNKNOWN_LOCATION );

		getHistory ( gv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, GENEVA );
		HISTORY_IS ( PLAYER_DRACULA, 1, UNKNOWN_LOCATION );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DS?.... "
		    "GST.... SST.... HST.... MST.... DD1....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"
		};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		TEST ( getCurrentPlayer ( gv ) == PLAYER_LORD_GODALMING,
		      "getCurrentPlayer ( gv ) == Godalming");
		TEST ( getHealth ( gv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 4,
		      "getHealth ( gv, Dracula ) == 36");
		IS_AT ( PLAYER_DRACULA, DOUBLE_BACK_1 );

		LocationID history[TRAIL_SIZE];
		getHistory ( gv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, DOUBLE_BACK_1 );
		HISTORY_IS ( PLAYER_DRACULA, 1, SEA_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 2, UNKNOWN_LOCATION );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
	} while ( 0 );


	do {////////////////////////////////////////////////////////////////
		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DEC.... "
		    "GST.... SST.... HST.... MST.... DD1....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"
		};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		TEST ( getCurrentPlayer ( gv ) == PLAYER_LORD_GODALMING,
		      "getCurrentPlayer ( gv ) == Godalming");
		TEST ( getHealth ( gv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 4,
		      "getHealth ( gv, Dracula ) == 36");
		IS_AT ( PLAYER_DRACULA, DOUBLE_BACK_1 );

		LocationID history[TRAIL_SIZE];
		getHistory ( gv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, DOUBLE_BACK_1 );
		HISTORY_IS ( PLAYER_DRACULA, 1, ENGLISH_CHANNEL );
		HISTORY_IS ( PLAYER_DRACULA, 2, UNKNOWN_LOCATION );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DS?.... "
		    "GGE.... SGE.... HGE.... MGE.... DC?T... "
		    "GST.... SST.... HST.... MST.... DD2....";
		PlayerMessage messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"
		};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		TEST ( getCurrentPlayer ( gv ) == PLAYER_LORD_GODALMING,
		      "getCurrentPlayer ( gv ) == Godalming");
		TEST ( getHealth ( gv, PLAYER_DRACULA ) == GAME_START_BLOOD_POINTS - 4,
		      "getHealth ( gv, Dracula ) == 36");
		IS_AT ( PLAYER_DRACULA, DOUBLE_BACK_2 );

		LocationID history[TRAIL_SIZE];
		getHistory ( gv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, DOUBLE_BACK_2 );
		HISTORY_IS ( PLAYER_DRACULA, 1, CITY_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 2, SEA_UNKNOWN );
		HISTORY_IS ( PLAYER_DRACULA, 3, UNKNOWN_LOCATION );
		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		char *trail = "";
		PlayerMessage messages[] = {};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		do {
			int size, *edges = connectedLocations ( gv, &size,
			                                       GALATZ, PLAYER_LORD_GODALMING, 0, true, false, false );

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for ( int i = 0; i < size; i++)
				seen[edges[i]] = true;

			TEST ( size == 5,
			      "connected ( gv, GA Galatz, Godalming@0, road ): 5 locations");
			EXPECT_TO_SEE ( GALATZ );
			EXPECT_TO_SEE ( CONSTANTA );
			EXPECT_TO_SEE ( BUCHAREST );
			EXPECT_TO_SEE ( KLAUSENBURG );
			EXPECT_TO_SEE ( CASTLE_DRACULA );

			free ( edges );
		} while ( 0 );
		do {
			int size, *edges = connectedLocations ( gv, &size,
			                                       CONSTANTA, PLAYER_LORD_GODALMING, 0, true, false, true );

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for ( int i = 0; i < size; i++)
				seen[edges[i]] = true;

			TEST ( size == 5,
			      "connected ( gv, GA Galatz, Godalming@0, road ): 5 locations");
			EXPECT_TO_SEE ( GALATZ );
			EXPECT_TO_SEE ( CONSTANTA );
			EXPECT_TO_SEE ( BUCHAREST );
			EXPECT_TO_SEE ( VARNA );
			EXPECT_TO_SEE ( BLACK_SEA );

			free ( edges );
		} while ( 0 );
		do {
			int size, *edges = connectedLocations ( gv, &size,
			                                       IONIAN_SEA, PLAYER_LORD_GODALMING, 0,
			                                       false, false, true );

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for ( int i = 0; i < size; i++)
				seen[edges[i]] = true;

			TEST ( size == 7,
			      "connected ( gv, IO Ionian, Godalming@0, sea ): 7 locations");
			EXPECT_TO_SEE ( IONIAN_SEA );
			EXPECT_TO_SEE ( BLACK_SEA );
			EXPECT_TO_SEE ( ADRIATIC_SEA );
			EXPECT_TO_SEE ( TYRRHENIAN_SEA );
			EXPECT_TO_SEE ( ATHENS );
			EXPECT_TO_SEE ( VALONA );
			EXPECT_TO_SEE ( SALONICA );

			free ( edges );
		} while ( 0 );

		do {
			int size, *edges = connectedLocations ( gv, &size,
			                                       ATHENS, PLAYER_LORD_GODALMING, 0,
			                                       false, true, false );

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for ( int i = 0; i < size; i++)
				seen[edges[i]] = true;

			TEST ( size == 1,
			      "connected ( gv, AT Athens, Godalming@0, rail ): 1 location");
			EXPECT_TO_SEE ( ATHENS );

			free ( edges );
		} while ( 0 );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital

		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... MCO.... DSRT... "
		    "GSNV... SMR.... HCD.... MAM.... DMAT... "
		    "GSRT... SCF.... HGA.... MBU.... DHIT... "
		    "GMATTD. SCF.... HGA.... MBU....";

		PlayerMessage messages[24] = {"Going somewhere",""};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		IS_AT ( PLAYER_LORD_GODALMING, ST_JOSEPH_AND_ST_MARYS );
		IS_AT ( PLAYER_DRACULA, HIDE );
		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "gv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital

		char *trail =
		    "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
		    "GMA.... SCF.... HGA.... MCO.... DSRT... "
		    "GSNV... SMR.... HCD.... MAM.... DMAT... "
		    "GSRT... SCF.... HGA.... MBU.... DHIT... "
		    "GMATTD. SCF.... HGA.... MBU.... DD1T... "
		    "GZA.... SCF.... HGA....";

		PlayerMessage messages[28] = {"Going somewhere",""};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		IS_AT ( PLAYER_LORD_GODALMING, ZAGREB );

		LocationID history[TRAIL_SIZE];
		getHistory ( gv, PLAYER_LORD_GODALMING, history );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 0, ZAGREB );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 1, MADRID );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 2, SARAGOSSA );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 3, SANTANDER );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 4, MADRID );
		HISTORY_IS ( PLAYER_LORD_GODALMING, 5, LISBON );

		IS_AT ( PLAYER_DRACULA, DOUBLE_BACK_1 );
		getHistory ( gv, PLAYER_DRACULA, history );
		HISTORY_IS ( PLAYER_DRACULA, 0, DOUBLE_BACK_1 );
		HISTORY_IS ( PLAYER_DRACULA, 1, HIDE );
		HISTORY_IS ( PLAYER_DRACULA, 2, MADRID );
		HISTORY_IS ( PLAYER_DRACULA, 3, SARAGOSSA );
		HISTORY_IS ( PLAYER_DRACULA, 4, SANTANDER );
		HISTORY_IS ( PLAYER_DRACULA, 5, UNKNOWN_LOCATION );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "hv is no longer a valid allocation");
	} while ( 0 );

	do {////////////////////////////////////////////////////////////////
		// Checking location and history for dracula TP moves

		char *trail =  "GMA.... SMN.... HAM.... MGA.... DGA.V.. "
		               "GAL.... SED.... HBU.... MGAVD.. DCDT... "
		               "GMS.... SNS.... HCO.... MCDTD.. DKLT... "
		               "GMR.... SHA.... HFR.... MCD.... DD2T... "
		               "GGO.... SBR.... HBR.... MCDTD.. DHIT... "
		               "GVE.... SPR.... HBD.... MJM.... DTPT... "
		               "GBD.... SBD.... HKLT... MBE....";

		PlayerMessage messages[34] = {"Going somewhere",""};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");
		TEST ( getRound ( gv ) == 6,
		      "getRound ( gv ) == 6");
		LocationID history[TRAIL_SIZE];

		IS_AT ( PLAYER_DRACULA, TELEPORT );
//				printf ("%s\n",idToName ( getLocation ( gv, PLAYER_DRACULA )));
		getHistory ( gv, PLAYER_DRACULA, history );

		HISTORY_IS ( PLAYER_DRACULA, 0, TELEPORT );
		HISTORY_IS ( PLAYER_DRACULA, 1, HIDE );
		HISTORY_IS ( PLAYER_DRACULA, 2, DOUBLE_BACK_2 );
		HISTORY_IS ( PLAYER_DRACULA, 3, KLAUSENBURG );
		HISTORY_IS ( PLAYER_DRACULA, 4, CASTLE_DRACULA );
		HISTORY_IS ( PLAYER_DRACULA, 5, GALATZ );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "hv is no longer a valid allocation");
	} while ( 0 );
	// rail test: test with 9 combinations:
	// round = 0 - 3    name = PLAYER_LORD_GODALMING
	// round = 0         name = PLAYER_LORD_GODALMING  - Dracula
	PlayerMessage messages[50] = {"----"};
	do {
		//Trail
		char *trail = "";
		//Player's name
		PlayerID name = PLAYER_LORD_GODALMING;
		int round = 0;
		int score = GAME_START_SCORE;
		int HP = GAME_START_HUNTER_LIFE_POINTS;
		// current player's location
		LocationID myLoc = UNKNOWN_LOCATION;
		// current player's history trail
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 1;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[1] = { BORDEAUX };

		bool road = false;
		bool rail = true;
		bool sea = false;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );

	do {
		//Trail
		char *trail = "GBO....";
		//Player's name
		PlayerID name = PLAYER_DR_SEWARD;
		int round = 0;
		int score = GAME_START_SCORE;
		int HP = GAME_START_HUNTER_LIFE_POINTS;
		// current player's location
		LocationID myLoc = UNKNOWN_LOCATION;
		// current player's history trail
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 3;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[3] = { BORDEAUX, PARIS, SARAGOSSA };

		bool road = false;
		bool rail = true;
		bool sea = false;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );

	do {
		//Trail
		char *trail = "GBO.... SBO....";
		//Player's name
		PlayerID name = PLAYER_VAN_HELSING;
		int round = 0;
		int score = GAME_START_SCORE;
		int HP = GAME_START_HUNTER_LIFE_POINTS;
		// current player's location
		LocationID myLoc = UNKNOWN_LOCATION;
		// current player's history trail
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 8;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[8] = { BORDEAUX, PARIS, SARAGOSSA, LE_HAVRE,
		                            BRUSSELS, MARSEILLES, MADRID, BARCELONA
		                          };

		bool road = false;
		bool rail = true;
		bool sea = false;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );

	do {
		//Trail
		char *trail = "GBO.... SBO.... HBO....";
		//Player's name
		PlayerID name = PLAYER_MINA_HARKER;
		int round = 0;
		int score = GAME_START_SCORE;
		int HP = GAME_START_HUNTER_LIFE_POINTS;
		// current player's location
		LocationID myLoc = UNKNOWN_LOCATION;
		// current player's history trail
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 12;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[12] = { BORDEAUX, PARIS, SARAGOSSA, LE_HAVRE,
		                             BRUSSELS, MARSEILLES, MADRID, BARCELONA,
		                             COLOGNE, SANTANDER, ALICANTE, LISBON
		                           };

		bool road = false;
		bool rail = true;
		bool sea = false;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );

	do {
		//Trail
		char *trail = "GBO.... SBO.... HBO.... MBO....";
		//Player's name
		PlayerID name = PLAYER_DRACULA;
		int round = 0;
		int score = GAME_START_SCORE;
		int HP = GAME_START_BLOOD_POINTS;
		// current player's location
		LocationID myLoc = UNKNOWN_LOCATION;
		// current player's history trail
		LocationID locID[6] = { UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 1;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[1] = { BORDEAUX};

		bool road = false;
		bool rail = true;
		bool sea = false;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );

	do {
		//Trail
		char *trail = "GBO.... SBO.... HBO.... MBO.... DC?.V..";
		//Player's name
		PlayerID name = PLAYER_LORD_GODALMING;
		int round = 1;
		int score = GAME_START_SCORE - 1;
		int HP = GAME_START_HUNTER_LIFE_POINTS;
		// current player's location
		LocationID myLoc = BORDEAUX;
		// current player's history trail
		LocationID locID[6] = { BORDEAUX,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 3;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[3] = { BORDEAUX, PARIS, SARAGOSSA };

		bool road = false;
		bool rail = true;
		bool sea = false;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );

	do {
		//Trail
		char *trail = "GBO.... SBO.... HBO.... MBO.... DC?.V.."
		              "GBO.... SBO.... HBO.... MBO.... DC?T...";
		//Player's name
		PlayerID name = PLAYER_LORD_GODALMING;
		int round = 2;
		int score = GAME_START_SCORE - 2;
		int HP = GAME_START_HUNTER_LIFE_POINTS;
		// current player's location
		LocationID myLoc = BORDEAUX;
		// current player's history trail
		LocationID locID[6] = { BORDEAUX,
		                        BORDEAUX,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 8;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[8] = { BORDEAUX, PARIS, SARAGOSSA, LE_HAVRE,
		                            BRUSSELS, MARSEILLES, MADRID, BARCELONA
		                          };

		bool road = false;
		bool rail = true;
		bool sea = false;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );

	do {
		//Trail
		char *trail = "GBO.... SBO.... HBO.... MBO.... DC?.V.."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T...";
		//Player's name
		PlayerID name = PLAYER_LORD_GODALMING;
		int round = 3;
		int score = GAME_START_SCORE - 3;
		int HP = GAME_START_HUNTER_LIFE_POINTS;
		// current player's location
		LocationID myLoc = BORDEAUX;
		// current player's history trail
		LocationID locID[6] = { BORDEAUX,
		                        BORDEAUX,
		                        BORDEAUX,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 12;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[12] = { BORDEAUX, PARIS, SARAGOSSA, LE_HAVRE,
		                             BRUSSELS, MARSEILLES, MADRID, BARCELONA,
		                             COLOGNE, SANTANDER, ALICANTE, LISBON
		                           };

		bool road = false;
		bool rail = true;
		bool sea = false;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );
	//Dracula hp can over 40
	//G will be tp to hospital since he is killed before rest.
	do {
		//Trail
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.."// +10hp
		              "GKL.... SBO.... HBO.... MBO.... DC?T..."
		              "GKL.... SBO.... HBO.... MBO.... DD2T..."//+10hp
		              "GKL.... SBO.... HBO.... MBO.... DHIT..."//+10hp
		              "GKL.... SBO.... HBO.... MBO.... DC?T..."
		              "GKLTD.. SBO.... HBO.... MBO.... DC?T.B."
		              "GKLTD.. SBO.... HBO.... MBO.... DBDT.V.";//-13 score -6score
		//Player's name
		PlayerID name = PLAYER_LORD_GODALMING;
		int round = 7;
		int score = GAME_START_SCORE - 7 - 13 - 6;
		int HP = 0; //before the turn: haven't recover yet.
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
		// total number of connected location
		int numLoc = 1;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[1] = { KLAUSENBURG};
		bool road = false;
		bool rail = true;
		bool sea = true;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );
	// in same turn: hp = 0
	do {
		//Trail
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.."// +10hp
		              "GKL.... SBO.... HBO.... MBO.... DC?T..."
		              "GKL.... SBO.... HBO.... MBO.... DD2T..."//+10hp
		              "GKL.... SBO.... HBO.... MBO.... DHIT..."//+10hp
		              "GKL.... SBO.... HBO.... MBO.... DC?T..."
		              "GKLTD.. SBO.... HBO.... MBO.... DC?T.B."
		              "GKLTD..";
		//Player's name
		PlayerID name = PLAYER_DR_SEWARD;
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

		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "gv := newGameView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");

		testGeneralInfo ( gv,trail, name , round , score );
		testPlayerInfo ( gv, PLAYER_LORD_GODALMING, HP, myLoc, locID, numLoc, expectLoc,  road, rail, sea );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "hv is no longer a valid allocation");

	} while ( 0 );
	do {
		//hp > 40
		//Trail
		char *trail = "GCD.... SBO.... HBO.... MBO.... DCD.V.."// +10hp
		              "GCD.... SBO.... HBO.... MBO.... ";
		//Player's name
		PlayerID name = PLAYER_DRACULA;
		int round = 1;
		int score = GAME_START_SCORE - 1;
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

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );
	do {
		//encounter with 2 traps and V: V can not damage hunter.
		//Trail
		char *trail = "GCD.... SBO.... HBO.... MBO.... DC?.V.."
		              "GCD.... SBO.... HBO.... MBO.... DHIT..."
		              "GCD.... SBO.... HBO.... MBO.... DD1T..."
		              "GGA.... SBO.... HBO.... MBO.... DC?T..."
		              "GCNTTV. SBO.... HBO.... MBO.... DC?T...";
		//Player's name
		PlayerID name = PLAYER_LORD_GODALMING;
		int round = 5;
		int score = GAME_START_SCORE - 5;
		int HP = 5;
		// current player's location
		LocationID myLoc = CONSTANTA;
		// current player's history trail
		LocationID locID[6] = { CONSTANTA,
		                        GALATZ,
		                        CASTLE_DRACULA,
		                        CASTLE_DRACULA,
		                        CASTLE_DRACULA,
		                        UNKNOWN_LOCATION
		                      };
		// total number of connected location
		int numLoc = 5;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[5] = { CONSTANTA, GALATZ, BUCHAREST, VARNA, BLACK_SEA};
		bool road = true;
		bool rail = true;
		bool sea = true;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );

	do {
		//long Dracula history: trail with Hide, double back
		//Trail
		char *trail = "GBO.... SBO.... HBO.... MBO.... DCD.V.."// +10hp
		              "GBO.... SBO.... HBO.... MBO.... DHIT..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DC?T..."
		              "GBO.... SBO.... HBO.... MBO.... DD5T.V."
		              "GBO.... SBO.... HBO.... MBO.... DHIT..."
		              "GBO.... SBO.... HBO.... MBO.... ";
		//Player's name
		PlayerID name = PLAYER_DRACULA;
		int round = 8;
		int score = GAME_START_SCORE - 8 - 13;
		int HP = 80;
		// current player's location
		LocationID myLoc = HIDE;
		// current player's history trail
		LocationID locID[6] = { HIDE,
		                        DOUBLE_BACK_5,
		                        CITY_UNKNOWN,
		                        CITY_UNKNOWN,
		                        CITY_UNKNOWN,
		                        CITY_UNKNOWN
		                      };
		// total number of connected location
		int numLoc = 3;
		// list of connected location, the start location is in 0 index.
		LocationID expectLoc[3] = {CAGLIARI, MEDITERRANEAN_SEA, TYRRHENIAN_SEA};
		bool road = false;
		bool rail = false;
		bool sea = true;

		completeTest ( trail,messages,
		             name, round, score, HP,
		             myLoc, locID, //history
		             numLoc, expectLoc,
		             road, rail, sea );
	} while ( 0 );
	do {////////////////////////////////////////////////////////////////
		//If a location is connected with road and rail
		//the get connection still can get the city that connected with the location via rail.
		char *trail = "GMR.... SBO.... HCG.... MSA.... DC?.V.."
		              "GMR.... SBO.... HCG.... MSA.... ";
		PlayerMessage messages[] = {};
		GameView gv = newGameView ( trail, messages );

		TEST ( gv != NULL, "hv := newHunterView (\"%s\")", trail );
		TEST (! mem_address_is_poisoned ( gv ), "gv is a valid allocation");
		//                               Name        rounds       score
		testGeneralInfo ( gv, trail, PLAYER_DRACULA, 1 , GAME_START_SCORE - 1 );
		LocationID locID_3[6];
		LocationID expectLoc_3[11] = {BORDEAUX, NANTES, CLERMONT_FERRAND, TOULOUSE, SARAGOSSA, PARIS, LE_HAVRE, BRUSSELS, MARSEILLES, MADRID, BARCELONA};//maximum location number.
		int numLoc_3 = 11;
		//PLAYER_DR_SEWARD
		locID_3[0] =  BORDEAUX ;
		locID_3[1] =  BORDEAUX ;
		locID_3[2] =  UNKNOWN_LOCATION ;
		locID_3[3] =  UNKNOWN_LOCATION ;
		locID_3[4] =  UNKNOWN_LOCATION ;
		locID_3[5] =  UNKNOWN_LOCATION ;

		testPlayerInfo ( gv, PLAYER_DR_SEWARD, GAME_START_HUNTER_LIFE_POINTS, BORDEAUX,
		                locID_3, numLoc_3, expectLoc_3,  true, true, false );

		disposeGameView ( gv );
		TEST ( mem_address_is_poisoned ( gv ),
		      "hv is no longer a valid allocation");
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
