#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <math.h>

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
};
struct ExitStruct
{
	std::vector<PositionStruct> Position;
	char icon = '=';
	int amount = 0;
};
struct RoomStruct
{
	WallStruct Wall;
	ExitStruct Exit;
	std::vector< std::vector<char> > staticDataMap;
	std::vector< std::vector<char> > completeDataMap;
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
int RandomNumberGenerator( int min, int max )
{
	static std::random_device randomEngine;
	std::mt19937 generator( randomEngine( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}
int RandomBooleanGenerator( )
{
	int randomNumber;

	do
	{
		randomNumber = RandomNumberGenerator( -1, 1 );
	}
	while( randomNumber == 0 );

	return randomNumber;
}

bool CheckForRoomExit( const InstanceStruct &Instance, int xPos, int yPos )
{
	for( int i = 0; i < Instance.Room.Exit.amount; i++ )
	{
		if( xPos == Instance.Room.Exit.Position[i].x &&
			yPos == Instance.Room.Exit.Position[i].y )
		{
			return true;
		}
	}

	return false;
}
bool CheckForRoomWall( const InstanceStruct &Instance, int xPos, int yPos, int startIterator )
{
	for( int i = startIterator; i < Instance.Room.Wall.amountAll; i++ )
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
	int range = 3;

	for( int yProtect = Instance.Unit.Player.Position.y - range; yProtect < Instance.Unit.Player.Position.y + range; yProtect++ )
	{
		for( int xProtect = Instance.Unit.Player.Position.x - range; xProtect < Instance.Unit.Player.Position.x + range; xProtect++ )
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
bool CheckForSpawnNearExits( const InstanceStruct &Instance, int xPos, int yPos )
{
	int range = 3;

	for( int i = 0; i < Instance.Room.Exit.amount; i++ )
	{
		for( int yProtect = Instance.Room.Exit.Position[i].y - range; yProtect < Instance.Room.Exit.Position[i].y + range; yProtect++ )
		{
			for( int xProtect = Instance.Room.Exit.Position[i].x - range; xProtect < Instance.Room.Exit.Position[i].x + range; xProtect++ )
			{
				if( xPos == xProtect &&
					yPos == yProtect )
				{
					return true;
				}
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

void SetStaticDataMap( InstanceStruct &Instance )
{
	if( Instance.Room.staticDataMap.size( ) == 0 )
	{
		Instance.Room.staticDataMap.resize( Instance.Room.length, std::vector<char>( Instance.Room.width ) );
	}

	for( int y = 0; y < Instance.Room.length; y++ )
	{
		for( int x = 0; x < Instance.Room.width; x++ )
		{
			if( CheckForRoomExit( Instance, x, y ) == true )
			{
				Instance.Room.staticDataMap[y][x] = '=';
			}
			else if( CheckForRoomWall( Instance, x, y, 0 ) == true )
			{
				Instance.Room.staticDataMap[y][x] = '#';
			}
			else
			{
				Instance.Room.staticDataMap[y][x] = '-';
			}
		}
	}
}
void SetCompleteDataMap( InstanceStruct &Instance )
{
	Instance.Room.completeDataMap = Instance.Room.staticDataMap;

	for( int y = 0; y < Instance.Room.length; y++ )
	{
		for( int x = 0; x < Instance.Room.width; x++ )
		{
			if( CheckForMonster( Instance, x, y ) == true )
			{
				Instance.Room.completeDataMap[y][x] = 'M';
			}
			else if( CheckForPlayer( Instance, x, y ) == true )
			{
				Instance.Room.completeDataMap[y][x] = 'P';
			}
		}
	}
}
bool EmptySurroundedTile( InstanceStruct &Instance, int xPos, int yPos )
{
	int surroundingWalls = 0;

	for( int y = -1; y <= 1; y++ )
	{
		for( int x = -1; y <= 1; y++ )
		{
			if( xPos == 0 || xPos == Instance.Room.width - 1 ||
				yPos == 0 || yPos == Instance.Room.length - 1 )
			{
				continue;
			}
			else if( CheckForRoomWall( Instance, xPos + x, yPos + y, Instance.Room.Wall.amountOuter ) == true )
			{
				surroundingWalls++;
			}
		}
	}

	if( surroundingWalls > 5 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void PrintGameRules( )
{
	std::cout << "Win  condition: Enter the exit.\n";
	std::cout << "Lose condition: Enter the same space as a monster.\n";
}
void SetRoomSize( InstanceStruct &Instance )
{
	std::string userChoice;

	std::cout << "\nWould you like to determine the room dimensions, [y/n]: ";
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
			Instance.Room.length = RandomNumberGenerator( 15, 30 );
			Instance.Room.width = RandomNumberGenerator( 50, 100 );
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
	PositionStruct Temp;
	Instance.Room.Exit.Position.push_back( Temp );
	Instance.Room.Exit.amount++;
	
	//for( int i = 0; i < 100; i++ )
	//{
	//	int n = RandomNumberGenerator( 0, Instance.Room.Wall.Position.size( ) - 1 );
	//	if(  )
	//}

	Instance.Room.Exit.Position[0].x = Instance.Room.width - 2;  // Default
	Instance.Room.Exit.Position[0].y = Instance.Room.length - 1; // bottom right corner.
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
void SetRoomRandomWalls( InstanceStruct &Instance )
{
	PositionStruct Temp;
	double high = sqrt( Instance.Room.length * Instance.Room.width ) * 1.5;
	double low = sqrt( Instance.Room.length * Instance.Room.width ) / 1.5;
	int randomSourceWalls = RandomNumberGenerator( (int)low, (int)high );
	int randomTries = RandomNumberGenerator( 5, 7 );

	while( randomSourceWalls > 0 ) // Place source walls.
	{
		for( int y = 0; y < Instance.Room.length; y++ )
		{
			for( int x = 0; x < Instance.Room.width; x++ )
			{
				if( CheckForRoomWall( Instance, x, y, 0 ) == false &&
					CheckForSpawnNearPlayer( Instance, x, y ) == false &&
					CheckForSpawnNearExits( Instance, x, y ) == false &&
					RandomNumberGenerator( 1, 1000 ) == 1 )
				{
					Temp.x = x;
					Temp.y = y;
					Instance.Room.Wall.Position.push_back( Temp );
					Instance.Room.Wall.amountAll++;
					randomSourceWalls--;
				}
			}
		}
	}

	std::cout << "\nLoading, please wait.\n";

	for( int i = 0; i < randomTries; i++ ) // Place extension walls.
	{
		std::cout << ".";

		for( int y = 0; y < Instance.Room.length; y++ )
		{
			for( int x = 0; x < Instance.Room.width; x++ )
			{
				if( CheckForRoomWall( Instance, x, y, Instance.Room.Wall.amountOuter ) == true )
				{
					Temp.x = x + RandomBooleanGenerator( ) * RandomNumberGenerator( 0, 1 );
					Temp.y = y + RandomBooleanGenerator( ) * RandomNumberGenerator( 0, 1 );

					if( CheckForRoomWall( Instance, Temp.x, Temp.y, Instance.Room.Wall.amountOuter ) == true ||
						CheckForSpawnNearPlayer( Instance, x, y ) == true &&
						CheckForSpawnNearExits( Instance, x, y ) == true )
					{
						continue;
					}

					Instance.Room.Wall.Position.push_back( Temp );
					Instance.Room.Wall.amountAll++;
				}
				else if( CheckForRoomWall( Instance, x, y, Instance.Room.Wall.amountOuter ) == false &&
						 EmptySurroundedTile( Instance, x, y ) == true )
				{
					Temp.x = x;
					Temp.y = y;
					Instance.Room.Wall.Position.push_back( Temp );
					Instance.Room.Wall.amountAll++;
				}
			}
		}
	}

	std::cout << std::endl;
}
void SetPlayerPosition( InstanceStruct &Instance )
{
	Instance.Unit.Player.Position.x = 1;
	Instance.Unit.Player.Position.y = 1;   // Top left corner.
}
void ChooseMonsterAmount( InstanceStruct &Instance )
{
	int maxMonsters = ( Instance.Room.length * Instance.Room.width ) - Instance.Room.Wall.amountAll;

	std::cout << "\nEnter amount of monsters: ";
	while( true )
	{
		Instance.Unit.Monster.amountDesired = GetPositiveInteger( );

		if( Instance.Unit.Monster.amountDesired >= maxMonsters &&
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
			xPosTemp = RandomNumberGenerator( 1, Instance.Room.width - 2 );
			yPosTemp = RandomNumberGenerator( 1, Instance.Room.length - 2 );

			//if( CheckForSpawnNearPlayer( Instance, xPosTemp, yPosTemp ) == false &&
			//	CheckForRoomExit       ( Instance, xPosTemp, yPosTemp ) == false &&
			//	CheckForRoomWall       ( Instance, xPosTemp, yPosTemp ) == false &&
			//	CheckForMonster        ( Instance, xPosTemp, yPosTemp ) == false &&
			//	CheckForPlayer         ( Instance, xPosTemp, yPosTemp ) == false )
			//{
			//	break;
			//}

			if( CheckForSpawnNearPlayer( Instance, xPosTemp, yPosTemp ) == false &&
				CheckForMonster( Instance, xPosTemp, yPosTemp ) == false &&
				Instance.Room.staticDataMap[yPosTemp][xPosTemp] == '-' )
			{
				break;
			}
		}

		if( Instance.Unit.Monster.amountCurrent < Instance.Unit.Monster.amountDesired )
		{
			Instance.Unit.Monster.amountCurrent++;
		}	// To avoid trying to access unvalid memory of monsterpositions
		    // before its allocated inside CheckForMonsterPosition( ).

		PositionStruct Temp;
		Temp.x = xPosTemp;
		Temp.y = yPosTemp;
		Instance.Unit.Monster.Position.push_back( Temp );
	}
}

void DrawRoom( const InstanceStruct &Instance )
{
	for( int y = 0; y < Instance.Room.length; y++ )
	{
		for( int x = 0; x < Instance.Room.width; x++ )
		{
			std::cout << Instance.Room.completeDataMap[y][x];
		}

		std::cout << "\n";
	}

	std::cout << std::endl;
}
void SayTurnOptions( )
{
	std::cout << "[W] Go up.\n";
	std::cout << "[S] Go down.\n";
	std::cout << "[A] Go left.\n";
	std::cout << "[D] Go Right.\n";
	std::cout << "[Q] Do nothing.\n";
	std::cout << "[E] Exit game.\n\n";
}
void ChooseTurnOptions( InstanceStruct &Instance )
{
	std::string userChoice;
	int yPosTemp, xPosTemp;

	RETRY:
	std::cout << "Your choice: ";
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
			return;
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

	if( Instance.Room.completeDataMap[yPosTemp][xPosTemp] != '#' )
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
			randomNumber = RandomNumberGenerator( 1, 16 );

			if( randomNumber <= 2 ) // Monster moves vertically.
			{
				xPosTemp = Instance.Unit.Monster.Position[i].x;
				yPosTemp = Instance.Unit.Monster.Position[i].y + RandomBooleanGenerator( );
			}
			else if( randomNumber <= 4 ) // Monster moves horizontally.
			{
				xPosTemp = Instance.Unit.Monster.Position[i].x + RandomBooleanGenerator( );
				yPosTemp = Instance.Unit.Monster.Position[i].y;
			}
			else // Monster stands still.
			{
				break;
			}

			//if( CheckForRoomExit( Instance, xPosTemp, yPosTemp ) == false &&
			//	CheckForRoomWall( Instance, xPosTemp, yPosTemp ) == false &&
			//	CheckForMonster ( Instance, xPosTemp, yPosTemp ) == false )
			//{
			//	Instance.Unit.Monster.Position[i].x = xPosTemp;
			//	Instance.Unit.Monster.Position[i].y = yPosTemp;
			//	break;
			//}

			if( Instance.Room.completeDataMap[yPosTemp][xPosTemp] == '-' ||
				Instance.Room.completeDataMap[yPosTemp][xPosTemp] == 'P')
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
		Instance.resize( 10 );

		system( "CLS" );
		PrintGameRules( );
		SetRoomSize( Instance[0] );
		SetRoomExits( Instance[0] );
		SetRoomOuterWalls( Instance[0] );
		SetRoomRandomWalls( Instance[0] );
		SetStaticDataMap( Instance[0] );
		SetPlayerPosition( Instance[0] );
		ChooseMonsterAmount( Instance[0] );
		SetRandomMonsterPositions( Instance[0] );

		while( true )
		{
			system( "CLS" );
			SetCompleteDataMap( Instance[0] );
			DrawRoom( Instance[0] );
			if( CheckForWinLoseCondition( Instance[0] ) == true )
			{
				break;
			}
			SayTurnOptions( );
			ChooseTurnOptions( Instance[0] );
			RandomizeMonsterMovement( Instance[0] );
		}
	}
	while( ChooseRestartGame( ) == true );

	return 0;
}