#include "Entity.h"
#include <vector>

class Room
{
	public:
		Entity player;
		std::vector<Entity> monsters;
		std::vector<Entity> walls;
		std::vector<Entity> exits;
		std::vector<Entity> paths;

		struct Portrait
		{
			static const char Player = '@';
			static const char Monster = 'M';
			static const char Wall = '#';
			static const char Exit = '=';
			static const char Path = ':';
			static const char Floor = '-';
		};

		bool EmptySurroundedTile( const Vector2i& current ) const;

		void BuildHiddenData( );
		void UpdateHiddenDataAt( const Vector2i& position, char icon );
		char GetHiddenDataAt( const Vector2i& position ) const;

		void BuildVisibleData( );
		void UpdateVisibleDataAt( const Vector2i& position, char icon );
		char GetVisibleDataAt( const Vector2i& position ) const;

		void BuildFogOfWarData( );
		void UpdateFogOfWarData( );
		bool GetFogOfWarDataAt( const Vector2i& position ) const;

		/* Configuration */
		void SetRoomSize( int gameType );
		void SetRoomLineOfSight( int gameType );
		void SetRoomMonsterAmount( int gameType );

		/* Automatic */
		void SetPlayerPosition( );
		void SetExits( );
		void SetOuterWalls( );
		void SetInvisiblePath( );
		void SetRandomWalls( );
		void SetRandomMonsterPositions( );

		/* Gameloop */
		void DrawRoom_Iterator1D( ) const;
		void DrawRoom_Iterator2D( ) const;
		char TurnOptions( );
		void RandomizeMonsterMovement( );

		/* Checked during gameloop */
		bool CheckWinCondition( ) const;
		bool CheckLoseCondition( ) const;

	private:
		size_t _maxCols;
		size_t _maxRows;
		int _lineOfSight;

		/* 1D arrays interpreted as 2D space */
		std::vector<char> _hiddenData;
		std::vector<char> _visibleData;
		std::vector<bool> _fogOfWarData;
};