#include "Game.h"
#include <vector>

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
				game.NewGame( );
				game.GameLoop( );

				break;
			}
			case '3':
			{
				game.SetDungeonConfiguration( );
				game.NewGame( );
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