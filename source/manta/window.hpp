#pragma once

#include <config.hpp>
#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iwindow
{
	extern bool init( const i32 width = WINDOW_WIDTH_DEFAULT, const i32 height = WINDOW_HEIGHT_DEFAULT );
	extern void show();
	extern void poll();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace window
{
	extern i32 width;
	extern i32 height;
	extern i32 widthDefault;
	extern i32 heightDefault;
	extern i32 widthMin;
	extern i32 heightMin;
	extern i32 widthMax;
	extern i32 heightMax;

	extern float scale;

	extern bool hasFocus;
	extern bool allowFullscreen;
	extern bool allowResize;
	extern bool fullscreen;
	extern bool resized;

	extern void update( const Delta delta );

	extern void show_message( const char *title, const char *message );

	extern bool size_set( const i32 width, const i32 height );
	extern void fullscreen_set( bool enabled );
}