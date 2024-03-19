#include <build/build.hpp>

#include <build/toolchains.hpp>

#include <build/assets.hpp>
#include <build/gfx.hpp>

#include <build/fileio.hpp>
#include <build/string.hpp>
#include <build/list.hpp>
#include <build/buffer.hpp>
#include <build/json.hpp>

#include <build/hashmap.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Build
{
	// Paths
	char pathEngine[PATH_SIZE];
	char pathProject[PATH_SIZE];
	char pathOutput[PATH_SIZE];
	char pathOutputBoot[PATH_SIZE];
	char pathOutputBuild[PATH_SIZE];
	char pathOutputGenerated[PATH_SIZE];
	char pathOutputGeneratedShaders[PATH_SIZE];
	char pathOutputRuntime[PATH_SIZE];
	char pathOutputRuntimeDistributables[PATH_SIZE];
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

void Build::compile_add_source( const char *cppPath, char *objPath )
{
	char buffer[PATH_SIZE];
	path_change_extension( buffer, sizeof( buffer ), objPath, Build::tc.linkerExtensionObj );
	Build::sources.add( { cppPath, buffer } );
}


u32 Build::compile_add_sources( const char *cppDirectory, const char *outputDirectory, const bool recurse )
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
		Build::compile_add_source( sourceFile.path, objPath );
	}

	// Logging
	const u32 sourcesCount = sourceFiles.size();
	if( verbose_output() )
	{
		PrintColor( LOG_CYAN, TAB TAB "%u source%s found in: %s", sourcesCount, sourcesCount == 1 ? "" : "s", cppDirectory );
		PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() );
	}
	return sourcesCount;
}


void Build::compile_add_library( const char *library )
{
	Build::libraries.add( library );
}


void Build::compile_add_framework( const char *framework )
{
	Build::frameworks.add( framework );
}


