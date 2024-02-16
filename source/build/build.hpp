#pragma once

#include <types.hpp>
#include <debug.hpp>

#include <vendor/stdarg.hpp>

#include <build/assets/textures.hpp>
#include <build/assets/sounds.hpp>
#include <build/assets/fonts.hpp>
#include <build/assets/sprites.hpp>

#include <build/arguments.hpp>
#include <build/binary.hpp>
#include <build/objects.hpp>
#include <build/toolchains.hpp>
#include <build/fileio.hpp>
#include <build/timer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Source
{
	Source( const char *cppPath, const char *objPath ) : cppPath( cppPath ), objPath( objPath ) { }
	String cppPath;
	String objPath;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace build
{
	// Paths
	extern char pathEngine[PATH_SIZE];
	extern char pathProject[PATH_SIZE];
	extern char pathOutput[PATH_SIZE];
	extern char pathOutputBoot[PATH_SIZE];
	extern char pathOutputBuild[PATH_SIZE];
	extern char pathOutputGenerated[PATH_SIZE];
	extern char pathOutputRuntime[PATH_SIZE];
	extern char pathOutputPackage[PATH_SIZE];
	extern char pathOutputBuildCache[PATH_SIZE];

	// Commands
	extern char commandNinja[1024];
	extern char commandRun[1024];

	// Pipeline
	extern Arguments args;
	extern Toolchain tc;

	// Timing
	extern Timer timer;

	// Cache
	extern bool cacheDirty;
	extern bool cacheDirtyObjects;
	extern bool cacheDirtyAssets;
	extern bool cacheDirtyShaders;
	extern bool cacheDirtyBinary;
	extern Buffer cacheBufferPrevious;
	extern Buffer cacheBufferCurrent;

	// Compile
	extern List<Source> sources;
	extern List<String> libraries;
	extern List<String> frameworks;
	extern List<String> includeDirectories;

	extern void compile_add_source( const char *cppPath, char *objPath );
	extern u32 compile_add_sources( const char *cppDirectory, const char *outputDirectory, const bool recurse );
	extern void compile_add_library( const char *library );
	extern void compile_add_framework( const char *framework );
	extern void compile_add_include_directory( const char *includePath );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BuildCore
{
	// Build
	virtual void build( int argc, char **argv );
	virtual void build_cache();

	// Arguments & Toolchain
	virtual void parse_arguments( int argc, char **argv );

	// Objects
	virtual void objects_gather();
	virtual void objects_cache();
	virtual void objects_parse();
	virtual void objects_write();

	// Assets
	virtual void assets_gather();
	virtual void assets_cache();
	virtual void assets_build();
	virtual void assets_write();

	// Shaders
	virtual void shaders_gather();
	virtual void shaders_cache();
	virtual void shaders_build();
	virtual void shaders_write();

	// Binary
	virtual void binary_cache();
	virtual void binary_write();

	// Compile / Ninja
	virtual void compile_project();
	virtual void compile_engine();
	virtual void compile_write_ninja();
	virtual void compile_run_ninja();

	// Run
	virtual void executable_run( int argc, char **argv );

	// Error
	//virtual void ErrorIf( const bool condition, const char *message, ... );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool verbose_output();