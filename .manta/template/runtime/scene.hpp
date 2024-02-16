#pragma once

#include <manta/objects.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern ObjectContext sceneObjects;

extern int gameScore;

extern void scene_init();
extern void scene_free();

extern void scene_update( const Delta delta );
extern void scene_draw( const Delta delta );

extern void create_asteroid();