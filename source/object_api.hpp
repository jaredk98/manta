#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Object Definition API
//
// This header is to be included in project/source/objects/*.cpp files
//
// The object source files using these macros are not actually compiled into the executable;
// rather, it is processed by the build system executable to generate optimized c++ code

#include <manta/objects.hpp>
#include <objects.generated.intellisense>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Includes:

	// Used with #include -- adds to objects.generated.hpp only
	#define HEADER_INCLUDES

	// Used with #include -- adds to objects.generated.cpp only
	#define SOURCE_INCLUDES


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Object Properties:

	// Type
	#define OBJECT( name ) using namespace iobjects::ObjectIntelliSense_##name;

	// Parent
	#define PARENT( parent ) constexpr bool PARENT_OBJECT = true;

	// Inherit (usage: INHERIT::event_create(); )
	#define INHERIT static_assert( PARENT_OBJECT, "Can not inherit without a declared PARENT(...)!" );

	// Max instantiation count (optional: default = -1)
	#define COUNT( count )

	// Max bucket capacity (optional: default = 1024)
	#define BUCKET_SIZE( bucket_size )

	// Disable instantiation
	#define ABSTRACT( abstract )

	// Disable inheriting
	#define NOINHERIT( noinherit )

	// Enable networking events
	#define NETWORKED( networked )

	// Serialization Version
	#define VERSION( version )


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Access Specifiers:

	// Public object member functions/data
	// Usage: PUBLIC int health;
	// Usage: PUBLIC void hit( const int damage ) { ... }
	#define PUBLIC

	// Private object member functions/data
	// Usage: PRIVATE u8 state;
	// Usage: PRIVATE void update_state() { ... }
	#define PRIVATE

	// Global helper functions/data
	// Usage: GLOBAL int g_RabbitKills;
	// Usage: GLOBAL void kill_all_rabbits() { ... }
	#define GLOBAL


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Events:

	// Disables an event (does not inherit from parent) (i.e. EVENT_STEP DISABLE)
	#define DISABLE

	// Prevents automatic engine calls to an object's event (i.e. EVENT_STEP MANUAL)
	#define MANUAL

	// Constructor
	#define CONSTRUCTOR void __ctor

	// Called upon instantiation
	#define EVENT_CREATE void event_create()

	// Custom initilization event
	#define EVENT_INITIALIZE void event_initialize()

	// Called upon deletion
	#define EVENT_DESTROY void event_destroy()

	// ObjectContext "step" tick
	#define EVENT_STEP_CUSTOM void event_step_custom( const Delta delta )
	#define EVENT_STEP_GUI void event_step_gui( const Delta delta )
	#define EVENT_STEP void event_step( const Delta delta )

	// ObjectContext "draw" tick
	#define EVENT_DRAW_CUSTOM void event_draw_custom( const Delta delta )
	#define EVENT_DRAW_GUI void event_draw_gui( const Delta delta )
	#define EVENT_DRAW void event_draw( const Delta delta )

	// Called at beginning & end of frames
	#define EVENT_FRAME_START void event_frame_start( const Delta delta )
	#define EVENT_FRAME_END void event_frame_end( const Delta delta )

	// Called at 'sleep' and 'wake' triggers
	#define EVENT_SLEEP void event_sleep( const Delta delta )
	#define EVENT_WAKE void event_wake( const Delta delta )

	// Sets object flags
	#define EVENT_FLAG void event_flag( const u64 code )

	// Partition update event
	#define EVENT_PARTITION void event_partition( void *ptr )

	// UI mask update event
	#define EVENT_UI_MASK void event_ui_mask( void *ptr )

	// Object save & load events
	#define EVENT_SAVE void event_save( byte *buffer )
	#define EVENT_LOAD void event_load( byte *buffer )

	// Network send & receive events
	#define EVENT_NETWORK_SEND void event_network_send( byte *buffer )
	#define EVENT_NETWORK_RECEIVE void event_network_receive( byte *buffer )