## Note: This is an early fork of the engine for showcase purposes only!

# Engine Overview:

Manta is C++ game engine & build system currently under development designed to achieve the following:

- Provide a simple library for creating games quickly
- Provide abstraction layers for cross-platform compilation (Windows/MacOS/Linux) and graphics API backends (OpenGL, DirectX, Vulkan, Metal)
- Provide a custom build system that prioritizes fast compile times and build caching
- Employ build-time code generation for engine systems (boilerplate internals of object, asset, and render systems, etc.)
- Have an “IDE-like” experience facilitated by a Visual Studio Code plugin (build + compile + run, debugging, *project*/workspace navigation, IntelliSense)

Manta is a personal project not meant for open use. That said, the source is available here to browse as you please.


# Engine Repository Structure:

1. .manta/
	- Contains a "template" project and "vscmanta" VS Code plugin
2. source/assets/
	- Built-in engine assets (default font, sprite, shader, etc.)
3. source/boot/
	- Source files for the boostrap executable (boot.exe)
4. source/build/
	- Source files for the manta build library (build.exe)
5. source/manta/
	- Source files for the manta runtime library (*project*.exe)
6. source/vendor/
	- Headers for external libraries and wrappers around C headers
	- As a compile time optimization (for development builds), Manta supports a "USE_OFFICIAL_HEADERS" macro which (when false) uses trimmed C headers
7. projects/
	- Location for project repositories (either copy template project here, or use VSCManta plugin to create a new project)

You may notice that there are seemingly duplicated .hpp/.cpp modules between source/boot, source/build, and source/manta. This is because they are three distinct programs with different goals.

As an example, boot.exe and build.exe are less strict about memory allocations. The container implementations available to those programs (list.hpp, string.hpp, etc.) reflect that by allowing RAII memory allocation/freeing and implicit copy/move semantics. This is fine (and very convenient) for a build tool running on a developer machine, but is less ideal for game code with performance and memory budget constraints.


# Project Repository Structure:

1. projects/*project*/assets
	- Assets for the project (*.font, *.sprite, *.shader, etc.)
	- Asset files are typically JSON or raw data (i.e., a sprite is a .sprite (json) and a .png)
2. projects/*project*/build
	- Project-specific sources for the build executable (implements main)
	- See: *project*/build/build.cpp and *project*/build/build.hpp
		- For project-specific build logic, create a struct that extends 'BuildCore' from "#include <build/build.hpp>" (engine build library) and override desired stages
3. projects/*project*/runtime
	- Project-specific sources for the runtime executable (implements main)
	- All game/project logic is programmed here. Manta library functions/systems can be accessed through "#include <manta/...>"
	- Entry point typically fills out a "ProjectCallbacks' struct containing init(), free(), and update() functions for the project and calls engine::main()
		- See: .manta/template/runtime/main.cpp
		- This is not strictly required, but allows the manta engine to init, update, and free itself automatically
4. projects/*project*/configs.json
	- Customizable compiler and linker flags for the runtime executable build (-config=debug, -config=release, -config=yourcustomconfig, etc.)


# Build & Run Pipeline:

1. boot.bat/sh
	- Args:
		- -project=*project name*
		- -os=*windows/linux/macos*
		- -architecture=*x64/arm64*
		- -toolchain=*msvc/llvm/gnu*
		- -config=*config name*
		- -gfx=*opengl/d3d11/d3d12/vulkan/metal*
		- -clean=*0/1*
		- -run=*0/1*
	- Compiles boot.exe if it doesn't exist
	- Runs boot.exe

2. boot.exe
	- Generates projects/*project*/generated/pipeline.generated.hpp
		- This contains platform/config specific macro definitions used by the engine
	- Compiles build.exe from projects/*project*/build and source/build sources
	- Runs build.exe

3. build.exe
	- Processes project & engine assets (source/assets and projects/*project*/assets)
	- Generates C++ code (written to *project*/output/generated)
	- Writes *project*.bin binary file
	- Compiles *project*.exe
	- Runs *project*.exe (if -run=1)

**Example Build Command:**
> boot.bat -project=asteroids -toolchain=llvm -gfx=d3d11 -config=release -run=1


# Additional Build Information:

Manta engine does not use precompiled headers nor link statically to the build or runtime libaries. Rather, Manta sources are compiled directly into the project executables.

The reasons for this are:

- Caching of builds and compiles is already done in boot.exe and build.exe (compiles cached with .ninja builds)
- The build tool employs code generation for various systems (assets, objects, graphics backend) which would force frequent rebuilds of engine PCH
- The engine library code will likely be updated continously alongside project code (until the engine matures)
- Compile times are already quite fast due to restrictions on standard includes, "unofficial" C headers for development builds (USE_OFFICIAL_HEADERS 0), and project structure
- It is easier to reason about, understand, and maintain a system when it is simpler

This may change in later versions of the engine, but for now it is structured this way.

