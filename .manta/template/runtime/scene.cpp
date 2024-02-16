#include <scene.hpp>
#include <manta/random.hpp>
#include <manta/timer.hpp>
#include <manta/assets.hpp>
#include <manta/draw.hpp>
#include <manta/window.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ObjectContext sceneObjects;

int gameScore = 0;


void scene_init()
{
	// Init ObjectContext
	sceneObjects.init();

	// Create Player
	sceneObjects.create( obj_rocket );

	// Create Astroids
	for( int i = 0; i < 5; i++ ) { create_asteroid(); }
}


void scene_free()
{
	// Free ObjectContext
	sceneObjects.free();
}


void scene_update( const Delta delta )
{
	// Spawn Asteroid
	if( frame::tickSecond ) { create_asteroid(); }

	// Step Objects
	sceneObjects.event_step( delta );
}


void scene_draw( const Delta delta )
{
	// Draw Objects
	sceneObjects.event_draw( delta );

	// Draw Score
	draw_text_f( fnt_consolas, 32, window::width * 0.25f, 16.0f, c_white, "Score: %d", gameScore );
}


void create_asteroid()
{
	sceneObjects.create<obj_asteroid>( random::value<float>( 4.0f, 8.0f ) );
}
