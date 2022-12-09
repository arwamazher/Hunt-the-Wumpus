/* ---------------------------------------------
Program 2: Wumpus with Superbats and Arrows
Description: This program prompts the user to navigate through a cave with obstacles in order to guess where the Wumpus monster is.

Course: CS 211, Fall 2022. Thurs 10am lab
System: Windows using Replit
Author: Arwa Mazher
---------------------------------------------
*/
#include <ctype.h>   // for toupper()
#include <stdbool.h> // for the bool type in C
#include <stdio.h>
#include <stdlib.h> // for srand

// global constants
#define MAX_LINE_LENGTH 81
#define NUMBER_OF_ROOMS 20

// Dynamic array for rooms
int **Rooms;

// Used to more conveniently pass all game information between functions.
struct GameInfo {
  int moveNumber; // Counts up from 1, incrementing for each move
  int personRoom; // Room 1..20 the person currently is in
  int wumpusRoom; // Room 1..20 the Wumpus is in
  int pitRoom1;   // Room 1..20 the first pit is in
  int pitRoom2;   // Room 1..20 the second pit is in
  int batRoom1;
  int batRoom2;
  int arrowRoom;
};

// Function prototype needed to allow calls in any order between functions checkForHazards() and inRoomWithBats()
void checkForHazards(struct GameInfo *theGameInfo, bool *personIsAlive, int randomNumbers[]);

//--------------------------------------------------------------------------------
void displayCave() {
  printf("\n"
         "       ______18______             \n"
         "      /      |       \\           \n"
         "     /      _9__      \\          \n"
         "    /      /    \\      \\        \n"
         "   /      /      \\      \\       \n"
         "  17     8        10     19       \n"
         "  | \\   / \\      /  \\   / |    \n"
         "  |  \\ /   \\    /    \\ /  |    \n"
         "  |   7     1---2     11  |       \n"
         "  |   |    /     \\    |   |      \n"
         "  |   6----5     3---12   |       \n"
         "  |   |     \\   /     |   |      \n"
         "  |   \\       4      /    |      \n"
         "  |    \\      |     /     |      \n"
         "  \\     15---14---13     /       \n"
         "   \\   /            \\   /       \n"
         "    \\ /              \\ /        \n"
         "    16---------------20           \n"
         "\n");
}

//--------------------------------------------------------------------------------
void displayInstructions()
{
    printf( "Hunt the Wumpus:                                             \n"
    		"The Wumpus lives in a completely dark cave of 20 rooms. Each \n"
    		"room has 3 tunnels leading to other rooms.                   \n"
    		"                                                             \n"
    		"Hazards:                                                     \n"
        	"1. Two rooms have bottomless pits in them.  If you go there you fall and die.   \n"
        	"2. Two other rooms have super-bats.  If you go there, the bats grab you and     \n"
        	"   fly you to some random room, which could be troublesome.  Then those bats go \n"
        	"   to a new room different from where they came from and from the other bats.   \n"
    		"3. The Wumpus is not bothered by the pits or bats, as he has sucker feet and    \n"
        	"   is too heavy for bats to lift.  Usually he is asleep.  Two things wake       \n"
        	"    him up: Anytime you shoot an arrow, or you entering his room.  The Wumpus   \n"
        	"    will move into the lowest-numbered adjacent room anytime you shoot an arrow.\n"
        	"    When you move into the Wumpus' room, then he wakes and moves if he is in an \n"
        	"    odd-numbered room, but stays still otherwise.  After that, if he is in your \n"
        	"    room, he snaps your neck and you die!                                       \n"
        	"                                                                                \n"
        	"Moves:                                                                          \n"
        	"On each move you can do the following, where input can be upper or lower-case:  \n"
        	"1. Move into an adjacent room.  To move enter 'M' followed by a space and       \n"
        	"   then a room number.                                                          \n"
        	"2. Shoot your guided arrow through a list of up to three adjacent rooms, which  \n"
        	"   you specify.  Your arrow ends up in the final room.                          \n"
        	"   To shoot enter 'S' followed by the number of rooms (1..3), and then the      \n"
        	"   list of the desired number (up to 3) of adjacent room numbers, separated     \n"
        	"   by spaces. If an arrow can't go a direction because there is no connecting   \n"
        	"   tunnel, it ricochets and moves to the lowest-numbered adjacent room and      \n"
        	"   continues doing this until it has traveled the designated number of rooms.   \n"
        	"   If the arrow hits the Wumpus, you win! If the arrow hits you, you lose. You  \n"
        	"   automatically pick up the arrow if you go through a room with the arrow in   \n"
        	"   it.                                                                          \n"
        	"3. Enter 'R' to reset the person and hazard locations, useful for testing.      \n"
        	"4. Enter 'C' to cheat and display current board positions.                      \n"
        	"5. Enter 'D' to display this set of instructions.                               \n"
        	"6. Enter 'P' to print the maze room layout.                                     \n"
        	"7. Enter 'X' to exit the game.                                                  \n"
        	"                                                                                \n"
        	"Good luck!                                                                      \n"
        	" \n\n");
}//end displayInstructions()

