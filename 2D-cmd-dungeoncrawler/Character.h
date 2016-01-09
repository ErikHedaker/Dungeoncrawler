#pragma once

#include "Entity.h"
#include "Enums.h"

class Character : public Entity
{
	public:
		Character( const Vector2i& position, char portrait, float speed, float armor, float damage, float health, float mana );

		void Move( const Orientation& orientation );
		void MoveTowards( const Vector2i& position );
		void MoveProbability( int north, int east, int south, int west, int still );
		void RevertPosition( );

		float GetSpeed( ) const;
		void SetSpeed( float value );

		float GetArmor( ) const;
		void SetArmor( float value );

		float GetDamage( ) const;
		void SetDamage( float value );

		float GetHealth( ) const;
		void SetHealth( float value );

		float GetMana( ) const;
		void SetMana( float value );

		bool GetAlive( ) const;
		void SetAlive( bool value );

		void Attack( Character* defender );

	private:
		Vector2i _positionPrev;
		float _baseSpeed;
		float _baseArmor;
		float _baseDamage;
		float _baseHealth;
		float _baseMana;
		bool _alive;
		void* _armor;
		void* _weapon;
};