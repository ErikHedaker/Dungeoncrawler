#pragma once

#include "Vector2i.h"
#include "Room.h"
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

extern const char iconPlayer;
extern const char iconMonster;
extern const char iconWall;
extern const char iconExit;
extern const char iconPath;
extern const char iconFloor;

extern int GetPositiveInteger( );
extern int RandomNumberGenerator( int min, int max );
extern int RandomPositiveNegativeGenerator( );

bool Room::CheckPosition( const Vector2i& current, const Vector2i& search ) const
{
	if( current == search )
	{
		return true;
	}

	return false;
}
bool Room::CheckEmptySurroundedTile( const Vector2i& current ) const
{
	Vector2i iterator;
	int surroundingWalls = 0;

	for( iterator.y = current.y - 1; iterator.y <= current.y + 1; iterator.y++ )
	{
		for( iterator.x = current.x - 1; iterator.x <= current.x + 1; iterator.x++ )
		{
			if( GetPositionVisibleDataMap( iterator ) == iconWall )
			{
				surroundingWalls++;
			}
		}
	}

	return surroundingWalls > 4;
}

void Room::SetLength( const int& length )
{
	_length = length;
}
int Room::GetLength( ) const
{
	return _length;
}

void Room::SetWidth( const int& width )
{
	_width = width;
}
int Room::GetWidth( ) const
{
	return _width;
}

void Room::SetLineOfSight( const int& LoS )
{
	_lineOfSight = LoS;
}
int Room::GetLineOfSight( ) const
{
	return _lineOfSight;
}

void Room::BuildHiddenDataMap( )
{
	_hiddenDataMap.clear( );
	_hiddenDataMap.resize( _length, std::vector<char>( _width, '0' ) );
}
void Room::UpdateHiddenDataMap( const Vector2i& position, const char& icon )
{
	_hiddenDataMap[position.y][position.x] = icon;
}
char Room::GetPositionHiddenDataMap( const Vector2i& position ) const
{
	return _hiddenDataMap[position.y][position.x];
}

void Room::BuildVisibleDataMap( )
{
	_visibleDataMap.clear( );
	_visibleDataMap.resize( _length, std::vector<char>( _width, '-' ) );
}
void Room::UpdateVisibleDataMap( const Vector2i& position, const char& icon )
{
	_visibleDataMap[position.y][position.x] = icon;
}
char Room::GetPositionVisibleDataMap( const Vector2i& position ) const
{
	return _visibleDataMap[position.y][position.x];
}

void Room::BuildFogOfWarDataMap( )
{
	_fogOfWarDataMap.clear( );
	_fogOfWarDataMap.resize( _length, std::vector<char>( _width, 0 ) );
}
void Room::UpdateFogOfWarDataMap( )
{
	Vector2i iterator;

	for( iterator.y = 0; iterator.y < _length; iterator.y++ )
	{
		for( iterator.x = 0; iterator.x < _width; iterator.x++ )
		{
			if( iterator.x >= player.GetPosition( ).x - _lineOfSight && iterator.x <= player.GetPosition( ).x + _lineOfSight &&
				iterator.y >= player.GetPosition( ).y - _lineOfSight && iterator.y <= player.GetPosition( ).y + _lineOfSight )
			{
				_fogOfWarDataMap[iterator.y][iterator.x] = 1;
			}
		}
	}
}
char Room::GetPositionFogOfWarDataMap( const Vector2i& position ) const
{
	return _fogOfWarDataMap[position.y][position.x];
}

