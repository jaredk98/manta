#include <manta/engine.hpp>

#include <manta/random.hpp>

#include <manta/objects.hpp>
#include <manta/timer.hpp>
#include <manta/window.hpp>
#include <manta/gfx.hpp>
#include <manta/input.hpp>
#include <manta/draw.hpp>
#include <manta/fonts.hpp>

#include <scene.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace project
{
	bool init( int argc, char **argv )
	{
		scene_init();
		return true;
	}

	bool free()
	{
		scene_free();
		return true;
	}

	void update( Delta delta )
	{
		// Hotkeys
		if( keyboard::check_pressed( vk_escape ) ) { engine::exit(); }
		if( keyboard::check_pressed( vk_f4 ) ) { window::fullscreen_set( !window::fullscreen ); }

		// Update Scene
		scene_update( delta );

		// Render
		Gfx::frame_begin();
		{
			Gfx::clear_color( { 20, 20, 40 } );
			Gfx::set_matrix_mvp( matrix_build_mvp_orthographic( 0.0f, window::width, window::height, 0.0f, 0.0, 10.0 ) );

			// Draw Scene
			scene_draw( delta );

			// Debug
			draw_text_f( fnt_consolas, 20, 8.0f, 8.0f, c_white, "FPS: %d", frame::fps );
		}
		Gfx::frame_end();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv )
{
	ProjectCallbacks callbacks { project::init, project::free, project::update };
	return engine::main( argc, argv, callbacks );
}