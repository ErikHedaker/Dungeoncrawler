#include <vector>
#include "Entity.h"

class Room
{
	public:
		Entity player;
		std::vector<Entity> monster;
		std::vector<Entity> wall;
		std::vector<Entity> exit;
		std::vector<Entity> path;

		// Functions used by functions.
		bool CheckPosition( Entity& current, Entity& search );
		bool CheckPosition( int xCurrent, int yCurrent, Entity& search );
		bool CheckEmptySurroundedTile( int xCurrent, int yCurrent );

		void SetLength( int input );
		int GetLength( );

		void SetWidth( int input );
		int GetWidth( );

		void SetLineOfSight( int input );
		int GetLineOfSight( );

		void BuildHiddenDataMap( );
		void SetHiddenDataMap( int x, int y, char value );
		char GetHiddenDataMap( int x, int y );

		void BuildVisibleDataMap( );
		void SetVisibleDataMap( int x, int y, char value );
		char GetVisibleDataMap( int x, int y );

		void BuildFogOfWarDataMap( );
		void SetFogOfWarDataMap( );
		char GetFogOfWarDataMap( int x, int y );

		// Configuration
		void SetRoomSize( int roomMode );
		void SetRoomLineOfSight( int roomMode );
		void SetMonsterAmount( int roomMode );

		// Automatic
		void SetPlayerPosition( );
		void SetExits( );
		void SetOuterWalls( );
		void SetInvisiblePath( );
		void SetRandomWalls( );
		void SetRandomMonsterPositions( );

		// Gameloop
		void DrawRoom( );
		char TurnOptions( );
		void RandomizeMonsterMovement( );

		// Checked during or after gameloop
		bool CheckWinCondition( );
		bool CheckLoseCondition( );

	private:
		int _length;
		int _width;
		int _lineOfSight;
		std::vector< std::vector<char> > _hiddenDataMap;
		std::vector< std::vector<char> > _visibleDataMap;
		std::vector< std::vector<char> > _fogOfWarDataMap;	// char is used as if it were bool.
};