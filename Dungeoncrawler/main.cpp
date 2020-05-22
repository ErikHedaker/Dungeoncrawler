#include "Game.h"
#include <iostream>

int main( int argc, char* argv[] )
{
    const bool noclear = ( argc > 1 ? !strcmp( argv[1], "noclear" ) : false );
    Game game( !noclear );

    game.Menu( );

    return 0;
}