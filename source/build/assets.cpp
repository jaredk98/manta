#include <build/assets.hpp>

#include <build/build.hpp>
#include <build/string.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace assets
{
	// Output Paths
	char pathHeader[PATH_SIZE];
	char pathSource[PATH_SIZE];

	// Output contents
	String source;
	String header;
	Buffer binary;

	// Cache
	usize assetFileCount = 0;
	FileTime timeCache;

	// Asset Types
	Textures textures;
	Glyphs glyphs;
	Sprites sprites;
	Fonts fonts;
	FontRanges fontRanges;
}


void assets::begin()
{
	// Paths
	strjoin( pathHeader, build::pathOutput, SLASH "generated" SLASH "assets.generated.hpp" );
	strjoin( pathSource, build::pathOutput, SLASH "generated" SLASH "assets.generated.cpp" );

	// Cache
	FileTime timeHeader;
	if( !file_time( pathHeader, &timeHeader ) ) { build::cacheDirtyAssets = true; return; }
	FileTime timeSource;
	if( !file_time( pathSource, &timeSource ) ) { build::cacheDirtyAssets = true; return; }

	timeCache = file_time_newer( timeHeader, timeSource ) ? timeHeader : timeSource;
}