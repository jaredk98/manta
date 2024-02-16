#include <manta/engine.hpp>

#include <manta/assets.hpp>
#include <manta/fileio.hpp>
#include <manta/gfx.hpp>
#include <manta/timer.hpp>
#include <manta/window.hpp>
#include <manta/objects.hpp>
#include <manta/input.hpp>
#include <manta/thread.hpp>
#include <manta/fonts.hpp>

#include <config.hpp>
#include <debug.hpp>
#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace engine
{
	static bool init( int argc, char **argv )
	{
		// Assets
		ErrorReturnIf( !iassets::init(), false, "Engine: failed to initialize assets" );

		// Timer
		ErrorReturnIf( !itimer::init(), false, "Engine: failed to initialize timer" );

		// Window
		ErrorReturnIf( !iwindow::init(), false, "Engine: failed to initialize window" );

		// Render
		ErrorReturnIf( !bGfx::init(), false, "Engine: failed to initialize graphics" );

		// Objects
		ErrorReturnIf( !iobjects::init(), false, "Engine: failed to initialize object system" );

		// Assets
		ErrorReturnIf( !iassets::init(), false, "Engine: failed to initialize assets" );

		// Fonts
		ErrorReturnIf( !fonts::init(), false, "Engine: failed to initialize font system" );

		// Success
		return true;
	}


	static bool free()
	{
		// Fonts
		ErrorReturnIf( !fonts::free(), false, "Engine: failed to free fonts" );

		// Assets
		// ErrorReturnIf( !iassets::free(), false, "Engine: failed to free assets" ); // TODO

		// Objects
		// ErrorReturnIf( !iobjects::free(), false, "Engine: failed to free objects" ); // TODO

		// Render
		ErrorReturnIf( !bGfx::free(), false, "Engine: failed to free render" );

		// Window
		// ErrorReturnIf( !iwindow::free(), false, "Engine: failed to free window" ); // TODO

		// Timer
		// ErrorReturnIf( !itimer::free(), false, "Engine: failed to free timer" ); // TODO

		// Assets
		ErrorReturnIf( !iassets::free(), false, "Engine: failed to free assets" );

		// Success
		return true;
	}


	static void update( const Delta delta )
	{
		// Input
		input::update( delta );

		// Window
		window::update( delta );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace engine
{
	static bool painted; // Delay showing the window until after a frame is rendered
	static bool exiting; // Terminates the main loop
	static bool restart; // Reboot engine upon exiting (without relaunching exe)

	int main( int argc, char **argv, const ProjectCallbacks &project )
	{
		// Working Directory & Binary
		path_get_directory( WORKING_DIRECTORY, sizeof( WORKING_DIRECTORY ), argv[0] );

		// Log
		PrintColor( LOG_YELLOW, "\n>" );
		for( int i = 0; i < argc; i++ ) { PrintColor( LOG_YELLOW, " %s", argv[i] ); }
		Print( "\n" );

		// Main Loop
		do
		{
			// Set flags
			painted = false;
			exiting = false;
			restart = false;

			// Init Engine & Project
			ErrorIf( !engine::init( argc, argv ), "Failed to initialize the engine" );
			ErrorIf( !project.init( argc, argv ), "Failed to initialize the project" );

			// Main Loop
			while( !exiting )
			{
				frame::start();
				{
					// Update Engine & Project
					engine::update( frame::delta );
					project.update( frame::delta );

					// Show the window after at least 1 frame has been rendered
					if( !painted ) { iwindow::show(); painted = true; }
				}
				frame::end();
			}

			if( true/*restart*/ )
			{
				// Free Project & Engine (if restarting--otherwise no need to cleanup)
				ErrorIf( !project.free(), "Failed to free the project" );
				ErrorIf( !engine::free(), "Failed to free the engine" );
			}
		}
		while( restart );

		// Return Code
		return error::code;
	}


	void exit()
	{
		exiting = true;
		restart = false;
	}


	void reboot()
	{
		exiting = true;
		restart = true;
	}
}