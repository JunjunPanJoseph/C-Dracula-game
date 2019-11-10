/*
 * //////////////////////////////////////////////////////////////////////
 * COMP2521 18x1 ... the Fury of Dracula
 * GameView.c: GameView ADT implementation
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
#include "Map.h" /* ... if you decide to use the Map ADT */
#include "structAndFun.h"
/*
 * structs define in struct And Fun.h, since they will be used in DracView and HunterView.
 * because the require of connectedLocations are different ( ignore trail / can't repeat with trail )
 * and the in GameView, it allow the return of C? S?
 * while for DracView, it must return real location
 * In this ADT, the functions only return 6 previous move
 * so it is impossible to determine the precise location of Dracula in DracView
 * Therefore GameView need get access of internal implementation of Struct.
 */
#define MAX_ROUNDS 366
/*
 * dracula live for 366 turns
 * i will record all the moves of all players
 * since 1: this information is provided by pastPlays
 *        2:  it can be used to deduce the next move of hunter
 */


/* get the string for this, and add the start to the start of next turn */
static char *getNextPlay ( char *pastPlays, int *start ) {
	char *tmp = malloc ( 8 * sizeof ( char ) );
	tmp[7] = '\0';
	int success = 1;
	while ( pastPlays[*start] != '\0' && success ) {
		if ( pastPlays[*start] == 'G' ||
		        pastPlays[*start] == 'S' ||
		        pastPlays[*start] == 'H' ||
		        pastPlays[*start] == 'M' ||
		        pastPlays[*start] == 'D' ) {
			success = 0;
			for ( int i = 0; i < 7; i++ ) {
				tmp[i]	= pastPlays[*start];
				*start	= (*start ) + 1;
			}
			continue;
		}

		*start = (*start ) + 1;
	}
	/* if can't find data completely: returnNULL */
	if ( success == 1 ) {
		free ( tmp );
		return ( NULL );
	}
	return ( tmp );
}


/* initialize the view. */
static GameView initializeGameView ( PlayerMessage *messages ) {
	GameView new = malloc ( sizeof *new );
	if ( new == NULL )
		err ( EX_OSERR, "couldn't allocate GameView" );
	new->points	= GAME_START_SCORE;
	new->turns	= 0;
	new->rounds	= 0;
	new->messages	= messages;
	new->bloodG	= GAME_START_HUNTER_LIFE_POINTS;
	new->bloodS	= GAME_START_HUNTER_LIFE_POINTS;
	new->bloodH	= GAME_START_HUNTER_LIFE_POINTS;
	new->bloodM	= GAME_START_HUNTER_LIFE_POINTS;
	new->bloodD	= GAME_START_BLOOD_POINTS;
	new->europe	= newMap ();
	for ( int i = 0; i < NUM_MAP_LOCATIONS; i++ ) {
		new->Encounters[i].Traps	= 0;
		new->Encounters[i].Vampire	= 0;
	}
	new->currG		= UNKNOWN_LOCATION;
	new->locationG[0]	= UNKNOWN_LOCATION;
	new->currS		= UNKNOWN_LOCATION;
	new->locationS[0]	= UNKNOWN_LOCATION;
	new->currH		= UNKNOWN_LOCATION;
	new->locationH[0]	= UNKNOWN_LOCATION;
	new->currM		= UNKNOWN_LOCATION;
	new->locationM[0]	= UNKNOWN_LOCATION;
	new->currD		= UNKNOWN_LOCATION;
	new->locationD[0]	= UNKNOWN_LOCATION;
	return ( new );
}


/*
 * if this player is resting in last movement - return 1
 * else or he is Dracula - return 0
 * use for AI. so won't be tested in this stage.
 */
