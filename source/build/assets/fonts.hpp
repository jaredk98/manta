#pragma once

#include <types.hpp>

#include <build/assets/textures.hpp>

#include <build/list.hpp>
#include <build/string.hpp>
#include <build/buffer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FontRange
{
	u32 start;
	u32 end;
};


struct FontRanges
{
	List<FontRange> fontRanges;

	u32 make_new( const FontRange &fontRange );
	void write();

	inline FontRange &operator[]( const u32 fontRangeID ) { return fontRanges[fontRangeID]; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Font
{
	u32 fontRangeFirst;
	u32 fontRangeCount;
	String file;

	String name;
	String ttf;
	List<u32> fontRangeIDs;
};


struct Fonts
{
	List<Font> fonts;

	void gather( const char *path, const bool recurse = true );
	void load( const char *path );
	void write();

	inline Font &operator[]( const u32 fontID ) { return fonts[fontID]; }
};
