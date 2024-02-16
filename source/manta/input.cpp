#include <manta/input.hpp>
#include <debug.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ikeyboard::Keyboard ikeyboard::keyboards[keyboardCount];
u8 ikeyboard::primary = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ikeyboard::Keyboard::update( const Delta delta )
{
	// Key State
	for( int key = 0; key < 256; key++ )
	{
		keyPrevious[key] = keyCurrent[key];
		keyRepeat[key] = false;
	}

	// Key Repeat
	for( int key = 0; key < 256; key++ )
	{
		// Not holding the key?
		if( !keyCurrent[key] )
		{
			keyRepeatTimer[key] = 16.0f;
			continue;
		}

		// Held the key long enough?
		if( keyRepeatTimer[key] <= 0.0f )
		{
			keyRepeat[key] = true;
			keyRepeatTimer[key] = 2.0f;
			continue;
		}

		// Decrement timer
		keyRepeatTimer[key] -= delta;
	}
}


void ikeyboard::Keyboard::clear()
{
	for( u16 key = 0; key < 256; key++ )
	{
		keyCurrent[key] = false;
		keyPrevious[key] = false;
		keyRepeat[key] = false;
		keyRepeatTimer[key] = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

imouse::Mouse imouse::mice[8];
u8 imouse::primary = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void imouse::Mouse::update( const Delta delta )
{
	buttonPrevious = buttonCurrent;

	wheelX = 0;
	wheelY = 0;
	wheelXVelocity = 0.0f;
	wheelYVelocity = 0.0f;
}


void imouse::Mouse::clear()
{
	buttonPrevious = 0;
	buttonCurrent = 0;

	wheelX = 0;
	wheelY = 0;
	wheelXVelocity = 0.0f;
	wheelYVelocity = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////