int rest ( GameView g, PlayerID id ) {
	switch ( id ) {
	case PLAYER_LORD_GODALMING:
		if ( g->turns % 5 >= 1 ) {
			/* get this round situation. */
			if ( g->rounds == 0 ) {
				return ( 0 );
			} else {
				return ( g->locationG[g->rounds] == g->currG &&
				       g->currG == g->locationG[g->rounds - 1]);
			}
		} else {
			/* get last round situation. */
			if ( g->rounds == 0 ) {
				/* this play's turn haven't start yet */
				return ( 0 );
			} else if ( g->rounds == 1 ) {
				return ( g->locationG[0] == UNKNOWN_LOCATION &&
				       g->currG == UNKNOWN_LOCATION );
			} else {
				return ( g->locationG[g->rounds - 1] == g->currG &&
				       g->currG == g->locationG[g->rounds - 2]);
			}
		}
		break;
	case PLAYER_DR_SEWARD:
		if ( g->turns % 5 >= 2 ) {
			/* get this round situation. */
			if ( g->rounds == 0 ) {
				return ( 0 );
			} else {
				return ( g->locationS[g->rounds] == g->currS &&
				       g->currS == g->locationS[g->rounds - 1]);
			}
		} else {
			/* get last round situation. */
			if ( g->rounds == 0 ) {
				/* this play's turn haven't start yet */
				return ( 0 );
			} else if ( g->rounds == 1 ) {
				return ( g->locationS[0] == UNKNOWN_LOCATION &&
				       g->currS == UNKNOWN_LOCATION );
			} else {
				return ( g->locationS[g->rounds - 1] == g->currS &&
				       g->currS == g->locationS[g->rounds - 2]);
			}
		}
		break;
	case PLAYER_VAN_HELSING:
		if ( g->turns % 5 >= 3 ) {
			/* get this round situation. */
			if ( g->rounds == 0 ) {
				return ( 0 );
			} else {
				return ( g->locationH[g->rounds] == g->currH &&
				       g->currH == g->locationH[g->rounds - 1]);
			}
		} else {
			/* get last round situation. */
			if ( g->rounds == 0 ) {
				/* this play's turn haven't start yet */
				return ( 0 );
			} else if ( g->rounds == 1 ) {
				return ( g->locationH[0] == UNKNOWN_LOCATION &&
				       g->currH == UNKNOWN_LOCATION );
			} else {
				return ( g->locationH[g->rounds - 1] == g->currH &&
				       g->currH == g->locationH[g->rounds - 2]);
			}
		}
		break;
	case PLAYER_MINA_HARKER:
		if ( g->turns % 5 >= 4 ) {
			/* get this round situation. */
			if ( g->rounds == 0 ) {
				return ( 0 );
			} else {
				return ( g->locationM[g->rounds] == g->currM &&
				       g->currM == g->locationM[g->rounds - 1]);
			}
		} else {
			/* get last round situation. */
			if ( g->rounds == 0 ) {
				/* this play's turn haven't start yet */
				return ( 0 );
			} else if ( g->rounds == 1 ) {
				return ( g->locationM[0] == UNKNOWN_LOCATION &&
				       g->currM == UNKNOWN_LOCATION );
			} else {
				return ( g->locationM[g->rounds - 1] == g->currM &&
				       g->currM == g->locationM[g->rounds - 2]);
			}
		}
		break;
	default:
		return ( 0 );
	}
}


/*
 * get the true location: if current location is HIDE / back
 *                              it will return the real, valid location, except TP.
 * because in GameView and HunterView, we are required to return TP as an move.
 * but in DracView, We must display CASTLE_DRACULA --although they are the same.
 */
LocationID trueLocation ( LocationID *tmp ) {
	if ( validPlace ( *tmp ) )
		return (*tmp );

	if ( *tmp == HIDE )
		return ( trueLocation ( tmp - 1 ) );
	if ( *tmp == DOUBLE_BACK_1 )
		return ( trueLocation ( tmp - 1 ) );
	if ( *tmp == DOUBLE_BACK_2 )
		return ( trueLocation ( tmp - 2 ) );
	if ( *tmp == DOUBLE_BACK_3 )
		return ( trueLocation ( tmp - 3 ) );
	if ( *tmp == DOUBLE_BACK_4 )
		return ( trueLocation ( tmp - 4 ) );
	if ( *tmp == DOUBLE_BACK_5 )
		return ( trueLocation ( tmp - 5 ) );
	return (*tmp );
}


