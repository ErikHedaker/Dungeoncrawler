#pragma once

#include "Player.h"
#include "Monster.h"
#include "Wall.h"
#include "Exit.h"
#include "Path.h"
#include "Orientation.h"
#include "GameState.h"
#include <vector>
#include <list>
#include <memory>

class Dungeon
{
	public:
		void Build( char gameType );
		void GameLoop( );

		/* Used in IO::OutputDungeon */
		const Entity* const GetEntityDataAt( const Vector2i& position ) const;
		const Entity* const GetHiddenDataAt( const Vector2i& position ) const;
		bool GetVisionDataAt( const Vector2i& position ) const;

	private:
		GameState _state;
		Vector2i _size;
		int _lineOfSight;

		/* All types inherit from Entity */
		std::unique_ptr<Player> _player;	// _player needs to be dynamically allocated.
		std::list<Monster> _monsters;
		std::list<Wall> _walls;				// std::list guarantees unchanged
		std::list<Exit> _exits;				// element addresses as the
		std::list<Path> _paths;				// container grows, unlike std::vector.

		/* 1D arrays interpreted as 2D space */
		std::vector<Entity*> _entityData;	// non-owning pointers.
		std::vector<Entity*> _hiddenData;
		std::vector<bool> _visionData;

		void UpdateEntityDataAt( const Vector2i& position, Entity* entity );
		void UpdateHiddenDataAt( const Vector2i& position, Entity* entity );
		void UpdateVisionData( );

		/* Configuration */
		void SetDungeonSize( char gameType );
		void SetDungeonLineOfSight( char gameType );
		void SetDungeonMonsterAmount( char gameType );

		/* Resize containers */
		void BuildEntityData( );
		void BuildHiddenData( );
		void BuildVisionData( );

		/* Build Dungeon */
		void SetPlayer( );
		void SetExits( );
		void SetOuterWalls( );
		void SetHiddenPath( );
		void SetRandomSourceWalls( );
		void SetRandomExtensionWalls( );
		void SetFillerWalls( );
		void SetRandomMonsterPositions( );

		/* GameLoop */
		void PlayerTurn( char choice );
		void PlayerMovement( Orientation orientation );
		void RandomMonsterMovement( );
		bool CheckGameState( ) const;
};