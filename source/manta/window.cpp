#include <manta/window.hpp>

#include <manta/input.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iwindow
{
	// Implementations: manta/backend/window/...
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace window
{
	i32 width = WINDOW_WIDTH_DEFAULT;
	i32 height = WINDOW_HEIGHT_DEFAULT;
	i32 widthDefault = WINDOW_WIDTH_DEFAULT;
	i32 heightDefault = WINDOW_HEIGHT_DEFAULT;
	i32 widthMin = WINDOW_WIDTH_MIN;
	i32 heightMin = WINDOW_HEIGHT_MIN;
	i32 widthMax = WINDOW_WIDTH_MAX;
	i32 heightMax = WINDOW_HEIGHT_MAX;

	float scale = 1.0f;

	bool hasFocus = true;
	bool allowFullscreen = true;
	bool allowResize = true;
	bool fullscreen = false;
	bool resized = false;

	void update( const Delta delta )
	{
		iwindow::poll();
	}
}
