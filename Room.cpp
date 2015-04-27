#include "Room.h"

template<class T> void Room::xSet( T &object, int input )
{
	object.position.x = input;
}
template<class T> void Room::ySet( T &object, int input )
{
	object.position.y = input;
}
template<class T> int Room::x( const T &object )
{
	return object.position.x;
}
template<class T> int Room::y( const T &object )
{
	return object.position.y;
}
template<class T, class Y> bool Room::checkPosition( const T &current, const Y &search )
{
	if( x( current ) == x( search ) &&
		y( current ) == y( search ) )
	{
		return true;
	}

	return false;
}
template<class T, class Y> bool Room::checkPosition( const T &current, const std::vector<Y> &search )
{
	for( unsigned int i = 0; i < search.size( ); i++ )
	{
		if( x( current ) == x( search[i] ) &&
			y( current ) == y( search[i] ) )
		{
			return true;
		}
	}

	return false;
}
template<class T, class Y> bool Room::checkProtectRange( const T &current, const Y &search, const int range )
{
	for( int y = y( current ) - range; y < y( current ) + range; y++ )
	{
		for( int x = x( current ) - range; x < x( current ) + range; x++ )
		{
			if( x == x( search ) &&
				y == y( search ) )
			{
				return true;
			}
		}
	}

	return false;
}
template<class T, class Y> bool Room::checkProtectRange( const T &current, const std::vector<Y> &search, const int range )
{
	for( int i = 0; i < seach.size( ); i++ )
	{
		for( int y = y( current ) - range; y < y( current ) + range; y++ )
		{
			for( int x = x( current ) - range; x < x( current ) + range; x++ )
			{
				if( x == x( search[i] ) &&
					y == y( search[i] ) )
				{
					return true;
				}
			}
		}
	}

	return false;
}
void Room::objectIconSet( )
{
	Player::icon = '@';
	Monster::icon = 'M';
	Wall::icon = '#';
	Exit::icon = '=';
}

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
void Room::staticDataMap( )
{
	Position tempPosition;

	if( _staticDataMap.size( ) == 0 )
	{
		_staticDataMap.resize( length( ), std::vector<char>( width( ) ) );
	}

	for( int y = 0; y < length( ); y++ )
	{
		for( int x = 0; x < width( ); x++ )
		{
			tempPosition.x = x;
			tempPosition.y = y;

			if( checkPosition( tempPosition, exit ) == true )
			{
				_staticDataMap[y][x] = '=';
			}
			else if( checkPosition( tempPosition, wall ) == true )
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
	Position tempPosition;

	if( _completeDataMap.size( ) == 0 )
	{
		_completeDataMap = _staticDataMap;
	}

	for( int y = 0; y < length( ); y++ )	// Overwriting values.
	{
		for( int x = 0; x < width( ); x++ )
		{
			tempPosition.x = x;
			tempPosition.y = y;

			if( checkPosition( tempPosition, monster ) == true )
			{
				_completeDataMap[y][x] = 'M';
			}
			else if( checkPosition( tempPosition, player ) == true )
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