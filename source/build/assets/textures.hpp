#pragma once

#include <types.hpp>

#include <build/assets/glyphs.hpp>

#include <build/textureio.hpp>
#include <build/buffer.hpp>
#include <build/list.hpp>
#include <build/string.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Texture
{
	Texture( String name ) : name( name ) { }

	String name;
	usize offset;
	u16 width = 0;
	u16 height = 0;

	List<GlyphID> glyphs;
	GlyphID add_glyph( Texture2DBuffer &&textureBuffer );
	void pack();
};

using TextureID = u16;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Textures
{
	List<Texture> textures;

	TextureID make_new( String &name );
	TextureID make_new( String &name, Texture2DBuffer &&textureBuffer );

	void write();

	inline Texture &operator[]( const TextureID id ) { return textures[id]; }
};