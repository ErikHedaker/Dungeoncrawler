#include "Dungeon.h"
#include "IO.h"
#include <random>
#include <queue>
#include <algorithm>
#include <Windows.h>

extern const Vector2i WINDOW_SIZE( 100, 50 );

void SetWindow( int Width, int Height )
{
	_COORD coord;
	coord.X = Width;
	coord.Y = Height;

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = Height - 1;
	Rect.Right = Width - 1;

	HANDLE Handle = GetStdHandle( STD_OUTPUT_HANDLE );      // Get Handle 
	SetConsoleScreenBufferSize( Handle, coord );            // Set Buffer Size 
	SetConsoleWindowInfo( Handle, TRUE, &Rect );            // Set Window Size 
}

int main( )
{
	const std::vector<char> gameTypeChoices { '1', '2', '3' };

	SetWindow( WINDOW_SIZE.col, WINDOW_SIZE.row );


	while( true )
	{
		Player player( Vector2i( -1, -1 ), 100.0f, 0.10f, 50.0f, 100.0f, 100.0f );
		Dungeon dungeon( &player );
		char gameType;

		Output::ClearScreen( );
		Output::GameTypes( );

		gameType = Input::ValidChar( "\nYour choice: ", gameTypeChoices );
		if( gameType == '3' )
		{
			break;
		}

		dungeon.Build( gameType );
		dungeon.GameLoop( );
	}

	return 0;
}