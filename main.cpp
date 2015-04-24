#include <iostream>
#include <vector>
#include <string>
#include <random>

struct PositionStruct
{
	int x;
	int y;
};
struct PlayerStruct
{
	PositionStruct Position;
	char icon = 'P';
};
struct MonsterStruct
{
	PositionStruct Position;
};
struct AllmonstersStruct
{
	char icon = 'M';
	int amountDesired = 0;
	int amountCurrent = 0;
};
struct UnitStruct
{
	std::vector<MonsterStruct> Monster;
	AllmonstersStruct Allmonsters;
	PlayerStruct Player;
};
struct WallStruct
{
	PositionStruct Position;
};
struct AllwallsStruct
{
	char icon = '#';
	int amountAll = 0;
	int amountOuter = 0;
	int amountInner = 0;
};
struct ExitStruct
{
	PositionStruct Position;
	char icon = '=';
	int amount = 0;
};
struct RoomStruct
{
	std::vector<WallStruct> Wall;
	AllwallsStruct Allwalls;
	ExitStruct Exit;
	int length;
	int width;
};
struct WorldStruct
{
	UnitStruct Unit;
	RoomStruct Room;
};

int GetPositiveInteger( )
{
	std::string userChoice;

	RETRY:
	std::cin >> userChoice;

	if( userChoice.size( ) > 9 ) // Manual max int input: 999 999 999, roughly half of the max size for signed int.
	{
		std::cout << "Input is too big, try again: ";
		goto RETRY;
	}

	for( unsigned int i = 0; i < userChoice.size( ); i++ )
	{
		if( isdigit( userChoice[i] ) == false )
		{
			std::cout << "Input needs to consist of digits only, try again: ";
			goto RETRY;
		}
	}

	int output = atoi( userChoice.c_str( ) );

	return output;
}
int randomNumberGenerator( int min, int max )
{
	std::random_device randomEngine;
	std::mt19937 generator( randomEngine( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}
int randomBooleanGenerator( )
{
	int randomNumber;

	do
	{
		randomNumber = randomNumberGenerator( -1, 1 );
	}
	while( randomNumber == 0 );

	return randomNumber;
}

bool CheckForRoomExit( const WorldStruct &World, int xPos, int yPos )
{
	for( int i = 0; i < World.Room.Exit.amount; i++ )
	{
		if( xPos == World.Room.Exit.Position.x &&
			yPos == World.Room.Exit.Position.y )
		{
			return true;
		}
	}

	return false;
}
bool CheckForRoomWall( const WorldStruct &World, int xPos, int yPos )
{
	for( int i = 0; i < World.Room.Allwalls.amountAll; i++ )
	{
		if( xPos == World.Room.Wall[i].Position.x &&
			yPos == World.Room.Wall[i].Position.y )
		{
			return true;
		}
	}

	return false;
}
bool CheckForPlayerPosition( const WorldStruct &World, int xPos, int yPos )
{
	if( xPos == World.Unit.Player.Position.x &&
		yPos == World.Unit.Player.Position.y )
	{
		return true;
	}

	return false;
}
bool CheckForSpawnNearPlayer( const WorldStruct &World, int xPos, int yPos )
{
	for( int yProtect = World.Unit.Player.Position.y - 2; yProtect < World.Unit.Player.Position.y + 2; yProtect++ )
	{
		for( int xProtect = World.Unit.Player.Position.x - 2; xProtect < World.Unit.Player.Position.x + 2; xProtect++ )
		{
			if( xPos == xProtect &&
				yPos == yProtect )
			{
				return true;
			}
		}
	}

	return false;
}
bool CheckForMonsterPosition( const WorldStruct &World, int xPos, int yPos )
{
	for( int i = 0; i < World.Unit.Allmonsters.amountCurrent; i++ )
	{
		if( xPos == World.Unit.Monster[i].Position.x &&
			yPos == World.Unit.Monster[i].Position.y )
		{
			return true;
		}
	}

	return false;
}
bool CheckForWinLoseCondition( const WorldStruct &World )
{
	if( CheckForRoomExit( World, World.Unit.Player.Position.x, World.Unit.Player.Position.y ) == true )
	{
		std::cout << "\nYou win!";
		return true;
	}
	else if( CheckForMonsterPosition( World, World.Unit.Player.Position.x, World.Unit.Player.Position.y ) == true )
	{
		std::cout << "\nYou lose!";
		return true;
	}
	else
	{
		return false;
	}
}

void SayGameRules( )
{
	std::cout << "\nWin  condition: Enter the exit.";
	std::cout << "\nLose condition: Enter the same space as a monster.\n\n";
}
void SetRoomSize( WorldStruct &World )
{
	std::string userChoice;

	std::cout << "Would you like to determine the room dimensions, [y/n]: ";
	while( true )
	{
		std::cin >> userChoice;

		if( userChoice[0] == 'Y' || userChoice[0] == 'y' )
		{
			std::cout << "Enter the playing Room length: ";
			World.Room.length = GetPositiveInteger( );
			std::cout << "Enter the playing Room width: ";
			World.Room.width = GetPositiveInteger( );
			break;
		}
		else if( userChoice[0] == 'N' || userChoice[0] == 'n' )
		{
			World.Room.length = 10;
			World.Room.width = 10;
			break;
		}
		else
		{
			std::cout << "Invalid input, try again: ";
		}
	}
}
void SetRoomExits( WorldStruct &World )
{
	World.Room.Exit.amount = 1;	// Temporary.
	World.Room.Exit.Position.x = 1;
	World.Room.Exit.Position.y = 0; // Top left corner.
}
void SetRoomOuterWalls( WorldStruct &World )
{
	WallStruct Temp;

	for( int ySet = 0; ySet < World.Room.length; ySet++ )
	{
		for( int xSet = 0; xSet < World.Room.width; xSet++ )
		{
			if( CheckForRoomExit( World, xSet, ySet ) == true )
			{
				continue;
			}
			else if( xSet == 0 || xSet == World.Room.width - 1 ||
					 ySet == 0 || ySet == World.Room.length - 1 )
			{
				Temp.Position.x = xSet;
				Temp.Position.y = ySet;
				World.Room.Wall.push_back( Temp );
				World.Room.Allwalls.amountAll++;
				World.Room.Allwalls.amountOuter++;
			}
		}
	}
}
/*bool SetOneExtendingInnerWall( WorldStruct &World, int chosenWall )
{
	int xPosTemp, yPosTemp;

	if( ( World.Room.Wall[chosenWall].Position.x == 0 || World.Room.Wall[chosenWall].Position.x == World.Room.width - 1 ) &&
		( World.Room.Wall[chosenWall].Position.y == 0 || World.Room.Wall[chosenWall].Position.y == World.Room.length - 1 ) )

	return false;   // Did not manage to create an extending wall.
}*/
/*void SetRoomInnerWalls( WorldStruct &World )
{
      WallStruct Temp;
      int amountExtendingWalls;
      int randomExistingWall;
      int xRand, yRand;
      int xPosTemp = 0;
      int yPosTemp = 0;
      int failCounter;
      bool RetryLoop;
     
      amountExtendingWalls = 1 + rand( ) % ( World.Room.Allwalls.amountAll ); // No real logic here.

      for( int i = 0; i < amountExtendingWalls; i++ )
      {
              RetryLoop = true;
              randomExistingWall = 1 + rand( ) % World.Room.Wall.size( );
              failCounter = 0;

              while( RetryLoop == true )
              {
                      yRand = 1 + rand( ) % 3;
                      xRand = 1 + rand( ) % 3;
                      failCounter++;

                      switch( yRand )
                      {
                              case 1: // Wall goes up.
                              {
                                      yPosTemp = World.Room.Wall[randomExistingWall].Position.y - 1;

                                      if( yPosTemp < 0 || yPosTemp > World.Room.length - 1  )
                                      {
                                              break;
                                      }
                                      else if( CheckForRoomWall( World, World.Room.Wall[randomExistingWall].Position.x, yPosTemp ) == false )
                                      {
                                              World.Room.Wall[randomExistingWall].Position.y = yPosTemp;
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              case 2: // Wall goes down.
                              {
                                      yPosTemp = World.Room.Wall[randomExistingWall].Position.y + 1;

                                      if( yPosTemp < 0 || yPosTemp > World.Room.length - 1 )
                                      {
                                              break;
                                      }
                                      else if( CheckForRoomWall( World, World.Room.Wall[randomExistingWall].Position.x, yPosTemp ) == false )
                                      {
                                              World.Room.Wall[randomExistingWall].Position.y = yPosTemp;
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              case 3: // Wall stands still.
                              {
                                      yPosTemp = World.Room.Wall[randomExistingWall].Position.y;

                                      if( failCounter == 10 )
                                      {
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              default:
                              {
                                      break;
                              }
                      }

                      switch( xRand )
                      {
                              case 1: // Wall goes left.
                              {
                                      xPosTemp = World.Room.Wall[randomExistingWall].Position.x - 1;

                                      if( xPosTemp < 0 || xPosTemp > World.Room.width - 1 )
                                      {
                                              break;
                                      }
                                      else if( CheckForRoomWall( World, xPosTemp, World.Room.Wall[randomExistingWall].Position.y ) == false )
                                      {
                                              World.Room.Wall[randomExistingWall].Position.x = xPosTemp;
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              case 2: // Wall goes right.
                              {
                                      xPosTemp = World.Room.Wall[randomExistingWall].Position.x + 1;

                                      if( xPosTemp < 0 || xPosTemp > World.Room.width - 1 )
                                      {
                                              break;
                                      }
                                      else if( CheckForRoomWall( World, xPosTemp, World.Room.Wall[randomExistingWall].Position.y ) == false )
                                      {
                                              World.Room.Wall[randomExistingWall].Position.x = xPosTemp;
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              case 3: // Wall stands still.
                              {
                                      xPosTemp = World.Room.Wall[randomExistingWall].Position.x;

                                      if( failCounter == 10 )
                                      {
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              default:
                              {
                                      break;
                              }
                      }
              }

              Temp.Position.x = xPosTemp;
              Temp.Position.y = yPosTemp;
              World.Room.Wall.push_back( Temp );
      }
}*/
void SetPlayerPosition( WorldStruct &World )
{
	World.Unit.Player.Position.x = World.Room.width - 2;
	World.Unit.Player.Position.y = World.Room.length - 2;   // Bottom right corner.
}
void ChooseMonsterAmount( WorldStruct &World )
{
	const int playerProtectRange = 9;       // From CheckForSpawnNearPlayer( const WorldStruct &World, int xPos, int yPos ).

	std::cout << "\nEnter amount of monsters: ";

	while( true )
	{
		World.Unit.Allmonsters.amountDesired = GetPositiveInteger( );

		if( World.Unit.Allmonsters.amountDesired >= World.Room.length * World.Room.width - playerProtectRange - World.Room.Allwalls.amountAll &&
			World.Unit.Allmonsters.amountDesired != 0 )
		{
			std::cout << "Too many monsters, try again: ";
			continue;
		}
		else
		{
			break;
		}
	}
}
void SetRandomMonsterPositions( WorldStruct &World )
{
	int xPosTemp, yPosTemp;
	MonsterStruct Temp;

	for( int i = 0; i < World.Unit.Allmonsters.amountDesired; i++ )
	{
		while( true )
		{
			xPosTemp = randomNumberGenerator( 1, World.Room.width - 2 );    // Between 1 and 8 if default RoomWidth value (10).
			yPosTemp = randomNumberGenerator( 1, World.Room.length - 2 );   // Between 1 and 8 if default RoomLength value (10).

			if( CheckForMonsterPosition( World, xPosTemp, yPosTemp ) == false &&
				CheckForSpawnNearPlayer( World, xPosTemp, yPosTemp ) == false &&
				CheckForPlayerPosition( World, xPosTemp, yPosTemp ) == false &&
				CheckForRoomExit( World, xPosTemp, yPosTemp ) == false &&
				CheckForRoomWall( World, xPosTemp, yPosTemp ) == false )
			{
				break;
			}
		}

		if( World.Unit.Allmonsters.amountCurrent < World.Unit.Allmonsters.amountDesired )
		{
			World.Unit.Allmonsters.amountCurrent++;
		}	// To avoid trying to access unvalid memory of monsterpositions before its allocated inside CheckForMonsterPosition( ).

		Temp.Position.x = xPosTemp;
		Temp.Position.y = yPosTemp;
		World.Unit.Monster.push_back( Temp );
	}
}

void DrawRoom( const WorldStruct &World )
{
	std::cout << "\n";

	for( int yDraw = 0; yDraw < World.Room.length; yDraw++ )
	{
		for( int xDraw = 0; xDraw < World.Room.width; xDraw++ )
		{
			if( CheckForRoomExit( World, xDraw, yDraw ) == true )
			{
				std::cout << World.Room.Exit.icon;              // '='.
			}
			else if( CheckForRoomWall( World, xDraw, yDraw ) == true )
			{
				std::cout << World.Room.Allwalls.icon;          // '#'.
			}
			else if( CheckForMonsterPosition( World, xDraw, yDraw ) == true )
			{
				std::cout << World.Unit.Allmonsters.icon;       // 'M'.
			}
			else if( CheckForPlayerPosition( World, xDraw, yDraw ) == true )
			{
				std::cout << World.Unit.Player.icon;            // 'P'.
			}
			else
			{
				std::cout << "-";
			}
		}

		std::cout << "\n";
	}
}
void SayTurnOptions( )
{
	std::cout << "\n";
	std::cout << "[W] Go up.\n";
	std::cout << "[S] Go down.\n";
	std::cout << "[A] Go left.\n";
	std::cout << "[D] Go Right.\n";
	std::cout << "[Q] Do nothing.\n";
	std::cout << "[E] Exit game.\n";
}
void ChooseTurnOptions( WorldStruct &World )
{
	std::string userChoice;
	int yPosTemp, xPosTemp;

	std::cout << "\nYour choice: ";
	RETRY:
	std::cin >> userChoice;

	switch( userChoice[0] )
	{
		case 'W':
		case 'w':       // Move up.
		{
			yPosTemp = World.Unit.Player.Position.y - 1;

			if( CheckForRoomWall( World, World.Unit.Player.Position.x, yPosTemp ) == false )
			{
				World.Unit.Player.Position.y = yPosTemp;
			}

			break;
		}

		case 'S':
		case 's':       // Move down.
		{
			yPosTemp = World.Unit.Player.Position.y + 1;

			if( CheckForRoomWall( World, World.Unit.Player.Position.x, yPosTemp ) == false )
			{
				World.Unit.Player.Position.y = yPosTemp;
			}

			break;
		}

		case 'A':
		case 'a':       // Move left.
		{
			xPosTemp = World.Unit.Player.Position.x - 1;

			if( CheckForRoomWall( World, xPosTemp, World.Unit.Player.Position.y ) == false )
			{
				World.Unit.Player.Position.x = xPosTemp;
			}

			break;
		}

		case 'D':
		case 'd':       // Move right.
		{
			xPosTemp = World.Unit.Player.Position.x + 1;

			if( CheckForRoomWall( World, xPosTemp, World.Unit.Player.Position.y ) == false )
			{
				World.Unit.Player.Position.x = xPosTemp;
			}

			break;
		}

		case 'Q':
		case 'q':       // Do nothing.
		{
			break;
		}

		case 'E':
		case 'e':       // Exit game.
		{
			exit( 0 );
		}

		default:
		{
			std::cout << "Invalid input, try again: ";
			goto RETRY;
		}
	}
}
void RandomizeMonsterMovement( WorldStruct &World )
{
	int xPosTemp;
	int yPosTemp;
	int randomNumber;

	for( int i = 0; i < World.Unit.Allmonsters.amountCurrent; i++ )
	{
		while( true )
		{
			randomNumber = randomNumberGenerator( 0, 15 );

			if( randomNumber < 4 ) // Monster moves.
			{
				if( randomNumber < 2 ) // Monster moves horizontally;
				{
					xPosTemp = World.Unit.Monster[i].Position.x + randomBooleanGenerator( );
					yPosTemp = World.Unit.Monster[i].Position.y;
				}
				else // Monster moves vertically.
				{
					xPosTemp = World.Unit.Monster[i].Position.x;
					yPosTemp = World.Unit.Monster[i].Position.y + randomBooleanGenerator( );
				}
			}
			else // Monster stands still.
			{
				break;
			}

			if( CheckForMonsterPosition( World, xPosTemp, yPosTemp ) == false &&
				CheckForRoomExit       ( World, xPosTemp, yPosTemp ) == false &&
				CheckForRoomWall       ( World, xPosTemp, yPosTemp ) == false )
			{
				World.Unit.Monster[i].Position.x = xPosTemp;
				World.Unit.Monster[i].Position.y = yPosTemp;
			}
		}
	}
}

bool ChooseRestartGame( )
{
	std::string userChoice;

	std::cout << "\n\nDo you wish to restart the game, [y/n]: ";
	while( true )
	{
		std::cin >> userChoice;

		if( userChoice[0] == 'Y' || userChoice[0] == 'y' )
		{
			return true;
		}
		else if( userChoice[0] == 'N' || userChoice[0] == 'n' )
		{
			return false;
		}
		else
		{
			std::cout << "Invalid input, try again: ";
		}
	}
}

int main( )
{
	do
	{
		WorldStruct World;

		SayGameRules( );
		SetRoomSize( World );
		SetRoomExits( World );
		SetRoomOuterWalls( World );
		SetPlayerPosition( World );
		ChooseMonsterAmount( World );
		SetRandomMonsterPositions( World );

		while( true )
		{
			system( "CLS" );
			DrawRoom( World );
			if( CheckForWinLoseCondition( World ) == true )
			{
				break;
			}
			SayTurnOptions( );
			ChooseTurnOptions( World );
			RandomizeMonsterMovement( World );
		}
	}
	while( ChooseRestartGame( ) == true );

	return 0;
}