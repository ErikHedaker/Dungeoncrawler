#include "Game.h"
#include "Functions.h"

int main( )
{
    const std::vector<Ability> abilities = LoadAbilities( );
    const std::vector<Character> characters = LoadCharacters( abilities );
    const std::vector<Structure> structures = LoadStructures( );
    Game game( abilities, characters, structures );

    game.Menu( );

    return 0;
}