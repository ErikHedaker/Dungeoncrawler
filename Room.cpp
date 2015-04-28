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
				_staticDataMap[y][x] = '=';
			}
			else if( checkPosition( x, y, wall ) == true )
			{
				_staticDataMap[y][x] = '#';
			}
			else
			{
				_staticDataMap[y][x] = '-';
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
				_completeDataMap[y][x] = 'M';
			}
			else if( checkPosition( x, y, player ) == true )
			{
				_completeDataMap[y][x] = 'P';
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