/* use string to update the GameView. */
int updateGameView ( GameView g, char *tmp ) {
	char tmpLocation[3];
	tmpLocation[2] = '\0';
	LocationID tmpLocationID;
	tmpLocation[0]	= tmp[1];
	tmpLocation[1]	= tmp[2];
	/* get the id. */
	tmpLocationID = abbrevToID ( tmpLocation );
	/*
	 * printf ("id = %d  name: %s\n", tmpLocationID, tmpLocation );
	 * printf ("str:  %s ID %d\n",tmp,tmpLocationID );
	 */
	LocationID	trueLoc;
	int		i;
	if ( tmp[0] == 'D' ) {
		/*
		 * dracula
		 * if trap is placed:
		 * because all of the input data is correct
		 * don need storage the encounters in a queue and check when they are droped
		 */
		if ( g->rounds >= 6 ) {
			if ( tmp[5] == 'M' && validPlace ( trueLocation ( &g->locationD[g->rounds - 6] ) ) ) {
				g->Encounters[trueLocation ( &g->locationD[g->rounds - 6] )].Traps--;
			}
			if ( tmp[5] == 'V' && validPlace ( trueLocation ( &g->locationD[g->rounds - 6] ) ) ) {
				g->Encounters[trueLocation ( &g->locationD[g->rounds - 6] )].Vampire--;
				g->points -= SCORE_LOSS_VAMPIRE_MATURES;
			}
		}
		if ( validPlace ( tmpLocationID ) ) {
			/* since all data are valid, it will never put encounters in sea or other invalid location. */
			if ( tmp[3] == 'T' ) {
				g->Encounters[tmpLocationID].Traps++;
			}
			if ( tmp[4] == 'V' ) {
				g->Encounters[tmpLocationID].Vampire++;
			}
		}

		g->points -= SCORE_LOSS_DRACULA_TURN;
		/* change location */
		if ( validPlace ( tmpLocationID ) ) {
			if ( idToType ( tmpLocationID ) == SEA ) {
				g->bloodD			-= LIFE_LOSS_SEA;
				g->locationD[g->rounds++]	= tmpLocationID;
				g->currD			= tmpLocationID;
			} else if ( tmpLocationID == CASTLE_DRACULA ) {
				g->bloodD			+= LIFE_GAIN_CASTLE_DRACULA;
				g->locationD[g->rounds++]	= CASTLE_DRACULA;
				g->currD			= CASTLE_DRACULA;
			} else {
				g->locationD[g->rounds++]	= tmpLocationID;
				g->currD			= tmpLocationID;
			}
		} else {
			if ( tmpLocation[0] == 'T' && tmpLocation[1] == 'P' ) {
				g->bloodD += LIFE_GAIN_CASTLE_DRACULA;
				/*
				 * g->locationD[g->rounds++] = CASTLE_DRACULA;
				 * g->currD = CASTLE_DRACULA;
				 */
				g->locationD[g->rounds++]	= TELEPORT;
				g->currD			= TELEPORT;
				if ( tmp[3] == 'T' ) {
					g->Encounters[CASTLE_DRACULA].Traps++;
				}
				if ( tmp[4] == 'V' ) {
					g->Encounters[CASTLE_DRACULA].Vampire++;
				}
			} else if ( tmpLocation[0] == 'S' && tmpLocation[1] == '?' ) {
				g->bloodD			-= LIFE_LOSS_SEA;
				g->locationD[g->rounds++]	= SEA_UNKNOWN;
				g->currD			= SEA_UNKNOWN;
			} else if ( tmpLocation[0] == 'C' && tmpLocation[1] == '?' ) {
				g->locationD[g->rounds++]	= CITY_UNKNOWN;
				g->currD			= CITY_UNKNOWN;
			} else if ( tmpLocation[0] == 'H' && tmpLocation[1] == 'I' ) {
				/* hide */
				trueLoc = trueLocation ( &( g->locationD[g->rounds - 1]) );
				if ( validPlace ( trueLoc ) ) {
					if ( tmp[3] == 'T' ) {
						g->Encounters[trueLoc].Traps++;
					}
					if ( tmp[4] == 'V' ) {
						g->Encounters[trueLoc].Vampire++;
					}
				}
				g->locationD[g->rounds++]	= HIDE;
				g->currD			= HIDE;
				/* consider the real Location: get healing or damage */
				if ( trueLoc == CASTLE_DRACULA || trueLoc == TELEPORT ) {
					g->bloodD += LIFE_GAIN_CASTLE_DRACULA;
				} else if ( trueLoc == SEA_UNKNOWN || ( validPlace ( trueLoc ) && idToType ( trueLoc ) == SEA ) ) {
					g->bloodD -= LIFE_LOSS_SEA;
				}
			} else {
				/* case for double back. */
				switch ( tmpLocation[1] ) {
				case '1':
					g->locationD[g->rounds++]	= DOUBLE_BACK_1;
					g->currD			= DOUBLE_BACK_1;
					break;
				case '2':
					g->locationD[g->rounds++]	= DOUBLE_BACK_2;
					g->currD			= DOUBLE_BACK_2;
					break;
				case '3':
					g->locationD[g->rounds++]	= DOUBLE_BACK_3;
					g->currD			= DOUBLE_BACK_3;
					break;
				case '4':
					g->locationD[g->rounds++]	= DOUBLE_BACK_4;
					g->currD			= DOUBLE_BACK_4;
					break;
				case '5':
					g->locationD[g->rounds++]	= DOUBLE_BACK_5;
					g->currD			= DOUBLE_BACK_5;
					break;
				}
				trueLoc = trueLocation ( &( g->locationD[g->rounds - 1]) );
				if ( validPlace ( trueLoc ) ) {
					if ( tmp[3] == 'T' ) {
						g->Encounters[trueLoc].Traps++;
					}
					if ( tmp[4] == 'V' ) {
						g->Encounters[trueLoc].Vampire++;
					}
				}
				if ( trueLoc == CASTLE_DRACULA ) {
					g->bloodD += LIFE_GAIN_CASTLE_DRACULA;
				} else if ( trueLoc == SEA_UNKNOWN || ( validPlace ( trueLoc ) && idToType ( trueLoc ) == SEA ) ) {
					g->bloodD -= LIFE_LOSS_SEA;
				}
			}
		}
		g->turns++;
	} else {
		switch ( tmp[0] ) {
		case 'G':
			/* if start the turn in hospital - recover full health */
			if ( g->currG == ST_JOSEPH_AND_ST_MARYS ) {
				g->bloodG = 9;
			}
			g->locationG[g->rounds] = tmpLocationID;
			g->currG		= tmpLocationID;
			for ( i = 3; i < 7 && tmp[i] != '.'; i++ ) {
				if ( tmp[i] == 'T' ) {
					g->Encounters[tmpLocationID].Traps--;
					g->bloodG -= LIFE_LOSS_TRAP_ENCOUNTER;
				} else if ( tmp[i] == 'V' ) {
					g->Encounters[tmpLocationID].Vampire--;
				} else if ( tmp[i] == 'D' ) {
					g->bloodG	-= LIFE_LOSS_DRACULA_ENCOUNTER;
					g->bloodD	-= LIFE_LOSS_HUNTER_ENCOUNTER;
					/*
					 * g->currD = tmpLocationID;
					 * g->locationD[g->rounds - 1] = tmpLocationID;
					 */
				}
				/* tp to the hospital */
				if ( g->bloodG <= 0 ) {
					g->points	-= SCORE_LOSS_HUNTER_HOSPITAL;
					g->currG	= ST_JOSEPH_AND_ST_MARYS;
					break;
				}
			}
			/* Healing */
			if ( rest ( g, PLAYER_LORD_GODALMING ) ) {
				g->bloodG += LIFE_GAIN_REST;
				if ( g->bloodG > 9 ) {
					g->bloodG = 9;
				}
			}
			break;
		case 'S':
			if ( g->currS == ST_JOSEPH_AND_ST_MARYS ) {
				g->bloodS = 9;
			}
			g->locationS[g->rounds] = tmpLocationID;
			g->currS		= tmpLocationID;
			for ( i = 3; i < 7 && tmp[i] != '.'; i++ ) {
				if ( tmp[i] == 'T' ) {
					g->Encounters[tmpLocationID].Traps--;
					g->bloodS -= LIFE_LOSS_TRAP_ENCOUNTER;
				} else if ( tmp[i] == 'V' ) {
					g->Encounters[tmpLocationID].Vampire--;
				} else if ( tmp[i] == 'D' ) {
					g->bloodS	-= LIFE_LOSS_DRACULA_ENCOUNTER;
					g->bloodD	-= LIFE_LOSS_HUNTER_ENCOUNTER;
					/*
					 * g->currD = tmpLocationID;
					 * g->locationD[g->rounds - 1] = tmpLocationID;
					 */
				}
				/* tp to the hospital */
				if ( g->bloodS <= 0 ) {
					g->points	-= SCORE_LOSS_HUNTER_HOSPITAL;
					g->currS	= ST_JOSEPH_AND_ST_MARYS;
					break;
				}
			}
			/* Healing */
			if ( rest ( g, PLAYER_DR_SEWARD ) ) {
				g->bloodS += LIFE_GAIN_REST;
				if ( g->bloodS > 9 ) {
					g->bloodS = 9;
				}
			}
			break;
		case 'H':
			if ( g->currH == ST_JOSEPH_AND_ST_MARYS ) {
				g->bloodH = 9;
			}
			g->locationH[g->rounds] = tmpLocationID;
			g->currH		= tmpLocationID;
			for ( i = 3; i < 7 && tmp[i] != '.'; i++ ) {
				if ( tmp[i] == 'T' ) {
					g->Encounters[tmpLocationID].Traps--;
					g->bloodH -= LIFE_LOSS_TRAP_ENCOUNTER;
				} else if ( tmp[i] == 'V' ) {
					g->Encounters[tmpLocationID].Vampire--;
				} else if ( tmp[i] == 'D' ) {
					g->bloodH	-= LIFE_LOSS_DRACULA_ENCOUNTER;
					g->bloodD	-= LIFE_LOSS_HUNTER_ENCOUNTER;
					/*
					 * g->currD = tmpLocationID;
					 * g->locationD[g->rounds - 1] = tmpLocationID;
					 */
				}
				/* tp to the hospital */
				if ( g->bloodH <= 0 ) {
					g->points	-= SCORE_LOSS_HUNTER_HOSPITAL;
					g->currH	= ST_JOSEPH_AND_ST_MARYS;
					break;
				}
			}
			/* Healing */
			if ( rest ( g, PLAYER_VAN_HELSING ) ) {
				g->bloodH += LIFE_GAIN_REST;
				if ( g->bloodH > 9 ) {
					g->bloodH = 9;
				}
			}
			break;
		case 'M':
			if ( g->currM == ST_JOSEPH_AND_ST_MARYS ) {
				g->bloodM = 9;
			}
			g->locationM[g->rounds] = tmpLocationID;
			g->currM		= tmpLocationID;
			for ( i = 3; i < 7 && tmp[i] != '.'; i++ ) {
				if ( tmp[i] == 'T' ) {
					g->Encounters[tmpLocationID].Traps--;
					g->bloodM -= LIFE_LOSS_TRAP_ENCOUNTER;
				} else if ( tmp[i] == 'V' ) {
					g->Encounters[tmpLocationID].Vampire--;
				} else if ( tmp[i] == 'D' ) {
					g->bloodM	-= LIFE_LOSS_DRACULA_ENCOUNTER;
					g->bloodD	-= LIFE_LOSS_HUNTER_ENCOUNTER;
					/*
					 * g->currD = tmpLocationID;
					 * g->locationD[g->rounds - 1] = tmpLocationID;
					 */
				}
				/* tp to the hospital */
				if ( g->bloodM <= 0 ) {
					g->points	-= SCORE_LOSS_HUNTER_HOSPITAL;
					g->currM	= ST_JOSEPH_AND_ST_MARYS;
					break;
				}
			}
			/* Healing */
			if ( rest ( g, PLAYER_MINA_HARKER ) ) {
				g->bloodM += LIFE_GAIN_REST;
				if ( g->bloodM > 9 ) {
					g->bloodM = 9;
				}
			}
			break;
		default:
			err ( EX_OSERR, "Invalid name of player" );
		}

		g->turns++;
	}
	return ( 1 );
}


