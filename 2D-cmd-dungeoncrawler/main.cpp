#include "Dungeon.h"
#include "IO.h"
#include <random>
#include <queue>
#include <algorithm>
#include <Windows.h>

extern const Vector2i WINDOWSIZE( 100, 50 );

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
	SetWindow( WINDOWSIZE.col, WINDOWSIZE.row );

	while( true )
	{
		Dungeon dungeon;
		char gameType;

		IO::OutputClearScreen( );
		IO::OutputStartMessage( );
		IO::OutputGameTypes( );

		gameType = IO::InputValidChar( "\nYour choice: ", std::vector<char>{ '1', '2', '3' } );
		if( gameType == '3' )
		{
			break;
		}

		dungeon.Build( gameType );
		dungeon.GameLoop( );
	}

	return 0;
}