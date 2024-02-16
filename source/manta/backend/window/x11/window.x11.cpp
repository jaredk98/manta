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

namespace iwindow
{
	Display *display;
	Window handle;

	bool init( const i32 defaultWidth, const i32 defaultHeight )
	{
		XVisualInfo *visual;
		XSetWindowAttributes attributes;
		XSizeHints hints;

		window::width = defaultWidth;
		window::height = defaultHeight;
		window::resized = true;

		// Open Display
		if( ( iwindow::display = XOpenDisplay( nullptr ) ) == nullptr )
			{ ErrorReturnMsg( false, "X11: Failed to open X11 display" ); }

		// Setup Window Visual
		if( ( visual = iwindow::x11_create_visual() ) == nullptr )
			{ ErrorReturnMsg( false, "X11: Failed to create X11 visual" ); }

		// Setup Window Attribute
		attributes.background_pixel = BlackPixel( iwindow::display, DefaultScreen( iwindow::display ) );
		attributes.border_pixel = BlackPixel( iwindow::display, DefaultScreen( iwindow::display ) );
		attributes.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask;
		attributes.colormap = XCreateColormap( iwindow::display, DefaultRootWindow( iwindow::display ), visual->visual, AllocNone );

		// Create Window
		iwindow::handle =
			XCreateWindow(
				iwindow::display,
				DefaultRootWindow( iwindow::display ),
				0,
				0,
				window::width,
				window::height,
				0,
				visual->depth,
				InputOutput,
				visual->visual,
				CWBackPixel | CWBorderPixel | CWEventMask | CWColormap,
				&attributes
			);

		// Setup Atoms
		WM_DELETE_WINDOW = XInternAtom( iwindow::display, "WM_DELETE_WINDOW", false );
		_NET_WM_STATE = XInternAtom( iwindow::display, "_NET_WM_STATE", false );
		_NET_WM_STATE_FULLSCREEN = XInternAtom( iwindow::display, "_NET_WM_STATE_FULLSCREEN", false );

		// Setup Protocols
		if( !XSetWMProtocols( iwindow::display, iwindow::handle, &WM_DELETE_WINDOW, 1 ) )
			{ ErrorReturnMsg( false, "X11: Failed to set X11 WM protocols" ); }

		// Setup Size
		hints.flags = PMinSize;
		hints.min_width = WINDOW_WIDTH_MIN;
		hints.min_height = WINDOW_HEIGHT_MIN;
		hints.max_width = WINDOW_WIDTH_MAX;
		hints.max_height = WINDOW_HEIGHT_MAX;
		XSetWMNormalHints( iwindow::display, iwindow::handle, &hints );

		// Set Window Title
		XStoreName( iwindow::display, iwindow::handle, PROJECT_CAPTION );

		// Success
		return true;
	}


	void show()
	{
		XMapRaised( iwindow::display, iwindow::handle );
	}


	void poll()
	{
		//PROFILE_FUNCTION();

		while( XPending( iwindow::display ) )
		{
			XEvent event;

			XNextEvent( iwindow::display, &event );

			if( XFilterEvent( &event, None ) ) { continue; }

			switch( event.type )
			{
				// Window Resize
				case ConfigureNotify:
					// Resize
					if( event.xconfigure.width != window::width || event.xconfigure.height != window::height )
					{
						window::width = event.xconfigure.width;
						window::height = event.xconfigure.height;
						window::resized = true;
						Gfx::viewport_update();
					}
				break;

				// The focus changed
				case FocusIn:
					window::hasFocus = true;
				break;

				case FocusOut:
					window::hasFocus = false;
				break;

				// Key Press / Release
				case KeyPress:
				case KeyRelease:
				{

					KeySym keysym = XLookupKeysym( &event.xkey, 0 );
					ikeyboard::keyboards[ikeyboard::primary].keyCurrent[ keysym & 0xFF ] = ( event.type == KeyPress );
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
					imouse::mice[imouse::primary].buttonCurrent |= ( 1 << event.xbutton.button );

					// Mouse wheel (TODO: Button6 & Button7 for horizontal scroll wheel)
					if( event.xbutton.button == Button4 ) { imouse::mice[imouse::primary].wheelY = -1; imouse::mice[imouse::primary].wheelYVelocity = 1.0f; }
					if( event.xbutton.button == Button5 ) { imouse::mice[imouse::primary].wheelY =  1; imouse::mice[imouse::primary].wheelYVelocity = 1.0f; }
					if( event.xbutton.button == 6/*Button6*/ ) { imouse::mice[imouse::primary].wheelX = -1; imouse::mice[imouse::primary].wheelXVelocity = 1.0f; }
					if( event.xbutton.button == 7/*Button7*/ ) { imouse::mice[imouse::primary].wheelX =  1; imouse::mice[imouse::primary].wheelXVelocity = 1.0f; }
				}
				break;

				// Mouse Release
				case ButtonRelease:
				{
					imouse::mice[imouse::primary].buttonCurrent &= ~( 1 << event.xbutton.button );
				}
				break;

				// Mouse Position
				case MotionNotify:
				{
					imouse::mice[imouse::primary].xPrevious = imouse::mice[imouse::primary].x;
					imouse::mice[imouse::primary].yPrevious = imouse::mice[imouse::primary].y;
					imouse::mice[imouse::primary].x = static_cast<float>( event.xmotion.x );
					imouse::mice[imouse::primary].y = static_cast<float>( event.xmotion.y );
				}
				break;


				// Another client (likely the window manager) has sent us a message
				case ClientMessage:
				{
					// Gracefully handle a close request from the window
					if( event.xclient.data.l[0] == WM_DELETE_WINDOW )
					{
						engine::exit();
						return;
					}
				}
				break;
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace window
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
		event.xclient.window = iwindow::handle;
		event.xclient.message_type = _NET_WM_STATE;
		event.xclient.format = 32;
		event.xclient.data.l[0] = fullscreen;
		event.xclient.data.l[1] = _NET_WM_STATE_FULLSCREEN;
		event.xclient.data.l[2] = 0;
		event.xclient.data.l[3] = 0;

		// Send the message to the window manager.
		XSendEvent( iwindow::display, DefaultRootWindow( iwindow::display ), false, SubstructureRedirectMask | SubstructureNotifyMask, &event );

		// Don't forget to update the internal fullscreen state!
		window::fullscreen = fullscreen;
	}
}