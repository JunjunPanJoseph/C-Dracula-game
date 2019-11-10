#define MAX_ROUNDS 366
#include "Map.h"
struct encounters {
	int Traps;
	int Vampire;
};
struct gameView {
	//game information
	int points;
	int turns;
	int rounds;
	PlayerMessage *messages;
	//blood & location
	int bloodG;
	LocationID locationG[MAX_ROUNDS];
	LocationID currG;

	int bloodS;
	LocationID locationS[MAX_ROUNDS];
	LocationID currS;

	int bloodH;
	LocationID locationH[MAX_ROUNDS];
	LocationID currH;

	int bloodM;
	LocationID locationM[MAX_ROUNDS];
	LocationID currM;

	int bloodD;
	LocationID locationD[MAX_ROUNDS];
	LocationID currD;

	Map europe;
	struct encounters Encounters[NUM_MAP_LOCATIONS];
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
};

void searchRail ( Map map, LocationID loc, int depth,int *nLocations, int *index, LocationID *result );

//if this player is resting in last movement - return 1
//else or he is Dracula - return 0
int rest ( GameView g, PlayerID id );
LocationID trueLocation ( LocationID *tmp );
int updateGameView ( GameView g, char *tmp );
int inArray ( LocationID loc, LocationID *result, int index );
int haveNotMoved ( GameView gv, PlayerID player );
