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

//- (void)terminate:(id)sender { engine::exit(); }

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
- (void)windowDidEnterFullScreen:(NSNotification *)notification { window::fullscreen = true; }
- (void)windowDidExitFullScreen:(NSNotification *)notification  { window::fullscreen = false; }

// Called when the window gains/loses focus
- (void)windowDidBecomeKey:(NSNotification *)notification { window::hasFocus = true; }
- (void)windowDidResignKey:(NSNotification *)notification { window::hasFocus = false; }

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
		ikeyboard::keyboards[ikeyboard::primary].keyCurrent[event.keyCode] = true;

		/*
		// Keyboard Input Text
		const char *buffer = [[event characters] UTF8String];
		keyboard_input_string_submit( buffer );
		*/
	}

- (void)keyUp: (NSEvent *)event { ikeyboard::keyboards[ikeyboard::primary].keyCurrent[event.keyCode] = false; }

// A modifier key was pressed or released
- (void)flagsChanged: (NSEvent *)event
{
	NSUInteger flags = event.modifierFlags & NSEventModifierFlagDeviceIndependentFlagsMask;
	ikeyboard::keyboards[ikeyboard::primary].keyCurrent[vk_shift] = flags & NSEventModifierFlagShift;
	ikeyboard::keyboards[ikeyboard::primary].keyCurrent[vk_control] = flags & NSEventModifierFlagControl;
	ikeyboard::keyboards[ikeyboard::primary].keyCurrent[vk_alt] = flags & NSEventModifierFlagOption;
	ikeyboard::keyboards[ikeyboard::primary].keyCurrent[vk_command] = flags & NSEventModifierFlagCommand;
}

// Left Mouse Button
- (void)mouseDown: (NSEvent *)event
	{
		if( !ikeyboard::keyboards[ikeyboard::primary].keyCurrent[vk_control] )
		{
			imouse::mice[imouse::primary].buttonCurrent |= mb_left;
		}
		else
		{
			imouse::mice[imouse::primary].buttonCurrent |= mb_middle;
		}
	}
- (void)mouseUp: (NSEvent *)event
	{
		imouse::mice[imouse::primary].buttonCurrent &= ~mb_left;
		if( ikeyboard::keyboards[ikeyboard::primary].keyCurrent[vk_control] ) { imouse::mice[imouse::primary].buttonCurrent  &= ~mb_middle; }
	}
- (void)rightMouseDown:(NSEvent *)event { imouse::mice[imouse::primary].buttonCurrent |= mb_right; }
- (void)rightMouseUp: (NSEvent *)event { imouse::mice[imouse::primary].buttonCurrent &= ~mb_right; }
- (void)otherMouseDown: (NSEvent *)event { imouse::mice[imouse::primary].buttonCurrent |= mb_middle; }
- (void)otherMouseUp: (NSEvent *)event { imouse::mice[imouse::primary].buttonCurrent &= ~mb_middle; }
- (void)scrollWheel: (NSEvent *)event
	{
		if( event.deltaY > 0 )
		{
			imouse::mice[imouse::primary].wheelY = -1; // up
			imouse::mice[imouse::primary].wheelYVelocity = static_cast<float>( event.deltaY ) * 0.25f;
		}

		else if( event.deltaY < 0 )
		{
			imouse::mice[imouse::primary].wheelY = 1; // down
			imouse::mice[imouse::primary].wheelYVelocity = static_cast<float>( -event.deltaY ) * 0.25f;
		}

		if( event.deltaX > 0 )
		{
			imouse::mice[imouse::primary].wheelX = 1; // right
			imouse::mice[imouse::primary].wheelXVelocity = static_cast<float>( event.deltaX ) * 0.25f;
		}

		else if( event.deltaX < 0 )
		{
			imouse::mice[imouse::primary].wheelX = -1; // left
			imouse::mice[imouse::primary].wheelXVelocity = static_cast<float>( -event.deltaX ) * 0.25f;
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

	imouse::mice[imouse::primary].xPrevious = imouse::mice[imouse::primary].x;
	imouse::mice[imouse::primary].yPrevious = imouse::mice[imouse::primary].y;
	imouse::mice[imouse::primary].x = static_cast<float>( point.x );
	imouse::mice[imouse::primary].y = static_cast<float>( frame.size.height - point.y );
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
	window::width = static_cast<i32>( dirtyRect.size.width );
	window::height = static_cast<i32>( dirtyRect.size.height );
	window::resized = true;
	Gfx::viewport_update();
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iwindow
{
	NSView *view;
	static NSAutoreleasePool *pool;
	static NSWindow *window;

	bool init( const i32 defaultWidth, const i32 defaultHeight )
	{
		// TODO
		window::width = defaultWidth;
		window::height = defaultHeight;
		window::resized = true;

		// Lazy-initialize the application singleton into the global 'NSApp' variable
		[GameApplication sharedApplication];

		// Turning ARC on in the compiler (default) is nto the end of the story
		// Objects marked 'autorelease' still require an active autorelease pool if you
		// don't want to leak memory
		pool = [NSAutoreleasePool new];

		// Get Screen Size
		NSScreen *screen = [NSScreen mainScreen];
		NSRect area = [screen frame];
		window::scale = [screen backingScaleFactor];

		// Create Window
		iwindow::window = [[GameWindow alloc] initWithContentRect: NSMakeRect( static_cast<int>( area.size.width - defaultWidth ) >> 1,
																			   static_cast<int>( area.size.height - defaultHeight ) >> 1,
																			   defaultWidth, defaultHeight )
																			   styleMask: WINDOW_STYLE
																			   backing: NSBackingStoreBuffered
																			   defer: NO ];

		// Check Window
		if( iwindow::window == nil )
			{ ErrorReturnMsg( false, "COCOA: Failed to create cocoa window" ); }

		// Create Window View
		iwindow::view = [GameView new];

		// Setup Window Properties
		[iwindow::window setContentView: iwindow::view];
		[iwindow::window makeFirstResponder: iwindow::view];
		[iwindow::window setDelegate: (id)[GameWindowDelegate new]];
		[iwindow::window setAcceptsMouseMovedEvents: YES];
		[iwindow::window setTitle: @WINDOW_TITLE];
		[iwindow::window setRestorable: NO];
		[iwindow::window setContentMinSize: NSMakeSize( WINDOW_WIDTH_MIN, WINDOW_WIDTH_MAX ) ];

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

	void show()
	{
		// Bring the Application into focus
		[NSApp activateIgnoringOtherApps: YES];

		// Bring the Window into focus
		[iwindow::window makeKeyAndOrderFront: nil];
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
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace window
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
		NSRect frame = [iwindow::window frameRectForContentRect:contentRect];

		// Receneter window
		NSRect screenRect = [[NSScreen mainScreen] frame];
		frame.origin.x = ( screenRect.size.width - frame.size.width ) / 2;
		frame.origin.y = ( screenRect.size.height - frame.size.height ) / 2;

		[iwindow::window setFrame:frame display:YES animate:YES];

		// Success
		return true;
	}

	void fullscreen_set( bool enabled )
	{
		if( fullscreen != window::fullscreen )
		{
			[iwindow::window toggleFullScreen:nil];

			// HACK
			for( int i = 0; i < 255; i++ )
			{
				ikeyboard::keyboards[ikeyboard::primary].keyCurrent[i] = false;
			}
		}
	}
}