//--------------------------------------------------------------------------------
// Return true if randomValue is already in array
int alreadyFound(int randomValue,     // New number we're checking
                 int randomNumbers[], // Set of numbers previously found
                 int limit)           // How many numbers previously found
{
  int returnValue = false;

  // compare random value against all previously found values
  for (int i = 0; i < limit; i++) {
    if (randomValue == randomNumbers[i]) {
      returnValue = true; // It is already there
      break;
    }
  }

  return returnValue;
}

//--------------------------------------------------------------------------------
// Fill this array with unique random integers 1..20
void setUniqueValues(int randomNumbers[], // Array of random numbers
                     int size)            // Size of random numbers array
{
  int randomValue = -1;

  for (int i = 0; i < size; i++) {
    do {
      randomValue = rand() % NUMBER_OF_ROOMS + 1; // random number 1..20
    } while (alreadyFound(randomValue, randomNumbers, i));
    randomNumbers[i] = randomValue;
  }
}

//--------------------------------------------------------------------------------
// Set the Wumpus, player, bats and pits in distinct random rooms
void initializeGame(struct GameInfo *gameInfo, int randomNumbers[]) // All game settings variables
{

  // Initialize cave room connections
  //       ______18______
  //      /      |       \
	//     /      _9__      \
	//    /      /    \      \
	//   /      /      \      \
	//  17     8        10    19
  // |  \   / \      /  \   / |
  // |   \ /   \    /    \ /  |
  // |    7     1---2     11  |
  // |    |    /     \    |   |
  // |    6----5     3---12   |
  // |    |     \   /     |   |
  // |    \       4      /    |
  // |     \      |     /     |
  //  \     15---14---13     /
  //   \   /            \   /
  //    \ /              \ /
  //    16---------------20

  gameInfo->personRoom = randomNumbers[0];
  gameInfo->wumpusRoom = randomNumbers[1];
  gameInfo->pitRoom1 = randomNumbers[2];
  gameInfo->pitRoom2 = randomNumbers[3];
  gameInfo->batRoom1 = randomNumbers[4];
  gameInfo->batRoom2 = randomNumbers[5];
  gameInfo->arrowRoom = randomNumbers[6];

  gameInfo->moveNumber = 1;
} // end initializeBoard(...)

//--------------------------------------------------------------------------------
// Returns true if nextRoom is adjacent to current room, else returns false.
int roomIsAdjacent(int tunnels[3], // Array of adjacent tunnels
                   int nextRoom)   // Desired room to move to
{
  return (tunnels[0] == nextRoom || tunnels[1] == nextRoom || tunnels[2] == nextRoom);
}

//--------------------------------------------------------------------------------
// Display where everything is on the board.
void displayCheatInfo(struct GameInfo gameInfo) {
  printf("Cheating! Game elements are in the following rooms: \n"
         "Player Wumpus Pit1 Pit2 Bats1 Bats2 Arrow  \n"
         "%4d %5d %6d %5d %5d %5d %5d \n\n",
         gameInfo.personRoom, gameInfo.wumpusRoom, gameInfo.pitRoom1,
         gameInfo.pitRoom2, gameInfo.batRoom1, gameInfo.batRoom2, gameInfo.arrowRoom);
} // end displayCheatInfo(...)