/* Creates a new GameView to summarize the current state of the game */
GameView newGameView ( char *pastPlays, PlayerMessage messages[] ) {
	/* before turn 0: initialize all variables. */
	GameView	new	= initializeGameView ( messages );
	int		start	= 0;                                    /* the start index for search in pastPlays */
	char		*tmp	= getNextPlay ( pastPlays, &start );     /* string from pastPlays; */
	while ( tmp != NULL ) {
		updateGameView ( new, tmp );
		free ( tmp );
		tmp = getNextPlay ( pastPlays, &start );
	}
	return ( new );
}


/* Frees all memory previously allocated for the GameView toBeDeleted */
void disposeGameView ( GameView toBeDeleted ) {
	/* COMPLETE THIS IMPLEMENTATION */
	if ( toBeDeleted == NULL )
		return;
	disposeMap ( toBeDeleted->europe );
	free ( toBeDeleted );
}


/* // Functions to return simple information about the current state of the game */

/* Get the current round */
Round getRound ( GameView gv ) {
	if ( gv == NULL )
		return ( 0 );
	return ( gv->rounds );
}


/* Get the id of current player - ie whose turn is it? */
PlayerID getCurrentPlayer ( GameView gv ) {
	int i = gv->turns % 5;
	if ( i == 0 )
		return ( PLAYER_LORD_GODALMING );
	if ( i == 1 )
		return ( PLAYER_DR_SEWARD );
	if ( i == 2 )
		return ( PLAYER_VAN_HELSING );
	if ( i == 3 )
		return ( PLAYER_MINA_HARKER );
	if ( i == 4 )
		return ( PLAYER_DRACULA );
	return (-1 );
}


