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
	std::vector<PositionStruct> Position;
	char icon = 'M';
	int amountDesired = 0;
	int amountCurrent = 0;
};
struct UnitStruct
{
	MonsterStruct Monster;
	PlayerStruct Player;
};
struct WallStruct
{
	std::vector<PositionStruct> Position;
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
	WallStruct Wall;
	ExitStruct Exit;
	int length;
	int width;
};
struct InstanceStruct
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

bool CheckForRoomExit( const InstanceStruct &Instance, int xPos, int yPos )
{
	for( int i = 0; i < Instance.Room.Exit.amount; i++ )
	{
		if( xPos == Instance.Room.Exit.Position.x &&
			yPos == Instance.Room.Exit.Position.y )
		{
			return true;
		}
	}

	return false;
}
bool CheckForRoomWall( const InstanceStruct &Instance, int xPos, int yPos )
{
	for( int i = 0; i < Instance.Room.Wall.amountAll; i++ )
	{
		if( xPos == Instance.Room.Wall.Position[i].x &&
			yPos == Instance.Room.Wall.Position[i].y )
		{
			return true;
		}
	}

	return false;
}
bool CheckForPlayer( const InstanceStruct &Instance, int xPos, int yPos )
{
	if( xPos == Instance.Unit.Player.Position.x &&
		yPos == Instance.Unit.Player.Position.y )
	{
		return true;
	}

	return false;
}
bool CheckForMonster( const InstanceStruct &Instance, int xPos, int yPos )
{
	for( int i = 0; i < Instance.Unit.Monster.amountCurrent; i++ )
	{
		if( xPos == Instance.Unit.Monster.Position[i].x &&
			yPos == Instance.Unit.Monster.Position[i].y )
		{
			return true;
		}
	}

	return false;
}
bool CheckForSpawnNearPlayer( const InstanceStruct &Instance, int xPos, int yPos )
{
	for( int yProtect = Instance.Unit.Player.Position.y - 2; yProtect < Instance.Unit.Player.Position.y + 2; yProtect++ )
	{
		for( int xProtect = Instance.Unit.Player.Position.x - 2; xProtect < Instance.Unit.Player.Position.x + 2; xProtect++ )
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
bool CheckForWinLoseCondition( const InstanceStruct &Instance )
{
	if( CheckForRoomExit( Instance, Instance.Unit.Player.Position.x, Instance.Unit.Player.Position.y ) == true )
	{
		std::cout << "\nYou win!";
		return true;
	}
	else if( CheckForMonster( Instance, Instance.Unit.Player.Position.x, Instance.Unit.Player.Position.y ) == true )
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
	std::cout << "Win  condition: Enter the exit.\n";
	std::cout << "Lose condition: Enter the same space as a monster.\n";
}
void SetRoomSize( InstanceStruct &Instance )
{
	std::string userChoice;

	std::cout << "Would you like to determine the room dimensions, [y/n]: ";
	while( true )
	{
		std::cin >> userChoice;

		if( userChoice[0] == 'Y' || userChoice[0] == 'y' )
		{
			std::cout << "Enter the playing Room length: ";
			Instance.Room.length = GetPositiveInteger( );
			std::cout << "Enter the playing Room width: ";
			Instance.Room.width = GetPositiveInteger( );
			break;
		}
		else if( userChoice[0] == 'N' || userChoice[0] == 'n' )
		{
			Instance.Room.length = 10;
			Instance.Room.width = 10;
			break;
		}
		else
		{
			std::cout << "Invalid input, try again: ";
		}
	}
}
void SetRoomExits( InstanceStruct &Instance )
{
	Instance.Room.Exit.amount = 1;	// Temporary.
	Instance.Room.Exit.Position.x = 1;
	Instance.Room.Exit.Position.y = 0; // Top left corner.
}
void SetRoomOuterWalls( InstanceStruct &Instance )
{
	PositionStruct Temp;

	for( int ySet = 0; ySet < Instance.Room.length; ySet++ )
	{
		for( int xSet = 0; xSet < Instance.Room.width; xSet++ )
		{
			if( CheckForRoomExit( Instance, xSet, ySet ) == true )
			{
				continue;
			}
			else if( xSet == 0 || xSet == Instance.Room.width - 1 ||
					 ySet == 0 || ySet == Instance.Room.length - 1 )
			{
				Temp.x = xSet;
				Temp.y = ySet;
				Instance.Room.Wall.Position.push_back( Temp );
				Instance.Room.Wall.amountAll++;
				Instance.Room.Wall.amountOuter++;
			}
		}
	}
}
/*bool SetOneExtendingInnerWall( InstanceStruct &Instance, int chosenWall )
{
	int xPosTemp, yPosTemp;

	if( ( Instance.Room.Wall[chosenWall].Position.x == 0 || Instance.Room.Wall[chosenWall].Position.x == Instance.Room.width - 1 ) &&
		( Instance.Room.Wall[chosenWall].Position.y == 0 || Instance.Room.Wall[chosenWall].Position.y == Instance.Room.length - 1 ) )

	return false;   // Did not manage to create an extending wall.
}*/
/*void SetRoomInnerWalls( InstanceStruct &Instance )
{
      WallStruct Temp;
      int amountExtendingWalls;
      int randomExistingWall;
      int xRand, yRand;
      int xPosTemp = 0;
      int yPosTemp = 0;
      int failCounter;
      bool RetryLoop;
     
      amountExtendingWalls = 1 + rand( ) % ( Instance.Room.Allwalls.amountAll ); // No real logic here.

      for( int i = 0; i < amountExtendingWalls; i++ )
      {
              RetryLoop = true;
              randomExistingWall = 1 + rand( ) % Instance.Room.Wall.size( );
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
                                      yPosTemp = Instance.Room.Wall[randomExistingWall].Position.y - 1;

                                      if( yPosTemp < 0 || yPosTemp > Instance.Room.length - 1  )
                                      {
                                              break;
                                      }
                                      else if( CheckForRoomWall( Instance, Instance.Room.Wall[randomExistingWall].Position.x, yPosTemp ) == false )
                                      {
                                              Instance.Room.Wall[randomExistingWall].Position.y = yPosTemp;
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              case 2: // Wall goes down.
                              {
                                      yPosTemp = Instance.Room.Wall[randomExistingWall].Position.y + 1;

                                      if( yPosTemp < 0 || yPosTemp > Instance.Room.length - 1 )
                                      {
                                              break;
                                      }
                                      else if( CheckForRoomWall( Instance, Instance.Room.Wall[randomExistingWall].Position.x, yPosTemp ) == false )
                                      {
                                              Instance.Room.Wall[randomExistingWall].Position.y = yPosTemp;
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              case 3: // Wall stands still.
                              {
                                      yPosTemp = Instance.Room.Wall[randomExistingWall].Position.y;

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
                                      xPosTemp = Instance.Room.Wall[randomExistingWall].Position.x - 1;

                                      if( xPosTemp < 0 || xPosTemp > Instance.Room.width - 1 )
                                      {
                                              break;
                                      }
                                      else if( CheckForRoomWall( Instance, xPosTemp, Instance.Room.Wall[randomExistingWall].Position.y ) == false )
                                      {
                                              Instance.Room.Wall[randomExistingWall].Position.x = xPosTemp;
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              case 2: // Wall goes right.
                              {
                                      xPosTemp = Instance.Room.Wall[randomExistingWall].Position.x + 1;

                                      if( xPosTemp < 0 || xPosTemp > Instance.Room.width - 1 )
                                      {
                                              break;
                                      }
                                      else if( CheckForRoomWall( Instance, xPosTemp, Instance.Room.Wall[randomExistingWall].Position.y ) == false )
                                      {
                                              Instance.Room.Wall[randomExistingWall].Position.x = xPosTemp;
                                              RetryLoop = false;
                                      }

                                      break;
                              }

                              case 3: // Wall stands still.
                              {
                                      xPosTemp = Instance.Room.Wall[randomExistingWall].Position.x;

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
              Instance.Room.Wall.push_back( Temp );
      }
}*/
void SetPlayerPosition( InstanceStruct &Instance )
{
	Instance.Unit.Player.Position.x = Instance.Room.width - 2;
	Instance.Unit.Player.Position.y = Instance.Room.length - 2;   // Bottom right corner.
}
void ChooseMonsterAmount( InstanceStruct &Instance )
{
	const int playerProtectRange = 9;       // From CheckForSpawnNearPlayer( const InstanceStruct &Instance, int xPos, int yPos ).

	std::cout << "Enter amount of monsters: ";

	while( true )
	{
		Instance.Unit.Monster.amountDesired = GetPositiveInteger( );

		if( Instance.Unit.Monster.amountDesired >= Instance.Room.length * Instance.Room.width - playerProtectRange - Instance.Room.Wall.amountAll &&
			Instance.Unit.Monster.amountDesired != 0 )
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
void SetRandomMonsterPositions( InstanceStruct &Instance )
{
	int xPosTemp;
	int yPosTemp;

	for( int i = 0; i < Instance.Unit.Monster.amountDesired; i++ )
	{
		while( true )
		{
			xPosTemp = randomNumberGenerator( 1, Instance.Room.width - 2 );    // Between 1 and 8 if default RoomWidth value (10).
			yPosTemp = randomNumberGenerator( 1, Instance.Room.length - 2 );   // Between 1 and 8 if default RoomLength value (10).

			if( CheckForSpawnNearPlayer( Instance, xPosTemp, yPosTemp ) == false &&
				CheckForRoomExit       ( Instance, xPosTemp, yPosTemp ) == false &&
				CheckForRoomWall       ( Instance, xPosTemp, yPosTemp ) == false &&
				CheckForMonster        ( Instance, xPosTemp, yPosTemp ) == false &&
				CheckForPlayer         ( Instance, xPosTemp, yPosTemp ) == false )
			{
				break;
			}
		}

		if( Instance.Unit.Monster.amountCurrent < Instance.Unit.Monster.amountDesired )
		{
			Instance.Unit.Monster.amountCurrent++;
		}	// To avoid trying to access unvalid memory of monsterpositions before its allocated inside CheckForMonsterPosition( ).

		PositionStruct Temp;
		Temp.x = xPosTemp;
		Temp.y = yPosTemp;
		Instance.Unit.Monster.Position.push_back( Temp );
	}
}

void DrawRoom( const InstanceStruct &Instance )
{
	for( int drawVertical = 0; drawVertical < Instance.Room.length; drawVertical++ )
	{
		for( int drawHorizontal = 0; drawHorizontal < Instance.Room.width; drawHorizontal++ )
		{
			if( CheckForRoomExit( Instance, drawHorizontal, drawVertical ) == true )
			{
				std::cout << Instance.Room.Exit.icon;          // '='.
			}
			else if( CheckForRoomWall( Instance, drawHorizontal, drawVertical ) == true )
			{
				std::cout << Instance.Room.Wall.icon;          // '#'.
			}
			else if( CheckForMonster( Instance, drawHorizontal, drawVertical ) == true )
			{
				std::cout << Instance.Unit.Monster.icon;       // 'M'.
			}
			else if( CheckForPlayer( Instance, drawHorizontal, drawVertical ) == true )
			{
				std::cout << Instance.Unit.Player.icon;        // 'P'.
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
	std::cout << "[W] Go up.\n";
	std::cout << "[S] Go down.\n";
	std::cout << "[A] Go left.\n";
	std::cout << "[D] Go Right.\n";
	std::cout << "[Q] Do nothing.\n";
	std::cout << "[E] Exit game.\n";
}
void ChooseTurnOptions( InstanceStruct &Instance )
{
	std::string userChoice;
	int yPosTemp, xPosTemp;

	RETRY:
	std::cout << "\nYour choice: ";
	std::cin >> userChoice;

	switch( userChoice[0] )
	{
		case 'W':
		case 'w':       // Move up.
		{
			xPosTemp = Instance.Unit.Player.Position.x;
			yPosTemp = Instance.Unit.Player.Position.y - 1;

			break;
		}

		case 'S':
		case 's':       // Move down.
		{
			xPosTemp = Instance.Unit.Player.Position.x;
			yPosTemp = Instance.Unit.Player.Position.y + 1;

			break;
		}

		case 'A':
		case 'a':       // Move left.
		{
			xPosTemp = Instance.Unit.Player.Position.x - 1;
			yPosTemp = Instance.Unit.Player.Position.y;

			break;
		}

		case 'D':
		case 'd':       // Move right.
		{
			xPosTemp = Instance.Unit.Player.Position.x + 1;
			yPosTemp = Instance.Unit.Player.Position.y;

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
			std::cout << "Invalid input, try again\n";
			goto RETRY;
		}
	}

	if( CheckForRoomWall( Instance, xPosTemp, yPosTemp ) == false )
	{
		Instance.Unit.Player.Position.x = xPosTemp;
		Instance.Unit.Player.Position.y = yPosTemp;
	}
}
void RandomizeMonsterMovement( InstanceStruct &Instance )
{
	int xPosTemp;
	int yPosTemp;
	int randomNumber;

	for( int i = 0; i < Instance.Unit.Monster.amountCurrent; i++ )
	{
		while( true )
		{
			randomNumber = randomNumberGenerator( 1, 16 );

			if( randomNumber <= 2 ) // Monster moves vertically.
			{
				xPosTemp = Instance.Unit.Monster.Position[i].x;
				yPosTemp = Instance.Unit.Monster.Position[i].y + randomBooleanGenerator( );
			}
			else if( randomNumber <= 4 ) // Monster moves horizontally.
			{
				xPosTemp = Instance.Unit.Monster.Position[i].x + randomBooleanGenerator( );
				yPosTemp = Instance.Unit.Monster.Position[i].y;
			}
			else // Monster stands still.
			{
				break;
			}

			if( CheckForRoomExit( Instance, xPosTemp, yPosTemp ) == false &&
				CheckForRoomWall( Instance, xPosTemp, yPosTemp ) == false &&
				CheckForMonster ( Instance, xPosTemp, yPosTemp ) == false )
			{
				Instance.Unit.Monster.Position[i].x = xPosTemp;
				Instance.Unit.Monster.Position[i].y = yPosTemp;
				break;
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
		std::vector<InstanceStruct> Instance;
		InstanceStruct Temp;
		Instance.push_back( Temp );

		system( "CLS" );
		SayGameRules( );
		std::cout << std::endl;
		SetRoomSize( Instance[0] );
		SetRoomExits( Instance[0] );
		SetRoomOuterWalls( Instance[0] );
		SetPlayerPosition( Instance[0] );
		std::cout << std::endl;
		ChooseMonsterAmount( Instance[0] );
		SetRandomMonsterPositions( Instance[0] );

		while( true )
		{
			system( "CLS" );
			DrawRoom( Instance[0] );
			if( CheckForWinLoseCondition( Instance[0] ) == true )
			{
				break;
			}
			std::cout << std::endl;
			SayTurnOptions( );
			ChooseTurnOptions( Instance[0] );
			RandomizeMonsterMovement( Instance[0] );
		}
	}
	while( ChooseRestartGame( ) == true );

	return 0;
}