#include <build/build.hpp>

#include <build/toolchains.hpp>

#include <build/assets.hpp>
#include <build/shaders.hpp>

#include <build/fileio.hpp>
#include <build/string.hpp>
#include <build/list.hpp>
#include <build/buffer.hpp>
#include <build/json.hpp>

#include <build/hashmap.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace build
{
	// Paths
	char pathEngine[PATH_SIZE];
	char pathProject[PATH_SIZE];
	char pathOutput[PATH_SIZE];
	char pathOutputBoot[PATH_SIZE];
	char pathOutputBuild[PATH_SIZE];
	char pathOutputGenerated[PATH_SIZE];
	char pathOutputRuntime[PATH_SIZE];
	char pathOutputPackage[PATH_SIZE];
	char pathOutputBuildCache[PATH_SIZE];

	// Commands
	char commandNinja[1024];
	char commandRun[1024];

	// Pipeline
	Arguments args;
	Toolchain tc;

	// Timer
	Timer timer;

	// Cache
	bool cacheDirty = false;
	bool cacheDirtyObjects = false;
	bool cacheDirtyAssets = false;
	bool cacheDirtyBinary = false;
	bool cacheDirtyShaders = false;
	Buffer cacheBufferPrevious;
	Buffer cacheBufferCurrent;

	// Compile
	List<Source> sources;
	List<String> libraries;
	List<String> frameworks;
	List<String> includeDirectories;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void build::compile_add_source( const char *cppPath, char *objPath )
{
	char buffer[PATH_SIZE];
	path_change_extension( buffer, sizeof( buffer ), objPath, build::tc.linkerExtensionObj );
	build::sources.add( { cppPath, buffer } );
}


u32 build::compile_add_sources( const char *cppDirectory, const char *outputDirectory, const bool recurse )
{
	// Gather sources files
	Timer timer;
	List<FileInfo> sourceFiles;
	directory_iterate( sourceFiles, cppDirectory, ".cpp", recurse ); // C++
	if( PIPELINE_OS_MACOS ) { directory_iterate( sourceFiles, cppDirectory, ".mm", recurse ); } // Objective-C++

	// Add sources
	char objPath[PATH_SIZE];
	for( FileInfo &sourceFile : sourceFiles )
	{
		strjoin( objPath, "objects" SLASH, sourceFile.path );
		build::compile_add_source( sourceFile.path, objPath );
	}

	// Logging
	const u32 sourcesCount = sourceFiles.size();
	if( verbose_output() )
	{
		PrintColor( LOG_CYAN, "\t\t%u source%s found in: %s", sourcesCount, sourcesCount == 1 ? "" : "s", cppDirectory );
		PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() );
	}
	return sourcesCount;
}


void build::compile_add_library( const char *library )
{
	build::libraries.add( library );
}


void build::compile_add_framework( const char *framework )
{
	build::frameworks.add( framework );
}


