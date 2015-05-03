#include "Room.h"

void Room::set_length( int input )
{
	_length = input;
}
void Room::set_width( int input )
{
	_width = input;
}
void Room::set_monsterAmountDesired( int input )
{
	_monsterAmountDesired = input;
}
void Room::set_lineOfSight( int input )
{
	_lineOfSight = input;
}
void Room::increase_outerWallsAmount( int input )
{
	_outerWallsAmount += input;
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
int Room::lineOfSight( )
{
	return _lineOfSight;
}
int Room::outerWallsAmount( )
{
	return _outerWallsAmount;
}
void Room::build_hiddenDataMap( )
{
	_hiddenDataMap.clear( );
	_hiddenDataMap.resize( length( ), std::vector<char>( width( ) ) );
	_hiddenDataMap.assign( length( ), std::vector<char>( width( ), '\0' ) );

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
void Room::build_visibleDataMap( )
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
void Room::build_fogOfWarDataMap( )
{
	_fogOfWarDataMap.clear( );
	_fogOfWarDataMap.resize( length( ), std::vector<char>( width( ) ) );
	_fogOfWarDataMap.assign( length( ), std::vector<char>( width( ), 0 ) );

	for( int y = 0; y < length( ); y++ )
	{
		for( int x = 0; x < width( ); x++ )
		{
			if( x >= player.x - lineOfSight( ) && x <= player.x + lineOfSight( ) &&
				y >= player.y - lineOfSight( ) && y <= player.y + lineOfSight( ) )
			{
				_fogOfWarDataMap[y][x] = 1;
			}
		}
	}
}
void Room::update_hiddenDataMap( int x, int y, char value )
{
	_hiddenDataMap[y][x] = value;
}
void Room::update_visibleDataMap( int x, int y, char value )
{
	_visibleDataMap[y][x] = value;
}
void Room::update_fogOfWarDataMap( int x, int y, char value )
{
	_fogOfWarDataMap[y][x] = value;
}
void Room::update_fogOfWarDataMap( )
{
	for( int y = 0; y < length( ); y++ )
	{
		for( int x = 0; x < width( ); x++ )
		{
			if( x >= player.x - lineOfSight( ) && x <= player.x + lineOfSight( ) &&
				y >= player.y - lineOfSight( ) && y <= player.y + lineOfSight( ) )
			{
				_fogOfWarDataMap[y][x] = 1;
			}
		}
	}
}
//void Room::visibleDataMapFogOfWarLineOfSightBuild( int range )
//{
//	// Not in use due to bugs.
//
//	_visibleDataMapFogOfWar.clear( );
//	_visibleDataMapFogOfWar.resize( length( ), std::vector<char>( width( ) ) );
//	_visibleDataMapFogOfWar.assign( length( ), std::vector<char>( width( ), 0 ) );
//
//	for( int y = player.y; y < player.y + range; y++ )
//	{
//		if( y > length( ) || y < 0 )
//		{
//			break;
//		}
//
//		for( int x = player.x; x < player.x + range; x++ )
//		{
//			if( x > width( ) || x < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_0;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_0:
//
//	for( int y = player.y; y > player.y - range; y-- )
//	{
//		if( y > length( ) || y < 0 )
//		{
//			break;
//		}
//
//		for( int x = player.x; x < player.x + range; x++ )
//		{
//			if( x > width( ) || x < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_1;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_1:
//
//	for( int y = player.y; y < player.y + range; y++ )
//	{
//		if( y > length( ) || y < 0 )
//		{
//			break;
//		}
//
//		for( int x = player.x; x > player.x - range; x-- )
//		{
//			if( x > width( ) || x < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_2;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_2:
//
//	for( int y = player.y; y > player.y - range; y-- )
//	{
//		if( y > length( ) || y < 0 )
//		{
//			break;
//		}
//
//		for( int x = player.x; x > player.x - range; x-- )
//		{
//			if( x > width( ) || x < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_3;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_3:
//
//	// Check again, this time y and x loops are reversed.
//	// This should fix any visual bugs.
//
//	for( int x = player.x; x < player.x + range; x++ )
//	{
//		if( x > width( ) || x < 0 )
//		{
//			break;
//		}
//
//		for( int y = player.y; y < player.y + range; y++ )
//		{
//			if( y > length( ) || y < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_4;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_4:
//
//	for( int x = player.x; x > player.x - range; x-- )
//	{
//		if( x > width( ) || x < 0 )
//		{
//			break;
//		}
//
//		for( int y = player.y; y < player.y + range; y++ )
//		{
//			if( y > length( ) || y < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_5;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_5:
//
//	for( int x = player.x; x < player.x + range; x++ )
//	{
//		if( x > width( ) || x < 0 )
//		{
//			break;
//		}
//
//		for( int y = player.y; y > player.y - range; y-- )
//		{
//			if( y > length( ) || y < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//goto next_6;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//
//	//next_6:
//
//	for( int x = player.x; x > player.x - range; x-- )
//	{
//		if( x > width( ) || x < 0 )
//		{
//			break;
//		}
//
//		for( int y = player.y; y > player.y - range; y-- )
//		{
//			if( y > length( ) || y < 0 )
//			{
//				break;
//			}
//
//			if( checkPosition( x, y, wall ) == true ||
//				checkPosition( x, y, exit ) == true )
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//				//return;
//				break;
//			}
//			else
//			{
//				_visibleDataMapFogOfWar[y][x] = 1;
//			}
//		}
//	}
//}
char Room::hiddenDataMap( int x, int y )
{
	return _hiddenDataMap[y][x];
}
char Room::visibleDataMap( int x, int y )
{
	return _visibleDataMap[y][x];
}
char Room::fogOfWarDataMap( int x, int y )
{
	return _fogOfWarDataMap[y][x];
}