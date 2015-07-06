#pragma once

#include "Vector2i.h"
#include "Entity.h"
#include <vector>

class Room
{
	public:
		Entity player;
		std::vector<Entity> monster;
		std::vector<Entity> wall;
		std::vector<Entity> exit;
		std::vector<Entity> path;

		// Functions used by functions.
		bool CheckPosition( const Vector2i& current, const Vector2i& search ) const;
		bool CheckEmptySurroundedTile( const Vector2i& current ) const;

		void SetLength( const int& length );
		int GetLength( ) const;

		void SetWidth( const int& width );
		int GetWidth( ) const;

		void SetLineOfSight( const int& LoS );
		int GetLineOfSight( ) const;

		void BuildHiddenDataMap( );
		void UpdateHiddenDataMap( const Vector2i& position, const char& icon );
		char GetPositionHiddenDataMap( const Vector2i& position ) const;

		void BuildVisibleDataMap( );
		void UpdateVisibleDataMap( const Vector2i& position, const char& icon );
		char GetPositionVisibleDataMap( const Vector2i& position ) const;

		void BuildFogOfWarDataMap( );
		void UpdateFogOfWarDataMap( );
		char GetPositionFogOfWarDataMap( const Vector2i& position ) const;

		// Configuration
		void SetRoomSize( const int& roomMode );
		void SetRoomLineOfSight( const int& roomMode );
		void SetMonsterAmount( const int& roomMode );

		// Automatic
		void SetPlayerPosition( );
		void SetExits( );
		void SetOuterWalls( );
		void SetInvisiblePath( );
		void SetRandomWalls( );
		void SetRandomMonsterPositions( );

		// Gameloop
		void DrawRoom( ) const;
		char TurnOptions( );
		void RandomizeMonsterMovement( );

		// Checked during or after gameloop
		bool CheckWinCondition( ) const;
		bool CheckLoseCondition( ) const;

	private:
		int _length;
		int _width;
		int _lineOfSight;
		std::vector< std::vector<char> > _hiddenDataMap;
		std::vector< std::vector<char> > _visibleDataMap;
		std::vector< std::vector<char> > _fogOfWarDataMap;	// char is used as if it were bool.
};