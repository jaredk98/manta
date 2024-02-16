#include <build/shaders.hpp>

#include <config.hpp>
#include <pipeline.hpp>

#include <build/assets.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace shaders
{
	// Output Paths
	char pathHeader[PATH_SIZE];
	char pathSource[PATH_SIZE];

	// Output contents
	String source;
	String header;

	// Cache
	usize shaderFileCount = 0;
	FileTime timeCache;

	// Shaders
	List<FileInfo> shaderFiles;
	List<Shader> shaders;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void shaders::begin()
{
	// Paths
	strjoin( pathHeader, build::pathOutput, SLASH "generated" SLASH "shaders.generated.hpp" );
	strjoin( pathSource, build::pathOutput, SLASH "generated" SLASH "shaders.generated.cpp" );

	// Cache
	FileTime timeHeader;
	if( !file_time( pathHeader, &timeHeader ) ) { build::cacheDirtyShaders = true; return; }
	FileTime timeSource;
	if( !file_time( pathSource, &timeSource ) ) { build::cacheDirtyShaders = true; return; }

	timeCache = file_time_newer( timeHeader, timeSource ) ? timeHeader : timeSource;
}


u32 shaders::gather( const char *path, const bool recurse )
{
	// TODO: Refactor
	#if GRAPHICS_OPENGL
		const char *extension = ".glsl";
	#elif GRAPHICS_D3D11 || GRAPHICS_D3D12
		const char *extension = ".hlsl";
	#elif GRAPHICS_VULKAN
		const char *extension = ".spv";
	#elif GRAPHICS_METAL
		const char *extension = ".metal";
	#else
		return 0;
	#endif

	// Gather Shaders
	Timer timer;
	const usize start = shaderFiles.size();
	directory_iterate( shaderFiles, path, extension, true );

	// Check Cache
	for( FileInfo &fileInfo : shaderFiles )
	{
		if( build::cacheDirtyShaders ) { break; }
		FileTime timeShader;
		file_time( fileInfo.path, &timeShader );
		build::cacheDirtyShaders |= file_time_newer( timeShader, timeCache );
	}

	// Log
	const u32 shaderCount = shaderFiles.size() - start;
	if( verbose_output() )
	{
		PrintColor( LOG_CYAN, "\t\t%u shader%s found in: %s", shaderCount, shaderCount == 1 ? "" : "s", path );
		PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() );
	}
	return shaderCount;
}


void shaders::build()
{
	// Load & Parse Shaders
	#if GRAPHICS_OPENGL
		for( FileInfo &fileInfo : shaderFiles ) { parse_shader_glsl( fileInfo ); }
	#elif GRAPHICS_D3D11
		for( FileInfo &fileInfo : shaderFiles ) { parse_shader_hlsl( fileInfo ); }
	#endif
}