void build::compile_add_include_directory( const char *includePath )
{
	build::includeDirectories.add( includePath );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuildCore::build( int argc, char **argv )
{
	// Setup
	{
		// Timer
		timer::init();
		build::timer.start();

		// Parse arguments
		parse_arguments( argc, argv );

		// Log
		PrintColor( LOG_YELLOW, "\n>" );
		for( int i = 0; i < argc; i++ )
		{ PrintColor( LOG_YELLOW, " %s", argv[i] ); }
		Print( "\n" );

		// Paths
		strjoin( build::pathEngine, "source" );
		strjoin( build::pathProject, "projects" SLASH, build::args.project );
		strjoin( build::pathOutput, build::pathProject, SLASH "output" );
		strjoin( build::pathOutputBoot, build::pathOutput, SLASH, "boot" );
		strjoin( build::pathOutputBuild, build::pathOutput, SLASH, "build" );
		strjoin( build::pathOutputGenerated, build::pathOutput, SLASH, "generated" );
		strjoin( build::pathOutputRuntime, build::pathOutput, SLASH, "runtime" );
		strjoin( build::pathOutputPackage, build::pathOutput, SLASH, "package" );
		strjoin( build::pathOutputBuildCache, build::pathOutputBuild, SLASH "build.cache" );

		// Output Directories
		char path[PATH_SIZE];
		strjoin( path, build::pathOutput, SLASH "runtime" );
		directory_create( path );
		strjoin( path, build::pathOutput, SLASH "generated" );
		directory_create( path );
	}

	// Check Cache
	{
		build_cache();
	}

	// Build Conditions
	bool codegen = ( strcmp( build::args.codegen, "1" ) == 0 );
	bool build = ( strcmp( build::args.build, "1" ) == 0 );
	bool run = ( strcmp( build::args.run, "1" ) == 0 );

	// Build Objects
	if( codegen )
	{
		PrintLnColor( LOG_WHITE, "\nBuild Objects" );
		Timer timer;

		objects::begin();
		objects_gather();
		objects_cache();
		objects_parse();
		objects_write();

		PrintLnColor( LOG_WHITE, "\tFinished (%.3f ms)", timer.elapsed_ms() );
	}

	// Build Shaders
	if( build )
	{
		PrintLnColor( LOG_WHITE, "\nBuild Shaders" );
		Timer timer;

		shaders::begin();
		shaders_gather();
		shaders_cache();
		shaders_build();
		shaders_write();

		PrintLnColor( LOG_WHITE, "\tFinished (%.3f ms)", timer.elapsed_ms() );
	}

	// Build Assets
	if( build )
	{
		PrintLnColor( LOG_WHITE, "\nBuild Assets" );
		Timer timer;

		assets::begin();
		assets_gather();
		assets_cache();
		assets_build();
		assets_write();

		PrintLnColor( LOG_WHITE, "\tFinished (%.3f ms)", timer.elapsed_ms() );
	}

	// Write Binary
	if( build )
	{
		PrintLnColor( LOG_WHITE, "\nWrite Binary" );
		Timer timer;

		binary_cache();
		binary_write();

		PrintLnColor( LOG_WHITE, "\tFinished (%.3f ms)", timer.elapsed_ms() );
	}

	// Compile Executable
	if( build )
	{
		PrintLnColor( LOG_WHITE, "\nCompile Code" );
		Timer timer;

		compile_project();
		compile_engine();
		compile_write_ninja();
		compile_run_ninja();

		PrintLnColor( LOG_WHITE, "\n\tCompile finished: %.3f s (%.3f ms)", timer.elapsed_s(), timer.elapsed_ms() );
	}

	// Finish
	{
		PrintColor( LOG_GREEN, "\nBuild Finished!" );
		PrintLnColor( LOG_WHITE, " (%.3f s)", build::timer.elapsed_s() );
		build::cacheBufferCurrent.save( build::pathOutputBuildCache );
	}

	// Run Executable
	if( run )
	{
		executable_run( argc, argv );
	}
}


void BuildCore::parse_arguments( int argc, char **argv )
{
	// Parse Arguments
	build::args.parse( argc, argv );
	build::tc.detect( build::args );
}


void BuildCore::build_cache()
{
	// Force build? (arg: -clean=1)
	const bool force = ( strcmp( build::args.clean, "1" ) == 0 );
	build::cacheDirty |= force;

	// Cache File
	if( !build::cacheBufferPrevious.load( build::pathOutputBuildCache, true ) ) { build::cacheDirty = true; }

	// Log
	PrintColor( LOG_WHITE, "Build Cache... " );

	if( build::cacheDirty )
	{
		PrintLnColor( LOG_RED, force ? "dirty (force)" : "dirty" );
	}
	else
	{
		PrintLnColor( LOG_GREEN, "clean" );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuildCore::objects_gather()
{
	PrintLnColor( LOG_WHITE, "\tGather Objects..." );
	char path[PATH_SIZE];

	// Engine
	strjoin( path, build::pathEngine, SLASH "manta" );
	objects::gather( path, true );

	// Project
	strjoin( path, build::pathProject, SLASH "runtime" );
	objects::gather( path, true );
}


void BuildCore::objects_cache()
{
	// Full rebuild?
	build::cacheDirtyObjects |= build::cacheDirty;

	// Mismatch object file count?
	build::cacheDirtyObjects |= ( objects::objectFilesCount != build::cacheBufferPrevious.read<usize>() );
	build::cacheBufferCurrent.write( objects::objectFilesCount );

	// Log
	PrintColor( LOG_WHITE, "\tObjects Cache... " );
	PrintLnColor( build::cacheDirtyObjects ? LOG_RED : LOG_GREEN, build::cacheDirtyObjects ? "dirty" : "skip stage" );
}


void BuildCore::objects_parse()
{
	if( !build::cacheDirtyObjects ) { return; }
	PrintLnColor( LOG_WHITE, "\tParse Objects..." );

	// Parse
	objects::parse();
}


void BuildCore::objects_write()
{
	if( !build::cacheDirtyObjects ) { return; }
	PrintLnColor( LOG_WHITE, "\tWrite Objects..." );

	// Resolve inheritance tree
	objects::resolve();

	// Generate C++ files
	objects::generate();

	// Write C++ files to disk
	objects::write();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuildCore::shaders_gather()
{
	PrintLnColor( LOG_WHITE, "\tGather Shaders..." );

	// Gather Shaders
	shaders::gather( build::pathEngine, true );
	shaders::gather( build::pathProject, true );
}


void BuildCore::shaders_cache()
{
	// Full rebuild?
	build::cacheDirtyShaders |= build::cacheDirty;

	// Mismatch asset file count?
	build::cacheDirtyShaders |= ( shaders::shaderFileCount != build::cacheBufferPrevious.read<usize>() );
	build::cacheBufferCurrent.write( shaders::shaderFileCount );

	// TODO: Fix this properly (both shaders and assets depend on the binary)
	build::cacheDirtyAssets |= build::cacheDirtyShaders;

	// Log
	PrintColor( LOG_WHITE, "\tShaders Cache... " );
	PrintLnColor( build::cacheDirtyShaders ? LOG_RED : LOG_GREEN, build::cacheDirtyShaders ? "dirty" : "skip stage" );
}


void BuildCore::shaders_build()
{
	if( !build::cacheDirtyShaders ) { return; }
	PrintLnColor( LOG_WHITE, "\tBuild Shaders..." );

	shaders::build();
}


void BuildCore::shaders_write()
{
	if( !build::cacheDirtyShaders ) { return; }
	PrintLnColor( LOG_WHITE, "\tWrite Shaders..." );

	shaders::write();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuildCore::assets_gather()
{
	PrintLnColor( LOG_WHITE, "\tGather Assets..." );

	// Gather Sprites
	assets::sprites.gather( build::pathEngine );
	assets::sprites.gather( build::pathProject );

	// Gather Fonts
	assets::fonts.gather( build::pathEngine );
	assets::fonts.gather( build::pathProject );

	// Gather Sounds
	// ...
}


void BuildCore::assets_cache()
{
	// Full rebuild?
	build::cacheDirtyAssets |= build::cacheDirty;

	// Mismatch asset file count?
	build::cacheDirtyAssets |= ( assets::assetFileCount != build::cacheBufferPrevious.read<usize>() );
	build::cacheBufferCurrent.write( assets::assetFileCount );

	// Log
	PrintColor( LOG_WHITE, "\tAssets Cache... " );
	PrintLnColor( build::cacheDirtyAssets ? LOG_RED : LOG_GREEN, build::cacheDirtyAssets ? "dirty" : "skip stage" );
}


void BuildCore::assets_build()
{
	if( !build::cacheDirtyAssets ) { return; }
	PrintLnColor( LOG_WHITE, "\tBuild Assets..." );

	// Write Textures
	assets::textures.write();

	// Write Glyphs
	assets::glyphs.write();

	// Write Sprites
	assets::sprites.write();

	// Write Fonts
	assets::fontRanges.write();
	assets::fonts.write();
}


void BuildCore::assets_write()
{
	if( !build::cacheDirtyAssets ) { return; }
	PrintLnColor( LOG_WHITE, "\tWrite Assets..." );

	// assets.generated.hpp
	{
		if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tWrite %s", assets::pathHeader ); }
		Timer timer;

		// Begin header
		String header;
		header.append( "/*\n" );
		header.append( " * File generated by build.exe--do not edit!\n" );
		header.append( " * Refer to: source/build/build.cpp (BuildCore::assets_write)\n" );
		header.append( " */\n" );
		header.append( "#pragma once\n\n" );
		header.append( "#include <types.hpp>\n\n\n" );

		// Append header
		header.append( assets::header );

		// Save header
		ErrorIf( !header.save( assets::pathHeader ), "Failed to write '%s'", assets::pathHeader );

		if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
	}

	// assets.generated.cpp
	{
		if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tWrite %s", assets::pathSource ); }
		Timer timer;

		// Begin source
		String source;
		source.append( "/*\n" );
		source.append( " * File generated by build.exe--do not edit!\n" );
		source.append( " * Refer to: source/build/build.cpp (BuildCore::assets_write)\n" );
		source.append( " */\n" );
		source.append( "#include <assets.generated.hpp>\n\n\n" );

		// Append source
		source.append( assets::source );

		// Save source
		ErrorIf( !source.save( assets::pathSource ), "Failed to write '%s'", assets::pathSource );

		if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuildCore::binary_cache()
{
	// Full rebuild?
	build::cacheDirtyBinary |= build::cacheDirty;

	// Any binary contents to save?
	build::cacheDirtyBinary |= ( assets::binary.size() > 0 );

	// Log
	PrintColor( LOG_WHITE, "\tBinary Cache... " );

	if( build::cacheDirtyBinary )
	{
		PrintLnColor( LOG_RED, "dirty" );
	}
	else
	{
		PrintLnColor( LOG_GREEN, "skip stage" );
	}
}


void BuildCore::binary_write()
{
	if( !build::cacheDirtyBinary ) { return; }

	char path[PATH_SIZE];
	strjoin( path, build::pathOutput, SLASH "runtime" SLASH, build::args.project, ".bin" );

	// Log
	PrintLnColor( LOG_WHITE, "\tWriting Binary" );
	if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tWrite %s", path ); }
	Timer timer;

	// Write
	ErrorIf( !assets::binary.save( path ), "Failed to write binary (%s)", path );

	// Log
	if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuildCore::compile_project()
{
	// Include Directories
	{
		build::compile_add_include_directory( ".." SLASH ".." SLASH "runtime" ); // Project
	}

	// C++ Sources + Library Linkage
	{
		char path[PATH_SIZE];
		PrintLnColor( LOG_WHITE, "\tGather Project Sources..." );
		strjoin( path, build::pathProject, SLASH "runtime" );
		build::compile_add_sources( path, "project", true ); // Project
	}
}


void BuildCore::compile_engine()
{
	// Include Directories
	{
		build::compile_add_include_directory( ".." SLASH "generated" ); // Generated C++ files
		build::compile_add_include_directory( ".." SLASH ".." SLASH ".." SLASH ".." SLASH "source" ); // Engine
	}

	// C++ Sources + Library Linkage
	PrintLnColor( LOG_WHITE, "\tGather Engine Sources..." );
	{
		char path[PATH_SIZE];
		char group[PATH_SIZE];

		// root/projects/<project>/output/generated/*.cpp
		strjoin( path, build::pathOutput, SLASH "generated" );
		strjoin( group, "engine" SLASH "generated" );
		build::compile_add_sources( path, group, false );

		// root/source/*.cpp
		strjoin( path, build::pathEngine );
		strjoin( group, "engine" );
		build::compile_add_sources( path, group, false );

		// -r root/source/vendor/*.cpp
		strjoin( path, build::pathEngine, SLASH "vendor" );
		strjoin( group, "engine" SLASH "vendor" );
		build::compile_add_sources( path, group, true );

		// root/source/manta/*.cpp
		strjoin( path, build::pathEngine, SLASH "manta" );
		strjoin( group, "engine" SLASH "manta" );
		build::compile_add_sources( path, group, false );

		// Backend Sources
		strjoin( group, "engine" SLASH "manta" SLASH "backend" );
		{
			// Audio | -r source/manta/backend/audio/*.cpp
			strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "audio" SLASH, BACKEND_AUDIO );
			ErrorIf( build::compile_add_sources( path, group, true ) == 0, "No engine backend sources found for 'audio' (%s)", path );

			// Filesystem | -r source/manta/backend/filesystem/*.cpp
			strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "filesystem" SLASH, BACKEND_FILESYSTEM );
			ErrorIf( build::compile_add_sources( path, group, true ) == 0, "No engine backend sources found for 'filesystem' (%s)", path );

			// Network | -r source/manta/backend/network/*.cpp
			strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "network" SLASH, BACKEND_NETWORK );
			ErrorIf( build::compile_add_sources( path, group, true ) == 0, "No engine backend sources found for 'network' (%s)", path );

			// Grahpics | -r source/manta/backend/gfx/*.cpp
			strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "gfx" SLASH, BACKEND_GRAPHICS );
			ErrorIf( build::compile_add_sources( path, group, false ) == 0, "No engine backend sources found for 'gfx' (%s)", path );

			if( GRAPHICS_OPENGL )
			{
				if( OS_WINDOWS )
				{
					// WGL
					strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "gfx" SLASH, BACKEND_GRAPHICS, SLASH "wgl" );
					ErrorIf( build::compile_add_sources( path, group, false ) == 0, "No engine backend sources found for opengl 'wgl' (%s)", path );
					build::compile_add_library( "opengl32" );
					build::compile_add_library( "gdi32" );
				}

				if( OS_MACOS )
				{
					// NSGL
					strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "gfx" SLASH, BACKEND_GRAPHICS, SLASH "nsgl" );
					ErrorIf( build::compile_add_sources( path, group, false ) == 0, "No engine backend sources found for opengl 'wgl' (%s)", path );
					build::compile_add_framework( "OpenGL" );
				}

				if( OS_LINUX )
				{
					// WGL
					strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "gfx" SLASH, BACKEND_GRAPHICS, SLASH "glx" );
					ErrorIf( build::compile_add_sources( path, group, false ) == 0, "No engine backend sources found for opengl 'wgl' (%s)", path );
					build::compile_add_library( "GL" );
				}
			}
			else
			if( GRAPHICS_D3D11 )
			{
				if( OS_WINDOWS )
				{
					build::compile_add_library( "d3d11" );
					build::compile_add_library( "d3dcompiler" );
					build::compile_add_library( "dxgi" );
				}
			}

			// Thread | -r source/manta/backend/thread/*.cpp
			strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "thread" SLASH, BACKEND_THREAD );
			ErrorIf( build::compile_add_sources( path, group, true ) == 0, "No engine backend sources found for 'thread' (%s)", path );

			// Time | -r source/manta/backend/time/*.cpp
			strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "timer" SLASH, BACKEND_TIMER );
			ErrorIf( build::compile_add_sources( path, group, true ) == 0, "No engine backend sources found for 'timer' (%s)", path );
			if( OS_WINDOWS ) { build::compile_add_library( "winmm" ); }

			// Window | -r source/manta/backend/window/*.cpp
			strjoin( path, build::pathEngine, SLASH "manta" SLASH "backend" SLASH "window" SLASH, BACKEND_WINDOW );
			ErrorIf( build::compile_add_sources( path, group, true ) == 0, "No engine backend sources found for 'window' (%s)", path );
			if( OS_WINDOWS ) { build::compile_add_library( "user32" ); build::compile_add_library( "Shell32" ); }
			if( OS_MACOS ) { build::compile_add_framework( "Cocoa" ); }
			if( OS_LINUX ) { build::compile_add_library( "X11" ); }
		}
	}
}


