#include <manta/window.hpp>
#include <manta/backend/window/cocoa/window.cocoa.hpp>

#import <Cocoa/Cocoa.h>

#include <config.hpp>
#include <debug.hpp>
#include <types.hpp>

#include <manta/engine.hpp>
#include <manta/input.hpp>
#include <manta/gfx.hpp>

#include <vendor/stdlib.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WINDOW_TITLE PROJECT_CAPTION
#define WINDOW_STYLE ( NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Subclass Application
@interface GameApplication : NSApplication @end
@implementation GameApplication

//- (void)terminate:(id)sender { Engine::exit(); }

@end

// Subclass Window
@interface GameWindow : NSWindow @end
@implementation GameWindow

- (BOOL)canBecomeKeyWindow  { return YES; }
- (BOOL)canBecomeMainWindow { return YES; }

@end

// Window Delegate
@interface GameWindowDelegate : NSObject @end
@implementation GameWindowDelegate

- (BOOL)windowShouldClose:(NSWindow *)sender
{
	// Terminate the application when window is closed
	[ NSApp terminate:nil ];

	// The window will be closed implicitly when the applicaiton exists
	return NO;
}

// Toggling fullscreen is asynch in Cocoa, so these callbacks tell us
- (void)windowDidEnterFullScreen:(NSNotification *)notification { Window::fullscreen = true; }
- (void)windowDidExitFullScreen:(NSNotification *)notification  { Window::fullscreen = false; }

// Called when the window gains/loses focus
- (void)windowDidBecomeKey:(NSNotification *)notification { Window::hasFocus = true; }
- (void)windowDidResignKey:(NSNotification *)notification { Window::hasFocus = false; }

@end

// Subclass View
@interface GameView : NSView @end
@implementation GameView

- (BOOL)canBecomeKeyView { return YES; }
- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)acceptsFirstMouse:(NSEvent *)event { return YES; }

// A regular key was pressed or released
- (void)keyDown: (NSEvent *)event
	{
		iKeyboard::keyboards[iKeyboard::primary].keyCurrent[event.keyCode] = true;

		/*
		// Keyboard Input Text
		const char *buffer = [[event characters] UTF8String];
		keyboard_input_string_submit( buffer );
		*/
	}

- (void)keyUp: (NSEvent *)event { iKeyboard::keyboards[iKeyboard::primary].keyCurrent[event.keyCode] = false; }

// A modifier key was pressed or released
- (void)flagsChanged: (NSEvent *)event
{
	NSUInteger flags = event.modifierFlags & NSEventModifierFlagDeviceIndependentFlagsMask;
	iKeyboard::keyboards[iKeyboard::primary].keyCurrent[vk_shift] = flags & NSEventModifierFlagShift;
	iKeyboard::keyboards[iKeyboard::primary].keyCurrent[vk_control] = flags & NSEventModifierFlagControl;
	iKeyboard::keyboards[iKeyboard::primary].keyCurrent[vk_alt] = flags & NSEventModifierFlagOption;
	iKeyboard::keyboards[iKeyboard::primary].keyCurrent[vk_command] = flags & NSEventModifierFlagCommand;
}

// Left Mouse Button
- (void)mouseDown: (NSEvent *)event
	{
		if( !iKeyboard::keyboards[iKeyboard::primary].keyCurrent[vk_control] )
		{
			iMouse::mice[iMouse::primary].buttonCurrent |= mb_left;
		}
		else
		{
			iMouse::mice[iMouse::primary].buttonCurrent |= mb_middle;
		}
	}
- (void)mouseUp: (NSEvent *)event
	{
		iMouse::mice[iMouse::primary].buttonCurrent &= ~mb_left;
		if( iKeyboard::keyboards[iKeyboard::primary].keyCurrent[vk_control] ) { iMouse::mice[iMouse::primary].buttonCurrent  &= ~mb_middle; }
	}
