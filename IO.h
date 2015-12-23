#pragma once

#include "Dungeon.h"
#include "GameState.h"
#include <vector>

class IO
{
	public:
		static void OutputClearScreen( );
		static void OutputStartMessage( );
		static void OutputGameTypes( );
		static void OutputString( const std::string& string );
		static void OutputDungeonCentered( const Dungeon& dungeon, const Vector2i& dungeonSize, Vector2i positionCenter );
		static void OutputDungeon( const Dungeon& dungeon, const Vector2i& dungeonSize );
		static void OutputHiddenDungeon( const Dungeon& dungeon, const Vector2i& dungeonSize );
		static void OutputTurnOptions( );
		static void OutputGameState( GameState state );

		static void InputEnter( );
		static char InputValidChar( const std::string& context, const std::vector<char>& validChoices );
		static int InputPositiveInteger( const std::string& context );
		
};