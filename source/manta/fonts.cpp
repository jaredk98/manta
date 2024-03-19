#include <manta/fonts.hpp>

#include <manta/memory.hpp>
#include <manta/utf8.hpp>
#include <manta/draw.hpp>
#include <manta/string.hpp>
#include <manta/fileio.hpp>
#include <manta/gfx.hpp>
#include <manta/color.hpp>

#include <vendor/vendor.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u32 FontGlyphKey::hash()
{
	// Calculate 'randomized' offset from font id and size using prime numbers
	const u32 p1 = 41;
	const u32 p2 = 37;
	const u32 p3 = 31;
	const u32 offset = ( ( font * p1 ) ^ ( size * p2 ) ) * p3 ^ ( ( ( font * p1 ) ^ ( size * p2 ) ) >> 16 );

	// Calculate hash index
	return static_cast<u32>( ( codepoint / FONTS_GROUP_SIZE + offset ) % FONTS_TABLE_SIZE );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool FontGlyphInfo::get_glyph_metrics( const u16 font, const u16 size, const u32 codepoint )
{
	FontInfo &fontInfo = iFonts::fontInfos[font];
	const float scale = stbtt_ScaleForPixelHeight( &fontInfo.info, size * ( 96.0f / 72.0f ) );

	// Max Ascent (TODO: considering caching this instead of getting it each time?)
	int mx0, my0, mx1, my1;
	stbtt_GetCodepointBitmapBox( &fontInfo.info, 'T', scale, scale, &mx0, &my0, &mx1, &my1 );

	// Get glyph metrics
	int x0, y0, x1, y1;
	stbtt_GetCodepointBitmapBox( &fontInfo.info, codepoint, scale, scale, &x0, &y0, &x1, &y1 );

	const int w = x1 - x0;
	const int h = y1 - y0;
	ErrorIf( w < 0 || h < 0, "Invalid glyph size (%dx%d) for codepoint %llu (font %d, size: %d)", w, h, codepoint, font, size );
	ErrorIf( w >= FONTS_GLYPH_SIZE_MAX || h >= FONTS_GLYPH_SIZE_MAX,
	         "Exceeded glyph size (%dx%d) for codepoint %llu (font %d, size: %d)", w, h, codepoint, font, size );

	int advance, leftSideBearing;
	stbtt_GetCodepointHMetrics( &fontInfo.info, codepoint, &advance, &leftSideBearing );

	// Update FontGlyphInfo
	this->advance = static_cast<u8>( advance * scale );
	this->width = static_cast<u8>( w );
	this->height = static_cast<u8>( h );
	this->xshift = x0;
	this->yshift = y0 - my0;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iFonts
{
	u16 insertX = FONTS_GLYPH_PADDING;
	u16 insertY = FONTS_GLYPH_PADDING;
	u16 lineHeight = 0;

	FontGlyphEntry *data = nullptr;

	List<File> fontFiles;
	List<FontInfo> fontInfos;
	List<FontGlyphEntry> dirtyGlyphs;

	Texture2DBuffer textureBuffer;
	GfxTexture2D texture2D;
	byte *bitmap;
}


bool iFonts::init()
{
	// Init Fonts Table
	Assert( data == nullptr );
	constexpr usize size = FONTS_GROUP_SIZE * FONTS_TABLE_DEPTH * FONTS_TABLE_SIZE * sizeof( FontGlyphEntry );
	data = reinterpret_cast<FontGlyphEntry *>( memory_alloc( size ) );
	ErrorReturnIf( data == nullptr, false, "Fonts: failed to allocate memory for RTFonts table" );
	memory_set( data, 0, size ); // Zero memory

	// Setup Paths
	char pathFont[PATH_SIZE];
	char pathDistributables[PATH_SIZE];
	strjoin_filepath( pathDistributables, WORKING_DIRECTORY, "distributables" );

	// Load Font MJetrics
	fontFiles.init( Assets::fontsCount );
	fontInfos.init( Assets::fontsCount );
	for( u16 fontID = 0; fontID < Assets::fontsCount; fontID++ )
	{
		// Load Font File
		File &fontFile = fontFiles.add( { } );
		strjoin_filepath( pathFont, pathDistributables, Assets::fonts[fontID].file );
		ErrorReturnIf( !fontFile.open( pathFont ), false, "Fonts: failed to open .ttf for font %d (%s)", fontID, pathFont );

		// Get Font Info
		FontInfo &fontInfo = fontInfos.add( { } );
		ErrorReturnIf( stbtt_InitFont( &fontInfo.info, fontFile.data, stbtt_GetFontOffsetForIndex( fontFile.data, 0 ) ) != 1,
		               false, "Fonts: failed to get metrics for font %d (%s)", fontID, pathFont );
	}

	// Init dirtyGlyphs list
	dirtyGlyphs.init();

	// Init Texture2DBuffer
	textureBuffer.init( FONTS_TEXTURE_SIZE, FONTS_TEXTURE_SIZE );
	textureBuffer.clear( { 0, 0, 0, 0 } );

	// Init Texture2D
	texture2D.init( textureBuffer.data, textureBuffer.width, textureBuffer.height, GfxColorFormat_R8G8B8A8 );

	// Init bitmap buffer
	bitmap = reinterpret_cast<byte *>( memory_alloc( FONTS_GLYPH_SIZE_MAX * FONTS_GLYPH_SIZE_MAX ) );
	ErrorReturnIf( bitmap == nullptr, false, "Fonts: failed to allocate memory for RTFont bitmap buffer" );

	// Success
	return true;
}


bool iFonts::free()
{
	// Free RTFonts table
	if( data != nullptr )
	{
		memory_free( data );
		data = nullptr;
	}

	// Free font metrics
	for( File &file : fontFiles ) { file.close(); }
	fontFiles.free();
	fontInfos.free();

	// Free dirtyGlyphs list
	dirtyGlyphs.free();

	// Free Texture2D
	texture2D.free();

	// Free bitmap buffer
	if( bitmap != nullptr )
	{
		memory_free( bitmap );
		bitmap = nullptr;
	}

	// Success
	return true;
}


FontGlyphInfo &iFonts::get( FontGlyphKey key )
{
	// About: The goal here is to efficiently cache and retrieve font glyphs at runtime using an optimized "hashtable"
	//
	// FontGlyphEntry is 16 bytes (FontGlyphKey + FontGlyphInfo) meaning 4 glyphs fit in a 64 byte cache line.
	// The 'hash' index below ensures consecutive codepoints of a font and size (i.e. 'a', 'b', 'c', 'd') all share
	// a single L1 cache line. Since the hashing function can produce collisions, 'FONTS_TABLE_DEPTH' number of
	// collisions are allowed before the glyph retrieval is aborted.

	const usize index = key.hash() * ( FONTS_GROUP_SIZE * FONTS_TABLE_DEPTH ) + ( key.codepoint % FONTS_GROUP_SIZE );

	for( u8 collision = 0; collision < FONTS_TABLE_DEPTH; collision++ )
	{
		// Retrieve FontGlyphEntry
		FontGlyphEntry &entry = data[index + collision * FONTS_GROUP_SIZE];

		// Found our key?
		if( LIKELY( entry.key == key ) )
		{
			return entry.value;
		}

		// Empty key?
		if( entry.key == NULL_RTFONTGLYPHKEY )
		{
			// Retrieve metrics & cache the glyph
			entry.key = key;
			entry.value.get_glyph_metrics( key.font, key.size, key.codepoint );

			// Pack Glyph
			if( !pack( entry.value ) )
			{
				// Packing failed -- out of room? Return a "null key"
				AssertMsg( false, "Failed to pack glyph in RTFont texture (codepoint: %llu)", index, key.codepoint );
				return data[0].value;
			}

			// Add glyph to rasterization list
			dirtyGlyphs.add( entry );
			return entry.value;
		}
	}

	// Return a "null key"
	AssertMsg( false, "Saturated RTFont table at index %d (codepoint: %llu)", index, key.codepoint );
	return data[0].value;
}


bool iFonts::pack( FontGlyphInfo &glyphInfo )
{
	// Check if the glyph pushes us onto a "new line"
	if( insertX + glyphInfo.width + FONTS_GLYPH_PADDING >= textureBuffer.width )
	{
		insertX = FONTS_GLYPH_PADDING;
		insertY += lineHeight + FONTS_GLYPH_PADDING * 2;
		lineHeight = 0;
	}

	// No more room?
	if( insertY + glyphInfo.height + FONTS_GLYPH_PADDING >= textureBuffer.height )
	{
		return false;
	}

	// Insert Glyph
	glyphInfo.u = insertX;
	glyphInfo.v = insertY;
	insertX += glyphInfo.width + FONTS_GLYPH_PADDING * 2;
	lineHeight = glyphInfo.height > lineHeight ? glyphInfo.height : lineHeight;
	return true;
}


void iFonts::flush()
{
	// Clear Glyph table cache
	constexpr usize size = FONTS_GROUP_SIZE * FONTS_TABLE_DEPTH * FONTS_TABLE_SIZE * sizeof( FontGlyphEntry );
	memory_set( data, 0, size );

	// Clear Texture2DBuffer
	textureBuffer.clear( { 0, 0, 0, 0 } );

	// Clear newGlyph list
	dirtyGlyphs.clear();

	// Update GPU texture
	texture2D.free();
	texture2D.init( textureBuffer.data, textureBuffer.width, textureBuffer.height, GfxColorFormat_R8G8B8A8 );

	// Reset packing state
	insertX = FONTS_GLYPH_PADDING;
	insertY = FONTS_GLYPH_PADDING;
	lineHeight = 0;
}


void iFonts::update()
{
	// Dirty?
	if( dirtyGlyphs.size() == 0 ) { return; }

	// Rasterize glyph bitmaps & copy to Texture2DBuffer
	for( FontGlyphEntry &entry : dirtyGlyphs )
	{
		// TODO: This can potentially cause frame spikes as rasterizing & copying glyphs is a CPU intensive task
		// Consider moving this to a background thread and double buffering?

		const FontGlyphKey &key = entry.key;
		const FontGlyphInfo &glyph = entry.value;
		const FontInfo &fontInfo = fontInfos[entry.key.font];

		// Bitmap
		const float scale = stbtt_ScaleForPixelHeight( &fontInfo.info, key.size * ( 96.0f / 72.0f ) );
		stbtt_MakeCodepointBitmap( &fontInfo.info, bitmap, glyph.width, glyph.height, glyph.width, scale, scale, key.codepoint );

		// Transfer to Bitmap to RGBA Texture2DBuffer
		for( u16 x = 0; x < glyph.width; x++ )
		{
			for( u16 y = 0; y < glyph.height; y++ )
			{
				const u32 srcIndex = y * glyph.width + x;
				const u32 dstIndex = ( glyph.v + y ) * textureBuffer.width + ( glyph.u + x );
				textureBuffer.data[dstIndex] = { 255, 255, 255, static_cast<u8>( bitmap[srcIndex] ) };
			}
		}
	}

	// Clear dirtyGlyphs
	dirtyGlyphs.clear();

	// Update GPU texture
	texture2D.free();
	texture2D.init( textureBuffer.data, textureBuffer.width, textureBuffer.height, GfxColorFormat_R8G8B8A8 );
}


void iFonts::cache( const u16 font, const u16 size, const u32 start, const u32 end )
{
	for( u32 codepoint = start; codepoint <= end; codepoint++ )
	{
		get( { font, size, codepoint } );
	}
}


void iFonts::cache( const u16 font, const u16 size, const char *buffer )
{
	u32 state = UTF8_ACCEPT;
	u32 codepoint;
	char c;
	while( ( c = *buffer++ ) != '\0' )
	{
		if( utf8_decode( &state, &codepoint, c ) != UTF8_ACCEPT ) { continue; }
		get( { font, size, codepoint } );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////