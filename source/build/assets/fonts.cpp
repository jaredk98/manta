#include <build/assets/fonts.hpp>

#include <build/build.hpp>

#include <build/assets.hpp>
#include <build/assets/textures.hpp>

#include <build/json.hpp>
#include <build/list.hpp>
#include <build/fileio.hpp>

#include <vendor/stb/stb_truetype.hpp>

#include <types.hpp>
#include <debug.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u32 FontRanges::make_new( const FontRange &fontRange )
{
	fontRanges.add( fontRange );
	return fontRanges.size() - 1;
}

void FontRanges::write()
{
	Buffer &binary = Assets::binary;
	String &header = Assets::header;
	String &source = Assets::source;

	// Binary - do nothing
	{
		// ...
	}

	// Header
	{
		// Group
		assets_group( header );

		// Struct
		assets_struct( header,
			"DiskFontRange",
			"u32 start;",
			"u32 end;" );

		// Table
		header.append( "namespace Assets\n{\n" );
		header.append( "\tconstexpr u32 fontRangesCount = " ).append( static_cast<int>( fontRanges.size() ) ).append( ";\n" );
		header.append( "\textern const DiskFontRange fontRanges[];\n" );
		header.append( "}\n\n" );
	}

	// Source
	{
		// Group
		assets_group( source );
		source.append( "namespace Assets\n{\n" );

		// Table
		char buffer[PATH_SIZE];
		source.append( "\tconst DiskFontRange fontRanges[fontRangesCount] =\n\t{\n" );
		for( FontRange &fontRange : fontRanges )
		{
			snprintf( buffer, PATH_SIZE, "\t\t{ %d, %d },\n",
				fontRange.start,
				fontRange.end );

			source.append( buffer );
		}
		source.append( "\t};\n" );
		source.append( "}\n\n" );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Fonts::gather( const char *path, const bool recurse )
{
	// Gather & Load fonts
	Timer timer;
	List<FileInfo> files;
	directory_iterate( files, path, ".font", recurse );
	for( FileInfo &fileInfo : files ) { load( fileInfo.path ); }

	// Log
	if( verbose_output() )
	{
		const u32 count = files.size();
		PrintColor( LOG_CYAN, TAB TAB "%u font%s found in: %s", count, count == 1 ? "" : "s", path );
		PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() );
	}
}

void Fonts::load( const char *path )
{
	// Open font file
	String fontFile;
	ErrorIf( !fontFile.load( path ), "Unable to load font file: %s", path );
	JSON fontJSON { fontFile };

	// Cache
	Assets::assetFileCount++;
	if( !Build::cacheDirtyAssets )
	{
		FileTime time;
		file_time( path, &time );
		Build::cacheDirtyAssets |= file_time_newer( time, Assets::timeCache );
	}

	// Read file (json)
	String name = fontJSON.GetString( "name" );
	String ttf = fontJSON.GetString( "ttf" );

	// Error Checking
	ErrorIf( name.length() == 0, "Font '%s' has an invalid name", path );
	ErrorIf( ttf.length() == 0, "Font '%s' has an invalid file", path );

	// Register Font
	Font &font = fonts.add( { } );
	font.name = name;
	font.ttf = ttf;
	font.file = name.append( ".ttf" );

	// Ranges
	JSON ranges = fontJSON.Array( "ranges" );
	JSON range = ranges.ObjectAt( font.fontRangeIDs.size() );
	while( range )
	{
		const u32 fontRangeID = Assets::fontRanges.make_new( { } );
		FontRange &fontRange = Assets::fontRanges[fontRangeID];
		font.fontRangeIDs.add( fontRangeID );

		const int start = range.GetInt( "start" );
		const int end = range.GetInt( "end" );

		ErrorIf( start == 0, "Font '%s' has an invalid range start %d", path, start );
		ErrorIf( end == 0, "Font '%s' has an invalid range end %d", path, end );
		ErrorIf( start >= end, "Font '%s' has an invalid range %d - %d", path, start, end );

		fontRange.start = start;
		fontRange.end = end;

		// Continue
		range = ranges.ObjectAt( font.fontRangeIDs.size() );
	}
	font.fontRangeFirst = font.fontRangeIDs[0];
	font.fontRangeCount = font.fontRangeIDs.size();

	// Load font .ttf file
	char pathRelative[PATH_SIZE];
	path_get_directory( pathRelative, sizeof( pathRelative ), path );
	strappend( pathRelative, SLASH );
	strappend( pathRelative, ttf.c_str() );

	char ttfPath[PATH_SIZE];
	strjoin( ttfPath, pathRelative );

	directory_create( Build::pathOutputRuntimeDistributables );

	char ttfPathDistributables[PATH_SIZE];
	strjoin_filepath( ttfPathDistributables, Build::pathOutputRuntimeDistributables, font.file.c_str() );

	// Try relative path first...
	if( !file_copy( ttfPath, ttfPathDistributables ) )
	{
		// Relative path failed, try absolute path next
		strjoin( ttfPath, ttf.c_str() );
		ErrorIf( !file_copy( ttfPath, ttfPathDistributables ), "Unable to load .tff for font %s: %s", name.c_str(), ttfPath );
	}
}


void Fonts::write()
{
	Buffer &binary = Assets::binary;
	String &header = Assets::header;
	String &source = Assets::source;

	Timer timer;

	// Binary - do nothing
	{
		// ...
	}

	// Header
	{
		// Group
		assets_group( header );

		// Struct
		assets_struct( header,
			"DiskFont",
			"u32 fontRangeFirst;",
			"u32 fontRangeCount;",
			"const char *file;" );

		// Enums
		header.append( "enum\n{\n" );
		for( Font &font : fonts )
		{
			header.append( "\t" ).append( font.name ).append( ",\n" );
		}
		header.append( "};\n\n" );

		// Tables
		header.append( "namespace Assets\n{\n" );
		header.append( "\tconstexpr u32 fontsCount = " ).append( static_cast<int>( fonts.size() ) ).append( ";\n" );
		header.append( "\textern const DiskFont fonts[];\n" );
		header.append( "}\n\n" );
	}

	// Source
	{
		// Group
		assets_group( source );
		source.append( "namespace Assets\n{\n" );

		// DiskFont Table
		char buffer[PATH_SIZE];
		source.append( "\tconst DiskFont fonts[fontsCount] =\n\t{\n" );
		for( Font &font : fonts )
		{
			char filename[PATH_SIZE];
			strjoin( filename, "\"", font.file.c_str(), "\"" );

			snprintf( buffer, PATH_SIZE, "\t\t{ %d, %d, %s },\n",
				font.fontRangeFirst,
				font.fontRangeCount,
				filename );

			source.append( buffer );
		}
		source.append( "\t};\n\n" );
		source.append( "}\n\n" );
	}

	if( verbose_output() )
	{
		const usize count = fonts.size();
		PrintColor( LOG_CYAN, "\t\tWrote %d font%s", count, count == 1 ? "" : "s" );
		PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() );
	}
}