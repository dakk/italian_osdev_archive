/* Tic-Tac-Toe program by Steve Chapel schapel@cs.ucsb.edu
   Uses alpha-beta pruning minimax search to play a "perfect" game.
   The alpha-beta pruning can be removed, but will increase search time.
   The heuristic and move ordering in Best_Move() can also be removed with
     an increase in search time. */

#include "cheapOSlib.h"

#define String_Length 80

#define Squares 9

void Game();
typedef char Square_Type;
typedef Square_Type Board_Type[ Squares ];
const Square_Type Empty = ' ';

const int Infinity = 10;		/* Higher value than any score */
const int Maximum_Moves = Squares;	/* Maximum moves in a game */

int Total_Nodes;			/* Nodes searched with minimax */

/* Array describing the eight combinations of three squares in a row */
#define Possible_Wins 8
const int Three_in_a_Row[Possible_Wins][3] = {
    { 0, 1, 2 },
    { 3, 4, 5 },
    { 6, 7, 8 },
    { 0, 3, 6 },
    { 1, 4, 7 },
    { 2, 5, 8 },
    { 0, 4, 8 },
    { 2, 4, 6 }
};

/* Array used in heuristic formula for each move. */
const int Heuristic_Array[4][4] = {
    {     0,   -10,  -100, -1000 },
    {    10,     0,     0,     0 },
    {   100,     0,     0,     0 },
    {  1000,     0,     0,     0 }
};

/* Structure to hold a move and its heuristic */
typedef struct {
    int Square;
    int Heuristic;
} Move_Heuristic_Type;

void clrscr()
{
	/*int i;
	for( i=0; i<80*24; i++ )
		print(" ");
*/
}

int main( int argc, char *argv[] )
{
    char Answer[ String_Length ];
	int console;

	console=1;

	if(argc==2)
		console=atoi(argv[1]);

	if(!openconsole(console))
		exit(1);


    clrscr(); print("\n");

    print( "Welcome to Tic-Tac-Toe!\n\n" );
    print( "Here is the board numbering:\n" );
    print( " 1 | 2 | 3\n" );
    print( "---+---+---\n" );
    print( " 4 | 5 | 6\n" );
    print( "---+---+---\n" );
    print( " 7 | 8 | 9\n" );
    print( "\n" );
    print( "Computer plays X, you play O.\n" );

    do
    {
	Game();
	print( "\nDo you want to play again? " );
	    //scanf( "%s", Answer );
	getstring(Answer,(String_Length-1));
	print("\n");
    } while ( Answer[ 0 ]  == 'y' );

    print( "\n" );
    closeconsole();
    return 0;
}



/* Clear the board */
void Initialize(Board_Type Board) {
    int I;
    for (I = 0; I < Squares; I++)
	Board[I] = Empty;
}

/* If a player has won, return the winner. If the game is a tie,
   return 'C' (for cat). If the game is not over, return Empty. */
Square_Type Winner(Board_Type Board) {
    int I;
    for (I = 0; I < Possible_Wins; I++) {
	Square_Type Possible_Winner = Board[Three_in_a_Row[I][0]];
	if (Possible_Winner != Empty &&
	    Possible_Winner == Board[Three_in_a_Row[I][1]] &&
	    Possible_Winner == Board[Three_in_a_Row[I][2]])
	    return Possible_Winner;
    }

    for (I = 0; I < Squares; I++)
	if (Board[I] == Empty)
	    return Empty;

    return 'C';
}

/* Return the other player */
Square_Type Other(Square_Type Player) {
    return Player == 'X' ? 'O' : 'X';
}

/* Make a move on the board */
void Play(Board_Type Board, int Square, Square_Type Player) {
    Board[Square] = Player;
}

/* Print the board */
void Print(Board_Type Board) {
    int I;
    for (I = 0; I < Squares; I += 3) {
	if (I > 0)
	    print("---+---+---\n");
	print(" %c |",Board[I]);
	print(" %c |", Board[I + 1]);
	print(" %c \n",  Board[I + 2]);
    }
    print("\n");
}

/* Return a heuristic used to determine the order in which the
   children of a node are searched */
int Evaluate(Board_Type Board, Square_Type Player) {
    int I;
    int Heuristic = 0;
    for (I = 0; I < Possible_Wins; I++) {
	int J;
	int Players = 0, Others = 0;
	for (J = 0; J < 3; J++) {
	    Square_Type Piece = Board[Three_in_a_Row[I][J]];
	    if (Piece == Player)
		Players++;
	    else if (Piece == Other(Player))
		Others++;
	}
	Heuristic += Heuristic_Array[Players][Others];
    }
    return Heuristic;
}