//--------------------------------------------------------------------------------
// Display room number and hazards detected
void displayRoomInfo(struct GameInfo gameInfo) {
  // Retrieve player's current room number and display it
  int currentRoom = gameInfo.personRoom;
  printf("You are in room %d. ", currentRoom);

  // Retrieve index values of all 3 adjacent rooms
  int room1 = Rooms[currentRoom][0];
  int room2 = Rooms[currentRoom][1];
  int room3 = Rooms[currentRoom][2];

  // Display hazard detection message if Wumpus is in an adjacent room
  int wumpusRoom = gameInfo.wumpusRoom;
  if (room1 == wumpusRoom || room2 == wumpusRoom || room3 == wumpusRoom) {
    printf("You smell a stench. ");
  }

  // Display hazard detection message if a pit is in an adjacent room
  int pit1Room = gameInfo.pitRoom1;
  int pit2Room = gameInfo.pitRoom2;
  if (room1 == pit1Room || room1 == pit2Room || room2 == pit1Room ||
      room2 == pit2Room || room3 == pit1Room || room3 == pit2Room) {
    printf("You feel a draft. ");
  }

	// Display hazard detection message if bats are in an adjacent room
  int bat1Room = gameInfo.batRoom1;
  int bat2Room = gameInfo.batRoom2;
  if (room1 == bat1Room || room1 == bat2Room || room2 == bat1Room ||
      room2 == bat2Room || room3 == bat1Room || room3 == bat2Room) {
    printf("You hear rustling of bat wings. ");
  }

  printf("\n\n");
} // end displayRoomInfo(...)

//--------------------------------------------------------------------------------
// If the player just moved into a room with a pit, the person dies.
// If the person just moved into the Wumpus room, then if the room number is odd
// the Wumpus moves to a random adjacent room.
void checkForHazards(
    struct GameInfo *gameInfo, // Hazards location and game info
    bool *personIsAlive,
    int randomNumbers[]) // Person is alive, but could die depending on the
                         // hazards
{
  // retrieve the room the person is in
  int personRoom = gameInfo->personRoom;

  // Check for the Wumpus
  if (personRoom == gameInfo->wumpusRoom) {
    
    if (gameInfo->wumpusRoom % 2 == 1) {
      // You got lucky and the Wumpus moves away
      printf("You hear a slithering sound, as the Wumpus slips away. \n"
             "Whew, that was close! \n");
      // Choose a random adjacent room for the Wumpus to go into
      gameInfo->wumpusRoom = Rooms[personRoom][0]; // Choose the lowest-numbered adjacent room
    } else {
      // Wumpus kills you
      printf(
          "You briefly feel a slimy tentacled arm as your neck is snapped. \n"
          "It is over.\n");
      *personIsAlive = false;
      return;
    }
  }

  // Check whether there is a pit
  if (personRoom == gameInfo->pitRoom1 || personRoom == gameInfo->pitRoom2) {
    // Person falls into pit
    printf("Aaaaaaaaahhhhhh....   \n");
    printf("    You fall into a pit and die. \n");
    *personIsAlive = false;
    return;
  }

  // Check for batRoom1
	if (personRoom == gameInfo->batRoom1) {
		int newRoom = rand() % NUMBER_OF_ROOMS + 1;

		// Move person to random room
    while (newRoom == gameInfo->personRoom) {
      newRoom = rand() % NUMBER_OF_ROOMS + 1;
    }
		gameInfo->personRoom = newRoom;
		randomNumbers[0] = newRoom;

		printf("Woah... you're flying! \n");
    printf("You've just been transported by bats to room %d.\n", gameInfo->personRoom);

		// Move bats to new room
		newRoom = rand() % NUMBER_OF_ROOMS + 1;
		while ( (newRoom == gameInfo->personRoom) || (newRoom == gameInfo->batRoom2) || (newRoom == personRoom) ) {
			newRoom = rand() % NUMBER_OF_ROOMS + 1;
		}
		gameInfo->batRoom1 = newRoom;
		randomNumbers[4] = newRoom;
		
	} // end batRoom1

	// Check for batRoom2
	if (personRoom == gameInfo->batRoom2) {
		int newRoom = rand() % NUMBER_OF_ROOMS + 1;

		// Move person to random room
    while (newRoom == gameInfo->personRoom) {
      newRoom = rand() % NUMBER_OF_ROOMS + 1;
    }
		gameInfo->personRoom = newRoom;
		randomNumbers[0] = newRoom;

		printf("Woah... you're flying! \n");
    printf("You've just been transported by bats to room %d.\n", gameInfo->personRoom);

		// Move bats to new room
		newRoom = rand() % NUMBER_OF_ROOMS + 1;
		while ( (newRoom == gameInfo->personRoom) || (newRoom == gameInfo->batRoom2) || (newRoom == personRoom) ) {
			newRoom = rand() % NUMBER_OF_ROOMS + 1;
		}
		gameInfo->batRoom2 = newRoom;
		randomNumbers[5] = newRoom;
	} // end batRoom2
	
	// If the player enters a room with the arrow, set arrowRoom to -1
	if (personRoom == gameInfo->arrowRoom) {
		gameInfo->arrowRoom = -1;
		printf("Congratulations, you found the arrow and can once again shoot.\n");
	}

} // end checkForHazards(...)