- (void)rightMouseDown:(NSEvent *)event { iMouse::mice[iMouse::primary].buttonCurrent |= mb_right; }
- (void)rightMouseUp: (NSEvent *)event { iMouse::mice[iMouse::primary].buttonCurrent &= ~mb_right; }
- (void)otherMouseDown: (NSEvent *)event { iMouse::mice[iMouse::primary].buttonCurrent |= mb_middle; }
- (void)otherMouseUp: (NSEvent *)event { iMouse::mice[iMouse::primary].buttonCurrent &= ~mb_middle; }
- (void)scrollWheel: (NSEvent *)event
	{
		if( event.deltaY > 0 )
		{
			iMouse::mice[iMouse::primary].wheelY = -1; // up
			iMouse::mice[iMouse::primary].wheelYVelocity = static_cast<float>( event.deltaY ) * 0.25f;
		}

		else if( event.deltaY < 0 )
		{
			iMouse::mice[iMouse::primary].wheelY = 1; // down
			iMouse::mice[iMouse::primary].wheelYVelocity = static_cast<float>( -event.deltaY ) * 0.25f;
		}

		if( event.deltaX > 0 )
		{
			iMouse::mice[iMouse::primary].wheelX = 1; // right
			iMouse::mice[iMouse::primary].wheelXVelocity = static_cast<float>( event.deltaX ) * 0.25f;
		}

		else if( event.deltaX < 0 )
		{
			iMouse::mice[iMouse::primary].wheelX = -1; // left
			iMouse::mice[iMouse::primary].wheelXVelocity = static_cast<float>( -event.deltaX ) * 0.25f;
		}
	}

// Mouse Position was changed
- (void)mouseMoved:(NSEvent *)event
{
	NSRect frame = [self frame];
	NSPoint point = [event locationInWindow];

	if( point.x < 0.0 || point.x >= frame.size.width ||
		point.y < 0.0 || point.y >= frame.size.height )
	{
		return;
	}

	iMouse::mice[iMouse::primary].xPrevious = iMouse::mice[iMouse::primary].x;
	iMouse::mice[iMouse::primary].yPrevious = iMouse::mice[iMouse::primary].y;
	iMouse::mice[iMouse::primary].x = static_cast<float>( point.x );
	iMouse::mice[iMouse::primary].y = static_cast<float>( frame.size.height - point.y );
}

// For some reason, when a mouse button is being held down, cocoa doesn't send
// mouse move events, instead sends us "dragged" events corresponding to the
// buttons being held. Let's just forward these to regular mouseMoved events
- (void)mouseDragged:     (NSEvent *)event { [self mouseMoved:event]; }
- (void)rightMouseDragged:(NSEvent *)event { [self mouseMoved:event]; }
- (void)otherMouseDragged:(NSEvent *)event { [self mouseMoved:event]; }

