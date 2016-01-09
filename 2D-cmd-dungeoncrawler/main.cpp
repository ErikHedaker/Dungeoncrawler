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
void Menu( Dungeon& dungeon )
{
	const std::vector<char> menuChoices { '1', '2', '3', '4' };

	switch( Input::ValidChar( "\nYour choice: ", menuChoices ) )
	{
		case '1':
		{
			dungeon.BuildDungeon( GameType::Randomized );

			break;
		}
		case '2':
		{
			dungeon.BuildDungeon( GameType::MinorConfiguration );

			break;
		}
		case '3':
		{
			dungeon.LoadDungeon( "dungeonSave.txt" );

			break;
		}
		case '4':
		{
			exit( 0 );
		}
		default:
		{
			Output::String( "\n\nSomething went wrong." );
			Output::String( "\nPress enter to continue: " );
			Input::Enter( );

			break;
		}
	}
}

int main( )
{
	SetWindow( WINDOW_SIZE.col, WINDOW_SIZE.row );

	while( true ) /* Menu (sort of). */
	{
		Dungeon dungeon;

		Output::ClearScreen( );
		Output::GameTypes( );

		try
		{
			Menu( dungeon );
		}
		catch( const std::exception& e )
		{
			Output::String( "\n\nSomething went wrong." );
			Output::String( "\nReason: " );
			Output::String( e.what( ) );
			Output::String( "\nPress enter to continue: " );
			Input::Enter( );

			continue;
		}
		catch( ... )
		{
			Output::String( "\n\nSomething went wrong." );
			Output::String( "\nPress enter to continue: " );
			Input::Enter( );

			continue;
		}

		dungeon.GameLoop( );
	}

	return 0;
}