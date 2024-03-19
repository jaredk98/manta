#include <manta/engine.hpp>

#include <manta/assets.hpp>
#include <manta/time.hpp>
#include <manta/window.hpp>
#include <manta/gfx.hpp>
#include <manta/objects.hpp>
#include <manta/fonts.hpp>

#include <manta/input.hpp>
#include <manta/fileio.hpp>

#include <config.hpp>
#include <debug.hpp>
#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
	static bool init( int argc, char **argv )
	{
		const bool newGfx = ( strcmp( PROJECT_NAME, "gfx_new" ) == 0 );

		// Assets
		ErrorReturnIf( !iAssets::init(), false, "Engine: failed to initialize assets" );

		// Time
		ErrorReturnIf( !iTime::init(), false, "Engine: failed to initialize timer" );

		// Window
		ErrorReturnIf( !iWindow::init(), false, "Engine: failed to initialize window" );

		// Graphics
		ErrorReturnIf( !fGfx::init(), false, "Engine: failed to initialize graphics" );

		// Objects
		ErrorReturnIf( !iObjects::init(), false, "Engine: failed to initialize object system" );

		// Fonts
		ErrorReturnIf( !iFonts::init(), false, "Engine: failed to initialize font system" );

		// Success
		return true;
	}

	static bool free()
	{
		// Fonts
		ErrorReturnIf( !iFonts::free(), false, "Engine: failed to free fonts" );

		// Objects
		ErrorReturnIf( !iObjects::free(), false, "Engine: failed to free objects" );

		// Graphics
		ErrorReturnIf( !fGfx::free(), false, "Engine: failed to free render" );

		// Window
		ErrorReturnIf( !iWindow::free(), false, "Engine: failed to free window" );

		// Time
		ErrorReturnIf( !iTime::free(), false, "Engine: failed to free timer" );

		// Assets
		ErrorReturnIf( !iAssets::free(), false, "Engine: failed to free assets" );

		// Success
		return true;
	}

	static void update( const Delta delta )
	{
		// Input
		Input::update( delta );

		// Window
		Window::update( delta );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
	static bool painted = false; // Delay showing the window until after a frame is rendered
	static bool exiting = false; // Terminates the main loop

	int main( int argc, char **argv, const ProjectCallbacks &project )
	{
		// Working Directory & Binary
		path_get_directory( WORKING_DIRECTORY, sizeof( WORKING_DIRECTORY ), argv[0] );

		// Log
		PrintColor( LOG_YELLOW, "\n>" );
		for( int i = 0; i < argc; i++ ) { PrintColor( LOG_YELLOW, " %s", argv[i] ); }
		Print( "\n" );

		// Main Loop
		{
			// Init Engine & Project
			ErrorIf( !Engine::init( argc, argv ), "Failed to initialize the engine" );
			ErrorIf( !project.init( argc, argv ), "Failed to initialize the project" );

			// Main Loop
			while( !exiting )
			{
				Frame::start();
				{
					// Update Engine & Project
					Engine::update( Frame::delta );
					project.update( Frame::delta );

					// Show the window after at least 1 frame has been rendered
					if( !painted ) { iWindow::show(); painted = true; }
				}
				Frame::end();
			}

			// Free Project & Engine (if restarting--otherwise no need to cleanup)
			#if 0
				ErrorIf( !project.free(), "Failed to free the project" );
				ErrorIf( !Engine::free(), "Failed to free the engine" );
			#else
				iEngine::memoryLeakDetection = false;
			#endif
		}

		// Return Code
		return iEngine::exitCode;
	}

	void exit()
	{
		exiting = true;
	}
}