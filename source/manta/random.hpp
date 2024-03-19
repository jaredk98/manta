#pragma once

#include <types.hpp>
#include <vendor/initializer_list.hpp> // TODO: only used for choose(), so maybe get rid of it?

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct RandomContext
{
	RandomContext() { } // default
	RandomContext( const u64 seed ) { this->seed( seed ); }

	void seed( const u64 seed );
	u32 base();

	u64 state = 0;
	u64 where = 0;

	// Usage: randomContext.value<int>(...), randomContext.value<double>(...), etc
	template <typename T> T random( const T min, const T max );
	template <typename T> T random( const T max );

	// Usage: randomContext.choose_value( { value1, ..., valueN } );
	template <typename T> T choose_value( std::initializer_list<T> values )
	{
		return *( values.begin() + this->random<int>( static_cast<int>( values.size() ) - 1 ) );
	}
};

template <> int RandomContext::random<int>( const int min, const int max );
template <> int RandomContext::random<int>( const int max );

template <> float RandomContext::random<float>( const float min, const float max );
template <> float RandomContext::random<float>( const float max );

template <> double RandomContext::random<double>( const double min, const double max );
template <> double RandomContext::random<double>( const double max );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iRandom
{
	// thread_local ensures calls to Random:: do not conflict states between threads
	extern thread_local RandomContext context;
}

namespace Random
{
	inline void seed( const u64 seed ) { iRandom::context.seed( seed ); }
	inline u32 base() { return iRandom::context.base(); }

	inline RandomContext &context() { return iRandom::context; }
}

template <typename T> inline T random( T min, T max );
template <typename T> inline T random( T max );

template <> inline int random<int>( const int min, const int max ) { return iRandom::context.random<int>( min, max ); }
template <> inline int random<int>( const int max ) { return iRandom::context.random<int>( max ); }

template <> inline float random<float>( const float min, const float max ) { return iRandom::context.random<float>( min, max ); }
template <> inline float random<float>( const float max ) { return iRandom::context.random<float>( max ); }

template <> inline double random<double>( const double min, const double max ) { return iRandom::context.random<double>( min, max ); }
template <> inline double random<double>( const double max ) { return iRandom::context.random<double>( max ); }

#define choose(...) iRandom::context.choose_value( { __VA_ARGS__ } )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////