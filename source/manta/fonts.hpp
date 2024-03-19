#pragma once

#include <config.hpp>
#include <types.hpp>
#include <debug.hpp>

#include <manta/gfx.hpp>
#include <manta/color.hpp>
#include <manta/assets.hpp>
#include <manta/list.hpp>
#include <manta/fileio.hpp>
#include <manta/textureio.hpp>

#include <vendor/stb/stb_truetype.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Move this to engine library

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FONTS_GROUP_SIZE     ( 4 )
#define FONTS_TABLE_DEPTH    ( 8 )
#define FONTS_TABLE_SIZE     ( 4096 )
#define FONTS_TEXTURE_SIZE   ( 1024 )
#define FONTS_GLYPH_PADDING  ( 1 )
#define FONTS_GLYPH_SIZE_MAX ( 256 ) // FontGlyphInfo width/height is u8

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ftstring
{
	char *buffer;
	usize count;

	u8 r, g, b, a;
	u8 flags;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FontGlyphKey
{
	FontGlyphKey( u16 font, u16 size, u32 codepoint ) :
		font( font ),
		size( size ),
		codepoint( codepoint ) { }

	u16 font = 0;
	u16 size = 0;
	u32 codepoint = 0;

	u32 hash();

	inline bool operator==( const FontGlyphKey &other ) const
	{
		return font == other.font && size == other.size && codepoint == other.codepoint;
	}
};

static_assert( sizeof( FontGlyphKey ) == 8, "FontGlyphKey not 8 bytes!" );

#define NULL_RTFONTGLYPHKEY FontGlyphKey( 0, 0, 0 )


struct FontGlyphInfo
{
	u32 u : 12; // 4096 max
	u32 v : 12; // 4096 max
	u32 advance : 8; // u8
	u8 width;
	u8 height;
	i8 xshift;
	i8 yshift;

	bool get_glyph_metrics( const u16 font, const u16 size, const u32 codepoint );
};

static_assert( sizeof( FontGlyphInfo ) == 8, "FontGlyphInfo not 8 bytes!" );


struct FontGlyphEntry
{
	FontGlyphEntry( FontGlyphKey key, FontGlyphInfo value ) : key{ key }, value{ value } { }

	FontGlyphKey key;
	FontGlyphInfo value;
};


struct FontInfo
{
	FontInfo() { }
	stbtt_fontinfo info;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iFonts
{
	extern bool init();
	extern bool free();

	extern bool pack( FontGlyphInfo &glyphInfo );

	extern FontGlyphInfo &get( FontGlyphKey key );

	extern bool pack( FontGlyphInfo &glyphInfo );
	extern void flush();
	extern void update();

	extern void cache( const u16 font, const u16 size, const u32 start, const u32 end );
	extern void cache( const u16 font, const u16 size, const char *buffer );

	extern u16 insertX;
	extern u16 insertY;
	extern u16 lineHeight;

	extern FontGlyphEntry *data;

	extern List<File> fontFiles;
	extern List<FontInfo> fontInfos;
	extern List<FontGlyphEntry> dirtyGlyphs;

	extern Texture2DBuffer textureBuffer;
	extern GfxTexture2D texture2D;
	extern byte *bitmap;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////