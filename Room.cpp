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
void Room::staticDataMap( )
{
	if( _staticDataMap.size( ) == 0 )
	{
		_staticDataMap.resize( length( ), std::vector<char>( width( ) ) );
	}

	for( int y = 0; y < length( ); y++ )
	{
		for( int x = 0; x < width( ); x++ )
		{
			if( checkPosition( x, y, exit ) == true )
			{
				_staticDataMap[y][x] = Exit::icon;	// '='
			}
			else if( checkPosition( x, y, wall ) == true )
			{
				_staticDataMap[y][x] = Wall::icon;	// '#'
			}
			else if( checkPosition( x, y, path ) == true )
			{
				_staticDataMap[y][x] = Path::icon;	// ':', only visible in _staticDataMap.
			}
			else
			{
				_staticDataMap[y][x] = '-';	// The floor.
			}
		}
	}
}
void Room::completeDataMap( )
{
	_completeDataMap = _staticDataMap;		// Initial overwrite.

	for( int y = 0; y < length( ); y++ )	// Overwriting values of staticDataMap( ).
	{
		for( int x = 0; x < width( ); x++ )
		{
			if( checkPosition( x, y, monster ) == true )
			{
				_completeDataMap[y][x] = Monster::icon;	// 'M'
			}
			else if( checkPosition( x, y, player ) == true )
			{
				_completeDataMap[y][x] = Player::icon;	// '@'
			}
			else if( checkPosition( x, y, path ) == true )
			{
				_completeDataMap[y][x] = '-';	// Path is now invisible.
			}
		}
	}
}
void Room::visibleDataMap( int range )
{
	if( _visibleDataMap.size( ) == 0 )
	{
		_visibleDataMap.resize( length( ), std::vector<char>( width( ) ) );
		_visibleDataMap.assign( length( ), std::vector<char>( width( ), 0 ) );
	}

	for( int y = 0; y < length( ); y++ )
	{
		for( int x = 0; x < width( ); x++ )
		{
			if( x >= player.x - range && x <= player.x + range &&
				y >= player.y - range && y <= player.y + range )
			{
				_visibleDataMap[y][x] = 1;
			}
		}
	}
}
char Room::staticDataMap( int x, int y )
{
	return _staticDataMap[y][x];
}
char Room::completeDataMap( int x, int y )
{
	return _completeDataMap[y][x];
}
char Room::visibleDataMap( int x, int y )
{
	return _visibleDataMap[y][x];
}