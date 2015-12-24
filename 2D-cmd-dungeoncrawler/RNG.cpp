#include "RNG.h"
#include <random>

int RandomNumberGenerator( int min, int max )
{
	static std::random_device rd;
	std::mt19937 generator( rd( ) );
	std::uniform_int_distribution<int> randomNumber( min, max );

	return randomNumber( generator );
}