void Room::SetRoomSize( const int& roomMode )
{
	if( roomMode == 2 )
	{
		RETRY:
		std::cout << "Enter the playing Room length: ";
		SetLength( GetPositiveInteger( ) );
		std::cout << "Enter the playing Room width: ";
		SetWidth( GetPositiveInteger( ) );

		if( GetLength( ) < 3 ||
			GetWidth( ) < 3 )
		{
			std::cout << "\nInput is too small, try again.\n\n";
			goto RETRY;
		}
	}
	else
	{
		SetLength( RandomNumberGenerator( 15, 30 ) );
		SetWidth( RandomNumberGenerator( 50, 100 ) );

	}
}
void Room::SetRoomLineOfSight( const int& roomMode )
{
	if( roomMode == 2 )
	{
		std::cout << "Enter the line of sight range: ";
		SetLineOfSight( GetPositiveInteger( ) );
	}
	else
	{
		SetLineOfSight( 4 );
	}
}
void Room::SetMonsterAmount( const int& roomMode )
{
	if( roomMode == 2 )
	{
		int amountMonsters;
		int minimumMonsters = ( GetLength( ) * GetWidth( ) ) - wall.size( ) - ( GetLength( ) * GetWidth( ) ) / 2;

		while( true )
		{
			std::cout << "Enter the amount of monsters: ";
			amountMonsters = GetPositiveInteger( );

			if( amountMonsters >= minimumMonsters &&
				amountMonsters != 0 )
			{
				std::cout << "Too many monsters, try again.\n";
			}
			else
			{
				monster.resize( amountMonsters );
				std::cout << "\n";
				break;
			}
		}
	}
	else
	{
		double high = sqrt( GetLength( ) * GetWidth( ) ) / 1.5;
		double low = sqrt( GetLength( ) * GetWidth( ) ) / 3;
		monster.resize( RandomNumberGenerator( static_cast<int>( low ), static_cast<int>( high ) ) );
	}
}