void shaders::write()
{
	Buffer &binary = assets::binary;
	String &header = shaders::header;
	String &source = shaders::source;

	Timer timer;

	// Binary
	{
		for( Shader &shader : shaders )
		{
			// Write Vertex Shader
 			if( shader.codeVertex.length() > 0 )
			{
				shader.offsetVertex = binary.tell;
				shader.sizeVertex = shader.codeVertex.current;
				binary.write( shader.codeVertex.data, shader.sizeVertex );
			}

			// Write Fragment Shader
 			if( shader.codeFragment.length() > 0 )
			{
				shader.offsetFragment = binary.tell;
				shader.sizeFragment = shader.codeFragment.current;
				binary.write( shader.codeFragment.data, shader.sizeFragment );
			}

			// Write Compute Shader
 			if( shader.codeCompute.length() > 0 )
			{
				shader.offsetCompute = binary.tell;
				shader.sizeCompute = shader.codeCompute.current;
				binary.write( shader.codeCompute.data, shader.sizeCompute );
			}
		}
	}

	// Header
	{
		// Header Guard
		header.append( "#pragma once\n\n" );
		header.append( "#include <types.hpp>\n\n" );
		header.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );

		// Struct
		assets_struct( header,
			"DiskShader",
			"u32 offsetVertex;",
			"u32 sizeVertex;",
			"u32 offsetFragment;",
			"u32 sizeFragment;",
			"u32 offsetCompute;",
			"u32 sizeCompute;" );

		// Shader Enums
		header.append( "enum\n{\n" );
		for( Shader &shader : shaders )
		{
			header.append( "\t" ).append( shader.name ).append( ",\n" );
		}
		header.append( "};\n\n" );

		// Table
		header.append( "namespace shaders\n{\n" );
		header.append( "\tconstexpr u32 shadersCount = " ).append( static_cast<int>( shaders::shaders.size() ) ).append( ";\n" );
		header.append( "\textern const DiskShader diskShaders[];\n" );
		header.append( "}\n\n" );

		// Save
		header.save( shaders::pathHeader );
	}

	// Source
	{
		source.append( "#include <shaders.generated.hpp>\n\n" );
		source.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );

		source.append( "namespace shaders\n{\n" );

		// Table
		char buffer[PATH_SIZE];
		source.append( "\tconst DiskShader diskShaders[shadersCount] =\n\t{\n" );
		for( Shader &shader : shaders )
		{
			snprintf( buffer, PATH_SIZE, "\t\t{ %lu, %lu, %lu, %lu, %lu, %lu },\n",
				shader.offsetVertex,
				static_cast<u32>( shader.codeVertex.length() ),
				shader.offsetFragment,
				static_cast<u32>( shader.codeFragment.length() ),
				shader.offsetCompute,
				static_cast<u32>( shader.codeCompute.length() ) );

			source.append( buffer );
		}
		source.append( "\t};\n" );
		source.append( "}\n\n" );

		// Save
		source.save( shaders::pathSource );
	}

	if( verbose_output() )
	{
		const usize count = shaders.size();
		PrintColor( LOG_CYAN, "\t\tWrote %d shaders%s", count, count == 1 ? "s" : "" );
		PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool parse_shader_glsl_region( const String &fileContents, String &code, const char *regionKeyStart, const char *regionKeyEnd )
{
	const char *glslVersion = "#version 150 core\n"; // TODO: move this

	usize start = fileContents.find( regionKeyStart );
	if( start == USIZE_MAX ) { return false; }
	usize end = fileContents.find( regionKeyEnd, start + strlen( regionKeyStart ) );
	if( end == USIZE_MAX ) { end = fileContents.length(); } else { end--; }

	code.append( glslVersion );
	code.append( fileContents.substr( start + strlen( regionKeyStart ), end ) );
	return true;
}


void shaders::parse_shader_glsl( const FileInfo &file )
{
	String fileContents;
	ErrorIf( !fileContents.load( file.path ), "Failed to load shader file: %s", file.path );

	// Register Shader
	char shaderName[PATH_SIZE];
	path_get_filename( shaderName, sizeof( shaderName ), file.path );
	path_remove_extension( shaderName );
	Shader &shader = shaders::shaders.add( { shaderName } );

	// Parse Code
	parse_shader_glsl_region( fileContents, shader.codeVertex, "#pragma vertex", "pragma" );
	parse_shader_glsl_region( fileContents, shader.codeFragment, "#pragma fragment", "pragma" );
	parse_shader_glsl_region( fileContents, shader.codeCompute, "#pragma compute", "pragma" );
}


void shaders::parse_shader_hlsl( const FileInfo &file )
{
	String fileContents;
	ErrorIf( !fileContents.load( file.path ), "Failed to load shader file: %s", file.path );

	// Register Shader
	char shaderName[PATH_SIZE];
	path_get_filename( shaderName, sizeof( shaderName ), file.path );
	path_remove_extension( shaderName );

	Shader &shader = shaders::shaders.add( { shaderName } );
	shader.codeVertex.copy( fileContents );
	shader.codeFragment.copy( fileContents );
}