void Build::compile_add_include_directory( const char *includePath )
{
	Build::includeDirectories.add( includePath );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <build/shaders/compiler.hpp>

void BuilderCore::build( int argc, char **argv )
{
	// Setup
	{
		// Timer
		Time::init();
		Build::timer.start();

		// Parse arguments
		parse_arguments( argc, argv );

		// Log
		PrintColor( LOG_YELLOW, "\n>" );
		const u32 exeArgsCount = verbose_output() ? argc : 1;
		for( u32 i = 0; i < exeArgsCount; i++ ) { PrintColor( LOG_YELLOW, " %s", argv[i] ); }
		Print( "\n" );

		// Paths
		strjoin( Build::pathEngine, "source" );
		strjoin( Build::pathProject, "projects" SLASH, Build::args.project );
		strjoin( Build::pathOutput, Build::pathProject, SLASH "output" );
		strjoin( Build::pathOutputBoot, Build::pathOutput, SLASH, "boot" );
		strjoin( Build::pathOutputBuild, Build::pathOutput, SLASH, "build" );
		strjoin( Build::pathOutputGenerated, Build::pathOutput, SLASH, "generated" );
		strjoin( Build::pathOutputGeneratedShaders, Build::pathOutputGenerated, SLASH, "shaders" );
		strjoin( Build::pathOutputRuntime, Build::pathOutput, SLASH, "runtime" );
		strjoin( Build::pathOutputRuntimeDistributables, Build::pathOutputRuntime, SLASH, "distributables" );
		strjoin( Build::pathOutputPackage, Build::pathOutput, SLASH, "package" );
		strjoin( Build::pathOutputBuildCache, Build::pathOutputBuild, SLASH "build.cache" );

		// Output Directories
		directory_create( Build::pathOutputRuntime );
		directory_create( Build::pathOutputRuntimeDistributables );
		directory_create( Build::pathOutputGenerated );
		directory_create( Build::pathOutputGeneratedShaders );
	}

	// Compile Shader
	/*
	ShaderCompiler::ShaderProgram shaderProgram;
	for( ShaderCompiler::ShaderType i = 0; i < ShaderCompiler::SHADERTYPE_COUNT; i++ )
	{
		shaderProgram.compile( i, "shaders" SLASH "SHADER_DEFAULT.shader", Build::pathOutputGeneratedShaders );
	}
	exit( 0 );
	*/

	// Check Cache
	{
		build_cache();
	}

	// Build Conditions
	bool codegen = ( strcmp( Build::args.codegen, "1" ) == 0 );
	bool build = ( strcmp( Build::args.build, "1" ) == 0 );
	bool run = ( strcmp( Build::args.run, "1" ) == 0 );

	// Build Objects
	if( codegen )
	{
		PrintLnColor( LOG_WHITE, "\nBuild Objects" );
		Timer timer;

		Objects::begin();
		objects_gather();
		objects_cache();
		objects_parse();
		objects_write();

		PrintLnColor( LOG_WHITE, TAB "Finished (%.3f ms)", timer.elapsed_ms() );
	}

	// Build Graphics
	if( build )
	{
		PrintLnColor( LOG_WHITE, "\nBuild Graphics" );
		Timer timer;

		Gfx::begin();
		shaders_gather();
		shaders_cache();
		shaders_build();
		shaders_write();

		PrintLnColor( LOG_WHITE, TAB "Finished (%.3f ms)", timer.elapsed_ms() );
	}

	// Build Assets
	if( build )
	{
		PrintLnColor( LOG_WHITE, "\nBuild Assets" );
		Timer timer;

		Assets::begin();
		assets_gather();
		assets_cache();
		assets_build();
		assets_write();

		PrintLnColor( LOG_WHITE, TAB "Finished (%.3f ms)", timer.elapsed_ms() );
	}

	// Build Binary
	if( build )
	{
		PrintLnColor( LOG_WHITE, "\nBuild Binary" );
		Timer timer;

		binary_cache();
		binary_write();

		PrintLnColor( LOG_WHITE, TAB "Finished (%.3f ms)", timer.elapsed_ms() );
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

		PrintLnColor( LOG_WHITE, "\n" TAB "Compile finished: %.3f s (%.3f ms)", timer.elapsed_s(), timer.elapsed_ms() );
	}

	// Finish
	{
		PrintColor( LOG_GREEN, "\nBuild Finished!" );
		PrintLnColor( LOG_WHITE, " (%.3f s)", Build::timer.elapsed_s() );
		Build::cacheBufferCurrent.save( Build::pathOutputBuildCache );
	}

	// Run Executable
	if( run )
	{
		executable_run( argc, argv );
	}
}


void BuilderCore::parse_arguments( int argc, char **argv )
{
	// Parse Arguments
	Build::args.parse( argc, argv );
	Build::tc.detect( Build::args );
}


void BuilderCore::build_cache()
{
	// Force build? (arg: -clean=1)
	const bool force = ( strcmp( Build::args.clean, "1" ) == 0 );
	Build::cacheDirty |= force;

	// Cache File
	if( !Build::cacheBufferPrevious.load( Build::pathOutputBuildCache, true ) ) { Build::cacheDirty = true; }

	// Log
	PrintColor( LOG_WHITE, "Build Cache... " );

	if( Build::cacheDirty )
	{
		PrintLnColor( LOG_RED, force ? "dirty (force)" : "dirty" );
	}
	else
	{
		PrintLnColor( LOG_GREEN, "clean" );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuilderCore::objects_gather()
{
	PrintLnColor( LOG_WHITE, TAB "Gather Objects..." );
	char path[PATH_SIZE];

	// Engine
	strjoin( path, Build::pathEngine, SLASH "manta" );
	Objects::gather( path, true );

	// Project
	strjoin( path, Build::pathProject, SLASH "runtime" );
	Objects::gather( path, true );
}


void BuilderCore::objects_cache()
{
	// Full rebuild?
	Build::cacheDirtyObjects |= Build::cacheDirty;

	// Mismatch object file count?
	Build::cacheDirtyObjects |= ( Objects::objectFilesCount != Build::cacheBufferPrevious.read<usize>() );
	Build::cacheBufferCurrent.write( Objects::objectFilesCount );

	// Log
	PrintColor( LOG_WHITE, TAB "Objects Cache... " );
	PrintLnColor( Build::cacheDirtyObjects ? LOG_RED : LOG_GREEN, Build::cacheDirtyObjects ? "dirty" : "skip stage" );
}


void BuilderCore::objects_parse()
{
	if( !Build::cacheDirtyObjects ) { return; }
	PrintLnColor( LOG_WHITE, TAB "Parse Objects..." );

	// Parse
	Objects::parse();
}


void BuilderCore::objects_write()
{
	if( !Build::cacheDirtyObjects ) { return; }
	PrintLnColor( LOG_WHITE, TAB "Write Objects..." );

	// Resolve inheritance tree
	Objects::resolve();

	// Generate C++ files
	Objects::generate();

	// Write C++ files to disk
	Objects::write();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuilderCore::shaders_gather()
{
	PrintLnColor( LOG_WHITE, TAB "Gather Shaders..." );

	// Gather Shaders
	Gfx::gather( Build::pathEngine, true );
	Gfx::gather( Build::pathProject, true );
}


void BuilderCore::shaders_cache()
{
	// Full rebuild?
	Build::cacheDirtyShaders |= Build::cacheDirty;

	// Mismatch asset file count?
	Build::cacheDirtyShaders |= ( Gfx::shaderFileCount != Build::cacheBufferPrevious.read<usize>() );
	Build::cacheBufferCurrent.write( Gfx::shaderFileCount );

	// TODO: Fix this properly (both shaders and assets depend on the binary)
	Build::cacheDirtyAssets |= Build::cacheDirtyShaders;

	// Log
	PrintColor( LOG_WHITE, TAB "Shaders Cache... " );
	PrintLnColor( Build::cacheDirtyShaders ? LOG_RED : LOG_GREEN, Build::cacheDirtyShaders ? "dirty" : "skip stage" );
}


void BuilderCore::shaders_build()
{
	if( !Build::cacheDirtyShaders ) { return; }
	PrintLnColor( LOG_WHITE, TAB "Build Shaders..." );

	Gfx::build();
}


void BuilderCore::shaders_write()
{
	if( !Build::cacheDirtyShaders ) { return; }
	PrintLnColor( LOG_WHITE, TAB "Write Shaders..." );

	Gfx::write();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuilderCore::assets_gather()
{
	PrintLnColor( LOG_WHITE, TAB "Gather Assets..." );

	// Gather Sprites
	Assets::sprites.gather( Build::pathEngine );
	Assets::sprites.gather( Build::pathProject );

	// Gather Materials
	Assets::materials.gather( Build::pathEngine );
	Assets::materials.gather( Build::pathProject );

	// Gather Fonts
	Assets::fonts.gather( Build::pathEngine );
	Assets::fonts.gather( Build::pathProject );

	// Gather Meshes
	Assets::meshes.gather( Build::pathEngine );
	Assets::meshes.gather( Build::pathProject );

	// Gather Sounds
	// ...
}


void BuilderCore::assets_cache()
{
	// Full rebuild?
	Build::cacheDirtyAssets |= Build::cacheDirty;

	// Mismatch asset file count?
	Build::cacheDirtyAssets |= ( Assets::assetFileCount != Build::cacheBufferPrevious.read<usize>() );
	Build::cacheBufferCurrent.write( Assets::assetFileCount );

	// Log
	PrintColor( LOG_WHITE, TAB "Assets Cache... " );
	PrintLnColor( Build::cacheDirtyAssets ? LOG_RED : LOG_GREEN, Build::cacheDirtyAssets ? "dirty" : "skip stage" );
}


void BuilderCore::assets_build()
{
	if( !Build::cacheDirtyAssets ) { return; }
	PrintLnColor( LOG_WHITE, TAB "Build Assets..." );

	// Write Textures
	Assets::textures.write();

	// Write Glyphs
	Assets::glyphs.write();

	// Write Sprites
	Assets::sprites.write();

	// Write Materials
	Assets::materials.write();

	// Write Fonts
	Assets::fontRanges.write();
	Assets::fonts.write();

	// Write Meshes
	Assets::meshes.write();
}


void BuilderCore::assets_write()
{
	if( !Build::cacheDirtyAssets ) { return; }
	PrintLnColor( LOG_WHITE, TAB "Write Assets..." );

	// assets.generated.hpp
	{
		if( verbose_output() ) { PrintColor( LOG_CYAN, TAB TAB "Write %s", Assets::pathHeader ); }
		Timer timer;

		// Begin header
		String header;
		header.append( "/*\n" );
		header.append( " * File generated by build.exe--do not edit!\n" );
		header.append( " * Refer to: source/build/build.cpp (BuilderCore::assets_write)\n" );
		header.append( " */\n" );
		header.append( "#pragma once\n\n" );
		header.append( "#include <types.hpp>\n\n\n" );

		// Append header
		header.append( Assets::header );

		// Save header
		ErrorIf( !header.save( Assets::pathHeader ), "Failed to write '%s'", Assets::pathHeader );

		if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
	}

	// assets.generated.cpp
	{
		if( verbose_output() ) { PrintColor( LOG_CYAN, TAB TAB "Write %s", Assets::pathSource ); }
		Timer timer;

		// Begin source
		String source;
		source.append( "/*\n" );
		source.append( " * File generated by build.exe--do not edit!\n" );
		source.append( " * Refer to: source/build/build.cpp (BuilderCore::assets_write)\n" );
		source.append( " */\n" );
		source.append( "#include <assets.generated.hpp>\n\n\n" );

		// Append source
		source.append( Assets::source );

		// Save source
		ErrorIf( !source.save( Assets::pathSource ), "Failed to write '%s'", Assets::pathSource );

		if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuilderCore::binary_cache()
{
	// Full rebuild?
	Build::cacheDirtyBinary |= Build::cacheDirty;

	// Any binary contents to save?
	Build::cacheDirtyBinary |= ( Assets::binary.size() > 0 );

	// Log
	PrintColor( LOG_WHITE, TAB "Binary Cache... " );

	if( Build::cacheDirtyBinary )
	{
		PrintLnColor( LOG_RED, "dirty" );
	}
	else
	{
		PrintLnColor( LOG_GREEN, "skip stage" );
	}
}


void BuilderCore::binary_write()
{
	if( !Build::cacheDirtyBinary ) { return; }

	char path[PATH_SIZE];
	strjoin( path, Build::pathOutput, SLASH "runtime" SLASH, Build::args.project, ".bin" );

	// Log
	PrintLnColor( LOG_WHITE, TAB "Writing Binary" );
	if( verbose_output() ) { PrintColor( LOG_CYAN, TAB TAB "Write %s", path ); }
	Timer timer;

	// Write
	ErrorIf( !Assets::binary.save( path ), "Failed to write binary (%s)", path );

	// Log
	if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuilderCore::compile_project()
{
	// Include Directories
	{
		Build::compile_add_include_directory( ".." SLASH ".." SLASH "runtime" ); // Project
	}

	// C++ Sources + Library Linkage
	{
		char path[PATH_SIZE];
		PrintLnColor( LOG_WHITE, TAB "Gather Project Sources..." );
		strjoin( path, Build::pathProject, SLASH "runtime" );
		Build::compile_add_sources( path, "project", true ); // Project
	}
}


void BuilderCore::compile_engine()
{
	// Include Directories
	{
		Build::compile_add_include_directory( ".." SLASH "generated" ); // Generated C++ files
		Build::compile_add_include_directory( ".." SLASH ".." SLASH ".." SLASH ".." SLASH "source" ); // Engine
	}

	// C++ Sources + Library Linkage
	PrintLnColor( LOG_WHITE, TAB "Gather Engine Sources..." );
	{
		char path[PATH_SIZE];
		char group[PATH_SIZE];

		// root/projects/<project>/output/generated/*.cpp
		strjoin( path, Build::pathOutput, SLASH "generated" );
		strjoin( group, "engine" SLASH "generated" );
		Build::compile_add_sources( path, group, false );

		// root/source/*.cpp
		strjoin( path, Build::pathEngine );
		strjoin( group, "engine" );
		Build::compile_add_sources( path, group, false );

		// -r root/source/vendor/*.cpp
		strjoin( path, Build::pathEngine, SLASH "vendor" );
		strjoin( group, "engine" SLASH "vendor" );
		Build::compile_add_sources( path, group, true );

		// root/source/manta/*.cpp
		strjoin( path, Build::pathEngine, SLASH "manta" );
		strjoin( group, "engine" SLASH "manta" );
		Build::compile_add_sources( path, group, false );

		// Backend Sources
		strjoin( group, "engine" SLASH "manta" SLASH "backend" );
		{
			// Audio | -r source/manta/backend/audio/*.cpp
			strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "audio" SLASH, BACKEND_AUDIO );
			ErrorIf( Build::compile_add_sources( path, group, true ) == 0, "No backend found for 'audio' (%s)", path );

			// Filesystem | -r source/manta/backend/filesystem/*.cpp
			strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "filesystem" SLASH, BACKEND_FILESYSTEM );
			ErrorIf( Build::compile_add_sources( path, group, true ) == 0, "No backend found for 'filesystem' (%s)", path );

			// Network | -r source/manta/backend/network/*.cpp
			strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "network" SLASH, BACKEND_NETWORK );
			ErrorIf( Build::compile_add_sources( path, group, true ) == 0, "No backend found for 'network' (%s)", path );

			// Grahpics | -r source/manta/backend/gfx/*.cpp
			strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "gfx" SLASH, BACKEND_GRAPHICS );
			ErrorIf( Build::compile_add_sources( path, group, false ) == 0, "No backend found for 'gfx' (%s)", path );

			if( GRAPHICS_OPENGL )
			{
				if( OS_WINDOWS )
				{
					// WGL
					strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "gfx" SLASH, BACKEND_GRAPHICS, SLASH "wgl" );
					ErrorIf( Build::compile_add_sources( path, group, false ) == 0, "No backend found for opengl 'wgl' (%s)", path );
					Build::compile_add_library( "opengl32" );
					Build::compile_add_library( "gdi32" );
				}

				if( OS_MACOS )
				{
					// NSGL
					strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "gfx" SLASH, BACKEND_GRAPHICS, SLASH "nsgl" );
					ErrorIf( Build::compile_add_sources( path, group, false ) == 0, "No backend found for opengl 'wgl' (%s)", path );
					Build::compile_add_framework( "OpenGL" );
				}

				if( OS_LINUX )
				{
					// WGL
					strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "gfx" SLASH, BACKEND_GRAPHICS, SLASH "glx" );
					ErrorIf( Build::compile_add_sources( path, group, false ) == 0, "No backend found for opengl 'wgl' (%s)", path );
					Build::compile_add_library( "GL" );
				}
			}
			else
			if( GRAPHICS_D3D11 )
			{
				if( OS_WINDOWS )
				{
					Build::compile_add_library( "d3d11" );
					Build::compile_add_library( "d3dcompiler" );
					Build::compile_add_library( "dxgi" );
				}
			}

			// Thread | -r source/manta/backend/thread/*.cpp
			strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "thread" SLASH, BACKEND_THREAD );
			ErrorIf( Build::compile_add_sources( path, group, true ) == 0, "No backend found for 'thread' (%s)", path );

			// Time | -r source/manta/backend/time/*.cpp
			strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "time" SLASH, BACKEND_TIMER );
			ErrorIf( Build::compile_add_sources( path, group, true ) == 0, "No backend found for 'time' (%s)", path );
			if( OS_WINDOWS ) { Build::compile_add_library( "winmm" ); }

			// Window | -r source/manta/backend/window/*.cpp
			strjoin( path, Build::pathEngine, SLASH "manta" SLASH "backend" SLASH "window" SLASH, BACKEND_WINDOW );
			ErrorIf( Build::compile_add_sources( path, group, true ) == 0, "No backend found for 'window' (%s)", path );
			if( OS_WINDOWS ) { Build::compile_add_library( "user32" ); Build::compile_add_library( "Shell32" ); }
			if( OS_MACOS ) { Build::compile_add_framework( "Cocoa" ); }
			if( OS_LINUX ) { Build::compile_add_library( "X11" ); }
		}
	}
}