void Room::SetPlayerPosition( )
{
	Vector2i newPosition( 1, 1 );
	player.SetPosition( newPosition );
	UpdateVisibleDataMap( newPosition, iconPlayer );
}
void Room::SetExits( )
{
	Vector2i newPosition( GetWidth( ) - 2, GetLength( ) - 1 );
	exit.emplace_back( newPosition );
	UpdateVisibleDataMap( newPosition, iconExit );
}
void Room::SetOuterWalls( )
{
	Vector2i iterator;

	for( iterator.y = 0; iterator.y < GetLength( ); iterator.y++ )
	{
		for( iterator.x = 0; iterator.x < GetWidth( ); iterator.x++ )
		{
			if( GetPositionVisibleDataMap( iterator ) == '=' )
			{
				continue;
			}
			else if( iterator.x == GetWidth( ) - 1 ||
					 iterator.y == GetLength( ) - 1 ||
					 iterator.x == 0 ||
					 iterator.y == 0 )
			{
				wall.emplace_back( iterator );
				UpdateVisibleDataMap( iterator, iconWall );
			}
		}
	}
}
void Room::SetInvisiblePath( )
{
	/*
	 Path from player starting point to exit.
	 Walls cannot be placed on path.
	 Player can no longer be cut off from exit due to randomly palced walls.
	 Function can be replaced by A* algorithm, or similar algorithm.
	*/
	Vector2i newPosition = player.GetPosition( );
	Vector2i oldPosition;
	//double high = sqrt( GetLength( ) * GetWidth( ) ) * 3;
	//double low = sqrt( GetLength( ) * GetWidth( ) ) * 2;
	//int threshold = RandomNumberGenerator( static_cast<int>( low ), static_cast<int>( high ) );
	//int thresholdCounter = 0;

	if( path.size( ) == 0 )
	{
		path.emplace_back( player.GetPosition( ) );
		UpdateHiddenDataMap( player.GetPosition( ), iconPath );
	}

	while( true )
	{
		oldPosition = newPosition;

		//random = RandomNumberGenerator( 1, 16 );
		//if( thresholdCounter < threshold ) // Equal chance to move in any direction.
		//{
		//	if( random <= 4 )
		//	{
		//		xNew++;
		//	}
		//	else if( random <= 8 )
		//	{
		//		yNew++;
		//	}
		//	else if( random <= 12 )
		//	{
		//		xNew--;
		//	}
		//	else
		//	{
		//		yNew--;
		//	}

		//	thresholdCounter++;
		//}
		//else // Increased chance to move right and down.
		//{
		//	if( random <= 6 )
		//	{
		//		xNew++;
		//	}
		//	else if( random <= 14 )
		//	{
		//		yNew++;
		//	}
		//	else if( random <= 15 )
		//	{
		//		xNew--;
		//	}
		//	else
		//	{
		//		yNew--;
		//	}
		//}

		if( RandomNumberGenerator( 0, 1 ) == true )
		{
			newPosition.x = RandomPositiveNegativeGenerator( );
		}
		else
		{
			newPosition.y = RandomPositiveNegativeGenerator( );
		}

		if( GetPositionVisibleDataMap( newPosition ) == iconWall )
		{
			newPosition = oldPosition;
		}
		else
		{
			path.emplace_back( newPosition );
			UpdateHiddenDataMap( newPosition, iconPath );
		}

		if( path.back( ).GetPosition( ) == exit.back( ).GetPosition( ) )
		{
			break;
		}
	}
}
void Room::SetRandomWalls( )
{
	Vector2i iterator;
	Vector2i newPosition;
	int infiniteLoopBreaker = 0;

	double high_0 = sqrt( GetLength( ) * GetWidth( ) ) * 1.5;
	double low_0 = sqrt( GetLength( ) * GetWidth( ) ) / 1.5;
	int randomSourceWalls = RandomNumberGenerator( ( int ) low_0, ( int ) high_0 );
	int randomTries = RandomNumberGenerator( 5, 10 );

	while( randomSourceWalls > 0 ) // Place source walls.
	{
		for( iterator.y = 0; iterator.y < GetLength( ); iterator.y++ )
		{
			for( iterator.x = 0; iterator.x < GetWidth( ); iterator.x++ )
			{
				if( GetPositionVisibleDataMap( iterator ) == iconFloor &&
					GetPositionHiddenDataMap( iterator ) != iconPath &&
					RandomNumberGenerator( 1, 1000 ) == 1 )
				{
					wall.emplace_back( iterator );
					UpdateVisibleDataMap( iterator, iconWall );
					randomSourceWalls--;

					std::cout << ".";
				}
			}
		}

		if( infiniteLoopBreaker++ > 10000 )
		{
			break;
		}
	}

	std::cout << "\n\nLoading walls.\n";

	for( int i = 0; i < randomTries; i++ ) // Place extension walls.
	{
		std::cout << ".";

		for( iterator.y = 1; iterator.y < GetLength( ) - 1; iterator.y++ )
		{
			for( iterator.x = 1; iterator.x < GetWidth( ) - 1; iterator.x++ )
			{
				if( GetPositionVisibleDataMap( iterator ) == iconWall )
				{
					newPosition.x = iterator.x + RandomNumberGenerator( 0, 1 ) * RandomPositiveNegativeGenerator( );
					newPosition.y = iterator.y + RandomNumberGenerator( 0, 1 ) * RandomPositiveNegativeGenerator( );

					if( GetPositionVisibleDataMap( iterator ) == iconFloor &&
						GetPositionHiddenDataMap( iterator ) != iconPath )
					{
						wall.emplace_back( newPosition );
						UpdateVisibleDataMap( newPosition, iconWall );
					}
				}
				else if( GetPositionVisibleDataMap( iterator ) == iconFloor &&
						 GetPositionHiddenDataMap( iterator ) != iconPath &&
						 CheckEmptySurroundedTile( iterator ) == true )
				{
					wall.emplace_back( iterator );
					UpdateVisibleDataMap( iterator, iconWall );
				}
			}
		}
	}

	std::cout << "\n";
}
void Room::SetRandomMonsterPositions( )
{
	Vector2i newPosition;
	int infiniteLoopBreaker = 0;

	for( unsigned int i = 0; i < monster.size( ); i++ )
	{
		std::cout << ".";

		do
		{
			newPosition.x = RandomNumberGenerator( 1, GetWidth( ) - 2 );
			newPosition.y = RandomNumberGenerator( 1, GetLength( ) - 2 );

			if( infiniteLoopBreaker++ > 10000 )
			{
				break;
			}
		}
		while( GetPositionVisibleDataMap( newPosition ) != iconFloor );

		monster[i].SetPosition( newPosition );
		UpdateVisibleDataMap( newPosition, iconMonster );
	}
}