Anecdotally (on my machine) I can do a clean build + compile (no cache) of boot.exe, build.exe, and the game.exe in roughly 1 second (~100 translation units) with both MSVC and Clang++.


# Object System

### I. Object Definitions
Manta features a custom object system utilizing its own C++ "scripting language" object definition files, prefixed *.opp* (*object*++)

The goal is to make the authoring and maintenance of object code as straightforward as possible with minimal compromise on runtime performance.

In short, *.opp* files contain psuedo-C++ code that describe an "object" class (data members, functions, events, etc.). The *.opp* files are processed by the build tool and used to generate correct C++ header and source implementations.

To aid with IntelliSense, *.opp* files can "#include <object_api.hpp>"

object_api.hpp provides both a reference to the available keywords of the "scripting" language, as well as enables C++ IntelliSense to work with *.opp* code without the need for a custom language server. You can view the available keywords in that file.

Objects in this engine use a single-inheritence polymorphic model. Objects can inherit/extend other objects by specifying the __PARENT( name )__ keyword.

This meets most needs for games that I create. Additionally, the data structure where object data is stored ensures sufficient locality for most gameplay needs: per-instance object data is allocated contiguously in memory within designed memory blocks for each object type.

If I ever need more performance (i.e., must guarantee the _best_ cache locality possible) I would question whether utilizing the object system is the right solution for the problem at hand.

**Sample object definition code: (obj_rabbit.opp)**
```c++
#include <object_api.hpp>

HEADER_INCLUDES // Headers to go in objects.generated.hpp
#include <manta/scene.hpp>

SOURCE_INCLUDES // Headers to go in objects.generated.cpp
#include <manta/draw.hpp>

// Object
OBJECT( obj_rabbit )
PARENT( obj_animal ) // obj_rabbit extends obj_animal

// Data
PUBLIC float x = 0.0f;
PUBLIC float y = 0.0f;
PUBLIC float speed_x = 0.0f;
PUBLIC float speed_y = 0.0f;
PUBLIC float health = 100.0f;

PRIVATE enum RabbitState
{
	RabbitState_Stand,
	RabbitState_Walk,
	...
};

PRIVATE RabbitState state = RabbitState_Stand;

// Constructors
CONSTRUCTOR( float spawnX, float spawnY )
{
	x = spawnX;
	y = spawnY;
}

// Events & Functions
EVENT_STEP
{
	// Inherit event
	INHERIT::event_step( delta );

	// State Machine
	switch( state )
	{
		// ...
	}

	// Health
	if( health <= 0.0f )
	{
		process_death();
	}
}

EVENT_DRAW
{
	draw_sprite( spr_rabbit, x, y );
}

PRIVATE void process_death()
{
	// Destroy ourselves
	sceneObjects.destroy( id ); // sceneObjects from scene.hpp
	return;
}
```

**Generated C++ engine code:**
<details>
<summary><span style="color: yellow;">objects.generated.hpp</span></summary>

```c++
// ...

__INTERNAL_OBJECT_DEFINITION_BEGIN()
class obj_animal_t : public BASE_OBJECT_t
{
public:
	obj_animal_t() = default;

	// PUBLIC DATA
	float x = 0.0f;
	float y = 0.0f;
	float speed_x = 0.0f;
	float speed_y = 0.0f;

	// EVENTS
	virtual void event_step( const Delta delta );

private:
};
__INTERNAL_OBJECT_DEFINITION_END()

template <> struct ObjectHandle<obj_animal>
{
	iobjects::obj_animal_t *data = nullptr;
	iobjects::obj_animal_t *operator->() const { Assert( data != nullptr ); return data; }
	explicit operator bool() const { return data != nullptr; }
	ObjectHandle( void *object ) { data = reinterpret_cast<iobjects::obj_animal_t *>( object ); }
};

__INTERNAL_OBJECT_DEFINITION_BEGIN()
class obj_rabbit_t : public obj_animal_t
{
public:
	obj_rabbit_t() = default;
	obj_rabbit_t( float spawnX, float spawnY );

	// PUBLIC DATA
	float health = 100.0f;

	// EVENTS
	virtual void event_step( const Delta delta );
	virtual void event_draw( const Delta delta );

private:
	// PRIVATE DATA
	enum RabbitState
	{
		RabbitState_Stand,
		RabbitState_Walk,
		...
	};
	RabbitState state = RabbitState_Stand;

	// PRIVATE FUNCTIONS
	void process_death();
};
__INTERNAL_OBJECT_DEFINITION_END()

template <> struct ObjectHandle<obj_rabbit>
{
	iobjects::obj_rabbit_t *data = nullptr;
	iobjects::obj_rabbit_t *operator->() const { Assert( data != nullptr ); return data; }
	explicit operator bool() const { return data != nullptr; }
	ObjectHandle( void *object ) { data = reinterpret_cast<iobjects::obj_rabbit_t *>( object ); }
};

// ... file continued
```
</details>

<details>
<summary><span style="color: yellow;">objects.generated.cpp</span></summary>