void BuilderCore::compile_write_ninja()
{
	String output;
	PrintLnColor( LOG_WHITE, TAB "Write Ninja" );
	{
		// Load <project>/config.json
		String configJSONContents;
		char pathConfig[PATH_SIZE];
		strjoin_filepath( pathConfig, "projects", Build::args.project, "configs.json" );
		ErrorIf( !configJSONContents.load( pathConfig ), "Failed to load configs file: %s\n", pathConfig );

		// Read config.json
		JSON configsJSON = JSON( configJSONContents ).Object( Build::args.config ).Object( "compile" ).Object( Build::args.toolchain );
		String configCompilerFlags = configsJSON.GetString( "compilerFlags" );
		String configCompilerFlagsWarnings = configsJSON.GetString( "compilerFlagsWarnings" );
		String configLinkerFlags = configsJSON.GetString( "linkerFlags" );

		// Rule compile
		output.append( "rule compile\n" );
		output.append( PIPELINE_COMPILER_MSVC ? "  deps = msvc\n" : "  deps = gcc\n  depfile = $out.d\n" );
		output.append( "  command = " );
		output.append( Build::tc.compilerName );
		output.append( " $in " );
		output.append( Build::tc.compilerOutput );
		output.append( "$out " );

		// Core compiler flags (build/toolchains.hpp)
		output.append( Build::tc.compilerFlags );

		// Compiler architecture (x64/arm/etc.)
		output.append( " " ).append( Build::tc.compilerFlagsArchitecture );
		if( configCompilerFlags.length() > 0 ) { output.append( " " ).append( configCompilerFlags ); } // Project flags (configs.json)
		if( configCompilerFlagsWarnings.length() > 0 ) { output.append( " " ).append( configCompilerFlagsWarnings ); } // Project warning flags (configs.json)
		output.append( " " ).append( Build::tc.compilerFlagsWarnings ); // Core compiler warnings (build/toolchains.hpp)
		char includeFlag[1024];
		for( String &includeDirectory : Build::includeDirectories )
		{
			snprintf( includeFlag, sizeof( includeFlag ), Build::tc.compilerFlagsIncludes, includeDirectory.c_str() ); // #include <...> directories
			output.append( " " ).append( includeFlag );
		}
		output.append( "\n\n" );

		// Rule Link
		output.append( "rule link\n  command = " );
		output.append( Build::tc.linkerName );
		output.append( " $in " );
		output.append( Build::tc.linkerOutput );
		output.append( "$out " );
		output.append( Build::tc.linkerFlags );
		if( configLinkerFlags.length() > 0 ) { output.append( " " ); output.append( configLinkerFlags ); }
		for( String &library : Build::libraries )
		{
			output.append( " " );
			output.append( Build::tc.linkerPrefixLibrary );
			output.append( library );
			output.append( Build::tc.linkerExtensionLibrary );
		}
		for( String &framework : Build::frameworks )
		{
			output.append( " -framework " );
			output.append( framework );
		}
		output.append( "\n\n" );

		// Build Sources
		for( Source &source : Build::sources )
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
		output.append( Build::args.project );
		output.append( Build::tc.linkerExtensionExe );
		output.append( ": link" );
		for( Source &source : Build::sources )
		{
			output.append( " " );
			output.append( source.objPath );
		}
		output.append( "\n" );

		// Write build.ninja
		char path[PATH_SIZE];
		strjoin( path, Build::pathOutput, SLASH "runtime" SLASH "build.ninja" );
		ErrorIf( !output.save( path ), "Failed to write %s", path );
		if( verbose_output() ) { PrintLnColor( LOG_CYAN, TAB TAB "Wrote ninja to: %s", path ); }
	}
}


void BuilderCore::compile_run_ninja()
{
	PrintLnColor( LOG_WHITE, TAB "Run Ninja" );

	char path[PATH_SIZE];
	strjoin( path, Build::pathOutput, SLASH "runtime" );
	strjoin( Build::commandNinja, "ninja -C ", path );

	// Run Ninja
	if( verbose_output() ) { PrintLnColor( LOG_MAGENTA, TAB TAB "> %s", Build::commandNinja ); }
	Print( "\n ");

	ErrorIf( system( Build::commandNinja ) != 0, "Compile failed" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuilderCore::executable_run( int argc, char **argv )
{
	strjoin( Build::commandRun, Build::pathOutput, SLASH "runtime" SLASH, Build::args.project, Build::tc.linkerExtensionExe );

	// Run Executable
	int code = system( Build::commandRun );
	ErrorIf( code, "%s%s terminated with code %d\n", Build::args.project, Build::tc.linkerExtensionExe, code );
	PrintLnColor( LOG_WHITE, "\n%s%s terminated with code %d\n", Build::args.project, Build::tc.linkerExtensionExe, code );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool verbose_output()
{
	return strcmp( Build::args.verbose, "1" ) == 0;
}