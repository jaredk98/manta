#include <scene.hpp>
#include <manta/random.hpp>
#include <manta/timer.hpp>
#include <manta/assets.hpp>
#include <manta/draw.hpp>
#include <manta/window.hpp>
#include <manta/input.hpp>
#include <manta/vector.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace Scene
{
	ObjectContext objects;
	int score = false;
	bool dead = true;
}


static void create_asteroid()
{
	Scene::objects.create<obj_asteroid>( random::value<float>( 4.0f, 8.0f ) );
}


void scene_init()
{
	// Init ObjectContext
	Scene::objects.init();

	// Create Player
	Scene::objects.create( obj_rocket );

	// Create Astroids
	for( int i = 0; i < 5; i++ ) { create_asteroid(); }
}


void scene_free()
{
	// Free ObjectContext
	Scene::objects.free();
}


void scene_update( const Delta delta )
{
	// Spawn Asteroid
	if( frame::tickSecond ) { create_asteroid(); }

	// Step Objects
	Scene::objects.event_step( delta );

	// Game State
	Scene::dead = Scene::objects.count( obj_rocket ) == 0;

	if( Scene::dead )
	{
		// Respawn
		if( keyboard::check_pressed( vk_space ) )
		{
			// Reset Astroids
			Scene::objects.destroy_all_type( obj_asteroid );
			for( int i = 0; i < 5; i++ ) { create_asteroid(); }

			// Create Player
			Scene::objects.create( obj_rocket );

			// Reset Score
			Scene::score = 0;
		}
	}
}


void scene_draw( const Delta delta )
{
	// Draw Objects
	Scene::objects.event_draw( delta );

	if( !Scene::dead )
	{
		// Draw Score
		const vec2i dScore = text_dimensions_f( fnt_consolas, 24, "Score: %d", Scene::score );
		draw_text_f( fnt_consolas, 32, window::width * 0.5f - dScore.x, 16.0f, c_white, "Score: %d", Scene::score );
	}
	else
	{
		// Death Screen
		const vec2i dMsg = text_dimensions_f( fnt_consolas, 48, "You died! Score: %d", Scene::score );
		const vec2i dRst = text_dimensions( fnt_consolas, 24, "Press Space to restart!" );

		const float cX = window::width * 0.5f;
		const float cY = window::height * 0.5f;

		draw_rectangle( 0.0f, cY - dMsg.y * 0.5f - 64.0f, window::width, cY + dRst.y * 0.5f + 64.0f, { 0, 0, 0, 127 } );
		draw_text_f( fnt_consolas, 48, cX - dMsg.x * 0.5f, cY - dMsg.y * 0.5f - 32.0f, c_white, "You died! Score: %d", Scene::score );
		draw_text( fnt_consolas, 24, cX - dRst.x * 0.5f, cY - dRst.y * 0.5f + 32.0f, c_white, "Press Space to restart!" );
	}
}