//--------------------------------------------------------------------------------
// Prompt for and reset the positions of the game hazards and the person's
// location, useful for testing.  No error checking is done on these values.
void resetPositions(struct GameInfo *theGameInfo) {
  printf("Enter the room locations (1..20) for player, Wumpus, pit1, pit2, bats1, "
         "bats2, and arrow: \n");
  
  scanf(" %d %d %d %d %d %d %d", &theGameInfo->personRoom,
        &theGameInfo->wumpusRoom, &theGameInfo->pitRoom1,
        &theGameInfo->pitRoom2, &theGameInfo->batRoom1, &theGameInfo->batRoom2,
        &theGameInfo->arrowRoom);
  printf("\n");
}

//--------------------------------------------------------------------------------

void shootArrow(struct GameInfo *gameInfo, bool *personIsAlive, int randomNumbers[]) {
	int numRooms;
	int room;
	gameInfo->arrowRoom = gameInfo->personRoom;
	printf("Enter the number of rooms (1..3) into which you want to shoot, followed by the rooms themselves: ");
	scanf(" %d", &numRooms);

	for (int i = 0; i < numRooms; i++) {
		// Get the room they want to shoot the arrow to
		scanf(" %d", &room);
		
		// If the room is adjacent, update arrowRoom and check for hazards
		if (roomIsAdjacent(Rooms[gameInfo->arrowRoom], room)) {
			gameInfo->arrowRoom = room;
			randomNumbers[6] = room;
			if (gameInfo->arrowRoom == gameInfo->wumpusRoom) {
				printf("Wumpus has just been pierced by your deadly arrow! \n"
					"Congratulations on your victory, you awesome hunter you.\n");
				*personIsAlive = false;
				break;
			}

			if (gameInfo->arrowRoom == gameInfo->personRoom) {
				printf("You just shot yourself.  \nMaybe Darwin was right.  You're dead.\n");
				*personIsAlive = false;
				break;
			}
		} // end if adjacent

		// If the room is not adjacent, the arrow ricochets
		else {
			printf("Room %d is not adjacent.  Arrow ricochets...\n", room);
			gameInfo->arrowRoom = Rooms[gameInfo->arrowRoom][0];
			randomNumbers[6] = gameInfo->arrowRoom;

			if (gameInfo->arrowRoom == gameInfo->personRoom) {
				printf("You just shot yourself, and are dying.\nIt didn't take long, you're dead.\n");
				*personIsAlive = false;
				break;
			}

			if (gameInfo->arrowRoom == gameInfo->wumpusRoom) {
				printf("Your arrow ricochet killed the Wumpus, you lucky dog!\n"
					"Accidental victory, but still you win!\n");
				*personIsAlive = false;
				break;
			}
		}

	} // end for loop
	
	// Wumpus moves to lowest numbered adjacent room
	gameInfo->wumpusRoom = Rooms[gameInfo->wumpusRoom][0];
} // end shootArrow()

