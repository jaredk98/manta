#pragma once

#include <types.hpp>

#include <build/list.hpp>
#include <build/string.hpp>
#include <build/buffer.hpp>

#include <build/assets/textures.hpp>
#include <build/assets/glyphs.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Sprite
{
	TextureID textureID;
	GlyphID glyphID;
	u16 count;
	u16 width;
	u16 height;
	i16 xorigin;
	i16 yorigin;
	String name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Sprites
{
	List<Sprite> sprites;

	void make_new( const Sprite &sprite );
	void gather( const char *path, const bool recurse = true );
	void load( const char *path );
	void write();

	inline Sprite &operator[]( const u32 spriteID ) { return sprites[spriteID]; }
};