#include <manta/draw.hpp>

#include <manta/gfx.hpp>
#include <manta/utf8.hpp>
#include <manta/fonts.hpp>
#include <manta/math.hpp>

#include <vendor/vendor.hpp>
#include <vendor/stdarg.hpp>
#include <vendor/stdio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static DiskSprite nullSprite = assets::sprites[SPRITE_PIXEL];
static DiskGlyph nullGlyph = assets::glyphs[nullSprite.glyph];
static GfxTexture2D *nullTexture = &bGfx::textures2D[nullSprite.texture];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_quad( const float x1, const float y1, const float x2, const float y2, const Color color, const float depth )
{
#if !RENDER_NONE
	Gfx::quad_batch_write( x1, y1, x2, y2, 0, 0, 0xFFFF, 0xFFFF, color, nullptr, depth );
#endif
}


void draw_quad_color( const float x1, const float y1, const float x2, const float y2,
                      const Color c1, const Color c2, const Color c3, const Color c4, const float depth )
{
#if !RENDER_NONE
	Gfx::quad_batch_write( x1, y1, x2, y2, 0, 0, 0xFFFF, 0xFFFF, c1, c2, c3, c4, nullptr, depth );
#endif
}


void draw_quad_uv( const float x1, const float y1, const float x2, const float y2,
                   const float u1, const float v1, const float u2, const float v2, const Color color, const float depth )
{
#if !RENDER_NONE
	const u16 U1 = static_cast<u16>( u1 * 65535.0 );
	const u16 V1 = static_cast<u16>( v1 * 65535.0 );
	const u16 U2 = static_cast<u16>( u2 * 65535.0 );
	const u16 V2 = static_cast<u16>( v2 * 65535.0 );
	Gfx::quad_batch_write( x1, y1, x2, y2, U1, V1, U2, V2, color, nullptr, depth );
#endif
}


void draw_quad_uv( const float  x1, const float  y1, const float  x2, const float  y2,
                   const u16 u1, const u16 v1, const u16 u2, const u16 v2, const Color color, const float depth )
{
#if !RENDER_NONE
	Gfx::quad_batch_write( x1, y1, x2, y2, u1, v1, u2, v2, color, nullptr, depth );
#endif
}


void draw_quad_uv( const float x1, const float y1, const float x2, const float y2,
                   const float x3, const float y3, const float x4, const float y4,
                   const float u1, const float v1, const float u2, const float v2, const Color color, const float depth )
{
#if !RENDER_NONE
	const u16 U1 = static_cast<u16>( u1 * 65535.0 );
	const u16 V1 = static_cast<u16>( v1 * 65535.0 );
	const u16 U2 = static_cast<u16>( u2 * 65535.0 );
	const u16 V2 = static_cast<u16>( v2 * 65535.0 );
	Gfx::quad_batch_write( x1, y1, x2, y2, x3, y3, x4, y4, U1, V1, U2, V2, color, nullptr, depth );
#endif
}


void draw_quad_uv( const float x1, const float y1, const float x2, const float y2,
                   const float x3, const float y3, const float x4, const float y4,
                   const u16 u1, const u16 v1, const u16 u2, const u16 v2, const Color color, const float depth )
{
#if !RENDER_NONE
	Gfx::quad_batch_write( x1, y1, x2, y2, x3, y3, x4, y4, u1, v1, u2, v2, color, nullptr, depth );
#endif
}


void draw_quad_uv_color( const float x1, const float y1, const float x2, const float y2,
                         const float u1, const float v1, const float u2, const float v2,
                         const Color c1, const Color c2, const Color c3, const Color c4, const float depth )
{
#if !RENDER_NONE
	const u16 U1 = static_cast<u16>( u1 * 65535.0 );
	const u16 V1 = static_cast<u16>( v1 * 65535.0 );
	const u16 U2 = static_cast<u16>( u2 * 65535.0 );
	const u16 V2 = static_cast<u16>( v2 * 65535.0 );
	Gfx::quad_batch_write( x1, y1, x2, y2, U1, V1, U2, V2, c1, c2, c3, c4, nullptr, depth );
#endif
}


