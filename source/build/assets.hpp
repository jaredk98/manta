#pragma once

#include <build/buffer.hpp>
#include <build/string.hpp>
#include <build/fileio.hpp>

#include <build/assets/textures.hpp>
#include <build/assets/glyphs.hpp>
#include <build/assets/sprites.hpp>
#include <build/assets/fonts.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace assets
{
	// Output Paths
	extern char pathHeader[PATH_SIZE];
	extern char pathSource[PATH_SIZE];

	// Output contents
	extern String source;
	extern String header;
	extern Buffer binary;

	// Cache
	extern usize assetFileCount;
	extern FileTime timeCache;

	// Asset Types
	extern Textures textures;
	extern Glyphs glyphs;
	extern Sprites sprites;
	extern Fonts fonts;
	extern FontRanges fontRanges;

	// Setup
	extern void begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Args>
void assets_struct( String &string, const char *name, Args... args )
{
	// struct <type> {
	string.append( "struct " );
	string.append( name );
	string.append( "\n{\n" );

	// Args...
	( string.append( "\t" ).append( args ).append( "\n" ), ... );

	// };
	string.append( "};\n\n" );
}


inline void assets_group( String &string )
{
	string.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );
}