/* Return the score of the best move found for a board
   The square to move to is returned in *Square */
int Best_Move(Board_Type Board, Square_Type Player, int *Square, int Move_Nbr,
	      int Alpha, int Beta) {
    int Best_Square = -1;
    int Moves = 0;
    int I;
    Move_Heuristic_Type Move_Heuristic[Squares];

    Total_Nodes++;

    /* Find the heuristic for each move and sort moves in descending order */
    for (I = 0; I < Squares; I++) {
	if (Board[I] == Empty) {
	    int Heuristic;
	    int J;
	    Play(Board, I, Player);
	    Heuristic = Evaluate(Board, Player);
	    Play(Board, I, Empty);
	    for (J = Moves-1; J >= 0 &&
			      Move_Heuristic[J].Heuristic < Heuristic; J--) {
		Move_Heuristic[J + 1].Heuristic = Move_Heuristic[J].Heuristic;
		Move_Heuristic[J + 1].Square = Move_Heuristic[J].Square;
	    }
	    Move_Heuristic[J + 1].Heuristic = Heuristic;
	    Move_Heuristic[J + 1].Square = I;
	    Moves++;
	}
    }

    for (I = 0; I < Moves; I++) {
	int Score;
	int Sq = Move_Heuristic[I].Square;
	Square_Type W;

	/* Make a move and get its score */
	Play(Board, Sq, Player);

	W = Winner(Board);
	if (W == 'X')
	    Score = (Maximum_Moves + 1) - Move_Nbr;
	else if (W == 'O')
	    Score = Move_Nbr - (Maximum_Moves + 1);
	else if (W == 'C')
	    Score = 0;
	else
	    Score = Best_Move(Board, Other(Player), Square, Move_Nbr + 1,
			      Alpha, Beta);

	Play(Board, Sq, Empty);

	/* Perform alpha-beta pruning */
	if (Player == 'X') {
	    if (Score >= Beta) {
		*Square = Sq;
		return Score;
	    } else if (Score > Alpha) {
		Alpha = Score;
		Best_Square = Sq;
	    }
	} else {
	    if (Score <= Alpha) {
		*Square = Sq;
		return Score;
	    } else if (Score < Beta) {
		Beta = Score;
		Best_Square = Sq;
	    }
	}
    }
    *Square = Best_Square;
    if (Player == 'X')
	return Alpha;
    else
	return Beta;
}

/* Provide an English description of the score returned by Best_Move */
void Describe(int Score)
{
    if ( Score < 0 )
    {
	print( "You have a guaranteed win.\n" );
    }
      else
    if ( Score == 0 )
    {
	print( "I can guarantee a tie.\n" );
    }
      else
    {
	print( "I have a guaranteed win by move %d.\n", Maximum_Moves -
	Score + 1 );
    }
}

/* Have the human or the computer move */
void Move( Board_Type Board, Square_Type Player, int Move_Nbr )
{
	char buffer[24];
    int Square;

    if ( Player == 'X' )
    {
	Total_Nodes = 0;
	Describe( Best_Move( Board, 'X', &Square, Move_Nbr, -Infinity, Infinity ) );
	print( "%d nodes examined.\n", Total_Nodes );
	Play( Board, Square, 'X' );
	print( "Move #%d - X moves to %d\n", (unsigned int)Move_Nbr, (unsigned int)(Square + 1) );
    }
      else
    {
	do
	{
	    print( "Move #%d - What is O's move? ", Move_Nbr );
		getstring(buffer,20);
		print("\n");
		Square=atoi(buffer);
	    //scanf( "%d", &Square );
	    Square--;
	} while ( Board[ Square ] != ' ' );
	Play( Board, Square, 'O' );
    }
}

/* Play a game of tic-tac-toe */
void Game() {
    Square_Type Player;
    char Answer[String_Length];
    Board_Type Board;
    int Move_Nbr = 1;

    Initialize( Board );

    print( "\nDo you want to move first? " );
	getstring(Answer,(String_Length-1));
    //scanf( "%s", Answer );

    print( "\n" );
    if ( Answer[ 0 ]  == 'y' )
    {
	Player = 'O';
    }
      else
    {
	Player = 'X';
    }

    while ( Winner( Board ) == ' ' )
    {
		clrscr();
		print("\n");
	Print( Board );
	Move( Board, Player, Move_Nbr );
	Player = Other( Player );
	Move_Nbr++;
    }
    Print( Board );

    if ( Winner( Board ) != 'C' )
    {
	print( "%c wins!\n", Winner( Board ) );
    }
      else
    {
	print( "It's a tie.\n" );
    }
}