void BuildCore::compile_write_ninja()
{
	String output;
	PrintLnColor( LOG_WHITE, "\tWrite Ninja" );
	{
		// Load <project>/config.json
		String configJSONContents;
		char pathConfig[PATH_SIZE];
		strjoin_filepath( pathConfig, "projects", build::args.project, "configs.json" );
		ErrorIf( !configJSONContents.load( pathConfig ), "Failed to load configs file: %s\n", pathConfig );

		// Read config.json
		JSON configsJSON = JSON( configJSONContents ).Object( build::args.config ).Object( "compile" ).Object( build::args.toolchain );
		String configCompilerFlags = configsJSON.GetString( "compilerFlags" );
		String configCompilerFlagsWarnings = configsJSON.GetString( "compilerFlagsWarnings" );
		String configLinkerFlags = configsJSON.GetString( "linkerFlags" );

		// Rule compile
		output.append( "rule compile\n" );
		output.append( PIPELINE_COMPILER_MSVC ? "  deps = msvc\n" : "  deps = gcc\n  depfile = $out.d\n" );
		output.append( "  command = " );
		output.append( build::tc.compilerName );
		output.append( " $in " );
		output.append( build::tc.compilerOutput );
		output.append( "$out " );
		output.append( build::tc.compilerFlags ); // Core compiler flags (build/toolchains.hpp)
		output.append( " " ).append( build::tc.compilerFlagsArchitecture ); // Compiler architecture (x64/arm/etc.)
		if( configCompilerFlags.length() > 0 ) { output.append( " " ).append( configCompilerFlags ); } // Project flags (configs.json)
		if( configCompilerFlagsWarnings.length() > 0 ) { output.append( " " ).append( configCompilerFlagsWarnings ); } // Project warning flags (configs.json)
		output.append( " " ).append( build::tc.compilerFlagsWarnings ); // Core compiler warnings (build/toolchains.hpp)
		char includeFlag[1024];
		for( String &includeDirectory : build::includeDirectories )
		{
			snprintf( includeFlag, sizeof( includeFlag ), build::tc.compilerFlagsIncludes, includeDirectory.c_str() ); // #include <...> directories
			output.append( " " ).append( includeFlag );
		}
		output.append( "\n\n" );

		// Rule Link
		output.append( "rule link\n  command = " );
		output.append( build::tc.linkerName );
		output.append( " $in " );
		output.append( build::tc.linkerOutput );
		output.append( "$out " );
		output.append( build::tc.linkerFlags );
		if( configLinkerFlags.length() > 0 ) { output.append( " " ); output.append( configLinkerFlags ); }
		for( String &library : build::libraries )
		{
			output.append( " " );
			output.append( build::tc.linkerPrefixLibrary );
			output.append( library );
			output.append( build::tc.linkerExtensionLibrary );
		}
		for( String &framework : build::frameworks )
		{
			output.append( " -framework " );
			output.append( framework );
		}
		output.append( "\n\n" );

		// Build Sources
		for( Source &source : build::sources )
		{
			output.append( "build " );
			output.append( source.objPath );
			output.append( ": compile .." SLASH ".." SLASH ".." SLASH ".." SLASH );
			output.append( source.cppPath );
			output.append( "\n" );
		}
		output.append( "\n" );

		// Build Exe
		output.append( "build " );
		output.append( build::args.project );
		output.append( build::tc.linkerExtensionExe );
		output.append( ": link" );
		for( Source &source : build::sources )
		{
			output.append( " " );
			output.append( source.objPath );
		}
		output.append( "\n" );

		// Write build.ninja
		char path[PATH_SIZE];
		strjoin( path, build::pathOutput, SLASH "runtime" SLASH "build.ninja" );
		ErrorIf( !output.save( path ), "Failed to write %s", path );
		if( verbose_output() ) { PrintLnColor( LOG_CYAN, "\t\tWrote ninja to: %s", path ); }
	}
}


void BuildCore::compile_run_ninja()
{
	PrintLnColor( LOG_WHITE, "\tRun Ninja" );

	char path[PATH_SIZE];
	strjoin( path, build::pathOutput, SLASH "runtime" );
	strjoin( build::commandNinja, "ninja -C ", path );

	// Run Ninja
	if( verbose_output() ) { PrintLnColor( LOG_MAGENTA, "\t\t> %s", build::commandNinja ); }
	Print( "\n ");

	ErrorIf( system( build::commandNinja ) != 0, "Compile failed" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuildCore::executable_run( int argc, char **argv )
{
	strjoin( build::commandRun, build::pathOutput, SLASH "runtime" SLASH, build::args.project, build::tc.linkerExtensionExe );

	// Run Executable
	int code = system( build::commandRun );
	ErrorIf( code, "%s%s terminated with code %d\n", build::args.project, build::tc.linkerExtensionExe, code );
	PrintLnColor( LOG_WHITE, "\n%s%s terminated with code %d\n", build::args.project, build::tc.linkerExtensionExe, code );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool verbose_output()
{
	return strcmp( build::args.verbose, "1" ) == 0;
}