//--------------------------------------------------------------------------------
int main(void) {
	// Seed the random number generator.
  srand(1);
	
  struct GameInfo gameInfo; // Used to more easily pass game info variables around
  bool personIsAlive = true; // Used in checking for end of game
  bool wumpusIsAlive = true; // Used in checking for end of game
  char typeOfMove;           // Used to handle user input letter
  int nextRoom; // User input of destination room number, used on a 'M' type move

	int* randomNumbers = malloc(7 * sizeof(int));
  setUniqueValues(randomNumbers, 7);

  Rooms = (int**)malloc(21 * sizeof(int *));
  for (int i = 0; i < 21; i++) {
    Rooms[i] = (int *)malloc(3 * sizeof(int));
  }

  // // Assigning rooms
  Rooms[0][0] = 0; Rooms[0][1] = 0; Rooms[0][2] = 0;

  // Rooms 1-5
  Rooms[1][0] = 2; Rooms[1][1] = 5; Rooms[1][2] = 8;
	Rooms[2][0] = 1; Rooms[2][1] = 3; Rooms[2][2] = 10;
  Rooms[3][0] = 2; Rooms[3][1] = 4; Rooms[3][2] = 12;
  Rooms[4][0] = 3; Rooms[4][1] = 5; Rooms[4][2] = 14;
  Rooms[5][0] = 1; Rooms[5][1] = 4; Rooms[5][2] = 6;

  // Rooms 6-10
  Rooms[6][0] = 5; Rooms[6][1] = 7; Rooms[6][2] = 15;
  Rooms[7][0] = 6; Rooms[7][1] = 8; Rooms[7][2] = 17;
  Rooms[8][0] = 1; Rooms[8][1] = 7; Rooms[8][2] = 9;
  Rooms[9][0] = 8; Rooms[9][1] = 10; Rooms[9][2] = 18;
  Rooms[10][0] = 2; Rooms[10][1] = 9; Rooms[10][2] = 11;

  // Rooms 11-15
  Rooms[11][0] = 10; Rooms[11][1] = 12; Rooms[11][2] = 19; 
	Rooms[12][0] = 3; Rooms[12][1] = 11; Rooms[12][2] = 13;
  Rooms[13][0] = 12; Rooms[13][1] = 14; Rooms[13][2] = 20;
  Rooms[14][0] = 4; Rooms[14][1] = 13; Rooms[14][2] = 15;
  Rooms[15][0] = 6; Rooms[15][1] = 14; Rooms[15][2] = 16;

  // Rooms 16-20
  Rooms[16][0] = 15; Rooms[16][1] = 17; Rooms[16][2] = 20;
  Rooms[17][0] = 7; Rooms[17][1] = 16; Rooms[17][2] = 18;
  Rooms[18][0] = 9; Rooms[18][1] = 17; Rooms[18][2] = 19;
  Rooms[19][0] = 11; Rooms[19][1] = 18; Rooms[19][2] = 20;
  Rooms[20][0] = 13; Rooms[20][1] = 16; Rooms[20][2] = 19;

  // Set random initial values for person, Wumpus, bats and pits
  initializeGame(&gameInfo, randomNumbers);

  // Main playing loop.  Break when player dies, or player kills Wumpus
  while (personIsAlive && wumpusIsAlive) {

    // Display current room information: Room number, hazards detected
    displayRoomInfo(gameInfo);

    // Prompt for and handle move
    printf("%d. Enter your move (or 'D' for directions): ", gameInfo.moveNumber);
    scanf(" %c", &typeOfMove);

    typeOfMove = toupper(typeOfMove); // Make uppercase to facilitate checking

    if (typeOfMove == 'D') {
      displayCave();
      displayInstructions();
      continue; // Loop back up to reprompt for the same move
    } 
		
		else if (typeOfMove == 'P') {
      // To assist with play, display the cave layout
      displayCave();
      continue; // Loop back up to reprompt for the same move
    } 
		
		else if (typeOfMove == 'M') {
      scanf(" %d", &nextRoom);

      if (roomIsAdjacent(Rooms[gameInfo.personRoom], nextRoom)) {
        gameInfo.personRoom = nextRoom; // move to a new room
        // Check if pit or wumpus is present in this new room
        checkForHazards(&gameInfo, &personIsAlive, randomNumbers);
      } else {
        printf("Invalid move.  Please retry. \n");
        continue; // Doesn't count as a move, so retry same move.
      }
    } 
			
		// Display Cheat information
		else if (typeOfMove == 'C') {
      displayCheatInfo(gameInfo);
      continue; // Doesn't count as a move, so retry same move.
    } 
		
		else if (typeOfMove == 'R') {
      resetPositions(&gameInfo);
      continue; // Doesn't count as a move, so retry same move.
    } 

		// Shoot arrow
		else if (typeOfMove == 'S') {
			if (gameInfo.arrowRoom == -1) {
				shootArrow(&gameInfo, &personIsAlive, randomNumbers);
			}
			else {
				printf("Sorry, you can't shoot an arrow you don't have.  Go find it.\n");
				gameInfo.moveNumber = gameInfo.moveNumber + 1;
				continue;
			}
		}

		// Exit program
		else if (typeOfMove == 'X') {
      personIsAlive = false; // Indicate person is dead as a way to exit playing loop
      break;
    }

    // Increment the move number
    gameInfo.moveNumber = gameInfo.moveNumber + 1;

  } // end while(personIsAlive && wumpusIsAlive)

  printf("\nExiting Program ...\n");
	free(randomNumbers);
	free(Rooms);
	
  return 0;
}