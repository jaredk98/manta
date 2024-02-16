#include <manta/timer.hpp>

#include <debug.hpp>
#include <types.hpp>

#include <mach/mach_time.h>


static u64 offset;
static double frequency;


bool itimer::init()
{
	mach_timebase_info_data_t info;
	if( mach_timebase_info( &info ) != KERN_SUCCESS ) { ErrorReturnMsg( false, "MACH: Failed get mach timer info" ); }

	frequency = info.denom * 1e+9 / static_cast<double>( info.numer );
	offset = mach_absolute_time();
	return true;
}


double timer::value()
{
	return ( mach_absolute_time() - offset ) / frequency;
}


u64 timer::seed()
{
	return static_cast<u64>( mach_absolute_time() );
}
