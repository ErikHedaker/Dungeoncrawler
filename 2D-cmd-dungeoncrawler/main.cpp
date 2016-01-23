#include "Game.h"
#include <Windows.h>
#include <vector>

extern const Vector2i WINDOW_SIZE( 100, 50 );

//void SetWindow( int Width, int Height )
//{
//	_COORD coord;
//	coord.X = Width;
//	coord.Y = Height;
//
//	_SMALL_RECT Rect;
//	Rect.Top = 0;
//	Rect.Left = 0;
//	Rect.Bottom = Height - 1;
//	Rect.Right = Width - 1;
//
//	HANDLE Handle = GetStdHandle( STD_OUTPUT_HANDLE );      // Get Handle 
//	SetConsoleScreenBufferSize( Handle, coord );            // Set Buffer Size 
//	SetConsoleWindowInfo( Handle, TRUE, &Rect );            // Set Window Size 
//}

int main( )
{
	while( true )
	{
		const std::vector<char> choices { '1', '2', '3', '4', '5', '6' };
		char choice;
		Game game;

		OutputClearScreen( );

		std::cout << "[1] Continue current game\n";
		std::cout << "[2] Build new dungeon (Randomization)\n";
		std::cout << "[3] Build new dungeon (Configuration)\n";
		std::cout << "[4] Load dungeons from file\n";
		std::cout << "[5] Save dungeons to file and exit\n";
		std::cout << "[6] Exit\n";

		choice = InputValidChar( "\nEnter choice: ", choices );

		switch( choice )
		{
			case '1':
			{
				if( game.existingGame )
				{
					game.GameLoop( );
				}

				break;
			}
			case '2':
			{
				game.NewGame( DungeonType::Randomization );
				game.GameLoop( );

				break;
			}
			case '3':
			{
				game.SetDungeonConfiguration( );
				game.NewGame( DungeonType::Configuration );
				game.GameLoop( );

				break;
			}
			case '4':
			{
				/* Temporary */

				break;
			}
			case '5':
			{
				/* Temporary */

				break;
			}
			case '6':
			{
				exit( 0 );
			}
		}
	}

	return 0;
}