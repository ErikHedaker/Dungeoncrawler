#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <functional>
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
    Effect& operator=( const Effect& other )
    {
        duration = other.duration;

        return *this;
    }

    const std::string name;
    const std::optional<Power> power;
    int duration;
};

struct Spell
{
    Spell( const std::string& name, const std::optional<Power>& power, int effects ) :
        name( name ),
        power( power ),
        effects( effects )
    { }

    const std::string name;
    const std::optional<Power> power;
    const int effects;
};

class BattleSystem
{
    public:
        BattleSystem( );

        void Encounter( Character& player, Character& enemy ) const;
        std::string TurnPlayer( Character& player, Character& enemy, std::string_view print, bool& flee ) const;
        std::string TurnAI( Character& AI, Character& enemy ) const;
        std::string UpdateEffects( Character& character ) const;
        std::string DisplayDetail( const Spell& spell ) const;
        std::string SelectTarget( Character*& target, const std::vector<Character*>& targets ) const;
        std::string SelectSpell( const Spell*& spell, const std::vector<std::reference_wrapper<const Spell>>& spells ) const;
        std::string AttackSpell( Character& attacker, Character& target, const Spell& spell ) const;
        std::string AttackMelee( Character& attacker, Character& target ) const;
        std::vector<std::reference_wrapper<const Effect>> GetEffects( int bitmask ) const;
        std::vector<std::reference_wrapper<const Spell>> GetSpells( int bitmask ) const;


    private:
        const std::map<int, Effect> _effects;
        const std::map<int, Spell> _spells;
};