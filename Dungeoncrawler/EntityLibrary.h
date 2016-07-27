#pragma once

#include "Enums.h"
#include <vector>
#include <string>

struct Character
{
    std::string name;
    char icon;
    int health;
    int healthMax;
    int healthRegen;
    int damage;
    int spells;
    int attributes;

    void Update( );
};

struct Structure
{
    std::string name;
    char icon;
    int attributes;
};

class EntityLibrary
{
    public:
        EntityLibrary( );

        //template<Category::CategoryType category, class T> const T& Get( int id ) const;
        const Character& GetCharacter( int id ) const;
        const Structure& GetStructure( int id ) const;
        char GetIcon( Category::CategoryType category, int id ) const;
        int GetAttribute( Category::CategoryType category, int id ) const;

    private:
        std::vector<Character> _characters;
        std::vector<Structure> _structures;

        void LoadCharacters( );
        void LoadStructures( );
};

//template<Category::CategoryType category, class T> const T& EntityLibrary::Get( int id ) const
//{
//    return T( );
//}
//
//template<Category::Character, class T = Character> const T& EntityLibrary::Get( int id ) const
//{
//    return _character[id];
//}
//
//template<Category::Structure, class T = Structure> const T& EntityLibrary::Get( int id ) const
//{
//    return _structure[id];
//}