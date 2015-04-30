#include <vector>

class Player
{
	public:
		friend class Room;
		static char icon;

	private:
		int x;
		int y;
};
class Monster
{
	public:
		friend class Room;
		static char icon;

	private:
		int x;
		int y;
};
class Wall
{
	public:
		friend class Room;
		static char icon;

	private:
		int x;
		int y;
};
class Exit
{
	public:
		friend class Room;
		static char icon;

	private:
		int x;
		int y;
};
class Path
{
	public:
		friend class Room;
		static char icon;

	private:
		int x;
		int y;
};
class Room
{
	public:
		Player player;
		std::vector<Monster> monster;
		std::vector<Wall> wall;
		std::vector<Exit> exit;
		std::vector<Path> path;

		template<class T> void xSet( T &object, int input );
		template<class T> void ySet( T &object, int input );
		template<class T> int x( const T &object );
		template<class T> int y( const T &object );
		template<class T> bool checkPosition( int xCurrent, int yCurrent, const T &search );
		template<class T> bool checkPosition( int xCurrent, int yCurrent, const std::vector<T> &search, int startIt = 0 );
		template<class T, class Y> bool checkPosition( const T &current, const Y &search );
		template<class T, class Y> bool checkPosition( const T &current, const std::vector<Y> &search, int startIt = 0 );
		template<class T> bool checkProtectRange( int xCurrent, int yCurrent, const T &search, int range = 2 );
		template<class T> bool checkProtectRange( int xCurrent, int yCurrent, const std::vector<T> &search, int range = 2 );

		void lengthSet( int input );
		void widthSet( int input );
		void monsterAmountDesiredSet( int input );
		int length( );
		int width( );
		int monsterAmountDesired( );
		int outerWallsAmount( );
		void outerWallsAmountIncrease( int n = 1 );
		void hiddenDataMapUpdate( );
		void hiddenDataMapUpdateSingle( int x, int y, int value );
		void visibleDataMapUpdate( );
		void visibleDataMapUpdateSingle( int x, int y, char value );
		void visibleDataMapFogOfWarUpdate( int range = 5 );
		void visibleDataMapFogOfWarLineOfSightUpdate( int range = 5 );
		char hiddenDataMap( int x, int y );
		char visibleDataMap( int x, int y );
		char visibleDataMapFogOfWar( int x, int y );

	private:
		int _length = 10;
		int _width = 10;
		int _monsterAmountDesired = 0;
		int _outerWallsAmount = 0;
		std::vector< std::vector<char> > _hiddenDataMap;
		std::vector< std::vector<char> > _visibleDataMap;
		std::vector< std::vector<char> > _visibleDataMapFogOfWar;	// Note: char is used as it were bool.
};

template<class T> void Room::xSet( T &object, int input )
{
	object.x = input;
}
template<class T> void Room::ySet( T &object, int input )
{
	object.y = input;
}
template<class T> int Room::x( const T &object )
{
	return object.x;
}
template<class T> int Room::y( const T &object )
{
	return object.y;
}
template<class T> bool Room::checkPosition( int xCurrent, int yCurrent, const T &search )
{
	if( xCurrent == x( search ) &&
		yCurrent == y( search ) )
	{
		return true;
	}

	return false;
}
template<class T> bool Room::checkPosition( int xCurrent, int yCurrent, const std::vector<T> &search, int startIt )
{
	for( unsigned int i = startIt; i < search.size( ); i++ )
	{
		if( xCurrent == x( search[i] ) &&
			yCurrent == y( search[i] ) )
		{
			return true;
		}
	}

	return false;
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
template<class T, class Y> bool Room::checkPosition( const T &current, const std::vector<Y> &search, int startIt )
{
	for( unsigned int i = startIt; i < search.size( ); i++ )
	{
		if( x( current ) == x( search[i] ) &&
			y( current ) == y( search[i] ) )
		{
			return true;
		}
	}

	return false;
}
template<class T> bool Room::checkProtectRange( int xCurrent, int yCurrent, const T &search, int range )
{
	for( int y = yCurrent - range; y < yCurrent + range; y++ )
	{
		for( int x = xCurrent - range; x < xCurrent + range; x++ )
		{
			if( x == search.x &&
				y == search.y )
			{
				return true;
			}
		}
	}

	return false;
}
template<class T> bool Room::checkProtectRange( int xCurrent, int yCurrent, const std::vector<T> &search, int range )
{
	for( unsigned int i = 0; i < search.size( ); i++ )
	{
		for( int y = yCurrent - range; y < yCurrent + range; y++ )
		{
			for( int x = xCurrent - range; x < xCurrent + range; x++ )
			{
				if( x == search[i].x &&
					y == search[i].y ) 
				{
					return true;
				}
			}
		}
	}

	return false;
}