/* Get the current score */
int getScore ( GameView gv ) {
	if ( gv == NULL ) {
		return ( 366 );
	}
	return ( gv->points );
}


/* return  1 if player have not move in this round */
int haveNotMoved ( GameView gv, PlayerID player ) {
	/* printf ("gv->turns  5 = %d\n",gv->turns%5 ); */
	if ( player == PLAYER_LORD_GODALMING )
		return ( gv->turns % 5 == 0 );
	if ( player == PLAYER_DR_SEWARD )
		return ( gv->turns % 5 <= 1 );
	if ( player == PLAYER_VAN_HELSING )
		return ( gv->turns % 5 <= 2 );
	if ( player == PLAYER_MINA_HARKER )
		return ( gv->turns % 5 <= 3 );
	if ( player == PLAYER_DRACULA )
		return ( 1 );
	return (-1 );
}


/* Get the current health points for a given player */
int getHealth ( GameView gv, PlayerID player ) {
	/*
	 * hp < 0 is not allowed.
	 * if ( player haven't moved this round )
	 */
	if ( haveNotMoved ( gv, player ) ) {
		if ( player == PLAYER_LORD_GODALMING )
			return ( ( gv->bloodG >= 0 ) ? gv->bloodG : 9 );
		if ( player == PLAYER_DR_SEWARD )
			return ( ( gv->bloodS >= 0 ) ? gv->bloodS : 9 );
		if ( player == PLAYER_VAN_HELSING )
			return ( ( gv->bloodH >= 0 ) ? gv->bloodH : 9 );
		if ( player == PLAYER_MINA_HARKER )
			return ( ( gv->bloodM >= 0 ) ? gv->bloodM : 9 );
		if ( player == PLAYER_DRACULA )
			return ( ( gv->bloodD >= 0 ) ? gv->bloodD : 9 );
	}
	if ( player == PLAYER_LORD_GODALMING )
		return ( ( gv->bloodG >= 0 ) ? gv->bloodG : 0 );
	if ( player == PLAYER_DR_SEWARD )
		return ( ( gv->bloodS >= 0 ) ? gv->bloodS : 0 );
	if ( player == PLAYER_VAN_HELSING )
		return ( ( gv->bloodH >= 0 ) ? gv->bloodH : 0 );
	if ( player == PLAYER_MINA_HARKER )
		return ( ( gv->bloodM >= 0 ) ? gv->bloodM : 0 );
	if ( player == PLAYER_DRACULA )
		return ( ( gv->bloodD >= 0 ) ? gv->bloodD : 0 );
	return (-1 );
}