// The view was resized
- (void)drawRect:(NSRect)dirtyRect
{
	Window::width = static_cast<i32>( dirtyRect.size.width );
	Window::height = static_cast<i32>( dirtyRect.size.height );
	Window::resized = true;
	Gfx::viewport_update();
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iWindow
{
	NSView *view;
	static NSAutoreleasePool *pool;
	static NSWindow *window;

	bool init( const i32 defaultWidth, const i32 defaultHeight )
	{
		// TODO
		Window::width = defaultWidth;
		Window::height = defaultHeight;
		Window::resized = true;

		// Lazy-initialize the application singleton into the global 'NSApp' variable
		[GameApplication sharedApplication];

		// Turning ARC on in the compiler (default) is nto the end of the story
		// Objects marked 'autorelease' still require an active autorelease pool if you
		// don't want to leak memory
		pool = [NSAutoreleasePool new];

		// Get Screen Size
		NSScreen *screen = [NSScreen mainScreen];
		NSRect area = [screen frame];
		Window::scale = [screen backingScaleFactor];

		// Create Window
		iWindow::window = [[GameWindow alloc] initWithContentRect: NSMakeRect( static_cast<int>( area.size.width - defaultWidth ) >> 1,
																			   static_cast<int>( area.size.height - defaultHeight ) >> 1,
																			   defaultWidth, defaultHeight )
																			   styleMask: WINDOW_STYLE
																			   backing: NSBackingStoreBuffered
																			   defer: NO ];

		// Check Window
		if( iWindow::window == nil )
			{ ErrorReturnMsg( false, "COCOA: Failed to create cocoa window" ); }

		// Create Window View
		iWindow::view = [GameView new];

		// Setup Window Properties
		[iWindow::window setContentView: iWindow::view];
		[iWindow::window makeFirstResponder: iWindow::view];
		[iWindow::window setDelegate: (id)[GameWindowDelegate new]];
		[iWindow::window setAcceptsMouseMovedEvents: YES];
		[iWindow::window setTitle: @WINDOW_TITLE];
		[iWindow::window setRestorable: NO];
		[iWindow::window setContentMinSize: NSMakeSize( WINDOW_WIDTH_MIN, WINDOW_WIDTH_MAX ) ];

		// Setup Apple Menu
		// TODO

		// macOS applications that are not bundled must be explicitly made into UI
		// apps by calling this function otherwise we won't appear in the dock and
		// the window will not be visible
		[NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];

		[NSApp finishLaunching];

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
		// Bring the Application into focus
		[NSApp activateIgnoringOtherApps: YES];

		// Bring the Window into focus
		[iWindow::window makeKeyAndOrderFront: nil];
	}


	void poll()
	{
		//PROFILE_FUNCTION();

		for( ;; )
		{
			// Get the next event in the queue
			NSEvent *event = [NSApp nextEventMatchingMask: NSEventMaskAny
									untilDate: nil
									inMode: NSDefaultRunLoopMode
									dequeue: YES ];

			// There are no more events, break
			if( event == nil ) { break; }

			// Forward the event to the rest of the application
			[NSApp sendEvent:event];
		}

		// All of the memory management for Cocoa is done here
		// First, drain the pool (one always exists from window_init() )
		[pool drain];

		// Next, create the pool for the next frame
		pool = [NSAutoreleasePool new];
	}


	void mouse_set_position( const int x, const int y )
	{
		NSPoint newPoint = NSMakePoint( x, y );

		NSEvent *mouseEvent = [ NSEvent mouseEventWithType: NSMouseMoved
		                                location: newPoint
		                                modifierFlags: 0
		                                timestamp: 0
		                                windowNumber: [ NSApp mainWindow ].windowNumber
		                                context: nil
		                                eventNumber: 0
		                                clickCount: 0
		                                pressure: 0 ];

		[NSApp postEvent: mouseEvent atStart: NO];
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Window
{
	void show_message( const char *title, const char *message )
	{
		NSApplication *app = [NSApplication sharedApplication];
		NSAlert *alert = [[NSAlert alloc] init];
		[alert setIcon:nil];
		[alert setMessageText:[NSString stringWithUTF8String:title]];
		[alert setInformativeText:[NSString stringWithUTF8String:message]];
		[alert runModal];
	}


	bool resize( i32 width, i32 height )
	{
		// Resize Window
		NSRect contentRect = { { 0, 0 }, { static_cast<CGFloat>( width ), static_cast<CGFloat>( height ) } };
		NSRect frame = [iWindow::window frameRectForContentRect:contentRect];

		// Receneter window
		NSRect screenRect = [[NSScreen mainScreen] frame];
		frame.origin.x = ( screenRect.size.width - frame.size.width ) / 2;
		frame.origin.y = ( screenRect.size.height - frame.size.height ) / 2;

		[iWindow::window setFrame:frame display:YES animate:YES];

		// Success
		return true;
	}


	void fullscreen_set( bool enabled )
	{
		if( fullscreen != Window::fullscreen )
		{
			[iWindow::window toggleFullScreen:nil];

			// HACK
			for( int i = 0; i < 255; i++ )
			{
				iKeyboard::keyboards[iKeyboard::primary].keyCurrent[i] = false;
			}
		}
	}
}