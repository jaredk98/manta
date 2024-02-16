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
	template <typename T> T value( const T min, const T max );
	template <typename T> T value( const T max );

	// Usage: randomContext.choose_value( { value1, ..., valueN } );
	template <typename T> T choose_value( std::initializer_list<T> values )
	{
		return *( values.begin() + this->value<int>( static_cast<int>( values.size() ) - 1 ) );
	}
};

template <> int RandomContext::value<int>( const int min, const int max );
template <> int RandomContext::value<int>( const int max );

template <> float RandomContext::value<float>( const float min, const float max );
template <> float RandomContext::value<float>( const float max );

template <> double RandomContext::value<double>( const double min, const double max );
template <> double RandomContext::value<double>( const double max );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace irandom
{
	// thread_local ensures calls to random:: do not conflict states between threads
	extern thread_local RandomContext context;
}

namespace random
{
	static inline void seed( const u64 seed ) { irandom::context.seed( seed ); }
	static inline u32 base() { return irandom::context.base(); }

	static inline RandomContext &context() { return irandom::context; }

	// Usage: random::value<int>(...), random::value<double>(...), etc
	template <typename T> inline T value( T min, T max );
	template <typename T> inline T value( T max );

	template <> inline int value<int>( const int min, const int max ) { return irandom::context.value<int>( min, max ); }
	template <> inline int value<int>( const int max ) { return irandom::context.value<int>( max ); }

	template <> inline float value<float>( const float min, const float max ) { return irandom::context.value<float>( min, max ); }
	template <> inline float value<float>( const float max ) { return irandom::context.value<float>( max ); }

	template <> inline double value<double>( const double min, const double max ) { return irandom::context.value<double>( min, max ); }
	template <> inline double value<double>( const double max ) { return irandom::context.value<double>( max ); }
}

#define choose(...) irandom::context.choose_value( { __VA_ARGS__ } )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////