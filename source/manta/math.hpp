#pragma once

#include <types.hpp>

#include <manta/vector.hpp>
#include <manta/matrix.hpp>

#include <vendor/math.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PI ( 3.14159265358979f )
#define ALIGN_16( value ) ( ( static_cast<int>( value ) >> 4 ) << 4 )
#define ALIGN_16_CEIL( value ) ( ( ( static_cast<int>( value ) >> 4 ) + 1 ) << 4 )
#define ALIGN_POWER( value, power ) ( ( static_cast<int>( value ) >> power ) << power )
#define ALIGN_POWER_CEIL( value, power ) ( ( ( static_cast<int>( value ) >> power ) + 1 ) << power )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline float fast_fmod(float x, float y)
{
	return x - ( static_cast<int>( x / y ) ) * y;
}


static inline int fast_floor( float x )
{
	int    i = static_cast<int>( x );
	return i - ( i > x );
}


static inline u32 ceilpow2( u32 v )
{
	v -= 1;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v += 1;
	return v;
}


extern void fast_sin_cos( const float angle, float &sin, float &cos );


static inline float degtorad( float degree )
{
	return degree * ( PI / 180.0f );
}


static inline float radtodeg( float radian )
{
	return radian * ( 180.0f / PI );
}


template <typename T> inline T lengthdir_x( T dist, float angle )
{
	return static_cast<T>( static_cast<float>( dist ) * cosf( degtorad( angle ) ) );
}


template <typename T> inline T lengthdir_y( T dist, float angle )
{
	return static_cast<T>( static_cast<float>( dist ) * sinf( degtorad( angle ) ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> inline T min( const T val1, const T val2 )
{
	return ( val1 < val2 ) ? val1 : val2;
}


template <typename T> inline T max( const T val1, const T val2 )
{
	return ( val1 > val2 ) ? val1 : val2;
}


template <typename T> inline T clamp( T value, const T min, const T max )
{
	if( value < min ) { value = min; }
	if( value > max ) { value = max; }
	return value;
}


template <typename T> inline T wrap( const T &value, const T &min_value, const T &max_value )
{
	return ( value > max_value ? min_value : ( value < min_value ? max_value : value ) );
}


template <typename T> inline void wrap( const T &value, const T &min_value, const T &max_value, T &out_value )
{
	out_value = ( value > max_value ? min_value : ( value < min_value ? max_value : value ) );
}


template <typename T> inline T lerp( const T value1, const T value2, const float amount )
{
	if( amount <= 0.0f ) { return value1; }
	if( amount >= 1.0f ) { return value2; }
	return static_cast<T>( value1 + ( value2 - value1 ) * amount );
}


template <typename T> inline void lerp( const T &value1, const T value2, const float amount, T &result )
{
	if( amount <= 0.0f ) { result = value1; return; }
	if( amount >= 1.0f ) { result = value2; return; }
	result = static_cast<T>( value1 + ( value2 - value1 ) * amount );
}


template <typename T> inline bool tween( T &variable, const T target, const T factor, const T delta, const float threshold = 0.25f )
{
	if( fabs( static_cast<double>( variable - target ) ) < static_cast<double>( threshold ) )
	{
		variable = target;
		return false;
	}
	else
	{
		variable += static_cast<T>( static_cast<float>( target - variable ) / max( static_cast<float>( factor / delta ), 1.0f ) );
		return true;
	}
}