/* Get the current location id of a given player */
LocationID getLocation ( GameView gv, PlayerID player ) {
	if ( player == PLAYER_LORD_GODALMING )
		return ( gv->currG );
	if ( player == PLAYER_DR_SEWARD )
		return ( gv->currS );
	if ( player == PLAYER_VAN_HELSING )
		return ( gv->currH );
	if ( player == PLAYER_MINA_HARKER )
		return ( gv->currM );
	if ( player == PLAYER_DRACULA )
		return ( gv->currD );
	return (-1 );
}


/* // Functions that return information about the history of the game */

/* Fills the trail array with the location ids of the last 6 turns */
void getHistory ( GameView gv, PlayerID player, LocationID trail[TRAIL_SIZE] ) {
	/* REPLACE THIS WITH YOUR OWN IMPLEMENTATION */
	if ( gv == NULL ) {
		return;
	}
	int index = gv->rounds;
	/* printf ("round = %d   turn = %d\n",gv->rounds, gv->turns ); */
	switch ( player ) {
	case PLAYER_LORD_GODALMING:
		/*
		 * whether this player's turn have finished in this round?
		 * the same below.
		 */
		index = index - ( gv->turns % 5 < 1 );
		for ( int i = 0; i < TRAIL_SIZE; i++ ) {
			if ( index >= 0 ) {
				trail[i] = gv->locationG[index--];
			} else {
				trail[i] = UNKNOWN_LOCATION;
			}
		}

		break;
	case PLAYER_DR_SEWARD:
		index = index - ( gv->turns % 5 < 2 );
		for ( int i = 0; i < TRAIL_SIZE; i++ ) {
			if ( index >= 0 ) {
				trail[i] = gv->locationS[index--];
			} else {
				trail[i] = UNKNOWN_LOCATION;
			}
		}
		break;
	case PLAYER_VAN_HELSING:
		index = index - ( gv->turns % 5 < 3 );
		for ( int i = 0; i < TRAIL_SIZE; i++ ) {
			if ( index >= 0 ) {
				trail[i] = gv->locationH[index--];
			} else {
				trail[i] = UNKNOWN_LOCATION;
			}
		}
		break;
	case PLAYER_MINA_HARKER:
		index = index - ( gv->turns % 5 < 4 );
		for ( int i = 0; i < TRAIL_SIZE; i++ ) {
			if ( index >= 0 ) {
				trail[i] = gv->locationM[index--];
			} else {
				trail[i] = UNKNOWN_LOCATION;
			}
		}
		break;
	case PLAYER_DRACULA:
		index = index - 1;
		for ( int i = 0; i < TRAIL_SIZE; i++ ) {
			if ( index >= 0 ) {
				trail[i] = gv->locationD[index--];
			} else {
				trail[i] = UNKNOWN_LOCATION;
			}
		}
		break;
	default:
		return;
	}
}


