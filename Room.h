#include <vector>

struct Position
{
	int x;
	int y;
};

class Player
{
	public:
		static char icon;

	private:
		Position position;
};
class Monster
{
	public:
		static char icon;

	private:
		Position position;
};
class Wall
{
	public:
		static char icon;

	private:
		Position position;
};
class Exit
{
	public:
		static char icon;

	private:
		Position position;
};

class Room
{
	public:
		Player player;
		std::vector<Monster> monster;
		std::vector<Wall> wall;
		std::vector<Exit> exit;

		template<class T> void xSet( T &object, int input );
		template<class T> void ySet( T &object, int input );
		template<class T> int x( const T &object );
		template<class T> int y( const T &object );
		template<class T, class Y> bool checkPosition( const T &current, const Y &search );
		template<class T, class Y> bool checkPosition( const T &current, const std::vector<Y> &search );
		template<class T, class Y> bool checkProtectRange( const T &current, const Y &search, const int range = 3 );
		template<class T, class Y> bool checkProtectRange( const T &current, const std::vector<Y> &search, const int range = 3 );
		void objectIconSet( );

		void lengthSet( int input );
		void widthSet( int input );
		void monsterAmountDesiredSet( int input );
		int length( );
		int width( );
		int monsterAmountDesired( );
		void staticDataMap( );
		void completeDataMap( );
		char staticDataMap( int x, int y );
		char completeDataMap( int x, int y );

	private:
		int _length;
		int _width;
		int _monsterAmountDesired;
		std::vector< std::vector<char> > _staticDataMap;
		std::vector< std::vector<char> > _completeDataMap;
};