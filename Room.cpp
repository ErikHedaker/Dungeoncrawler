#include "Room.h"

void Room::lengthSet( int input )
{
	_length = input;
}
void Room::widthSet( int input )
{
	_width = input;
}
void Room::monsterAmountDesiredSet( int input )
{
	_monsterAmountDesired = input;
}
int Room::length( )
{
	return _length;
}
int Room::width( )
{
	return _width;
}
int Room::monsterAmountDesired( )
{
	return _monsterAmountDesired;
}
int Room::outerWallsAmount( )
{
	return _outerWallsAmount;
}
void Room::outerWallsAmountIncrease( int n )
{
	_outerWallsAmount += n;	// Default n = 1;
}
void Room::hiddenDataMapUpdate( )
{
	_hiddenDataMap.clear( );
	_hiddenDataMap.resize( length( ), std::vector<char>( width( ) ) );

	for( int y = 0; y < length( ); y++ )
	{
		for( int x = 0; x < width( ); x++ )
		{
			if( checkPosition( x, y, path ) == true )
			{
				_hiddenDataMap[y][x] = Path::icon;
			}
		}
	}
}
void Room::hiddenDataMapUpdateSingle( int x, int y, int value )
{
	_hiddenDataMap[y][x] = value;
}
void Room::visibleDataMapUpdate( )
{
	_visibleDataMap.clear( );
	_visibleDataMap.resize( length( ), std::vector<char>( width( ) ) );
	_visibleDataMap.assign( length( ), std::vector<char>( width( ), '-' ) );

	for( int y = 0; y < length( ); y++ )
	{
		for( int x = 0; x < width( ); x++ )
		{
			if( checkPosition( x, y, exit ) == true )
			{
				_visibleDataMap[y][x] = Exit::icon;		// '='
			}
			else if( checkPosition( x, y, wall ) == true )
			{
				_visibleDataMap[y][x] = Wall::icon;		// '#'
			}
			else if( checkPosition( x, y, monster ) == true )
			{
				_visibleDataMap[y][x] = Monster::icon;	// 'M'
			}
			else if( checkPosition( x, y, player ) == true )
			{
				_visibleDataMap[y][x] = Player::icon;	// '@'
			}
			else
			{
				_visibleDataMap[y][x] = '-';
			}
		}
	}
}
void Room::visibleDataMapUpdateSingle( int x, int y, char value )
{
	_visibleDataMap[y][x] = value;
}
void Room::visibleDataMapFogOfWarUpdate( int range )
{
	_visibleDataMapFogOfWar.clear( );
	_visibleDataMapFogOfWar.resize( length( ), std::vector<char>( width( ) ) );
	_visibleDataMapFogOfWar.assign( length( ), std::vector<char>( width( ), 0 ) );

	for( int y = 0; y < length( ); y++ )
	{
		for( int x = 0; x < width( ); x++ )
		{
			if( x >= player.x - range && x <= player.x + range &&
				y >= player.y - range && y <= player.y + range )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}
}
void Room::visibleDataMapFogOfWarLineOfSightUpdate( int range )
{
	_visibleDataMapFogOfWar.clear( );
	_visibleDataMapFogOfWar.resize( length( ), std::vector<char>( width( ) ) );
	_visibleDataMapFogOfWar.assign( length( ), std::vector<char>( width( ), 0 ) );

	for( int y = player.y; y < player.y + range; y++ )
	{
		if( y > length( ) || y < 0 )
		{
			break;
		}

		for( int x = player.x; x < player.x + range; x++ )
		{
			if( x > width( ) || x < 0 )
			{
				break;
			}

			if( checkPosition( x, y, wall ) == true ||
				checkPosition( x, y, exit ) == true )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
				//goto next_0;
				break;
			}
			else
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}

	//next_0:

	for( int y = player.y; y > player.y - range; y-- )
	{
		if( y > length( ) || y < 0 )
		{
			break;
		}

		for( int x = player.x; x < player.x + range; x++ )
		{
			if( x > width( ) || x < 0 )
			{
				break;
			}

			if( checkPosition( x, y, wall ) == true ||
				checkPosition( x, y, exit ) == true )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
				//goto next_1;
				break;
			}
			else
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}

	//next_1:

	for( int y = player.y; y < player.y + range; y++ )
	{
		if( y > length( ) || y < 0 )
		{
			break;
		}

		for( int x = player.x; x > player.x - range; x-- )
		{
			if( x > width( ) || x < 0 )
			{
				break;
			}

			if( checkPosition( x, y, wall ) == true ||
				checkPosition( x, y, exit ) == true )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
				//goto next_2;
				break;
			}
			else
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}

	//next_2:

	for( int y = player.y; y > player.y - range; y-- )
	{
		if( y > length( ) || y < 0 )
		{
			break;
		}

		for( int x = player.x; x > player.x - range; x-- )
		{
			if( x > width( ) || x < 0 )
			{
				break;
			}

			if( checkPosition( x, y, wall ) == true ||
				checkPosition( x, y, exit ) == true )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
				//goto next_3;
				break;
			}
			else
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}

	//next_3:

	// Check again, this time y and x loops are reversed.
	// This should fix any visual bugs.

	for( int x = player.x; x < player.x + range; x++ )
	{
		if( x > width( ) || x < 0 )
		{
			break;
		}

		for( int y = player.y; y < player.y + range; y++ )
		{
			if( y > length( ) || y < 0 )
			{
				break;
			}

			if( checkPosition( x, y, wall ) == true ||
				checkPosition( x, y, exit ) == true )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
				//goto next_4;
				break;
			}
			else
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}

	//next_4:

	for( int x = player.x; x > player.x - range; x-- )
	{
		if( x > width( ) || x < 0 )
		{
			break;
		}

		for( int y = player.y; y < player.y + range; y++ )
		{
			if( y > length( ) || y < 0 )
			{
				break;
			}

			if( checkPosition( x, y, wall ) == true ||
				checkPosition( x, y, exit ) == true )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
				//goto next_5;
				break;
			}
			else
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}

	//next_5:

	for( int x = player.x; x < player.x + range; x++ )
	{
		if( x > width( ) || x < 0 )
		{
			break;
		}

		for( int y = player.y; y > player.y - range; y-- )
		{
			if( y > length( ) || y < 0 )
			{
				break;
			}

			if( checkPosition( x, y, wall ) == true ||
				checkPosition( x, y, exit ) == true )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
				//goto next_6;
				break;
			}
			else
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}

	//next_6:

	for( int x = player.x; x > player.x - range; x-- )
	{
		if( x > width( ) || x < 0 )
		{
			break;
		}

		for( int y = player.y; y > player.y - range; y-- )
		{
			if( y > length( ) || y < 0 )
			{
				break;
			}

			if( checkPosition( x, y, wall ) == true ||
				checkPosition( x, y, exit ) == true )
			{
				_visibleDataMapFogOfWar[y][x] = 1;
				//return;
				break;
			}
			else
			{
				_visibleDataMapFogOfWar[y][x] = 1;
			}
		}
	}
}
char Room::hiddenDataMap( int x, int y )
{
	return _hiddenDataMap[y][x];
}
char Room::visibleDataMap( int x, int y )
{
	return _visibleDataMap[y][x];
}
char Room::visibleDataMapFogOfWar( int x, int y )
{
	return _visibleDataMapFogOfWar[y][x];
}