```c++
// ...

void iobjects::obj_animal_t::event_step( const Delta delta )
{
	// Movement
	x += speed_x;
	y += speed_y;
}


iobjects::obj_rabbit_t::obj_rabbit_t( float spawnX, float spawnY )
{
	x = spawnX;
	y = spawnY;
}

void iobjects::obj_rabbit_t::event_step( const Delta delta )
{
	// Inherit event
	iobjects::obj_animal_t::event_step( delta );

	// State Machine
	switch( state )
	{
		// ...
	}

	// Health
	if( health <= 0.0f )
	{
		process_death();
	}
}

void iobjects::obj_rabbit_t::event_draw( const Delta delta )
{
	draw_sprite( spr_rabbit, x, y );
}

void iobjects::obj_rabbit_t::process_death()
{
	// Destroy ourselves
	sceneObjects.destroy( id ); // sceneObjects from scene.hpp
	return;
}

/// ... file continued
```
</details>

### II. Object System / Gameplay Programming

The object system works off three fundamental types:

1. **ObjectContext**
   - The data structure and interface for object creation, destruction, data lookup, and looping
2. **Object**
   - A unique "identifier" for every instantiated object in an ObjectContext
3. **ObjectHandle\<_ObjectType_>**
   - A handle to a specific object instance data

In the generated code above, you may notice that the object class names are suffixed with **_t** and wrapped within an internal namespace. The reason for this is "raw" object types are not to be used directly in gameplay code. Rather, object types in gameplay code are represented as __ObjectType__ enums. This allows them to be stored in variables and passed as runtime function parameters.

The table looks something like:
```c++
// objects.generated.hpp

enum ObjectType
{
	...
	obj_animal,  // internal::obj_animal_t class
	obj_rabbit,  // internal::obj_rabbit_t class
	obj_chicken, // internal::obj_chicken_t class
	...
};
```

This allows for gameplay code like:
```c++
const ObjectType type = spawnRabbit ? obj_rabbit : obj_chicken;
context.create( type ); // Create an obj_rabbit or obj_chicken
```

Here are some more examples:
```c++
#include <manta/objects.hpp> // Object system library

// ObjectContext Initialization
ObjectContext context;
context.init(); // There can be multiple contexts in a program (ui, scene, etc.)

// Object Creation
Object rabbit1 = context.create<obj_rabbit>( 64.0f, 64.0f ); // Create with constructor parameters
Object rabbit2 = context.create( obj_rabbit ); // Create with default parameters
Object animal1 = context.create( choose( obj_chicken, obj_rabbit ) ); // Creation of chicken OR rabbit (runtime randomness)

// ObjectHandle Retrieval
ObjectHandle<obj_rabbit> rabbitHandle = rabbit1.handle<obj_rabbit>( context ); // or: auto rabbitHandle = ...

// ObjectHandle Validation (handle can be null if the Object does not exist)
if( rabbitHandle )
{
	rabbitHandle->x = 0.0f;
	rabbitHandle->y = 0.0f;
}

// Looping over all obj_rabbit instances
for( auto rabbitHandle : context.objects<obj_rabbit>() )
{
	rabbitHandle->x = 0.0f;
	rabbitHandle->y = 0.0f;
}

// Looping over all obj_animal instances (includes obj_rabbits & obj_chicken)
for( auto animalHandle : context.objects<obj_animal>( true ) )
{
	animalHandle->x = 0.0f;
	animalHandle->y = 0.0f;
}

// Object Destruction
context.destroy( rabbit1 );

// ObjectContext freeing
context.free();
```

# Project Setup:

### VSCManta Extension:

- VS Code -> Extensions -> Install from VSIX -> .manta/vscmanta/output/vscmanta.vsix
- If you want to modify VSCManta, you must have Node.js & npm installed: https://nodejs.org/en
- Build with: .manta/vscmanta/install.bat or .manta/vscmanta/install.sh

Helpful VSCManta keybindings (add to keybindings.json):

    { "key": "f5", "when": "resourceExists('.manta')", "command": "vscmanta.buildAndRun" },
    { "key": "f6", "when": "resourceExists('.manta')", "command": "vscmanta.debugRuntime" },
    { "key": "f7", "when": "resourceExists('.manta')", "command": "vscmanta.renderdoc" },
    { "key": "f8", "when": "resourceExists('.manta')", "command": "vscmanta.build" },
    { "key": "ctrl+shift+c", "when": "resourceExists('.manta')", "command": "vscmanta.cleanProject" },

### Building & Compiling:

- Note: Manta engine uses C++20 features

- Ninja: https://ninja-build.org/

- Compilers (Windows): (to simply build & test you only need one of these, probably MSVC)
	- MSVC: https://visualstudio.microsoft.com/downloads/
		- May need to modify boot.bat 'vcvars64.bat' path to point to the correct install location for your machine
	- GNU: https://winlibs.com/
	- LLVM: https://github.com/llvm/llvm-*project*/releases/tag/llvmorg-16.0.4
