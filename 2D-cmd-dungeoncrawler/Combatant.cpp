#include "Combatant.h"

Combatant::Combatant( const std::string& name, int health, int damage, int abilities ) :
	name( name ),
	health( health ),
	damage( damage ),
	abilities( abilities )
{ }