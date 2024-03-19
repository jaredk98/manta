#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Simplex
{
	extern void seed( int seed );

	extern float sample( float x, float y );

	extern float sample_fbm( float x, float y, float f, float a, float l, float p, int o );

	inline float sample_normalized( float x, float y )
	{
		return sample( x, y ) * 0.5f + 0.5f;
	}
};