/*
 * search the loc in the array. Since usually there are only 6 elements in the array
 * I won't use BST to make things become more complex.
 */
int inArray ( LocationID loc, LocationID *result, int index ) {
	for ( int i = 0; i < index; i++ ) {
		if ( result[i] == loc )
			return ( 1 );
	}
	return ( 0 );
}


/* // Functions that query the map to find information about connectivity */

/* Returns an array of LocationIDs for all directly connected locations */

LocationID * connectedLocations ( GameView gv, int *numLocations,
                                 LocationID from, PlayerID player, Round round,
                                 bool road, bool rail, bool sea ) {
	LocationID tmpfrom = from;
	*numLocations = 0;
	int dracula = ( player == PLAYER_DRACULA );
	/* in the location of Dracula is unknown */
	if ( dracula && gv->rounds != 0 && !validPlace ( tmpfrom ) ) {
		/* try to get true location. */

		tmpfrom = trueLocation ( &( gv->locationD[gv->rounds - 1]) );
		if ( tmpfrom == TELEPORT ) {
			tmpfrom = CASTLE_DRACULA;
		}
		if ( !validPlace ( tmpfrom ) ) {
			return ( NULL );
		}
	}
	int i = 0;
	if ( gv == NULL ) {
		return ( NULL );
	}
	LocationID	tmp[NUM_MAP_LOCATIONS];
	Map		tmpMap = gv->europe;
	if ( tmpMap == NULL ) {
		return ( NULL );
	}
	VList tmpNode;
	/* always include start location..... even if player is Dracula. */
	tmp[0] = tmpfrom;
	i++;
	*numLocations = *numLocations + 1;

	tmpNode = tmpMap->connections[tmpfrom];
	while ( tmpNode != NULL ) {
		if ( ( road && tmpNode->type == ROAD ) ||
		        ( sea && tmpNode->type == BOAT ) ) {
			/* Dracula can not  move to hospital. */
			if ( !( ( tmpNode->v == ST_JOSEPH_AND_ST_MARYS ) && dracula ) ) {
				tmp[i++] = tmpNode->v;
				(*numLocations )++;
			}
		}
		tmpNode = tmpNode->next;
	}
	int distance = ( round + player ) % 4;
	/* distance != 0 && isn't Dracula */
	if ( rail && !dracula && distance ) {
		searchRail ( tmpMap, tmpfrom, distance, numLocations, &i, tmp );
	}
	/* make suitable array for return. */
	LocationID *returnList = malloc ( sizeof ( LocationID ) * (*numLocations ) );
	for ( i = 0; i < *numLocations; i++ ) {
		returnList[i] = tmp[i];
	}
	return ( returnList );
}


/* recursively search available location in rail. DFS. */
void searchRail ( Map map, LocationID loc, int depth,
                 int *nLocations, int *index, LocationID *result ) {
	VList tmp = map->connections[loc];
	while ( depth && tmp != NULL ) { /* depth != 0 */
		if ( tmp->type == RAIL ) {
			if ( !( inArray ( tmp->v, result, *index ) ) ) {
				result[(*index )++]	= tmp->v;
				*( nLocations )		= *( nLocations ) + 1;
			}
			/* printf ("find: %s\n",idToAbbrev ( tmp->v )); */
			searchRail ( map, tmp->v, depth - 1, nLocations, index, result );
		}
		tmp = tmp->next;
	}
}


