#include <manta/input.hpp>

#include <manta/window.hpp>

#include <debug.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

iKeyboard::KeyboardListener iKeyboard::keyboards[keyboardCount];
u8 iKeyboard::primary = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void iKeyboard::KeyboardListener::update( const Delta delta )
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


void iKeyboard::KeyboardListener::clear()
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

iMouse::MouseListener iMouse::mice[8];
u8 iMouse::primary = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Mouse
{
	void set_position( const int x, const int y ) { iWindow::mouse_set_position( x, y ); }
}


void iMouse::MouseListener::update( const Delta delta )
{
	buttonPrevious = buttonCurrent;

	wheelX = 0;
	wheelY = 0;
	wheelXVelocity = 0.0f;
	wheelYVelocity = 0.0f;
}


void iMouse::MouseListener::clear()
{
	buttonPrevious = 0;
	buttonCurrent = 0;

	wheelX = 0;
	wheelY = 0;
	wheelXVelocity = 0.0f;
	wheelYVelocity = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////