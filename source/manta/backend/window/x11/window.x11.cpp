#include <manta/window.hpp>
#include <manta/backend/window/x11/window.x11.hpp>

#include <config.hpp>
#include <debug.hpp>
#include <types.hpp>

#include <manta/engine.hpp>
#include <manta/input.hpp>
#include <manta/gfx.hpp>

#include <vendor/stdlib.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Macros...

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Atom WM_DELETE_WINDOW;
static Atom _NET_WM_STATE;
static Atom _NET_WM_STATE_FULLSCREEN;

namespace iWindow
{
	Display *display;
	Window handle;

	bool init( const i32 defaultWidth, const i32 defaultHeight )
	{
		XVisualInfo *visual;
		XSetWindowAttributes attributes;
		XSizeHints hints;

		Window::width = defaultWidth;
		Window::height = defaultHeight;
		Window::resized = true;

		// Open Display
		if( ( iWindow::display = XOpenDisplay( nullptr ) ) == nullptr )
			{ ErrorReturnMsg( false, "X11: Failed to open X11 display" ); }

		// Setup Window Visual
		if( ( visual = iWindow::x11_create_visual() ) == nullptr )
			{ ErrorReturnMsg( false, "X11: Failed to create X11 visual" ); }

		// Setup Window Attribute
		attributes.background_pixel = BlackPixel( iWindow::display, DefaultScreen( iWindow::display ) );
		attributes.border_pixel = BlackPixel( iWindow::display, DefaultScreen( iWindow::display ) );
		attributes.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask;
		attributes.colormap = XCreateColormap( iWindow::display, DefaultRootWindow( iWindow::display ), visual->visual, AllocNone );

		// Create Window
		iWindow::handle =
			XCreateWindow(
				iWindow::display,
				DefaultRootWindow( iWindow::display ),
				0,
				0,
				Window::width,
				Window::height,
				0,
				visual->depth,
				InputOutput,
				visual->visual,
				CWBackPixel | CWBorderPixel | CWEventMask | CWColormap,
				&attributes
			);

		// Setup Atoms
		WM_DELETE_WINDOW = XInternAtom( iWindow::display, "WM_DELETE_WINDOW", false );
		_NET_WM_STATE = XInternAtom( iWindow::display, "_NET_WM_STATE", false );
		_NET_WM_STATE_FULLSCREEN = XInternAtom( iWindow::display, "_NET_WM_STATE_FULLSCREEN", false );

		// Setup Protocols
		if( !XSetWMProtocols( iWindow::display, iWindow::handle, &WM_DELETE_WINDOW, 1 ) )
			{ ErrorReturnMsg( false, "X11: Failed to set X11 WM protocols" ); }

		// Setup Size
		hints.flags = PMinSize;
		hints.min_width = WINDOW_WIDTH_MIN;
		hints.min_height = WINDOW_HEIGHT_MIN;
		hints.max_width = WINDOW_WIDTH_MAX;
		hints.max_height = WINDOW_HEIGHT_MAX;
		XSetWMNormalHints( iWindow::display, iWindow::handle, &hints );

		// Set Window Title
		XStoreName( iWindow::display, iWindow::handle, PROJECT_CAPTION );

		// Success
		return true;
	}


	bool free()
	{
		// Success
		return true;
	}


	void show()
	{
		XMapRaised( iWindow::display, iWindow::handle );
	}


