#pragma once

#include <vector>
#include <string>
#include <map>
#include <optional>

struct Character;

struct Power
{
    bool beneficial;
    int rolls;
    int sides;
    int modifier;
};

struct Effect
{
    Effect( const std::string& name, const std::optional<Power>& power, int duration ) :
        name( name ),
        power( power ),
        duration( duration )
    { }

    std::string name;
    std::optional<Power> power;
    int duration;
};

struct Spell
{
    Spell( const std::string& name, const std::optional<Power>& power, int effects ) :
        name( name ),
        power( power ),
        effects( effects )
    { }

    std::string name;
    std::optional<Power> power;
    int effects;
};

class BattleSystem
{
    public:
        BattleSystem( );

        void Encounter( Character& player, Character& enemy ) const;
        void TurnPlayer( Character& player, Character& enemy, std::string& event, std::string_view print, bool& flee ) const;
        void TurnAI( Character& AI, Character& enemy, std::string& event ) const;
        void Update( Character& character, std::string& event ) const;
        std::optional<Spell> InputSpell( const std::vector<Spell>& spells ) const;
        std::vector<Effect> GetEffects( int bitmask ) const;
        std::vector<Spell> GetSpells( int bitmask ) const;


    private:
        const std::map<int, Effect> _effects;
        const std::map<int, Spell> _spells;
};