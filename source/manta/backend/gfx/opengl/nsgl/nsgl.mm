#include <manta/backend/gfx/opengl/opengl.hpp>
#include <manta/backend/window/cocoa/window.cocoa.hpp>

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>

#include <debug.hpp>


static const NSOpenGLPixelFormatAttribute attributes[]
{
	// Double buffered context
	NSOpenGLPFADoubleBuffer,

	// OpenGL 3.2 (Core Profile) context
	NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,

	// 32-bit color buffer
	NSOpenGLPFAColorSize, 32,

	// NOTE: Apple says to do this
	NSOpenGLPFAAllowOfflineRenderers,

	// Null-terminate the attribute list
	0,
};

static NSOpenGLContext *context;


bool opengl_init()
{
	// Create Pixel Format
	NSOpenGLPixelFormat *format;
	if( ( format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes]) == nil )
		{ ErrorReturnMsg( false, "OpenGL: Failed to create pixel format" ); }

	// Create OpenGL Context
	if( ( context = [[NSOpenGLContext alloc] initWithFormat:format shareContext:nil] ) == nil )
		{ ErrorReturnMsg( false, "OpenGL: Failed to create OpenGL context" ); }

	// Disable Retina Framebuffer
	[iwindow::view setWantsBestResolutionOpenGLSurface:YES];

	// Bind Context
	[context setView:iwindow::view];
	[context makeCurrentContext];

	// Disable Vsync (TODO)
	int vsync = 0;
	[context setValues:&vsync forParameter:NSOpenGLContextParameterSwapInterval];

	// Success
	return true;
}


bool opengl_swap()
{
	[context flushBuffer];

	// Success
	return true;
}


void *opengl_proc( const char *name )
{
	return nullptr;
}


void opengl_update()
{
	//PROFILE_FUNCTION();
	[context update];
}
