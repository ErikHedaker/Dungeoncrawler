#include "Game.h"

int main( int argc, char* argv[] )
{
    const bool noclear  = ( argc > 1 ? !strcmp( argv[1], "noclear"  ) : false );
    const bool nosave   = ( argc > 2 ? !strcmp( argv[2], "nosave"   ) : false );
    const bool noexit   = ( argc > 3 ? !strcmp( argv[3], "noexit"   ) : false );
    const bool noconfig = ( argc > 4 ? !strcmp( argv[4], "noconfig" ) : false );
    Game game( !noclear, !nosave, !noexit, !noconfig );

    game.Menu( );

    return 0;
}