void draw_quad_uv_color( const float x1, const float y1, const float x2, const float y2,
                         const u16 u1, const u16 v1, const u16 u2, const u16 v2,
                         const Color c1, const Color c2, const Color c3, const Color c4, const float depth )
{
#if !RENDER_NONE
	Gfx::quad_batch_write( x1, y1, x2, y2, u1, v1, u2, v2, c1, c2, c3, c4, nullptr, depth );
#endif
}


void draw_quad_uv_color( const float x1, const float y1, const float x2, const float y2,
                         const float x3, const float y3, const float x4, const float y4,
                         const float u1, const float v1, const float u2, const float v2,
                         const Color c1, const Color c2, const Color c3, const Color c4, const float depth )
{
#if !RENDER_NONE
	const u16 U1 = static_cast<u16>( u1 * 65535.0 );
	const u16 V1 = static_cast<u16>( v1 * 65535.0 );
	const u16 U2 = static_cast<u16>( u2 * 65535.0 );
	const u16 V2 = static_cast<u16>( v2 * 65535.0 );
	Gfx::quad_batch_write( x1, y1, x2, y2, x3, y3, x4, y4, U1, V1, U2, V2, c1, c2, c3, c4, nullptr, depth );
#endif
}


void draw_quad_uv_color( const float  x1, const float  y1, const float  x2, const float  y2,
                         const float  x3, const float  y3, const float  x4, const float  y4,
                         const u16 u1, const u16 v1, const u16 u2, const u16 v2,
                         const Color  c1, const Color  c2, const Color  c3, const Color  c4, const float depth )
{
#if !RENDER_NONE
	Gfx::quad_batch_write( x1, y1, x2, y2, x3, y3, x4, y4, u1, v1, u2, v2, c1, c2, c3, c4, nullptr, depth );
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_sprite( const u32 sprite, const u16 subimg, float x, float y, const float xscale, const float yscale, const Color color, const float depth )
{
#if !RENDER_NONE
	const DiskSprite &diskSprite = assets::sprites[sprite];
	const DiskGlyph &diskGlyph = assets::glyphs[diskSprite.glyph + subimg];
	const GfxTexture2D *const texture = &bGfx::textures2D[diskSprite.texture];

	float width  = diskSprite.width * xscale;
	float height = diskSprite.height * yscale;

	x -= diskSprite.xorigin * xscale;
	y -= diskSprite.yorigin * yscale;

	Gfx::quad_batch_write( x, y, x + width, y + height, diskGlyph.u1, diskGlyph.v1, diskGlyph.u2, diskGlyph.v2, color, texture, depth );
#endif
}


void draw_sprite_part( const u32 sprite, const u16 subimg, float x, float y,
                                const float u1, const float v1, const float u2, const float v2,
                                const float xscale, const float yscale, const Color color, const float depth )
{
#if !RENDER_NONE
	const DiskSprite &diskSprite = assets::sprites[sprite];
	const DiskGlyph &diskGlyph = assets::glyphs[diskSprite.glyph + subimg];
	const GfxTexture2D *const texture = &bGfx::textures2D[diskSprite.texture];

	float width  = diskSprite.width * xscale;
	float height = diskSprite.height * yscale;

	x -= diskSprite.xorigin * xscale;
	y -= diskSprite.yorigin * yscale;

	const u16 u = diskGlyph.u2 - diskGlyph.u1;
	const u16 v = diskGlyph.v2 - diskGlyph.v1;
	Gfx::quad_batch_write( x, y, x + width, y + height,
	                          diskGlyph.u1 + static_cast<u16>( u1 * u ), diskGlyph.v1 + static_cast<u16>( v1 * v ),
	                          diskGlyph.u1 + static_cast<u16>( u2 * v ), diskGlyph.v1 + static_cast<u16>( v2 * v ), color, texture, depth );
#endif
}


void draw_sprite_angle( const u32 sprite, const u16 subimg, float x, float y, const float angle, const float xscale, const float yscale, const Color color, const float depth )
{
#if !RENDER_NONE
	const DiskSprite &diskSprite = assets::sprites[sprite];
	const DiskGlyph &diskGlyph = assets::glyphs[diskSprite.glyph + subimg];
	const GfxTexture2D *const texture = &bGfx::textures2D[diskSprite.texture];

	const float width = diskSprite.width * xscale;
	const float height = diskSprite.height * yscale;
	const float dx = diskSprite.xorigin * xscale;
	const float dy = diskSprite.yorigin * yscale;

#if 0
	const float s = sinf( degtorad( angle ) );
	const float c = cosf( degtorad( angle ) );
#else
	float s, c;
	fast_sin_cos( degtorad( angle ), s, c );
#endif

	const float x1 = x - ( dx ) * c + ( dy ) * s;
	const float y1 = y - ( dx ) * s - ( dy ) * c;
	const float x2 = x - ( dx - width ) * c + ( dy ) * s;
	const float y2 = y - ( dx - width ) * s - ( dy ) * c;
	const float x3 = x - ( dx ) * c + ( dy - height ) * s;
	const float y3 = y - ( dx ) * s - ( dy - height ) * c;
	const float x4 = x - ( dx - width ) * c + ( dy - height ) * s;
	const float y4 = y - ( dx - width ) * s - ( dy - height ) * c;

	Gfx::quad_batch_write( x1, y1, x2, y2, x3, y3, x4, y4, diskGlyph.u1, diskGlyph.v1, diskGlyph.u2, diskGlyph.v2, color, texture, depth );
#endif
}


void draw_sprite_fast( const u32 sprite, const u16 subimg, float x, float y, const Color color )
{
#if !RENDER_NONE
	const DiskSprite &diskSprite = assets::sprites[sprite];
	const DiskGlyph &diskGlyph = assets::glyphs[diskSprite.glyph + subimg];
	const GfxTexture2D *const texture = &bGfx::textures2D[diskSprite.texture];

	float width = diskSprite.width;
	float height = diskSprite.height;

	Gfx::quad_batch_write( x, y, x + diskSprite.width, y + diskSprite.height, diskGlyph.u1, diskGlyph.v1, diskGlyph.u2, diskGlyph.v2, color, texture, 0.0f );
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_surface( const GfxSurface2D &surface, float x, float y, const float xscale, const float yscale, const Color color, const float depth )
{
#if !RENDER_NONE
	const float x1 = x;
	const float y1 = y;
	const float x2 = x + surface.width * xscale;
	const float y2 = y + surface.height * yscale;

	const u16 u1 = 0;
	const u16 v1 = 0;
	const u16 u2 = 0xFFFF;
	const u16 v2 = 0xFFFF;

	Gfx::quad_batch_write( x1, y1, x2, y2, u1, v1, u2, v2, color, &surface.textureColor, depth );
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_rectangle( const float x1, const float y1, const float x2, const float y2, const Color color, const bool outline, const float depth )
{
#if !RENDER_NONE
	if( outline )
	{
		draw_rectangle( x1,        y1,        x1 + 1.0f, y2,        color, false, depth ); // left
		draw_rectangle( x2 - 1.0f, y1,        x2,        y2,        color, false, depth ); // right
		draw_rectangle( x1 + 1.0f, y1,        x2 - 1.0f, y1 + 1.0f, color, false, depth ); // top
		draw_rectangle( x1,        y2 - 1.0f, x2,        y2,        color, false, depth ); // bottom
	}
	else
	{
		Gfx::quad_batch_write( x1, y1, x2, y2, nullGlyph.u1, nullGlyph.v1, nullGlyph.u2, nullGlyph.v2, color, nullTexture, depth );
	}
#endif
}


void draw_rectangle_angle( const float x1, const float y1, const float x2, const float y2, const float angle, const Color color, const bool outline, const float depth )
{
#if !RENDER_NONE
	if( outline )
	{
		if( angle != 0.0f )
		{
			const vec2f x2y1 = vec2f( x2, y1 ).rotate_origin( x1, y1, angle );
			const vec2f x1y2 = vec2f( x1, y2 ).rotate_origin( x1, y1, angle );
			draw_rectangle_angle( x1,     y1,     x1 + 1.0f,            y2,                   angle, color, false, depth ); // left
			draw_rectangle_angle( x2y1.x, x2y1.y, x2y1.x - 1.0f,        x2y1.y + ( y2 - y1 ), angle, color, false, depth ); // right
			draw_rectangle_angle( x1,     y1,     x2,                   y1 + 1.0f,            angle, color, false, depth ); // top
			draw_rectangle_angle( x1y2.x, x1y2.y, x1y2.x + ( x2 - x1 ), x1y2.y - 1.0f,        angle, color, false, depth ); // bottom
		}
		else
		{
			draw_rectangle( x1,        y1,        x1 + 1.0f, y2,        color, false, depth ); // left
			draw_rectangle( x2 - 1.0f, y1,        x2,        y2,        color, false, depth ); // right
			draw_rectangle( x1 + 1.0f, y1,        x2 - 1.0f, y1 + 1.0f, color, false, depth ); // top
			draw_rectangle( x1,        y2 - 1.0f, x2,        y2,        color, false, depth ); // bottom
		}
	}
	else
	{
		if( angle != 0.0f )
		{
			const vec2f x2y1 = vec2f( x2, y1 ).rotate_origin( x1, y1, angle );
			const vec2f x1y2 = vec2f( x1, y2 ).rotate_origin( x1, y1, angle );
			const vec2f x2y2 = vec2f( x2, y2 ).rotate_origin( x1, y1, angle );
			Gfx::quad_batch_write( x1, y1, x2y1.x, x2y1.y, x1y2.x, x1y2.y, x2y2.x, x2y2.y,
			                          nullGlyph.u1, nullGlyph.v1, nullGlyph.u2, nullGlyph.v2, color, nullTexture, depth );
		}
		else
		{
			Gfx::quad_batch_write( x1, y1, x2, y2, nullGlyph.u1, nullGlyph.v1, nullGlyph.u2, nullGlyph.v2, color, nullTexture, depth );
		}
	}
#endif
}


void draw_rectangle_gradient( const float x1, const float y1, const float x2, const float y2,
                              const Color c1, const Color c2, const Color c3, const Color c4, const bool outline, const float depth )
{
#if !RENDER_NONE
	if( outline )
	{
		draw_rectangle_gradient( x1       , y1       , x1 + 1.0f, y2       , c1, c1, c3, c3, false, depth ); // Left
		draw_rectangle_gradient( x2 - 1.0f, y1       , x2       , y2       , c2, c2, c4, c4, false, depth ); // Right
		draw_rectangle_gradient( x1 + 1.0f, y1       , x2 - 1.0f, y1 + 1.0f, c1, c2, c1, c2, false, depth ); // Top
		draw_rectangle_gradient( x1       , y2 - 1.0f, x2       , y2       , c3, c4, c3, c4, false, depth ); // Bottom
	}
	else
	{
		Gfx::quad_batch_write( x1, y1, x2, y2, nullGlyph.u1, nullGlyph.v1, nullGlyph.u2, nullGlyph.v2, c1, c2, c3, c4, nullTexture, depth );
	}
#endif
}


void draw_rectangle_gradient_angle( const float x1, const float y1, const float x2, const float y2, const float angle,
                                    const Color c1, const Color c2, const Color c3, const Color c4, const bool outline, const float depth )
{
#if !RENDER_NONE
	if( outline )
	{
		if( angle != 0.0f )
		{
			const vec2f x2y1 = vec2f( x2, y1 ).rotate_origin( x1, y1, angle );
			const vec2f x1y2 = vec2f( x1, y2 ).rotate_origin( x1, y1, angle );
			draw_rectangle_gradient_angle( x1,     y1,     x1 + 1.0f,            y2,                   angle, c1, c1, c3, c3, false, depth ); // left
			draw_rectangle_gradient_angle( x2y1.x, x2y1.y, x2y1.x - 1.0f,        x2y1.y + ( y2 - y1 ), angle, c2, c2, c4, c4, false, depth ); // right
			draw_rectangle_gradient_angle( x1,     y1,     x2,                   y1 + 1.0f,            angle, c1, c2, c1, c2, false, depth ); // top
			draw_rectangle_gradient_angle( x1y2.x, x1y2.y, x1y2.x + ( x2 - x1 ), x1y2.y - 1.0f,        angle, c3, c4, c3, c4, false, depth ); // bottom
		}
		else
		{
			draw_rectangle_gradient( x1       , y1       , x1 + 1.0f, y2       , c1, c1, c3, c3, false, depth ); // left
			draw_rectangle_gradient( x2 - 1.0f, y1       , x2       , y2       , c2, c2, c4, c4, false, depth ); // right
			draw_rectangle_gradient( x1 + 1.0f, y1       , x2 - 1.0f, y1 + 1.0f, c1, c2, c1, c2, false, depth ); // top
			draw_rectangle_gradient( x1       , y2 - 1.0f, x2       , y2       , c3, c4, c3, c4, false, depth ); // bottom
		}
	}
	else
	{
		if( angle != 0.0f )
		{
			const vec2f x2y1 = vec2f( x2, y1 ).rotate_origin( x1, y1, angle );
			const vec2f x1y2 = vec2f( x1, y2 ).rotate_origin( x1, y1, angle );
			const vec2f x2y2 = vec2f( x2, y2 ).rotate_origin( x1, y1, angle );

			Gfx::quad_batch_write( x1, y1, x2y1.x, x2y1.y, x1y2.x, x1y2.y, x2y2.x, x2y2.y,
			                          nullGlyph.u1, nullGlyph.v1, nullGlyph.u2, nullGlyph.v2,
			                          c1, c2, c3, c4, nullTexture, depth );
		}
		else
		{
			Gfx::quad_batch_write( x1, y1, x2, y2, nullGlyph.u1, nullGlyph.v1, nullGlyph.u2, nullGlyph.v2, c1, c2, c3, c4, nullTexture, depth );
		}
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_line( const float x1, const float y1, const float x2, const float y2, const Color color, const float thickness, const float depth )
{
#if !RENDER_NONE
	if ( x1 == x2 )
	{
		draw_rectangle( x1, y1, x1 + thickness, y2, color, false, depth ); // x-axis aligned line
	}
	else if ( y1 == y2 )
	{
		draw_rectangle( x1, y1, x2, y1 + thickness, color, false, depth ); // y-axis aligned line
	}
	else
	{
		const float angle = atanf( ( y1 - y2 ) / ( x2 - x1 ) ) + ( x2 < x1 ? PI : 0.0f );
		const float length = vec2_length( x1, y1, x2, y2 );
		draw_rectangle_angle( x1, y1, x1 + length, y1 + thickness, radtodeg( angle ), color, false, depth ); // rotated line
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_circle_gradient( float x, float y, float radius, Color c1, Color c2, u32 resolution, float depth )
{
	const float increment = 2.0f * PI / resolution;

	for( u32 i = 0; i < resolution; i++ )
	{
		const float angle1 = increment * ( i + 0.0f );
		const float angle2 = increment * ( i + 0.5f );
		const float angle3 = increment * ( i + 1.0f );

        float x1 = x;
        float y1 = y;
        float x2 = x + radius * cos( angle1 );
        float y2 = y + radius * sin( angle1 );
        float x3 = x + radius * cos( angle3 );
        float y3 = y + radius * sin( angle3 );
        float x4 = x + radius * cos( angle2 );
        float y4 = y + radius * sin( angle2 );

		Gfx::quad_batch_write( x1, y1, x2, y2, x3, y3, x4, y4,
		                          nullGlyph.u1, nullGlyph.v1, nullGlyph.u2, nullGlyph.v2,
		                          c1, c2, c2, c2, nullTexture, depth );
	}
}


void draw_circle_outline_gradient( float x, float y, float radius, float thickness, Color c1, Color c2, u32 resolution, float depth )
{
	resolution <<= 1;
	const float increment = 2.0f * PI / resolution;
	const float radiusOuter = ( radius + thickness * 0.5f );
	const float radiusInner = ( radius - thickness * 0.5f );

	for( u32 i = 0; i < resolution; i++ )
	{
        const float angle1 = increment * ( i + 0.0f );
		constexpr float epsilon = 0.001f; // HACK: Prevents gaps between quads due to not using proper triangle strip
        const float angle2 = increment * ( i + 1.0f + epsilon );

        const float outerX1 = x + radiusOuter * cos( angle1 );
        const float outerY1 = y + radiusOuter * sin( angle1 );
        const float outerX2 = x + radiusOuter * cos( angle2 );
        const float outerY2 = y + radiusOuter * sin( angle2 );

        const float innerX1 = x + radiusInner * cos( angle1 );
        const float innerY1 = y + radiusInner * sin( angle1 );
        const float innerX2 = x + radiusInner * cos( angle2 );
        const float innerY2 = y + radiusInner * sin( angle2 );

		Gfx::quad_batch_write( innerX1, innerY1, innerX2, innerY2, outerX1, outerY1, outerX2, outerY2,
		                          nullGlyph.u1, nullGlyph.v1, nullGlyph.u2, nullGlyph.v2,
		                          c1, c1, c2, c2, nullTexture, depth );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_text( const u16 font, const u16 size, const float x, const float y, Color color, const char *string )
{
#if !RENDER_NONE
	Assert( font < assets::fontsCount );
	Assert( size > 0 );

	int offsetX = 0;
	int offsetY = 0;

	u32 state = UTF8_ACCEPT;
	u32 codepoint;
	char c;

	while( ( c = *string++ ) != '\0' )
	{
		if( utf8_decode( &state, &codepoint, c ) != UTF8_ACCEPT ) { continue; }

		// Newline char
		if( UNLIKELY( codepoint == '\n' ) )
		{
			offsetX = 0;
			offsetY += size;
			continue;
		}

		// Retrieve FontGlyphInfo
		FontGlyphInfo &glyphInfo = fonts::get( { font, size, codepoint } );

		// Draw Quad
		if( glyphInfo.width != 0 && glyphInfo.height != 0 )
		{
			const float glyphX1 = x + offsetX + glyphInfo.xshift;
			const float glyphY1 = y + offsetY + glyphInfo.yshift;
			const float glyphX2 = x + offsetX + glyphInfo.xshift + glyphInfo.width;
			const float glyphY2 = y + offsetY + glyphInfo.yshift + glyphInfo.height;

			constexpr u16 uvScale = ( 1 << 16 ) / FONTS_TEXTURE_SIZE;
			const u16 u1 = ( glyphInfo.u ) * uvScale;
			const u16 v1 = ( glyphInfo.v ) * uvScale;
			const u16 u2 = ( glyphInfo.u + glyphInfo.width ) * uvScale;
			const u16 v2 = ( glyphInfo.v + glyphInfo.height ) * uvScale;

			// TODO: Implement this properly...
			if( UNLIKELY( bGfx::quadBatchPointer - bGfx::quadBatchStart >= RENDER_QUAD_BATCH_SIZE ) ||
			    UNLIKELY( bGfx::quadBatchTexture != &fonts::texture2D ) )
			{
				fonts::update();
			}

			Gfx::quad_batch_write( glyphX1, glyphY1, glyphX2, glyphY2, u1, v1, u2, v2, color, &fonts::texture2D, 0.0f );
		}

		// Advance Character
		offsetX += glyphInfo.advance;
	}
#endif
}


void draw_text_f( const u32 font, const u16 size, const float x, const float y, Color color, const char *format, ... )
{
#if !RENDER_NONE
	va_list args;
	va_start( args, format );
	char buffer[1024];
	vsnprintf( buffer, 1024, format, args );
	va_end( args );

	draw_text( font, size, x, y, color, buffer );
#endif
}


vec2i text_dimensions( const u16 font, const u16 size, const char *string )
{
	Assert( font < assets::fontsCount );
	Assert( size > 0 );

	int offsetX = 0;
	int offsetY = 0;
	vec2i result = 0;

	u32 state = UTF8_ACCEPT;
	u32 codepoint;
	char c;

	while( ( c = *string++ ) != '\0' )
	{
		if( utf8_decode( &state, &codepoint, c ) != UTF8_ACCEPT ) { continue; }

		// Newline char
		if( UNLIKELY( codepoint == '\n' ) )
		{
			offsetX = 0;
			result.y = max( result.y, offsetY + size );
			offsetY += size;
			continue;
		}

		// Retrieve FontGlyphInfo & advance
		FontGlyphInfo &glyphInfo = fonts::get( { font, size, codepoint } );
		result.x = max( result.x, offsetX + static_cast<int>( glyphInfo.advance ) );
		result.y = max( result.y, offsetY + glyphInfo.height );
		offsetX += glyphInfo.advance;
	}

	return result;
}


vec2i text_dimensions_f( const u32 font, const u16 size, const char *format, ... )
{
#if !RENDER_NONE
	va_list args;
	va_start( args, format );
	char buffer[1024];
	vsnprintf( buffer, 1024, format, args );
	va_end( args );

	return text_dimensions( font, size, buffer );
#endif
}