void Room::DrawRoom( ) const
{
	Vector2i iterator;

	for( iterator.y = 0; iterator.y < GetLength( ); iterator.y++ )
	{
		for( iterator.x = 0; iterator.x < GetWidth( ); iterator.x++ )
		{
			if( GetPositionFogOfWarDataMap( iterator ) == 1 )
			{
				std::cout << GetPositionVisibleDataMap( iterator );
			}
			else
			{
				std::cout << " ";
			}
		}

		std::cout << "\n";
	}

	std::cout << "\n";
}
char Room::TurnOptions( )
{
	Vector2i newPosition;
	std::string userChoice;

	std::cout << "[W] Go up\n";
	std::cout << "[S] Go down\n";
	std::cout << "[A] Go left\n";
	std::cout << "[D] Go Right\n";
	std::cout << "[Q] Do nothing\n";
	std::cout << "[E] Exit to meny\n";

	RETRY:
	std::cout << "\nYour choice: ";
	std::cin >> userChoice;

	switch( userChoice[0] )
	{
		case 'W':
		case 'w':       // Move up.
		{
			newPosition.x = player.GetPosition( ).x;
			newPosition.y = player.GetPosition( ).y - 1;

			break;
		}

		case 'S':
		case 's':       // Move down.
		{
			newPosition.x = player.GetPosition( ).x;
			newPosition.y = player.GetPosition( ).y + 1;

			break;
		}

		case 'A':
		case 'a':       // Move left.
		{
			newPosition.x = player.GetPosition( ).x - 1;
			newPosition.y = player.GetPosition( ).y;

			break;
		}

		case 'D':
		case 'd':       // Move right.
		{
			newPosition.x = player.GetPosition( ).x + 1;
			newPosition.y = player.GetPosition( ).y;

			break;
		}

		case 'Q':
		case 'q':       // Do nothing.
		{
			return 0;
		}

		case 'E':
		case 'e':       // Exit game.
		{
			system( "CLS" );

			return 'E';
		}

		default:
		{
			std::cout << "Invalid input, try again.\n";
			goto RETRY;
		}
	}

	if( GetPositionVisibleDataMap( newPosition ) != iconWall )
	{
		UpdateVisibleDataMap( player.GetPosition( ), iconFloor );
		player.SetPosition( newPosition );
		UpdateVisibleDataMap( newPosition, iconPlayer );
	}

	return 0;
}
void Room::RandomizeMonsterMovement( )
{
	Vector2i newPosition;
	Vector2i oldPosition;
	int random;

	for( unsigned int i = 0; i < monster.size( ); i++ )
	{
		while( true )
		{
			random = RandomNumberGenerator( 1, 8 );		// 25% to move, 75% to stand still.
			newPosition = player.GetPosition( );

			if( random == 1 )							// Monster moves vertically.
			{
				newPosition.x = newPosition.x + RandomPositiveNegativeGenerator( );
			}
			else if( random == 2 )						// Monster moves horizontally.
			{
				newPosition.y = newPosition.y + RandomPositiveNegativeGenerator( );
			}
			else										// Monster stands still.
			{
				break;
			}

			if( GetPositionVisibleDataMap( newPosition ) != iconWall ||
				GetPositionVisibleDataMap( newPosition ) != iconExit )
			{
				oldPosition = monster[i].GetPosition( );

				if( CheckPosition( oldPosition, player.GetPosition( ) ) == false )	// Player movement is executed before monster movement, so
				{															// clearing the monster's previous position after the player
					UpdateVisibleDataMap( oldPosition, iconFloor );				// has moved to that position will make the player icon invisible.
				}															// (this check avoids that)

				monster[i].SetPosition( newPosition );
				UpdateVisibleDataMap( newPosition, iconMonster );
				break;
			}
		}
	}
}

bool Room::CheckWinCondition( ) const
{
	for( unsigned int i = 0; i < exit.size( ); i++ )
	{
		if( CheckPosition( player.GetPosition( ), exit[i].GetPosition( ) ) == true )
		{
			return true;
		}
	}

	return false;
}
bool Room::CheckLoseCondition( ) const
{
	for( unsigned int i = 0; i < monster.size( ); i++ )
	{
		if( CheckPosition( player.GetPosition( ), monster[i].GetPosition( ) ) == true )
		{
			return true;
		}
	}

	return false;
}