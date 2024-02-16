#include <manta/timer.hpp>

#include <time.h>


thread_local static timespec offset;


bool itimer::init()
{
	// Success
	return clock_gettime( CLOCK_MONOTONIC, &offset ) == 0;
}


double timer::value()
{
    timespec current;

    // Get Absolute Offset
    clock_gettime( CLOCK_MONOTONIC, &current );

	// Get Relative Offset
	return ( current.tv_sec - offset.tv_sec ) + ( current.tv_nsec - offset.tv_nsec ) / 1e+9;
}


u64 timer::seed()
{
	return static_cast<u64>( offset.tv_nsec );
}
