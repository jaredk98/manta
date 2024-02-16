#include <manta/random.hpp>
#include <manta/timer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace irandom
{
	// thread_local ensures calls to random:: do not conflict states between threads
	thread_local RandomContext context { timer::seed() };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RandomContext::seed( const u64 seed )
{
	state = 0;
	where = ( seed << 1 ) | 1;
	base();
	state += seed;
	base();
}


u32 RandomContext::base()
{
    // Advance State
	u64 oldstate = state;
    state = oldstate * 6364136223846793005 + where;

    // Calculate Output
    u32 xorshifted = static_cast<u32>( ( ( oldstate >> 18 ) ^ oldstate ) >> 27 );
    u32 rot = oldstate >> 59;
    return ( xorshifted >> rot ) | ( xorshifted << ( ( ~rot + 1 ) & 31 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Note: engine only supports random for types int, float, and double

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <> int RandomContext::value<int>( const int min, const int max )
{
	u32 x = base();
	u64 m = static_cast<u64>( x ) * static_cast<u64>( max - min + 1 );
	return ( m >> 32 ) + min;
}


template <> int RandomContext::value<int>( const int max )
{
	u32 x = base();
	u64 m = static_cast<u64>( x ) * static_cast<u64>( max + 1 );
	return ( m >> 32 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <> float RandomContext::value<float>( float min, float max )
{
	return base() * 0x1.0p-32f * ( max - min ) + min;
}


template <> float RandomContext::value<float>( const float max )
{
	return base() * 0x1.0p-32f * max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <> double RandomContext::value<double>( const double min, const double max )
{
	return base() * 0x1.0p-32 * ( max - min ) + min;
}


template <> double RandomContext::value<double>( const double max )
{
	return base() * 0x1.0p-32 * max;
}