	void poll()
	{
		//PROFILE_FUNCTION();

		while( XPending( iWindow::display ) )
		{
			XEvent event;

			XNextEvent( iWindow::display, &event );

			if( XFilterEvent( &event, None ) ) { continue; }

			switch( event.type )
			{
				// Window Resize
				case ConfigureNotify:
					// Resize
					if( event.xconfigure.width != Window::width || event.xconfigure.height != Window::height )
					{
						Window::width = event.xconfigure.width;
						Window::height = event.xconfigure.height;
						Window::resized = true;
						Gfx::viewport_update();
					}
				break;

				// The focus changed
				case FocusIn:
					Window::hasFocus = true;
				break;

				case FocusOut:
					Window::hasFocus = false;
				break;

				// Key Press / Release
				case KeyPress:
				case KeyRelease:
				{

					KeySym keysym = XLookupKeysym( &event.xkey, 0 );
					iKeyboard::keyboards[iKeyboard::primary].keyCurrent[ keysym & 0xFF ] = ( event.type == KeyPress );
					/*
					// Keyboard Input Text
					if( event.type == KeyPress )
					{
						char buffer[8];
						XLookupString( &event.xkey, buffer, 8, &keysym, nullptr );
						keyboard_input_string_submit( buffer );
					}
					*/
				}
				break;

				// Mouse Press
				case ButtonPress:
				{
					iMouse::mice[iMouse::primary].buttonCurrent |= ( 1 << event.xbutton.button );

					// Mouse wheel (TODO: Button6 & Button7 for horizontal scroll wheel)
					if( event.xbutton.button == Button4 ) { iMouse::mice[iMouse::primary].wheelY = -1; iMouse::mice[iMouse::primary].wheelYVelocity = 1.0f; }
					if( event.xbutton.button == Button5 ) { iMouse::mice[iMouse::primary].wheelY =  1; iMouse::mice[iMouse::primary].wheelYVelocity = 1.0f; }
					if( event.xbutton.button == 6/*Button6*/ ) { iMouse::mice[iMouse::primary].wheelX = -1; iMouse::mice[iMouse::primary].wheelXVelocity = 1.0f; }
					if( event.xbutton.button == 7/*Button7*/ ) { iMouse::mice[iMouse::primary].wheelX =  1; iMouse::mice[iMouse::primary].wheelXVelocity = 1.0f; }
				}
				break;

				// Mouse Release
				case ButtonRelease:
				{
					iMouse::mice[iMouse::primary].buttonCurrent &= ~( 1 << event.xbutton.button );
				}
				break;

				// Mouse Position
				case MotionNotify:
				{
					iMouse::mice[iMouse::primary].xPrevious = iMouse::mice[iMouse::primary].x;
					iMouse::mice[iMouse::primary].yPrevious = iMouse::mice[iMouse::primary].y;
					iMouse::mice[iMouse::primary].x = static_cast<float>( event.xmotion.x );
					iMouse::mice[iMouse::primary].y = static_cast<float>( event.xmotion.y );
				}
				break;


				// Another client (likely the window manager) has sent us a message
				case ClientMessage:
				{
					// Gracefully handle a close request from the window
					if( event.xclient.data.l[0] == WM_DELETE_WINDOW )
					{
						Engine::exit();
						return;
					}
				}
				break;
			}
		}
	}


	void mouse_set_position( const int x, const int y )
	{
		XWarpPointer( iWindow::display, None, iWindow::handle, 0, 0, 0, 0, x, y );
		XSync( iWindow::display, false );
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Window
{
	void fullscreen_set( bool enabled )
	{
		// TODO:
		// This is based on the 'Extended Window Manager Hints' specification, so
		// maybe you need a "modern" window manager for anything to happen. Is it
		// worth it to support zoomers' shitty knee-high vintage window managers?

		XEvent event = { };

		// Write a client message telling the window manager to add or remove
		// the _NET_WM_STATE_FULLSCREEN atom from the _NET_WM_STATE depending
		// on whether we're entering or exiting fullscreen.
		event.xclient.type = ClientMessage;
		event.xclient.window = iWindow::handle;
		event.xclient.message_type = _NET_WM_STATE;
		event.xclient.format = 32;
		event.xclient.data.l[0] = fullscreen;
		event.xclient.data.l[1] = _NET_WM_STATE_FULLSCREEN;
		event.xclient.data.l[2] = 0;
		event.xclient.data.l[3] = 0;

		// Send the message to the window manager.
		XSendEvent( iWindow::display, DefaultRootWindow( iWindow::display ), false, SubstructureRedirectMask | SubstructureNotifyMask, &event );

		// Don't forget to update the internal fullscreen state!
		Window::fullscreen = fullscreen;
	}
}