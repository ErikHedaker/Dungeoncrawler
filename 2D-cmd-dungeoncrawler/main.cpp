#include "Game.h"
#include "Functions.h"
#include <vector>

int main( )
{
	Game game;

	while( true )
	{
		system( "CLS" );
		std::cout << "[1] Continue current game\n";
		std::cout << "[2] Build new dungeon (Randomization)\n";
		std::cout << "[3] Build new dungeon (Configuration)\n";
		std::cout << "[4] Load dungeons from file\n";
		std::cout << "[5] Save dungeons to file\n";
		std::cout << "[6] Exit\n";

		const std::vector<char> choices =
		{
			'1',
			'2',
			'3',
			'4',
			'5',
			'6'
		};
		const char choice = GetValidChar( "\nEnter choice: ", choices );

		switch( choice )
		{
			case '1':
			{
				if( game.ExistingGame( ) )
				{
					game.GameLoop( );
				}

				break;
			}
			case '2':
			{
				game.SetDungeonConfiguration( GameConfig::Default );
				system( "CLS" );
				game.NewGame( );
				game.GameLoop( );

				break;
			}
			case '3':
			{
				game.SetDungeonConfiguration( GameConfig::Configure );
				system( "CLS" );
				game.NewGame( );
				game.GameLoop( );

				break;
			}
			case '4':
			{
				game.SetDungeonConfiguration( GameConfig::Default );

				try
				{
					game.LoadDungeons( );
				}
				catch( const std::exception& e )
				{
					std::cout << "Exception: " << e.what( );
					GetEnter( );

					break;
				}

				game.GameLoop( );

				break;
			}
			case '5':
			{
				game.SaveDungeons( );

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