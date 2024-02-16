#include <manta/color.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Color Color::operator*( const Color &c )
{
	Color outColor = *this;
	if( c.r == 255 && c.g == 255 && c.b == 255 && c.a == 255 ) { return outColor; }
	outColor.r = static_cast<u8>( clamp( ( r / 255.0f ) * ( c.r / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.g = static_cast<u8>( clamp( ( g / 255.0f ) * ( c.g / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.b = static_cast<u8>( clamp( ( b / 255.0f ) * ( c.b / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.a = static_cast<u8>( clamp( ( a / 255.0f ) * ( c.a / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	return outColor;
}


Color Color::operator*( Color &c )
{
	Color outColor = *this;
	if( c.r == 255 && c.g == 255 && c.b == 255 && c.a == 255 ) { return outColor; }
	outColor.r = static_cast<u8>( clamp( ( r / 255.0f ) * ( c.r / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.g = static_cast<u8>( clamp( ( g / 255.0f ) * ( c.g / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.b = static_cast<u8>( clamp( ( b / 255.0f ) * ( c.b / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.a = static_cast<u8>( clamp( ( a / 255.0f ) * ( c.a / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	return outColor;
}


Color Color::operator*( const float s )
{
	Color outColor;
	outColor.r = static_cast<u8>( clamp( ( r / 255.0f ) * s, 0.0f, 1.0f ) * 255.0f );
	outColor.g = static_cast<u8>( clamp( ( g / 255.0f ) * s, 0.0f, 1.0f ) * 255.0f );
	outColor.b = static_cast<u8>( clamp( ( b / 255.0f ) * s, 0.0f, 1.0f ) * 255.0f );
	outColor.a = static_cast<u8>( clamp( ( a / 255.0f ) * s, 0.0f, 1.0f ) * 255.0f );
	return outColor;
}


Color Color::operator*=( const Color &c )
{
	if( c.r == 255 && c.g == 255 && c.b == 255 && c.a == 255 ) { return *this; }
	r = static_cast<u8>( clamp( ( r / 255.0f ) * ( c.r / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	g = static_cast<u8>( clamp( ( g / 255.0f ) * ( c.g / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	b = static_cast<u8>( clamp( ( b / 255.0f ) * ( c.b / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	a = static_cast<u8>( clamp( ( a / 255.0f ) * ( c.a / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	return *this;
}


Color Color::operator*=( Color &c )
{
	if( c.r == 255 && c.g == 255 && c.b == 255 && c.a == 255 ) { return *this; }
	r = static_cast<u8>( clamp( ( r / 255.0f ) * ( c.r / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	g = static_cast<u8>( clamp( ( g / 255.0f ) * ( c.g / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	b = static_cast<u8>( clamp( ( b / 255.0f ) * ( c.b / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	a = static_cast<u8>( clamp( ( a / 255.0f ) * ( c.a / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	return *this;
}


Color Color::operator*=( const float s )
{
	r = static_cast<u8>( clamp( ( r / 255.0f ) * s, 0.0f, 1.0f ) * 255.0f );
	g = static_cast<u8>( clamp( ( g / 255.0f ) * s, 0.0f, 1.0f ) * 255.0f );
	b = static_cast<u8>( clamp( ( b / 255.0f ) * s, 0.0f, 1.0f ) * 255.0f );
	a = static_cast<u8>( clamp( ( a / 255.0f ) * s, 0.0f, 1.0f ) * 255.0f );
	return *this;
}


Color Color::operator/( const Color &c )
{
	Color outColor = *this;
	if( c.r == 255 && c.g == 255 && c.b == 255 && c.a == 255 ) { return outColor; }
	outColor.r = static_cast<u8>( clamp( ( r / 255.0f ) / ( c.r / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.g = static_cast<u8>( clamp( ( g / 255.0f ) / ( c.g / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.b = static_cast<u8>( clamp( ( b / 255.0f ) / ( c.b / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.a = static_cast<u8>( clamp( ( a / 255.0f ) / ( c.a / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	return outColor;
}


Color Color::operator/( Color &c )
{
	Color outColor = *this;
	if( c.r == 255 && c.g == 255 && c.b == 255 && c.a == 255 ) { return outColor; }
	outColor.r = static_cast<u8>( clamp( ( r / 255.0f ) / ( c.r / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.g = static_cast<u8>( clamp( ( g / 255.0f ) / ( c.g / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.b = static_cast<u8>( clamp( ( b / 255.0f ) / ( c.b / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	outColor.a = static_cast<u8>( clamp( ( a / 255.0f ) / ( c.a / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	return outColor;
}


Color Color::operator/( const float s )
{
	Color outColor;
	outColor.r = static_cast<u8>( clamp( ( r / 255.0f ) / s, 0.0f, 1.0f ) * 255.0f );
	outColor.g = static_cast<u8>( clamp( ( g / 255.0f ) / s, 0.0f, 1.0f ) * 255.0f );
	outColor.b = static_cast<u8>( clamp( ( b / 255.0f ) / s, 0.0f, 1.0f ) * 255.0f );
	outColor.a = static_cast<u8>( clamp( ( a / 255.0f ) / s, 0.0f, 1.0f ) * 255.0f );
	return outColor;
}


Color Color::operator/=( const Color &c )
{
	if( c.r == 255 && c.g == 255 && c.b == 255 && c.a == 255 ) { return *this; }
	r = static_cast<u8>( clamp( ( r / 255.0f ) / ( c.r / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	g = static_cast<u8>( clamp( ( g / 255.0f ) / ( c.g / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	b = static_cast<u8>( clamp( ( b / 255.0f ) / ( c.b / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	a = static_cast<u8>( clamp( ( a / 255.0f ) / ( c.a / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	return *this;
}


Color Color::operator/=( Color &c )
{
	if( c.r == 255 && c.g == 255 && c.b == 255 && c.a == 255 ) { return *this; }
	r = static_cast<u8>( clamp( ( r / 255.0f ) / ( c.r / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	g = static_cast<u8>( clamp( ( g / 255.0f ) / ( c.g / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	b = static_cast<u8>( clamp( ( b / 255.0f ) / ( c.b / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	a = static_cast<u8>( clamp( ( a / 255.0f ) / ( c.a / 255.0f ), 0.0f, 1.0f ) * 255.0f );
	return *this;
}


Color Color::operator/=( const float s )
{
	r = static_cast<u8>( clamp( ( r / 255.0f ) / s, 0.0f, 1.0f ) * 255.0f );
	g = static_cast<u8>( clamp( ( g / 255.0f ) / s, 0.0f, 1.0f ) * 255.0f );
	b = static_cast<u8>( clamp( ( b / 255.0f ) / s, 0.0f, 1.0f ) * 255.0f );
	a = static_cast<u8>( clamp( ( a / 255.0f ) / s, 0.0f, 1.0f ) * 255.0f );
	return *this;
}


Color Color::operator+( const Color &c )
{
	Color outColor;
	outColor.r = static_cast<u8>( clamp( r + c.r, 0, 255 ) );
	outColor.g = static_cast<u8>( clamp( g + c.g, 0, 255 ) );
	outColor.b = static_cast<u8>( clamp( b + c.b, 0, 255 ) );
	outColor.a = static_cast<u8>( clamp( a + c.a, 0, 255 ) );
	return outColor;
}


Color Color::operator+( Color &c )
{
	Color outColor;
	outColor.r = static_cast<u8>( clamp( r + c.r, 0, 255 ) );
	outColor.g = static_cast<u8>( clamp( g + c.g, 0, 255 ) );
	outColor.b = static_cast<u8>( clamp( b + c.b, 0, 255 ) );
	outColor.a = static_cast<u8>( clamp( a + c.a, 0, 255 ) );
	return outColor;
}


Color Color::operator+=( const Color &c )
{
	r = static_cast<u8>( clamp( r + c.r, 0, 255 ) );
	g = static_cast<u8>( clamp( g + c.g, 0, 255 ) );
	b = static_cast<u8>( clamp( b + c.b, 0, 255 ) );
	a = static_cast<u8>( clamp( a + c.a, 0, 255 ) );
	return *this;
}


Color Color::operator+=( Color &c )
{
	r = static_cast<u8>( clamp( r + c.r, 0, 255 ) );
	g = static_cast<u8>( clamp( g + c.g, 0, 255 ) );
	b = static_cast<u8>( clamp( b + c.b, 0, 255 ) );
	a = static_cast<u8>( clamp( a + c.a, 0, 255 ) );
	return *this;
}


Color Color::operator-( const Color &c )
{
	Color outColor;
	outColor.r = static_cast<u8>( clamp( r - c.r, 0, 255 ) );
	outColor.g = static_cast<u8>( clamp( g - c.g, 0, 255 ) );
	outColor.b = static_cast<u8>( clamp( b - c.b, 0, 255 ) );
	outColor.a = static_cast<u8>( clamp( a - c.a, 0, 255 ) );
	return outColor;
}


Color Color::operator-( Color &c )
{
	Color outColor;
	outColor.r = static_cast<u8>( clamp( r - c.r, 0, 255 ) );
	outColor.g = static_cast<u8>( clamp( g - c.g, 0, 255 ) );
	outColor.b = static_cast<u8>( clamp( b - c.b, 0, 255 ) );
	outColor.a = static_cast<u8>( clamp( a - c.a, 0, 255 ) );
	return outColor;
}


Color Color::operator-=( const Color &c )
{
	r = static_cast<u8>( clamp( r - c.r, 0, 255 ) );
	g = static_cast<u8>( clamp( g - c.g, 0, 255 ) );
	b = static_cast<u8>( clamp( b - c.b, 0, 255 ) );
	a = static_cast<u8>( clamp( a - c.a, 0, 255 ) );
	return *this;
}


Color Color::operator-=( Color &c )
{
	r = static_cast<u8>( clamp( r - c.r, 0, 255 ) );
	g = static_cast<u8>( clamp( g - c.g, 0, 255 ) );
	b = static_cast<u8>( clamp( b - c.b, 0, 255 ) );
	a = static_cast<u8>( clamp( a - c.a, 0, 255 ) );
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Color color_hsv_to_rgb( const ColorHSV &colorHSV )
{
	const float h = colorHSV.h * 360.0f / 255.0f;
	const float s = colorHSV.s / 255.0f;
	const float v = colorHSV.v / 255.0f;

	const float c = v * s; // Chroma
	const float x = c * ( 1.0f - static_cast<float>( fabs( fmod( static_cast<double>( h ) / 60.0, 2.0 ) ) - 1.0 ) ); // X Value
	const float m = v - c; // Minimum RGB

	float r, g, b;
	if( h >=   0 && h <  60 ) { r = c; g = x; b = 0; } else
	if( h >=  60 && h < 120 ) { r = x; g = c; b = 0; } else
	if( h >= 120 && h < 180 ) { r = 0; g = c; b = x; } else
	if( h >= 180 && h < 240 ) { r = 0; g = x; b = c; } else
	if( h >= 240 && h < 300 ) { r = x; g = 0; b = c; } else
							  { r = c; g = 0; b = x; }

	return Color( static_cast<u8>( ( r + m ) * 255.0 ),
				  static_cast<u8>( ( g + m ) * 255.0 ),
				  static_cast<u8>( ( b + m ) * 255.0 ),
				  colorHSV.a );
}


ColorHSV color_rgb_to_hsv( const Color &color )
{
	const float r = color.r / 255.0f;
	const float g = color.g / 255.0f;
	const float b = color.b / 255.0f;

	const float cmax = max( max( r, g ), b );
	const float cmin = min( min( r, g ), b );
	const float diff = cmax - cmin;

	float hue;
	if( diff == 0.0f ) { hue = 0.0f; } else
	if( cmax == r    ) { hue = 60.0f * static_cast<float>( fmod( static_cast<double>( ( g - b ) / diff ), 6.0 ) ); } else
	if( cmax == g    ) { hue = 60.0f * ( ( ( b - r ) / diff ) + 2.0f ); } else
					   { hue = 60.0f * ( ( ( r - g ) / diff ) + 4.0f ); }
	if( hue <= 0.0f )  { hue += 360.0f; }

	const float saturation = ( cmax == 0.0f ) ? 0.0f : diff / cmax;
	const float value = cmax;

	return ColorHSV( static_cast<u8>( ( hue / 360.0f ) * 255.0f ),
					 static_cast<u8>( saturation * 255.0f ),
					 static_cast<u8>( value * 255.0f ),
					 color.a );
}
