#pragma once

#include <types.hpp>
#include <manta/math.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum
{
	RGBA8,
	R10G10B10A2,
	R8,
	R8G8,
	R16,
	R16G16,
	R24G8,
	R32,
};

struct rgba8
{
	u8 r, g, b, a;
};

struct r10g10b10a2
{
	u8 r, g, b, a;
};

struct r8
{
	u8 r;
};

struct r8g8
{
	u8 r, g;
};

struct r16
{
	u16 r;
};

struct r16g16
{
	u16 r, g;
};

struct r24g8
{
	u32 r : 24;
	u32 g : 8;
};

struct r32
{
	u32 r;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Color;
struct ColorHSV;

extern struct Color color_hsv_to_rgb( const struct ColorHSV &colorHSV );
extern struct ColorHSV color_rgb_to_hsv( const struct Color &color );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Color
{
	constexpr Color( const u8 r = 0, const u8 g = 0, const u8 b = 0, const u8 a = 255 ) : r( r ), g( g ), b( b ), a( a ) { }

	u8 r, g, b, a;

	Color operator*( const Color &c );
	Color operator*( Color &c );
	Color operator*( const float s );

	Color operator*=( const Color &c );
	Color operator*=( Color &c );
	Color operator*=( const float s );

	Color operator/( const Color &c );
	Color operator/( Color &c );
	Color operator/( const float s );

	Color operator/=( const Color &c );
	Color operator/=( Color &c );
	Color operator/=( const float s );

	Color operator+( const Color &c );
	Color operator+( Color &c );

	Color operator+=( const Color &c );
	Color operator+=( Color &c );

	Color operator-( const Color &c );
	Color operator-( Color &c );

	Color operator-=( const Color &c );
	Color operator-=( Color &c );

	Color( ColorHSV &colorHSV )
	{
		const Color converted = color_hsv_to_rgb( colorHSV );
		r = converted.r;
		g = converted.g;
		b = converted.b;
		a = converted.a;
	}

	Color & operator=( const ColorHSV &colorHSV )
	{
		const Color converted = color_hsv_to_rgb( colorHSV );
		r = converted.r;
		g = converted.g;
		b = converted.b;
		a = converted.a;
		return *this;
	}

	Color & operator=( ColorHSV &ColorHSV )
	{
		const Color converted = color_hsv_to_rgb( ColorHSV );
		r = converted.r;
		g = converted.g;
		b = converted.b;
		a = converted.a;
		return *this;
	}

	inline bool operator==( const Color &c ) const
	{
		return ( r == c.r && g == c.g && b == c.b && a == c.a );
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ColorHSV
{
	ColorHSV( const u8 h = 0, const u8 s = 0, const u8 v = 0, const u8 a = 255 ) : h( h ), s( s ), v( v ), a( a ) { }

	u8 h, s, v, a;

	const ColorHSV & operator=( const Color &colorRGB )
	{
		const ColorHSV converted = color_rgb_to_hsv( colorRGB );
		h = converted.h;
		s = converted.s;
		v = converted.v;
		a = converted.a;
		return *this;
	}

	ColorHSV & operator=( Color &colorRGB )
	{
		const ColorHSV converted = color_rgb_to_hsv( colorRGB );
		h = converted.h;
		s = converted.s;
		v = converted.v;
		a = converted.a;
		return *this;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ColorGrade
{
	float hue = 1.0f;           //  0.0 to 1.0 - Default: 1.0
	float hueMergeRatio = 0.0f; //  0.0 to 1.0 - Default: 0.0
	float saturation = 1.0f;    //  0.0 to 2.0 - Default: 1.0
	float brightness = 1.10f;   //  0.0 to 2.0 - Default: 1.0
	float contrast = 1.25f;     //  0.0 to 2.0 - Default: 1.0
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Color color_make_bw( const u8 l ) { return Color( l, l, l, 255 ); }
inline Color color_make_bw( const float brightness )
{
	const u8 intensity = static_cast<u8>( 255.0f * brightness );
	return Color( intensity, intensity, intensity, 255 );
}


inline Color color_change_brightness( Color color, const float brightness )
{
	const u8 intensityR = static_cast<u8>( clamp( static_cast<float>( color.r ) * brightness, 0.0f, 255.0f ) );
	const u8 intensityG = static_cast<u8>( clamp( static_cast<float>( color.g ) * brightness, 0.0f, 255.0f ) );
	const u8 intensityB = static_cast<u8>( clamp( static_cast<float>( color.b ) * brightness, 0.0f, 255.0f ) );
	return Color( intensityR, intensityG, intensityB, color.a );
}


inline Color color_swap_alpha( const Color rgba, const u8 a = 0xFF ) { return Color( rgba.r, rgba.g, rgba.b, a ); }
inline void color_swap_alpha( Color &rgba, const u8 a = 0xFF ) { rgba.a = a; }


inline void color_mix_alpha( const Color &sourceColor, const Color targetColor, const float amount, Color &outColor )
{
	outColor.r = static_cast<u8>( sourceColor.r + ( targetColor.r - sourceColor.r ) * amount );
	outColor.g = static_cast<u8>( sourceColor.g + ( targetColor.g - sourceColor.g ) * amount );
	outColor.b = static_cast<u8>( sourceColor.b + ( targetColor.b - sourceColor.b ) * amount );
	outColor.a = static_cast<u8>( sourceColor.a + ( targetColor.a - sourceColor.a ) * amount );
}


inline Color color_mix( Color sourceColor, const Color targetColor, const float amount )
{
	sourceColor.r = static_cast<u8>( sourceColor.r + ( targetColor.r - sourceColor.r ) * amount );
	sourceColor.g = static_cast<u8>( sourceColor.g + ( targetColor.g - sourceColor.g ) * amount );
	sourceColor.b = static_cast<u8>( sourceColor.b + ( targetColor.b - sourceColor.b ) * amount );
	return sourceColor;
}


inline Color color_mix_alpha( Color sourceColor, const Color targetColor, const float amount )
{
	sourceColor.r = static_cast<u8>( sourceColor.r + ( targetColor.r - sourceColor.r ) * amount );
	sourceColor.g = static_cast<u8>( sourceColor.g + ( targetColor.g - sourceColor.g ) * amount );
	sourceColor.b = static_cast<u8>( sourceColor.b + ( targetColor.b - sourceColor.b ) * amount );
	sourceColor.a = static_cast<u8>( sourceColor.a + ( targetColor.a - sourceColor.a ) * amount );
	return sourceColor;
}


inline ColorHSV color_mix( ColorHSV sourceColor, const ColorHSV targetColor, const float amount )
{
	sourceColor.h = static_cast<u8>( sourceColor.h + ( targetColor.h - sourceColor.h ) * amount );
	sourceColor.s = static_cast<u8>( sourceColor.s + ( targetColor.s - sourceColor.s ) * amount );
	sourceColor.v = static_cast<u8>( sourceColor.v + ( targetColor.v - sourceColor.v ) * amount );
	return sourceColor;
}


inline ColorHSV color_mix_alpha( ColorHSV sourceColor, const ColorHSV targetColor, const float amount )
{
	sourceColor.h = static_cast<u8>( sourceColor.h + ( targetColor.h - sourceColor.h ) * amount );
	sourceColor.s = static_cast<u8>( sourceColor.s + ( targetColor.s - sourceColor.s ) * amount );
	sourceColor.v = static_cast<u8>( sourceColor.v + ( targetColor.v - sourceColor.v ) * amount );
	sourceColor.a = static_cast<u8>( sourceColor.a + ( targetColor.a - sourceColor.a ) * amount );
	return sourceColor;
}


inline u32 color_pack_u32( Color color )
{
	u32 packedColor = 0;
	packedColor |= ( static_cast<u32>( color.r )       );
	packedColor |= ( static_cast<u32>( color.g ) << 8  );
	packedColor |= ( static_cast<u32>( color.b ) << 16 );
	packedColor |= ( static_cast<u32>( color.a ) << 24 );
	return packedColor;
}


inline Color color_unpack_u32( const u32 packedColor )
{
	Color color;
	color.r = static_cast<u8>( ( packedColor       ) & 0xFF );
	color.g = static_cast<u8>( ( packedColor >> 8  ) & 0xFF );
	color.b = static_cast<u8>( ( packedColor >> 16 ) & 0xFF );
	color.a = static_cast<u8>( ( packedColor >> 24 ) & 0xFF );
	return color;
}


inline int color_pack_int( Color color )
{
	int packedColor = 0;
	packedColor |= ( static_cast<int>( color.r )       );
	packedColor |= ( static_cast<int>( color.g ) << 8  );
	packedColor |= ( static_cast<int>( color.b ) << 16 );
	packedColor |= ( static_cast<int>( color.a ) << 24 );
	return packedColor;
}


inline Color color_unpack_int( const int packedColor )
{
	Color color;
	color.r = static_cast<u8>( ( packedColor       ) & 0xFF );
	color.g = static_cast<u8>( ( packedColor >> 8  ) & 0xFF );
	color.b = static_cast<u8>( ( packedColor >> 16 ) & 0xFF );
	color.a = static_cast<u8>( ( packedColor >> 24 ) & 0xFF );
	return color;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define c_no_alpha         Color(   0,   0,   0,   0 )
#define c_aqua             Color(   0, 255, 255, 255 )
#define c_black            Color(   0,   0,   0, 255 )
#define c_blue             Color(   0,   0, 255, 255 )
#define c_dkgray           Color(  64,  64,  64, 255 )
#define c_fushia           Color( 255,   0, 255, 255 )
#define c_gray             Color( 128, 128, 128, 255 )
#define c_green            Color(   0, 128,   0, 255 )
#define c_lime             Color(   0, 255,   0, 255 )
#define c_ltgray           Color( 192, 192, 192, 255 )
#define c_silver           Color( 192, 192, 192, 255 )
#define c_maroon           Color( 128,   0,   0, 255 )
#define c_navy             Color(   0,   0, 128, 255 )
#define c_olive            Color( 128, 128,   0, 255 )
#define c_orange           Color( 255, 128,   0, 255 )
#define c_purple           Color( 128,   0, 128, 255 )
#define c_red              Color( 255,   0,   0, 255 )
#define c_brown            Color( 128,  64,  32, 255 )
#define c_teal             Color(   0,   0, 128, 255 )
#define c_white            Color( 255, 255, 255, 255 )
#define c_yellow           Color( 255, 255,   0, 255 )

#define c_console_true     Color( 130, 215,  65, 255 )
#define c_console_false    Color( 240, 50,   50, 255 )
#define c_console_arg_req  Color( 215, 220, 255, 255 )
#define c_console_arg_opt  Color( 125, 125, 175, 255 )
#define c_console_success  Color(  90, 210,  15, 255 )
#define c_console_warning  Color( 255, 225,  90, 255 )
#define c_console_error    Color( 225,   0,   0, 255 )
#define c_console_message  Color( 255, 225, 185, 255 )